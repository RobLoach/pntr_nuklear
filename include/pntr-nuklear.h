/**********************************************************************************************
*
*   pntr-nuklear - Nuklear immediate-mode graphical user interface for pntr.
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

#include "nuklear.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PNTR_NUKLEAR_API
    #define PNTR_NUKLEAR_API PNTR_API
#endif

PNTR_NUKLEAR_API struct nk_context* pntr_load_nuklear(pntr_font* font);
PNTR_NUKLEAR_API void pntr_unload_nuklear(struct nk_context* ctx);
PNTR_NUKLEAR_API void pntr_draw_nuklear(pntr_image* dst, struct nk_context* ctx);

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

#ifndef PNTR_MALLOC
    #include <stdlib.h>
    /**
     * Allocates the requested memory and returns a pointer to it.
     */
    #define PNTR_MALLOC(size) malloc((size_t)(size))
#endif  // PNTR_MALLOC

#ifndef PNTR_FREE
    #include <stdlib.h>
    /**
     * Deallocates the previously allocated memory.
     */
    #define PNTR_FREE(obj) free((void*)(obj))
#endif  // PNTR_FREE

float pntr_nuklear_text_width(nk_handle font, float height, const char* text, int len) {
    pntr_font* pntrFont = (pntr_font*)font.ptr;
    pntr_vector size = pntr_measure_text_ex(pntrFont, text);
    return size.x;
}

PNTR_NUKLEAR_API struct nk_context* pntr_load_nuklear(pntr_font* font) {
    if (font == NULL) {
        return NULL;
    }

    struct nk_context* ctx = (struct nk_context*)PNTR_MALLOC(sizeof(struct nk_context));
    struct nk_user_font* userFont = (struct nk_user_font*)PNTR_MALLOC(sizeof(struct nk_user_font));

    pntr_vector size = pntr_measure_text_ex(font, "Hello World!");
    userFont->height = size.y;
    userFont->width = pntr_nuklear_text_width;
    userFont->userdata.id = 1;
    userFont->userdata.ptr = font;

    // Create the nuklear environment.
    if (nk_init_default(ctx, userFont) == 0) {
        return NULL;
    }

    return ctx;
}

PNTR_NUKLEAR_API void pntr_unload_nuklear(struct nk_context* ctx) {
    // Skip unloading if it's not set.
    if (ctx == NULL) {
        return;
    }

    // Unload the font.
    struct nk_user_font* userFont = (struct nk_user_font*)ctx->style.font;
    if (userFont != NULL) {
        // Clear the user font.
        PNTR_FREE(userFont);
        ctx->style.font = NULL;
    }

    // Unload any custom user data.
    if (ctx->userdata.ptr != NULL) {
        PNTR_FREE(ctx->userdata.ptr);
        ctx->userdata.ptr = NULL;
    }

    // Unload the nuklear context.
    nk_free(ctx);
}

PNTR_NUKLEAR_API pntr_color pntr_color_from_nuklear(struct nk_color color) {
    return pntr_new_color(color.r, color.g, color.b, color.a);
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
                // TODO(RobLoach): Verify if NK_COMMAND_SCISSOR works.
                //const struct nk_command_scissor *s =(const struct nk_command_scissor*)cmd;
                //BeginScissorMode((int)(s->x), (int)(s->y), (int)(s->w), (int)(s->h));
            } break;

            case NK_COMMAND_LINE: {
                const struct nk_command_line *l = (const struct nk_command_line *)cmd;
                pntr_color color = pntr_color_from_nuklear(l->color);
                //pntr_vector startPos = {l->begin.x, l->begin.y};
                //pntr_vector endPos = {l->end.x, l->end.y};
                //DrawLineEx(startPos, endPos, l->line_thickness, color);
                pntr_draw_line(dst, l->begin.x, l->begin.y, l->end.x, l->end.y, color);
            } break;

            case NK_COMMAND_CURVE: {
                const struct nk_command_curve *q = (const struct nk_command_curve *)cmd;
                pntr_color color = pntr_color_from_nuklear(q->color);
                // pntr_vector start = {q->begin.x, q->begin.y};
                pntr_vector start = {q->begin.x, q->begin.y};
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
                const struct nk_command_rect *r = (const struct nk_command_rect *)cmd;
                pntr_color color = pntr_color_from_nuklear(r->color);
                //Rectangle rect = {r->x, r->y, r->w, r->h};
                //float roundness = r->rounding * RAYLIB_NUKLEAR_ROUNDING_SCALE / (rect.width + rect.height);
                // if (roundness > 0.0f) {
                //     DrawRectangleRoundedLines(rect, roundness, RAYLIB_NUKLEAR_DEFAULT_ARC_SEGMENTS, r->line_thickness, color);
                // }
                // else {
                    //DrawRectangleLinesEx(rect, r->line_thickness, color);
                    pntr_draw_rectangle(dst, r->x, r->y, r->w, r->h, r->line_thickness, color);
                //}
            } break;

            case NK_COMMAND_RECT_FILLED: {
                const struct nk_command_rect_filled *r = (const struct nk_command_rect_filled *)cmd;
                pntr_color color = pntr_color_from_nuklear(r->color);
                //Rectangle rect = {r->x, r->y, r->w, r->h};
                // float roundness = r->rounding * RAYLIB_NUKLEAR_ROUNDING_SCALE / (rect.width + rect.height);
                // if (roundness > 0.0f) {
                //     DrawRectangleRounded(rect, roundness, RAYLIB_NUKLEAR_DEFAULT_ARC_SEGMENTS, color);
                // }
                // else {
                    //DrawRectangleRec(rect, color);
                    pntr_draw_rectangle_fill(dst, r->x, r->y, r->w, r->h, color);
                //}
            } break;

            case NK_COMMAND_RECT_MULTI_COLOR: {
                // const struct nk_command_rect_multi_color* rectangle = (const struct nk_command_rect_multi_color *)cmd;
                // Rectangle position = {rectangle->x, rectangle->y, rectangle->w, rectangle->h};
                // pntr_color left = pntr_color_from_nuklear(rectangle->left);
                // pntr_color top = pntr_color_from_nuklear(rectangle->top);
                // pntr_color bottom = pntr_color_from_nuklear(rectangle->bottom);
                // pntr_color right = pntr_color_from_nuklear(rectangle->right);
                // DrawRectangleGradientEx(position, left, bottom, right, top);
            } break;

            case NK_COMMAND_CIRCLE: {
                const struct nk_command_circle *c = (const struct nk_command_circle *)cmd;
                pntr_color color = pntr_color_from_nuklear(c->color);
                pntr_draw_ellipse(dst, (int)(c->x + c->w / 2.0f), (int)(c->y + c->h / 2.0f), (int)(c->w / 2.0f), (int)(c->h / 2.0f), color);
            } break;

            case NK_COMMAND_CIRCLE_FILLED: {
                const struct nk_command_circle_filled *c = (const struct nk_command_circle_filled *)cmd;
                pntr_color color = pntr_color_from_nuklear(c->color);
                pntr_draw_ellipse_fill(dst, (int)(c->x + c->w / 2.0f), (int)(c->y + c->h / 2.0f), (int)(c->w / 2.0f), (int)(c->h / 2.0f), color);
                //DrawEllipse((int)(c->x + c->w / 2.0f), (int)(c->y + c->h / 2.0f), (int)(c->w / 2), (int)(c->h / 2), color);
            } break;

            case NK_COMMAND_ARC: {
                // const struct nk_command_arc *a = (const struct nk_command_arc*)cmd;
                // pntr_color color = pntr_color_from_nuklear(a->color);
                // pntr_vector center = {a->cx, a->cy};
                // DrawRingLines(center, 0, a->r, a->a[0] * RAD2DEG - 45, a->a[1] * RAD2DEG - 45, RAYLIB_NUKLEAR_DEFAULT_ARC_SEGMENTS, color);
            } break;

            case NK_COMMAND_ARC_FILLED: {
                // const struct nk_command_arc_filled *a = (const struct nk_command_arc_filled*)cmd;
                // pntr_color color = pntr_color_from_nuklear(a->color);
                // pntr_vector center = {a->cx, a->cy};
                // DrawRing(center, 0, a->r, a->a[0] * RAD2DEG - 45, a->a[1] * RAD2DEG - 45, RAYLIB_NUKLEAR_DEFAULT_ARC_SEGMENTS, color);
            } break;

            case NK_COMMAND_TRIANGLE: {
                const struct nk_command_triangle *t = (const struct nk_command_triangle*)cmd;
                pntr_color color = pntr_color_from_nuklear(t->color);
                // pntr_vector point1 = {t->b.x, t->b.y};
                // pntr_vector point2 = {t->a.x, t->a.y};
                // pntr_vector point3 = {t->c.x, t->c.y};
                // DrawTriangleLines(point1, point2, point3, color);

                pntr_draw_triangle(dst, t->b.x, t->b.y, t->a.x, t->a.y, t->c.x, t->c.y, color);
            } break;

            case NK_COMMAND_TRIANGLE_FILLED: {
                const struct nk_command_triangle_filled *t = (const struct nk_command_triangle_filled*)cmd;
                pntr_color color = pntr_color_from_nuklear(t->color);
                pntr_draw_triangle_fill(dst, t->b.x, t->b.y, t->a.x, t->a.y, t->c.x, t->c.y, color);
            } break;

            case NK_COMMAND_POLYGON: {
                // // TODO: Confirm Polygon
                // const struct nk_command_polygon *p = (const struct nk_command_polygon*)cmd;
                // pntr_color color = pntr_color_from_nuklear(p->color);
                // struct pntr_vector* points = (struct pntr_vector*)MemAlloc(p->point_count * (unsigned short)sizeof(pntr_vector));
                // unsigned short i;
                // for (i = 0; i < p->point_count; i++) {
                //     points[i].x = p->points[i].x;
                //     points[i].y = p->points[i].y;
                // }
                // DrawTriangleStrip(points, p->point_count, color);
                // MemFree(points);
            } break;

            case NK_COMMAND_POLYGON_FILLED: {
                // // TODO: Polygon filled expects counter clockwise order
                // const struct nk_command_polygon_filled *p = (const struct nk_command_polygon_filled*)cmd;
                // pntr_color color = pntr_color_from_nuklear(p->color);
                // struct pntr_vector* points = (struct pntr_vector*)MemAlloc(p->point_count * (unsigned short)sizeof(pntr_vector));
                // unsigned short i;
                // for (i = 0; i < p->point_count; i++) {
                //     points[i].x = p->points[i].x;
                //     points[i].y = p->points[i].y;
                // }
                // DrawTriangleFan(points, p->point_count, color);
                // MemFree(points);
            } break;

            case NK_COMMAND_POLYLINE: {
                // // TODO: Polygon expects counter clockwise order
                // const struct nk_command_polyline *p = (const struct nk_command_polyline *)cmd;
                // pntr_color color = pntr_color_from_nuklear(p->color);
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
                pntr_color color = pntr_color_from_nuklear(text->foreground);
                pntr_font* font = (pntr_font*)text->font->userdata.ptr;
                pntr_draw_text(dst, font, (const char*)text->string, text->x, text->y, color);

                // if (font != NULL) {
                //     pntr_vector position = {text->x, text->y};
                //     DrawTextEx(*font, (const char*)text->string, position, fontSize, fontSize / 10.0f, color);
                // }
                // else {
                //     DrawText((const char*)text->string, (int)(text->x), (int)(text->y), (int)fontSize, color);
                // }
            } break;

            case NK_COMMAND_IMAGE: {
                // const struct nk_command_image *i = (const struct nk_command_image *)cmd;
                // Texture texture = *(Texture*)i->img.handle.ptr;
                // Rectangle source = {0, 0, texture.width, texture.height};
                // Rectangle dest = {i->x, i->y, i->w, i->h};
                // pntr_vector origin = {0, 0};
                // pntr_color tint = pntr_color_from_nuklear(i->col);
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

#ifdef __cplusplus
}
#endif

#endif  // PNTR_NUKLEAR_IMPLEMENTATION_ONCE
#endif  // PNTR_NUKLEAR_IMPLEMENTATION
