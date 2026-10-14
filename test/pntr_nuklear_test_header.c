/**
 * Compiles pntr_nuklear.h the way a consumer would: as a plain header, in a
 * second translation unit, without PNTR_NUKLEAR_IMPLEMENTATION. Every function
 * in the README's API list is called here, so a declaration that is missing from
 * the header fails to build rather than relying on the definition being earlier
 * in the same file.
 *
 * Nothing in here is run; it only has to compile and link.
 */
#define PNTR_ENABLE_DEFAULT_FONT
#include "pntr.h"

#include "pntr_nuklear.h"

void pntr_nuklear_test_header(pntr_image* image, pntr_font* font);

void pntr_nuklear_test_header(pntr_image* image, pntr_font* font) {
    struct nk_context* ctx = pntr_load_nuklear(font);

    pntr_nuklear_update(ctx, NULL);
    pntr_draw_nuklear(image, ctx);

    struct nk_rect rect = pntr_rectangle_to_nk_rect(PNTR_CLITERAL(pntr_rectangle) {0, 0, 1, 1});
    struct nk_color color = pntr_color_to_nk_color(PNTR_WHITE);
    struct nk_colorf colorf = pntr_color_to_nk_colorf(PNTR_WHITE);
    pntr_color fromColor = pntr_nk_color_to_color(color);
    pntr_color fromColorf = pntr_nk_colorf_to_color(colorf);

    struct nk_vec2i point;
    point.x = 0;
    point.y = 0;
    pntr_vector vector = pntr_nk_vec2i_to_vector(point);

    struct nk_image nkImage = pntr_image_nk(image);

    struct nk_vec2i points[3];
    points[0] = point;
    points[1] = point;
    points[2] = point;
    pntr_nuklear_draw_polygon_fill(image, points, 3, fromColor);

    // Keep every result used, so nothing above is optimized into nothing.
    (void)rect;
    (void)fromColorf;
    (void)vector;
    (void)nkImage;

    pntr_unload_nuklear(ctx);
}
