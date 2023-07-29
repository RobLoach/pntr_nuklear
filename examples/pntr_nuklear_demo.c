#include <stdio.h>

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
//#define INCLUDE_CANVAS
#define INCLUDE_OVERVIEW
//#define INCLUDE_NODE_EDITOR

#ifdef INCLUDE_STYLE
  #include "demo/style.c"
#endif
#ifdef INCLUDE_CALCULATOR
  #include "demo/calculator.c"
#endif
#ifdef INCLUDE_CANVAS
  #include "demo/canvas.c"
#endif
#ifdef INCLUDE_OVERVIEW
  #include "demo/overview.c"
#endif
#ifdef INCLUDE_NODE_EDITOR
  #include "demo/node_editor.c"
#endif

bool Init(void* userData) {
    AppData* app = (AppData*)userData;
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

bool Update(pntr_image* screen, void* userData) {
    AppData* app = (AppData*)userData;

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

void Event(pntr_app_event* event, void* userData) {
    AppData* app = (AppData*)userData;
    if (app == NULL) {
        return;
    }

    pntr_nuklear_event(app->ctx, event);
}

void Close(void* userData) {
    AppData* app = (AppData*)userData;
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
