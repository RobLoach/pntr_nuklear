#define PNTR_ENABLE_DEFAULT_FONT
//#define PNTR_ENABLE_FILTER_SMOOTH
//#define PNTR_ENABLE_TTF
//#define PNTR_ENABLE_MATH

#define PNTR_APP_IMPLEMENTATION
#include "pntr_app.h"

#define PNTR_NUKLEAR_ENABLE_GAMEPAD
#define PNTR_NUKLEAR_IMPLEMENTATION
#include "pntr_nuklear.h"

typedef struct AppData {
    struct nk_context* ctx;
    pntr_font* font;
    pntr_color bg;
} AppData;

bool Init(pntr_app* application) {
    AppData* app = (AppData*)pntr_app_userdata(application);
    app->font = pntr_load_font_default();
    app->ctx = pntr_load_nuklear(app->font);
    app->bg = pntr_get_color(0x141414FF);

    return true;
}

bool Update(pntr_app* application, pntr_image* screen) {
    AppData* app = (AppData*)pntr_app_userdata(application);

    struct nk_context* ctx = app->ctx;
    pntr_nuklear_update(app->ctx, application);
    struct nk_gamepads* gamepads = pntr_nuklear_gamepads(app->ctx);
    struct nk_colorf bg = pntr_color_to_nk_colorf(app->bg);

    // GUI
    if (nk_begin(ctx, "Demo", nk_rect(20, 20, 230, 250),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
        NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
    {
        enum {EASY, HARD};
        static int op = EASY;
        static int property = 20;

        nk_layout_row_dynamic(ctx, 0, 1);
        if (nk_button_label(ctx, "Press A Button") || nk_gamepad_is_button_pressed(gamepads, -1, NK_GAMEPAD_BUTTON_A)) {
            pntr_app_log(PNTR_APP_LOG_INFO, "Button Pressed!");
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
        .width = 640,
        .height = 480,
        .title = "pntr_nuklear: Example",
        .init = Init,
        .update = Update,
        .close = Close,
        .fps = 60,
        .userData = pntr_load_memory(sizeof(AppData)),
    };
}
