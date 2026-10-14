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

    // nk_image_ptr() zeroes the image region, so pntr_draw_nuklear() has to fall
    // back to the source image's own dimensions rather than scaling by zero.
    {
        pntr_image* source = pntr_gen_image_color(4, 4, PNTR_RED);
        PNTR_ASSERT(source);

        pntr_image* target = pntr_gen_image_color(320, 220, PNTR_BLACK);
        PNTR_ASSERT(target);

        struct nk_rect bounds = nk_rect(0, 0, 0, 0);
        if (nk_begin(ctx, "Image", nk_rect(0, 0, 200, 200), NK_WINDOW_BORDER)) {
            nk_layout_row_static(ctx, 32, 32, 1);
            bounds = nk_widget_bounds(ctx);
            nk_image(ctx, nk_image_ptr(source));
        }
        nk_end(ctx);

        pntr_draw_nuklear(target, ctx);

        PNTR_ASSERT(bounds.w > 0.0f);
        PNTR_ASSERT(bounds.h > 0.0f);
        PNTR_ASSERT(pntr_image_get_color(target,
            (int)(bounds.x + bounds.w / 2.0f),
            (int)(bounds.y + bounds.h / 2.0f)).value == PNTR_RED.value);

        pntr_unload_image(target);
        pntr_unload_image(source);
    }

    // A clip the caller set has to be honoured while drawing too, not just handed
    // back afterwards, so a GUI cannot paint outside of the viewport it was given.
    {
        pntr_image* viewport = pntr_gen_image_color(320, 220, PNTR_RAYWHITE);
        PNTR_ASSERT(viewport);

        pntr_image_set_clip(viewport, 100, 10, 80, 60);

        // A window wider and taller than the clip, with a button that spans the whole
        // row, so that both the widget and the window border overflow the clip.
        if (nk_begin(ctx, "Clipped", nk_rect(10, 10, 300, 200), NK_WINDOW_BORDER)) {
            nk_layout_row_dynamic(ctx, 30, 1);
            nk_button_label(ctx, "Clipped");
        }
        nk_end(ctx);

        pntr_draw_nuklear(viewport, ctx);

        // Inside the clip, the GUI was drawn.
        PNTR_ASSERT(pntr_image_get_color(viewport, 140, 25).value != PNTR_RAYWHITE.value);
        PNTR_ASSERT(pntr_image_get_color(viewport, 120, 60).value != PNTR_RAYWHITE.value);

        // Outside of it, in every direction, nothing was touched. The button row is
        // drawn after Nuklear's scissor, and the window border after the nk_null_rect
        // that follows it, so both have to end up restricted to the clip.
        PNTR_ASSERT(pntr_image_get_color(viewport, 50, 25).value == PNTR_RAYWHITE.value);
        PNTR_ASSERT(pntr_image_get_color(viewport, 250, 25).value == PNTR_RAYWHITE.value);
        PNTR_ASSERT(pntr_image_get_color(viewport, 140, 80).value == PNTR_RAYWHITE.value);
        PNTR_ASSERT(pntr_image_get_color(viewport, 140, 5).value == PNTR_RAYWHITE.value);
        PNTR_ASSERT(pntr_image_get_color(viewport, 10, 150).value == PNTR_RAYWHITE.value);
        PNTR_ASSERT(pntr_image_get_color(viewport, 200, 209).value == PNTR_RAYWHITE.value);

        // And the caller's clip is still what it was.
        pntr_rectangle clip = pntr_image_get_clip(viewport);
        PNTR_ASSERT(clip.x == 100);
        PNTR_ASSERT(clip.y == 10);
        PNTR_ASSERT(clip.width == 80);
        PNTR_ASSERT(clip.height == 60);

        pntr_unload_image(viewport);
    }

    // A GUI entirely outside of the caller's clip has to clip to nothing, rather
    // than to an empty rectangle that pntr_image_set_clip() would have ignored.
    {
        pntr_image* viewport = pntr_gen_image_color(320, 220, PNTR_RAYWHITE);
        PNTR_ASSERT(viewport);

        pntr_image_set_clip(viewport, 200, 150, 50, 50);

        if (nk_begin(ctx, "Outside", nk_rect(0, 0, 100, 100), NK_WINDOW_BORDER)) {
            nk_layout_row_dynamic(ctx, 30, 1);
            nk_button_label(ctx, "Outside");
        }
        nk_end(ctx);

        pntr_draw_nuklear(viewport, ctx);

        for (int y = 0; y < viewport->height; y++) {
            for (int x = 0; x < viewport->width; x++) {
                PNTR_ASSERT(pntr_image_get_color(viewport, x, y).value == PNTR_RAYWHITE.value);
            }
        }

        pntr_rectangle clip = pntr_image_get_clip(viewport);
        PNTR_ASSERT(clip.x == 200);
        PNTR_ASSERT(clip.y == 150);
        PNTR_ASSERT(clip.width == 50);
        PNTR_ASSERT(clip.height == 50);

        pntr_unload_image(viewport);
    }

    // Unload
    pntr_unload_font(font);
    pntr_unload_image(image);
    pntr_unload_nuklear(ctx);

    return 0;
}
