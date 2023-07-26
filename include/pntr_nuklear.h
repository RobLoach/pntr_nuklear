/**********************************************************************************************
*
*   pntr_nuklear - Nuklear immediate-mode graphical user interface for pntr.
*
*   Copyright 2023 Rob Loach (@RobLoach)
*
*   DEPENDENCIES:
*       pntr https://github.com/robloach/pntr
*       nuklear https://github.com/Immediate-Mode-UI/Nuklear
*
*   LICENSE: zlib/libpng
*
*   pntr-nuklear is licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software:
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#ifndef PNTR_NUKLEAR_H_
#define PNTR_NUKLEAR_H_

#include <stdbool.h>

#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_COMMAND_USERDATA
// TODO: Replace NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_COMMAND_USERDATA
#define NK_INCLUDE_STANDARD_BOOL

#ifndef PNTR_NUKLEAR_NUKLEAR_H
#define PNTR_NUKLEAR_NUKLEAR_H "nuklear.h"
#endif
#include PNTR_NUKLEAR_NUKLEAR_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PNTR_NUKLEAR_API
    #define PNTR_NUKLEAR_API PNTR_API
#endif

PNTR_NUKLEAR_API struct nk_context* pntr_load_nuklear(pntr_font* font);
PNTR_NUKLEAR_API void pntr_unload_nuklear(struct nk_context* ctx);
PNTR_NUKLEAR_API void pntr_draw_nuklear(pntr_image* dst, struct nk_context* ctx);
PNTR_NUKLEAR_API struct nk_rect pntr_rectangle_to_nk_rect(pntr_rectangle rectangle);
PNTR_NUKLEAR_API pntr_color pntr_color_from_nk_color(struct nk_color color);
PNTR_NUKLEAR_API pntr_vector pntr_vector_from_nk_vec2i(struct nk_vec2i vector);

#ifdef __cplusplus
}
#endif

#endif  // PNTR_NUKLEAR_H_

#ifdef PNTR_NUKLEAR_IMPLEMENTATION
#ifndef PNTR_NUKLEAR_IMPLEMENTATION_ONCE
#define PNTR_NUKLEAR_IMPLEMENTATION_ONCE

#define NK_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear.h"

#ifdef __cplusplus
extern "C" {
#endif

float pntr_nuklear_text_width(nk_handle font, float height, const char* text, int len) {
    pntr_font* pntrFont = (pntr_font*)font.ptr;
    pntr_vector size = pntr_measure_text_ex(pntrFont, text);
    return size.x;
}

/**
 * Initialize the nuklear pntr context.
 *
 * @param font The font to use when rendering text.
 *
 * @see pntr_unload_nuklear()
 */
PNTR_NUKLEAR_API struct nk_context* pntr_load_nuklear(pntr_font* font) {
    if (font == NULL) {
        return NULL;
    }

    struct nk_context* ctx = (struct nk_context*)pntr_load_memory(sizeof(struct nk_context));
    struct nk_user_font* userFont = (struct nk_user_font*)pntr_load_memory(sizeof(struct nk_user_font));

    pntr_vector size = pntr_measure_text_ex(font, "Hello World!");
    userFont->height = size.y;
    userFont->width = pntr_nuklear_text_width;
    userFont->userdata.id = 1;
    userFont->userdata.ptr = font;

    // Create the nuklear environment.
    if (nk_init_default(ctx, userFont) == 0) {
        pntr_unload_memory(userFont);
        pntr_unload_memory(ctx);
        return NULL;
    }

    return ctx;
}

PNTR_NUKLEAR_API void pntr_unload_nuklear(struct nk_context* ctx) {
    // Skip unloading if it's not set.
    if (ctx == NULL) {
        return;
    }

    // Clear the user font.
    pntr_unload_memory((void*)ctx->style.font);
    ctx->style.font = NULL;

    // Unload any custom user data.
    pntr_unload_memory(ctx->userdata.ptr);
    ctx->userdata.ptr = NULL;

    // Unload the nuklear context.
    nk_free(ctx);
}

PNTR_NUKLEAR_API void pntr_draw_nuklear(pntr_image* dst, struct nk_context* ctx) {
    if (ctx == NULL || dst == NULL) {
        return;
    }

    const struct nk_command *cmd;

    nk_foreach(cmd, ctx) {
        switch (cmd->type) {
            case NK_COMMAND_NOP: {
                break;
            }

            case NK_COMMAND_SCISSOR: {
                // TODO: Add NK_COMMAND_SCISSOR clipping
                //const struct nk_command_scissor *s =(const struct nk_command_scissor*)cmd;
                //BeginScissorMode((int)(s->x), (int)(s->y), (int)(s->w), (int)(s->h));
            } break;

            case NK_COMMAND_LINE: {
                // TODO: Add NK_COMMAND_LINE line thickness
                const struct nk_command_line *l = (const struct nk_command_line *)cmd;
                pntr_color color = pntr_color_from_nk_color(l->color);
                pntr_draw_line_vec(dst,
                    pntr_vector_from_nk_vec2i(l->begin),
                    pntr_vector_from_nk_vec2i(l->end),
                    color
                );
            } break;

            case NK_COMMAND_CURVE: {
                // TODO: Add NK_COMMAND_CURVE
                const struct nk_command_curve *q = (const struct nk_command_curve *)cmd;
                pntr_color color = pntr_color_from_nk_color(q->color);
                // pntr_vector start = {q->begin.x, q->begin.y};
                pntr_vector start = PNTR_CLITERAL(pntr_vector) {q->begin.x, q->begin.y};
                // pntr_vector controlPoint1 = (pntr_vector){q->ctrl[0].x, q->ctrl[0].y};
                // pntr_vector controlPoint2 = (pntr_vector){q->ctrl[1].x, q->ctrl[1].y};
                // pntr_vector end = {q->end.x, q->end.y};
                pntr_vector end = {q->end.x, q->end.y};
                // TODO: Encorporate segmented control point bezier curve?
                // DrawLineBezier(start, controlPoint1, q->line_thickness, color);
                // DrawLineBezier(controlPoint1, controlPoint2, q->line_thickness, color);
                // DrawLineBezier(controlPoint2, end, q->line_thickness, color);
                // DrawLineBezier(start, end, q->line_thickness, color);
                //DrawLineBezier(start, end, q->line_thickness, color);
            } break;

            case NK_COMMAND_RECT: {
                // TODO: NK_COMMAND_RECT Add Rounding to the rectangle
                const struct nk_command_rect *r = (const struct nk_command_rect *)cmd;
                pntr_color color = pntr_color_from_nk_color(r->color);
                pntr_draw_rectangle(dst, (int)r->x, (int)r->y, (int)r->w, (int)r->h, (int)r->line_thickness, color);

                //Rectangle rect = {r->x, r->y, r->w, r->h};
                //float roundness = r->rounding * RAYLIB_NUKLEAR_ROUNDING_SCALE / (rect.width + rect.height);
                // if (roundness > 0.0f) {
                //     DrawRectangleRoundedLines(rect, roundness, RAYLIB_NUKLEAR_DEFAULT_ARC_SEGMENTS, r->line_thickness, color);
                // }
                // else {
                    //DrawRectangleLinesEx(rect, r->line_thickness, color);
                //}
            } break;

            case NK_COMMAND_RECT_FILLED: {
                // TODO: NK_COMMAND_RECT Add Rounding to the rectangle filled
                const struct nk_command_rect_filled *r = (const struct nk_command_rect_filled *)cmd;
                pntr_color color = pntr_color_from_nk_color(r->color);
                pntr_draw_rectangle_fill(dst, r->x, r->y, r->w, r->h, color);

                //Rectangle rect = {r->x, r->y, r->w, r->h};
                // float roundness = r->rounding * RAYLIB_NUKLEAR_ROUNDING_SCALE / (rect.width + rect.height);
                // if (roundness > 0.0f) {
                //     DrawRectangleRounded(rect, roundness, RAYLIB_NUKLEAR_DEFAULT_ARC_SEGMENTS, color);
                // }
                // else {
                    //DrawRectangleRec(rect, color);
                //}
            } break;

            case NK_COMMAND_RECT_MULTI_COLOR: {
                const struct nk_command_rect_multi_color* rectangle = (const struct nk_command_rect_multi_color *)cmd;
                pntr_rectangle rect = PNTR_CLITERAL(pntr_rectangle) {(int)rectangle->x, (int)rectangle->y, (int)rectangle->w, (int)rectangle->h};
                pntr_color left = pntr_color_from_nk_color(rectangle->left);
                pntr_color top = pntr_color_from_nk_color(rectangle->top);
                pntr_color bottom = pntr_color_from_nk_color(rectangle->bottom);
                pntr_color right = pntr_color_from_nk_color(rectangle->right);
                pntr_draw_rectangle_gradient_rec(dst, rect, left, top, bottom, right);
            } break;

            case NK_COMMAND_CIRCLE: {
                const struct nk_command_circle *c = (const struct nk_command_circle *)cmd;
                pntr_color color = pntr_color_from_nk_color(c->color);
                if (c->w == c->h) {
                    pntr_draw_circle(dst, (int)(c->x + c->w / 2.0f), (int)(c->y + c->h / 2.0f), (int)(c->w / 2.0f), color);
                }
                else {
                    pntr_draw_ellipse(dst, (int)(c->x + c->w / 2.0f), (int)(c->y + c->h / 2.0f), (int)(c->w / 2.0f), (int)(c->h / 2.0f), color);
                }
            } break;

            case NK_COMMAND_CIRCLE_FILLED: {
                const struct nk_command_circle_filled *c = (const struct nk_command_circle_filled *)cmd;
                pntr_color color = pntr_color_from_nk_color(c->color);
                if (c->w == c->h) {
                    pntr_draw_circle_fill(dst, (int)(c->x + c->w / 2.0f), (int)(c->y + c->h / 2.0f), (int)(c->w / 2.0f), color);
                }
                else {
                    pntr_draw_ellipse_fill(dst, (int)(c->x + c->w / 2.0f), (int)(c->y + c->h / 2.0f), (int)(c->w / 2.0f), (int)(c->h / 2.0f), color);
                }
            } break;

            case NK_COMMAND_ARC: {
                // TODO: Add NK_COMMAND_ARC
                // const struct nk_command_arc *a = (const struct nk_command_arc*)cmd;
                // pntr_color color = pntr_color_from_nk_color(a->color);
                // pntr_vector center = {a->cx, a->cy};
                // DrawRingLines(center, 0, a->r, a->a[0] * RAD2DEG - 45, a->a[1] * RAD2DEG - 45, RAYLIB_NUKLEAR_DEFAULT_ARC_SEGMENTS, color);
            } break;

            case NK_COMMAND_ARC_FILLED: {
                // TODO: Add NK_COMMAND_ARC_FILLED
                // const struct nk_command_arc_filled *a = (const struct nk_command_arc_filled*)cmd;
                // pntr_color color = pntr_color_from_nk_color(a->color);
                // pntr_vector center = {a->cx, a->cy};
                // DrawRing(center, 0, a->r, a->a[0] * RAD2DEG - 45, a->a[1] * RAD2DEG - 45, RAYLIB_NUKLEAR_DEFAULT_ARC_SEGMENTS, color);
            } break;

            case NK_COMMAND_TRIANGLE: {
                const struct nk_command_triangle *t = (const struct nk_command_triangle*)cmd;
                pntr_color color = pntr_color_from_nk_color(t->color);
                pntr_draw_triangle(dst, t->b.x, t->b.y, t->a.x, t->a.y, t->c.x, t->c.y, color);
            } break;

            case NK_COMMAND_TRIANGLE_FILLED: {
                const struct nk_command_triangle_filled *t = (const struct nk_command_triangle_filled*)cmd;
                pntr_color color = pntr_color_from_nk_color(t->color);
                pntr_draw_triangle_fill(dst, t->b.x, t->b.y, t->a.x, t->a.y, t->c.x, t->c.y, color);
            } break;

            case NK_COMMAND_POLYGON: {
                // TODO: Confirm Polygon
                // TODO: Add line thickness to NK_COMMAND_POLYGON
                const struct nk_command_polygon *p = (const struct nk_command_polygon*)cmd;
                pntr_color color = pntr_color_from_nk_color(p->color);
                pntr_vector* points = (pntr_vector*)pntr_load_memory((size_t)p->point_count * sizeof(pntr_vector));
                unsigned short i;
                for (i = 0; i < p->point_count; i++) {
                    points[i] = pntr_vector_from_nk_vec2i(p->points[i]);
                }
                pntr_draw_polygon(dst, points, (int)p->point_count, color);
                pntr_unload_memory(points);
            } break;

            case NK_COMMAND_POLYGON_FILLED: {
                // TODO: Confirm Polygon Fill
                const struct nk_command_polygon *p = (const struct nk_command_polygon*)cmd;
                pntr_color color = pntr_color_from_nk_color(p->color);
                pntr_vector* points = (pntr_vector*)pntr_load_memory((size_t)p->point_count * sizeof(pntr_vector));
                unsigned short i;
                for (i = 0; i < p->point_count; i++) {
                    points[i] = pntr_vector_from_nk_vec2i(p->points[i]);
                }
                pntr_draw_polygon_fill(dst, points, (int)p->point_count, color);
                pntr_unload_memory(points);
            } break;

            case NK_COMMAND_POLYLINE: {
                // // TODO: Polygon expects counter clockwise order
                // const struct nk_command_polyline *p = (const struct nk_command_polyline *)cmd;
                // pntr_color color = pntr_color_from_nk_color(p->color);
                // struct pntr_vector* points = (struct pntr_vector*)MemAlloc(p->point_count * (unsigned short)sizeof(pntr_vector));
                // unsigned short i;
                // for (i = 0; i < p->point_count; i++) {
                //     points[i].x = p->points[i].x;
                //     points[i].y = p->points[i].y;
                // }
                // DrawTriangleStrip(points, p->point_count, color);
                // MemFree(points);
            } break;

            case NK_COMMAND_TEXT: {
                const struct nk_command_text *text = (const struct nk_command_text*)cmd;
                pntr_color color = pntr_color_from_nk_color(text->foreground);
                pntr_font* font = (pntr_font*)text->font->userdata.ptr;
                pntr_draw_text(dst, font, (const char*)text->string, text->x, text->y, color);
            } break;

            case NK_COMMAND_IMAGE: {
                // const struct nk_command_image *i = (const struct nk_command_image *)cmd;
                // Texture texture = *(Texture*)i->img.handle.ptr;
                // Rectangle source = {0, 0, texture.width, texture.height};
                // Rectangle dest = {i->x, i->y, i->w, i->h};
                // pntr_vector origin = {0, 0};
                // pntr_color tint = pntr_color_from_nk_color(i->col);
                // DrawTexturePro(texture, source, dest, origin, 0, tint);
            } break;

            case NK_COMMAND_CUSTOM: {
                // TraceLog(LOG_WARNING, "NUKLEAR: Unverified custom callback implementation NK_COMMAND_CUSTOM");
                // const struct nk_command_custom *custom = (const struct nk_command_custom *)cmd;
                // custom->callback(NULL, (short)(custom->x), (short)(custom->y), (unsigned short)(custom->w), (unsigned short)(custom->h), custom->callback_data);
            } break;

            default: {
                //TraceLog(LOG_WARNING, "NUKLEAR: Missing implementation %i", cmd->type);
            } break;
        }
    }

    nk_clear(ctx);
}

PNTR_NUKLEAR_API inline struct nk_rect pntr_rectangle_to_nk_rect(pntr_rectangle rectangle) {
    return nk_rect(
        (float)rectangle.x,
        (float)rectangle.y,
        (float)rectangle.width,
        (float)rectangle.height
    );
}

PNTR_NUKLEAR_API inline pntr_color pntr_color_from_nk_color(struct nk_color color) {
    return pntr_new_color(color.r, color.g, color.b, color.a);
}

PNTR_NUKLEAR_API inline pntr_vector pntr_vector_from_nk_vec2i(struct nk_vec2i vector) {
    return PNTR_CLITERAL(pntr_vector) {
        (int)vector.x,
        (int)vector.y
    };
}

#ifdef __cplusplus
}
#endif

#endif  // PNTR_NUKLEAR_IMPLEMENTATION_ONCE
#endif  // PNTR_NUKLEAR_IMPLEMENTATION
