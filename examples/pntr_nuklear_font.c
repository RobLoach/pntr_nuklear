#include <stdio.h>

#define PNTR_ENABLE_DEFAULT_FONT
//#define PNTR_ENABLE_FILTER_SMOOTH
#define PNTR_ENABLE_TTF
#define PNTR_ENABLE_MATH

#define PNTR_APP_IMPLEMENTATION
#include "pntr_app.h"

#define PNTR_NUKLEAR_IMPLEMENTATION
#include "pntr_nuklear.h"

typedef struct AppData {
    struct nk_context* ctx;
    pntr_font* font;
    pntr_color bg;
} AppData;

bool Init(pntr_app* application) {
    AppData* app = (AppData*)pntr_app_userdata(application);
    app->font = pntr_load_font_ttf("resources/blkchcry.ttf", 64);
    app->ctx = pntr_load_nuklear(app->font);
    app->bg = pntr_get_color(0x141414FF);

    return true;
}

bool Update(pntr_app* application, pntr_image* screen) {
    AppData* app = (AppData*)pntr_app_userdata(application);

    struct nk_context* ctx = app->ctx;
    pntr_nuklear_update(app->ctx, application);
    struct nk_colorf bg = pntr_color_to_nk_colorf(app->bg);

    // GUI
    if (nk_begin(ctx, "Demo", nk_rect(40, 40, 460, 500),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
        NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
    {
        enum {EASY, HARD};
        static int op = EASY;
        static int property = 20;

        nk_layout_row_static(ctx, 60, 160, 1);
        if (nk_button_label(ctx, "button")) {
            pntr_app_log(PNTR_APP_LOG_INFO, "Button Pressed!");
        }

        nk_layout_row_dynamic(ctx, 60, 2);
        if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
        if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;
        nk_layout_row_dynamic(ctx, 50, 1);
        nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

        nk_layout_row_dynamic(ctx, 40, 1);
        nk_label(ctx, "background:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(ctx, 50, 1);
        if (nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx), 800))) {
            nk_layout_row_dynamic(ctx, 240, 1);
            bg = nk_color_picker(ctx, bg, NK_RGBA);
            nk_layout_row_dynamic(ctx, 50, 1);
            bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f, 0.005f);
            bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f, 0.005f);
            bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f, 0.005f);
            bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f, 0.005f);
            nk_combo_end(ctx);
        }
    }
    nk_end(ctx);

    // Port the Nuklear state to pntr values.
    app->bg = pntr_color_from_nk_colorf(bg);

    pntr_clear_background(screen, app->bg);
    pntr_draw_nuklear(screen, app->ctx);

    return true;
}

void Close(pntr_app* application) {
    AppData* app = (AppData*)pntr_app_userdata(application);
    pntr_unload_nuklear(app->ctx);
    pntr_unload_font(app->font);
}

pntr_app Main(int argc, char* argv[]) {
    return (pntr_app) {
        .width = 1280,
        .height = 960,
        .title = "pntr_nuklear: Example",
        .init = Init,
        .update = Update,
        .close = Close,
        .fps = 60,
        .userData = pntr_load_memory(sizeof(AppData)),
    };
}
