#include <stdio.h>

#define PNTR_ENABLE_DEFAULT_FONT
#define PNTR_ENABLE_MATH

#define PNTR_APP_IMPLEMENTATION
#include "pntr_app.h"

#define PNTR_NUKLEAR_IMPLEMENTATION
#include "pntr_nuklear.h"

typedef struct AppData {
    struct nk_context* ctx;
    pntr_font* font;
    pntr_image* image;
} AppData;

bool Init(pntr_app* application) {
    AppData* app = (AppData*)pntr_app_userdata(application);
    app->font = pntr_load_font_default();
    app->ctx = pntr_load_nuklear(app->font);
    app->image = pntr_load_image("resources/test-image.png");

    return app->image != NULL;
}

bool Update(pntr_app* application, pntr_image* screen) {
    AppData* app = (AppData*)pntr_app_userdata(application);

    struct nk_context* ctx = app->ctx;

    if (app->image == NULL) {
        return false;
    }

    // GUI
    if (nk_begin(ctx, "Demo", nk_rect(20, 20, 300, 300),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
        NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
    {
        // Setup the layout
        nk_layout_row_static(ctx, app->image->height, app->image->width, 1);

        // Draw the image
        nk_image(ctx, pntr_image_nk(app->image));
    }
    nk_end(ctx);

    // Render
    pntr_clear_background(screen, PNTR_DARKGRAY);
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
    pntr_unload_image(app->image);
    pntr_unload_font(app->font);
    pntr_unload_nuklear(app->ctx);
}

pntr_app Main(int argc, char* argv[]) {
    return (pntr_app) {
        .width = 640,
        .height = 480,
        .title = "pntr_nuklear: Image Example",
        .init = Init,
        .update = Update,
        .close = Close,
        .event = Event,
        .fps = 60,
        .userData = pntr_load_memory(sizeof(AppData)),
    };
}
