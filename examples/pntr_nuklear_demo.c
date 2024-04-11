#include <stdio.h>
#include <limits.h>

#define PNTR_ENABLE_DEFAULT_FONT
//#define PNTR_ENABLE_FILTER_SMOOTH
//#define PNTR_ENABLE_TTF
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

#define INCLUDE_STYLE
#define INCLUDE_CALCULATOR
#define INCLUDE_CANVAS
#define INCLUDE_OVERVIEW
//#define INCLUDE_NODE_EDITOR

// Include the examples
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
#ifdef INCLUDE_STYLE
  #include "demo/common/style.c"
#endif
#ifdef INCLUDE_CALCULATOR
  #include "demo/common/calculator.c"
#endif
#ifdef INCLUDE_CANVAS
  #include "demo/common/canvas.c"
#endif
#ifdef INCLUDE_OVERVIEW
  #include "demo/common/overview.c"
#endif
#ifdef INCLUDE_NODE_EDITOR
  #include "demo/common/node_editor.c"
#endif
#pragma GCC diagnostic pop

bool Init(pntr_app* application) {
    AppData* app = (AppData*)pntr_app_userdata(application);
    app->font = pntr_load_font_default();
    app->ctx = pntr_load_nuklear(app->font);
    app->bg = PNTR_RAYWHITE;

    #ifdef INCLUDE_STYLE
        #ifdef STYLE_WHITE
            set_style(app->ctx, THEME_WHITE);
        #elif defined(STYLE_RED)
            set_style(app->ctx, THEME_RED);
        #elif defined(STYLE_BLUE)
            set_style(app->ctx, THEME_BLUE);
        #elif defined(STYLE_DARK)
            set_style(app->ctx, THEME_DARK);
        #endif
    #endif

    return true;
}

bool Update(pntr_app* application, pntr_image* screen) {
    AppData* app = (AppData*)pntr_app_userdata(application);

    struct nk_context* ctx = app->ctx;
    struct nk_colorf bg = pntr_color_to_nk_colorf(app->bg);

    #ifdef INCLUDE_CALCULATOR
        calculator(ctx);
    #endif
    #ifdef INCLUDE_CANVAS
        canvas(ctx);
    #endif
    #ifdef INCLUDE_OVERVIEW
        overview(ctx);
    #endif
    #ifdef INCLUDE_NODE_EDITOR
        node_editor(ctx);
    #endif

    pntr_clear_background(screen, pntr_get_color(0x141414FF));
    pntr_draw_nuklear(screen, app->ctx);

    return true;
}

void Event(pntr_app* application, pntr_app_event* event) {
    AppData* app = (AppData*)pntr_app_userdata(application);
    if (app == NULL) {
        return;
    }

    pntr_nuklear_event(app->ctx, event);
}

void Close(pntr_app* application) {
    AppData* app = (AppData*)pntr_app_userdata(application);
    pntr_unload_nuklear(app->ctx);
    pntr_unload_font(app->font);
}

pntr_app Main(int argc, char* argv[]) {
    return (pntr_app) {
        .width = 800,
        .height = 600,
        .title = "pntr_nuklear: Example",
        .init = Init,
        .update = Update,
        .close = Close,
        .event = Event,
        .fps = 60,
        .userData = pntr_load_memory(sizeof(AppData)),
    };
}
