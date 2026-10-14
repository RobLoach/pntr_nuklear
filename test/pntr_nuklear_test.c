#include <stdio.h>

#define PNTR_DISABLE_MATH
#define PNTR_IMPLEMENTATION
#define PNTR_ENABLE_DEFAULT_FONT
#include "pntr.h"
#include "pntr_assert.h"

#define PNTR_NUKLEAR_IMPLEMENTATION
#include "pntr_nuklear.h"

/**
 * Build the example GUI. Called once per pntr_draw_nuklear(), since
 * pntr_draw_nuklear() clears the command queue when it is done with it.
 */
static void pntr_nuklear_test_gui(struct nk_context* ctx) {
    enum {EASY, HARD};
    static int op = EASY;
    static float value = 0.6f;

    if (nk_begin(ctx, "pntr_nuklear Example", nk_rect(10, 10, 300, 200),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_CLOSABLE)) {
        nk_layout_row_static(ctx, 30, 80, 1);
        if (nk_button_label(ctx, "Button")) {
            printf("Button Pressed!\n");
        }

        // Fixed widget window ratio width
        nk_layout_row_dynamic(ctx, 30, 2);
        if (nk_option_label(ctx, "Easy", op == EASY)) op = EASY;
        if (nk_option_label(ctx, "Hard", op == HARD)) op = HARD;

        // Custom widget pixel width
        nk_layout_row_begin(ctx, NK_STATIC, 30, 2);
        {
            nk_layout_row_push(ctx, 80);
            nk_label(ctx, "Volume:", NK_TEXT_LEFT);
            nk_layout_row_push(ctx, 110);
            nk_slider_float(ctx, 0, &value, 1.0f, 0.1f);
        }
        nk_layout_row_end(ctx);
    }
    nk_end(ctx);
}

int main() {
    pntr_font* font = pntr_load_font_default();
    PNTR_ASSERT(font);

    struct nk_context* ctx = pntr_load_nuklear(font);
    PNTR_ASSERT(ctx);

    pntr_nuklear_test_gui(ctx);

    // Build a screen
    pntr_image* image = pntr_gen_image_color(320, 220, PNTR_RAYWHITE);
    PNTR_ASSERT(image);

    // Render to the image
    pntr_draw_nuklear(image, ctx);

    // Save the image
    PNTR_ASSERT(pntr_save_image(image, "pntr_nuklear_test.png"));

    // pntr_draw_nuklear() must leave the clip region alone, so that whatever the
    // caller draws afterwards is not silently cropped to a Nuklear window.
    {
        pntr_rectangle clip = pntr_image_get_clip(image);
        PNTR_ASSERT(clip.x == 0);
        PNTR_ASSERT(clip.y == 0);
        PNTR_ASSERT(clip.width == image->width);
        PNTR_ASSERT(clip.height == image->height);

        // The Nuklear window is at (10, 10, 300, 200), so the bottom-right corner
        // of the 320x220 image is outside of it and must still be drawable.
        pntr_draw_rectangle_fill(image, 0, 0, image->width, image->height, PNTR_BLUE);
        PNTR_ASSERT(pntr_image_get_color(image, image->width - 1, image->height - 1).value == PNTR_BLUE.value);
        PNTR_ASSERT(pntr_image_get_color(image, 0, 0).value == PNTR_BLUE.value);
    }

    // A clip the caller set themselves must be given back, rather than reset to
    // the whole image.
    {
        pntr_image* clipped = pntr_gen_image_color(320, 220, PNTR_RAYWHITE);
        PNTR_ASSERT(clipped);

        pntr_image_set_clip(clipped, 40, 50, 60, 70);
        pntr_nuklear_test_gui(ctx);
        pntr_draw_nuklear(clipped, ctx);

        pntr_rectangle clip = pntr_image_get_clip(clipped);
        PNTR_ASSERT(clip.x == 40);
        PNTR_ASSERT(clip.y == 50);
        PNTR_ASSERT(clip.width == 60);
        PNTR_ASSERT(clip.height == 70);

        pntr_unload_image(clipped);
    }

    // Unload
    pntr_unload_font(font);
    pntr_unload_image(image);
    pntr_unload_nuklear(ctx);

    return 0;
}
