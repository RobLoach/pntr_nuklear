#include <stdio.h>
#include <assert.h>

#define PNTR_DISABLE_MATH
#define PNTR_IMPLEMENTATION
#define PNTR_ENABLE_DEFAULT_FONT
#include "pntr.h"

#define PNTR_NUKLEAR_IMPLEMENTATION
#include "pntr_nuklear.h"

int main() {
    pntr_font* font = pntr_load_font_default();
    assert(font);

    struct nk_context* ctx = pntr_load_nuklear(font);
    assert(ctx);

    // Nuklear GUI Code
    // if (nk_begin(ctx, "pntr-nuklear", nk_rect(50, 50, 300, 200),
    //                 NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_CLOSABLE)) {
    //     nk_button_label(ctx, "Button");
    // }
    // nk_end(ctx);

    enum {EASY, HARD};
    static int op = EASY;
    static float value = 0.6f;
    static int i =  20;

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

    // Build a screen
    pntr_image* image = pntr_gen_image_color(320, 220, PNTR_RAYWHITE);
    assert(image);

    // Render to the image
    pntr_draw_nuklear(image, ctx);

    // Save the image
    assert(pntr_save_image(image, "pntr_nuklear_test.png"));

    // Unload
    pntr_unload_font(font);
    pntr_unload_image(image);
    pntr_unload_nuklear(ctx);

    return 0;
}
