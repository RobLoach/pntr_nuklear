# pntr_nuklear

[Nuklear](https://github.com/Immediate-Mode-UI/Nuklear) immediate-mode graphical user interface for [pntr](https://github.com/robloach/pntr).

![Screenshot of pntr_nuklear](test/pntr_nuklear_test.png)

## Usage

``` c
#define PNTR_NUKLEAR_IMPLEMENTATION
#include "pntr_nuklear.h"

int main() {
    // Initialize
    pntr_font* font = pntr_load_font_default();
    struct nk_context* ctx = pntr_load_nuklear(font);

    // Nuklear GUI Code
    if (nk_begin(ctx, "Hello World", nk_rect(50, 50, 200, 150),
                    NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_CLOSABLE)) {
        if (nk_button_label(ctx, "Button")) {
            printf("Hello World!\n");
        }
    }
    nk_end(ctx);

    // Render
    pntr_draw_nuklear(screen, ctx);

    // Unload
    pntr_unload_nuklear(ctx);
    pntr_unload_font(font);

    return 0;
}
```

### API

``` c
struct nk_context* pntr_load_nuklear(pntr_font* font);
void pntr_unload_nuklear(struct nk_context* ctx);
void pntr_nuklear_event(struct nk_context* ctx, pntr_app_event* event);
void pntr_draw_nuklear(pntr_image* dst, struct nk_context* ctx);
struct nk_rect pntr_rectangle_to_nk_rect(pntr_rectangle rectangle);
pntr_color pntr_color_from_nk_color(struct nk_color color);
struct nk_color pntr_color_to_nk_color(pntr_color color);
pntr_vector pntr_vector_from_nk_vec2i(struct nk_vec2i vector);
pntr_color pntr_color_from_nk_colorf(struct nk_colorf color);
```

## License

Unless stated otherwise, all works are:

- Copyright (c) 2023 [Rob Loach](https://robloach.net)

... and licensed under:

- [zlib License](LICENSE)
