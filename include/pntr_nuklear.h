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

// Set up the nuklear configuration.
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_FIXED_TYPES
#define NK_VSNPRINTF
#define NK_INCLUDE_COMMAND_USERDATA
#define NK_INCLUDE_STANDARD_BOOL
#define NK_MEMSET PNTR_MEMSET
#define NK_MEMCPY PNTR_MEMCPY
#define NK_SIN PNTR_SINF
#define NK_COS PNTR_COSF

// Include Nuklear
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

#ifdef PNTR_APP_API
#define PNTR_APP_EVENT pntr_app_event
#else
#define PNTR_APP_EVENT void
#endif

/**
 * Initialize the nuklear pntr context.
 *
 * @param font The font to use when rendering text. Required.
 *
 * @return The new nuklear context, or NULL on failure.
 *
 * @see pntr_unload_nuklear()
 */
PNTR_NUKLEAR_API struct nk_context* pntr_load_nuklear(pntr_font* font);

/**
 * Unloads the given nuklear context.
 *
 * @param ctx The context to unload.
 *
 * @see pntr_load_nuklear()
 */
PNTR_NUKLEAR_API void pntr_unload_nuklear(struct nk_context* ctx);

/**
 * Process the given pntr_app event.
 *
 * The pntr_app integration is optional, and only used if pntr_app is included before pntr_nuklear.
 *
 * https://github.com/robloach/pntr_app
 *
 * @param ctx The nuklear context to handle the event.
 * @param event The event to process.
 */
PNTR_NUKLEAR_API void pntr_nuklear_event(struct nk_context* ctx, PNTR_APP_EVENT* event);

/**
 * Draws the given nuklear context on the destination image.
 *
 * @param dst The destination image to render to.
 * @param ctx The nuklear context to render.
 */
PNTR_NUKLEAR_API void pntr_draw_nuklear(pntr_image* dst, struct nk_context* ctx);
PNTR_NUKLEAR_API struct nk_rect pntr_rectangle_to_nk_rect(pntr_rectangle rectangle);
PNTR_NUKLEAR_API pntr_color pntr_color_from_nk_color(struct nk_color color);
PNTR_NUKLEAR_API struct nk_color pntr_color_to_nk_color(pntr_color color);
PNTR_NUKLEAR_API pntr_vector pntr_vector_from_nk_vec2i(struct nk_vec2i vector);
PNTR_NUKLEAR_API pntr_color pntr_color_from_nk_colorf(struct nk_colorf color);

/**
 * Creates a Nuklear reference to the given pntr_image for use in nk_image().
 *
 * @code
 * nk_image(ctx, pntr_image_nk(app->image));
 * @endcode
 *
 * @see nk_image()
 * @see pntr_load_image()
 */
PNTR_NUKLEAR_API struct nk_image pntr_image_nk(pntr_image* image);

#ifdef __cplusplus
}
#endif

#endif  // PNTR_NUKLEAR_H_

#ifdef PNTR_NUKLEAR_IMPLEMENTATION
#ifndef PNTR_NUKLEAR_IMPLEMENTATION_ONCE
#define PNTR_NUKLEAR_IMPLEMENTATION_ONCE

// When PNTR_ENABLE_FONT is used, it will use its own STB Truetype and STB Rect Pack.
#ifdef STB_RECT_PACK_IMPLEMENTATION
#undef STB_RECT_PACK_IMPLEMENTATION
#endif
#ifdef STB_TRUETYPE_IMPLEMENTATION
#undef STB_TRUETYPE_IMPLEMENTATION
#endif
#define STB_INCLUDE_STB_RECT_PACK_H

#define NK_IMPLEMENTATION
#include PNTR_NUKLEAR_NUKLEAR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Nuklear callback to calculate the width of the given text.
 *
 * @internal
 * @private
 */
float _pntr_nuklear_text_width(nk_handle font, float height, const char* text, int len) {
    pntr_font* pntrFont = (pntr_font*)font.ptr;

    // TODO: Get the text width without requiring a new buffer.
    char buffer[1024];
    pntr_memory_copy(buffer, (void*)text, (size_t)len);
    buffer[len] = '\0';

    return pntr_measure_text(pntrFont, buffer);
}

void* pntr_nuklear_alloc(nk_handle handle, void *old, nk_size size) {
    return pntr_load_memory((size_t)size);
}

void pntr_nuklear_free(nk_handle handle, void *old) {
    if (old != NULL) {
        pntr_unload_memory(old);
    }
}

PNTR_NUKLEAR_API struct nk_context* pntr_load_nuklear(pntr_font* font) {
    if (font == NULL) {
        return NULL;
    }

    // Build the memory.
    struct nk_context* ctx = (struct nk_context*)pntr_load_memory(sizeof(struct nk_context));
    struct nk_user_font* userFont = (struct nk_user_font*)pntr_load_memory(sizeof(struct nk_user_font));
    struct nk_allocator allocator;
    allocator.alloc = pntr_nuklear_alloc;
    allocator.free = pntr_nuklear_free;

    // Set up the font.
    pntr_vector size = pntr_measure_text_ex(font, "H", 1);
    userFont->height = size.y;
    userFont->width = _pntr_nuklear_text_width;
    userFont->userdata.id = 1;
    userFont->userdata.ptr = font;

    // Create the nuklear environment.
    if (nk_init(ctx, &allocator, userFont) == 0) {
        pntr_unload_memory(userFont);
        pntr_unload_memory(ctx);
        return NULL;
    }

    // Let Nuklear know that it may now process events.
    nk_input_begin(ctx);

    return ctx;
}

PNTR_NUKLEAR_API void pntr_unload_nuklear(struct nk_context* ctx) {
    // Skip unloading if it's not set.
    if (ctx == NULL) {
        return;
    }

    // Clear up anything remaining from the context.
    nk_input_end(ctx);
    nk_clear(ctx);

    // Clear the user font.
    pntr_unload_memory((void*)ctx->style.font);
    ctx->style.font = NULL;

    // Unload any custom user data.
    pntr_unload_memory(ctx->userdata.ptr);
    ctx->userdata.ptr = NULL;

    // Unload the nuklear context.
    nk_free(ctx);

    // Unload the memory
    pntr_unload_memory(ctx);
}

PNTR_NUKLEAR_API void pntr_nuklear_event(struct nk_context* ctx, PNTR_APP_EVENT* event) {
    if (ctx == NULL || event == NULL) {
        return;
    }

    // TODO: pntr_nuklear_event: Add support to route your own event system.
#ifdef PNTR_APP_API
    switch (event->type) {
        case PNTR_APP_EVENTTYPE_KEY_DOWN:
        case PNTR_APP_EVENTTYPE_KEY_UP: {
            nk_bool down = event->type == PNTR_APP_EVENTTYPE_KEY_DOWN;
            switch (event->key) {
                case PNTR_APP_KEY_LEFT_SHIFT:
                case PNTR_APP_KEY_RIGHT_SHIFT: nk_input_key(ctx, NK_KEY_SHIFT, down); break;
                case PNTR_APP_KEY_LEFT_CONTROL:
                case PNTR_APP_KEY_RIGHT_CONTROL: nk_input_key(ctx, NK_KEY_CTRL, down); break;
                case PNTR_APP_KEY_DELETE: nk_input_key(ctx, NK_KEY_DEL, down); break;
                case PNTR_APP_KEY_ENTER: nk_input_key(ctx, NK_KEY_ENTER, down); break;
                case PNTR_APP_KEY_TAB: nk_input_key(ctx, NK_KEY_TAB, down); break;
                case PNTR_APP_KEY_BACKSPACE: nk_input_key(ctx, NK_KEY_BACKSPACE, down); break;
                //case COPY: nk_input_key(ctx, NK_KEY_COPY, down); break;
                //case CUT: nk_input_key(ctx, NK_KEY_CUT, down); break;
                //case PASTE: nk_input_key(ctx, NK_KEY_PASTE, down); break;
                case PNTR_APP_KEY_UP: nk_input_key(ctx, NK_KEY_UP, down); break;
                case PNTR_APP_KEY_DOWN: nk_input_key(ctx, NK_KEY_DOWN, down); break;
                case PNTR_APP_KEY_LEFT: {
                    if (pntr_app_key_down(event->app, PNTR_APP_KEY_LEFT_CONTROL)) {
                        nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT, down);
                    }
                    else {
                        nk_input_key(ctx, NK_KEY_LEFT, down);
                    }
                } break;
                case PNTR_APP_KEY_RIGHT: {
                    if (pntr_app_key_down(event->app, PNTR_APP_KEY_LEFT_CONTROL)) {
                        nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT, down);
                    }
                    else {
                        nk_input_key(ctx, NK_KEY_RIGHT, down);
                    }
                } break;
                case PNTR_APP_KEY_INSERT: nk_input_key(ctx, NK_KEY_TEXT_INSERT_MODE, down); break;
                //case TEXT_REPLACE_MODE: nk_input_key(ctx, NK_KEY_TEXT_REPLACE_MODE, down); break;
                //case TEXT_RESET_MODE: nk_input_key(ctx, NK_KEY_TEXT_RESET_MODE, down); break;
                case PNTR_APP_KEY_HOME: nk_input_key(ctx, NK_KEY_TEXT_LINE_START, down); break;
                case PNTR_APP_KEY_END: nk_input_key(ctx, NK_KEY_TEXT_LINE_END, down); break;
                //case TEXT_START: nk_input_key(ctx, NK_KEY_TEXT_START, down); break;
                //case TEXT_END: nk_input_key(ctx, NK_KEY_TEXT_END, down); break;
                //case TEXT_UNDO: nk_input_key(ctx, NK_KEY_TEXT_UNDO, down); break;
                //case TEXT_REDO: nk_input_key(ctx, NK_KEY_TEXT_REDO, down); break;
                //case TEXT_SELECT_ALL: nk_input_key(ctx, NK_KEY_TEXT_SELECT_ALL, down); break;
                //case TEXT_WORD_LEFT: nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT, down); break;
                //case TEXT_WORD_RIGHT: nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT, down); break;
                //case SCROLL_START: nk_input_key(ctx, NK_KEY_SCROLL_START, down); break;
                //case SCROLL_END: nk_input_key(ctx, NK_KEY_SCROLL_END, down); break;
                case PNTR_APP_KEY_PAGE_DOWN: nk_input_key(ctx, NK_KEY_SCROLL_DOWN, down); break;
                case PNTR_APP_KEY_PAGE_UP: nk_input_key(ctx, NK_KEY_SCROLL_UP, down); break;
                default:
                    if (down) {
                        // Check for capital with SHIFT
                        if (event->key >= PNTR_APP_KEY_A && event->key <= PNTR_APP_KEY_Z) {
                            if (!pntr_app_key_down(event->app, PNTR_APP_KEY_LEFT_SHIFT) && !pntr_app_key_down(event->app, PNTR_APP_KEY_RIGHT_SHIFT)) {
                                event->key += 32;
                            }
                        }
                        nk_input_unicode(ctx, event->key);
                    }
                    break;
            }
        }
        break;

        case PNTR_APP_EVENTTYPE_MOUSE_MOVE: {
            // printf("Mouse Move: %d, %d\n" , event->mouseX, event->mouseY);
            nk_input_motion(ctx, event->mouseX, event->mouseY);
        }
        break;

        case PNTR_APP_EVENTTYPE_MOUSE_WHEEL: {
            struct nk_vec2 wheel = nk_vec2(0.0f, (event->mouseWheel > 0) ? 1.0f : -1.0f);
            nk_input_scroll(ctx, wheel);
        }
        break;

        case PNTR_APP_EVENTTYPE_MOUSE_BUTTON_DOWN:
        case PNTR_APP_EVENTTYPE_MOUSE_BUTTON_UP: {
            enum nk_buttons button = NK_BUTTON_MAX;
            switch (event->mouseButton) {
                case PNTR_APP_MOUSE_BUTTON_LEFT: button = NK_BUTTON_LEFT; break;
                case PNTR_APP_MOUSE_BUTTON_RIGHT: button = NK_BUTTON_RIGHT; break;
                case PNTR_APP_MOUSE_BUTTON_MIDDLE: button = NK_BUTTON_MIDDLE; break;
            }
            if (button != NK_BUTTON_MAX) {
                // printf("Mouse Event: %s %dx%d\n" , event->type == PNTR_APP_EVENTTYPE_MOUSE_BUTTON_DOWN ? "Down" : "Up  ", event->mouseX, event->mouseY);
                nk_input_button(ctx, button, event->mouseX, event->mouseY,
                    event->type == PNTR_APP_EVENTTYPE_MOUSE_BUTTON_DOWN ? nk_true : nk_false
                );
            }
        }
        break;
    }
#endif  // PNTR_APP_API
}

PNTR_NUKLEAR_API void pntr_draw_nuklear(pntr_image* dst, struct nk_context* ctx) {
    if (dst == NULL || ctx == NULL) {
        return;
    }

    // Finish processing events as we'll now draw the context.
    nk_input_end(ctx);
    bool scissorMode = false;
    pntr_image* destinationImage = dst;

    // Iterate through each drawing command.
    const struct nk_command *cmd;
    nk_foreach(cmd, ctx) {
        switch (cmd->type) {
            case NK_COMMAND_NOP: {
                break;
            }

            case NK_COMMAND_SCISSOR: {
                // TODO: Add NK_COMMAND_SCISSOR clipping
                const struct nk_command_scissor *s =(const struct nk_command_scissor*)cmd;
                //BeginScissorMode((int)(s->x), (int)(s->y), (int)(s->w), (int)(s->h));
                //printf("Scissor: %dx%d %dx%d\n", (int)(s->x), (int)(s->y), (int)(s->w), (int)(s->h));
                if (scissorMode) {
                    pntr_unload_image(destinationImage);
                }
                scissorMode = true;
                destinationImage = pntr_image_subimage(dst, 0, 0, (int)(s->x) + (int)(s->w), (int)(s->y) + (int)(s->h));
            } break;

            case NK_COMMAND_LINE: {
                // TODO: Add NK_COMMAND_LINE line thickness
                const struct nk_command_line *l = (const struct nk_command_line *)cmd;
                pntr_color color = pntr_color_from_nk_color(l->color);
                pntr_draw_line_vec(destinationImage,
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
                // Skip rounded rectangles for now
                if (true) {
                //if (r->rounding == 0) {
                    pntr_draw_rectangle(destinationImage, (int)r->x, (int)r->y, (int)r->w, (int)r->h, (int)r->line_thickness, color);
                }
                else {
                    // TODO: Add (int)r->line_thickness
                    int rounding = (int)r->rounding;
                    pntr_draw_rectangle_rounded(destinationImage, (int)r->x, (int)r->y, (int)r->w, (int)r->h, rounding, rounding, rounding, rounding, color);
                }
            } break;

            case NK_COMMAND_RECT_FILLED: {
                // TODO: NK_COMMAND_RECT Add Rounding to the rectangle filled
                const struct nk_command_rect_filled *r = (const struct nk_command_rect_filled *)cmd;
                pntr_color color = pntr_color_from_nk_color(r->color);
                // Skip rounded rectangles for now
                if (true) {
                //if (r->rounding == 0) {
                    pntr_draw_rectangle_fill(destinationImage, r->x, r->y, r->w, r->h, color);
                }
                else {
                    pntr_draw_rectangle_rounded_fill(destinationImage, (int)r->x, (int)r->y, (int)r->w, (int)r->h, (int)r->rounding, color);
                }
            } break;

            case NK_COMMAND_RECT_MULTI_COLOR: {
                const struct nk_command_rect_multi_color* rectangle = (const struct nk_command_rect_multi_color *)cmd;
                pntr_rectangle rect = PNTR_CLITERAL(pntr_rectangle) {(int)rectangle->x, (int)rectangle->y, (int)rectangle->w, (int)rectangle->h};
                pntr_color left = pntr_color_from_nk_color(rectangle->left);
                pntr_color top = pntr_color_from_nk_color(rectangle->top);
                pntr_color bottom = pntr_color_from_nk_color(rectangle->bottom);
                pntr_color right = pntr_color_from_nk_color(rectangle->right);
                pntr_draw_rectangle_gradient_rec(destinationImage, rect, left, top, bottom, right);
            } break;

            case NK_COMMAND_CIRCLE: {
                const struct nk_command_circle *c = (const struct nk_command_circle *)cmd;
                pntr_color color = pntr_color_from_nk_color(c->color);
                if (c->w == c->h) {
                    pntr_draw_circle(destinationImage, (int)(c->x + c->w / 2.0f), (int)(c->y + c->h / 2.0f), (int)(c->w / 2.0f), color);
                }
                else {
                    pntr_draw_ellipse(destinationImage, (int)(c->x + c->w / 2.0f), (int)(c->y + c->h / 2.0f), (int)(c->w / 2.0f), (int)(c->h / 2.0f), color);
                }
            } break;

            case NK_COMMAND_CIRCLE_FILLED: {
                const struct nk_command_circle_filled *c = (const struct nk_command_circle_filled *)cmd;
                pntr_color color = pntr_color_from_nk_color(c->color);
                if (c->w == c->h) {
                    pntr_draw_circle_fill(destinationImage, (int)(c->x + c->w / 2.0f), (int)(c->y + c->h / 2.0f), (int)(c->w / 2.0f), color);
                }
                else {
                    pntr_draw_ellipse_fill(destinationImage, (int)(c->x + c->w / 2.0f), (int)(c->y + c->h / 2.0f), (int)(c->w / 2.0f), (int)(c->h / 2.0f), color);
                }
            } break;

            case NK_COMMAND_ARC: {
                // TODO: Add line thickness to arc
                const struct nk_command_arc *a = (const struct nk_command_arc*)cmd;
                pntr_color color = pntr_color_from_nk_color(a->color);

                #ifndef PNTR_NUKLEAR_ARC_SEGMENTS
                    #define PNTR_NUKLEAR_ARC_SEGMENTS 20
                #endif
                float startAngle = a->a[0];
                float endAngle = a->a[1];

                pntr_draw_arc(destinationImage, (int)a->cx, (int)a->cy, a->r, startAngle, endAngle, PNTR_NUKLEAR_ARC_SEGMENTS, color);
            } break;

            case NK_COMMAND_ARC_FILLED: {
                // TODO: Add NK_COMMAND_ARC_FILLED
                const struct nk_command_arc_filled *a = (const struct nk_command_arc_filled*)cmd;
                pntr_color color = pntr_color_from_nk_color(a->color);

                #ifndef PNTR_NUKLEAR_ARC_FILL_SEGMENTS
                    #define PNTR_NUKLEAR_ARC_FILL_SEGMENTS 20
                #endif
                float startAngle = a->a[0];
                float endAngle = a->a[1];

                pntr_draw_arc_fill(destinationImage, (int)a->cx, (int)a->cy, a->r, startAngle, endAngle, PNTR_NUKLEAR_ARC_SEGMENTS, color);
            } break;

            case NK_COMMAND_TRIANGLE: {
                const struct nk_command_triangle *t = (const struct nk_command_triangle*)cmd;
                pntr_color color = pntr_color_from_nk_color(t->color);
                pntr_draw_triangle(destinationImage, t->b.x, t->b.y, t->a.x, t->a.y, t->c.x, t->c.y, color);
            } break;

            case NK_COMMAND_TRIANGLE_FILLED: {
                const struct nk_command_triangle_filled *t = (const struct nk_command_triangle_filled*)cmd;
                pntr_color color = pntr_color_from_nk_color(t->color);
                pntr_draw_triangle_fill(destinationImage, t->b.x, t->b.y, t->a.x, t->a.y, t->c.x, t->c.y, color);
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
                pntr_draw_polygon(destinationImage, points, (int)p->point_count, color);
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
                pntr_draw_polygon_fill(destinationImage, points, (int)p->point_count, color);
                pntr_unload_memory(points);
            } break;

            case NK_COMMAND_POLYLINE: {
                // TODO: Confirm Polyline works
                const struct nk_command_polyline *p = (const struct nk_command_polyline *)cmd;
                pntr_color color = pntr_color_from_nk_color(p->color);
                struct pntr_vector* points = (struct pntr_vector*)pntr_load_memory(sizeof(pntr_vector) * (size_t)p->point_count);

                for (unsigned short i = 0; i < p->point_count; i++) {
                    points[i].x = (int)p->points[i].x;
                    points[i].y = (int)p->points[i].y;
                }

                pntr_draw_polyline(destinationImage, points, (int)p->point_count, color);
                pntr_unload_memory(points);
            } break;

            case NK_COMMAND_TEXT: {
                const struct nk_command_text *text = (const struct nk_command_text*)cmd;
                pntr_color color = pntr_color_from_nk_color(text->foreground);
                pntr_font* font = (pntr_font*)text->font->userdata.ptr;
                pntr_draw_text(destinationImage, font, (const char*)text->string, text->x, text->y, color);
            } break;

            case NK_COMMAND_IMAGE: {
                const struct nk_command_image *i = (const struct nk_command_image *)cmd;
                if (i == NULL) {
                    break;
                }

                pntr_image* image = (pntr_image*)(i->img.handle.ptr);
                if (image == NULL) {
                    break;
                }

                pntr_color tint = pntr_color_from_nk_color(i->col);
                pntr_rectangle srcRect = {i->img.region[0], i->img.region[1], i->img.region[2], i->img.region[3]};
                pntr_rectangle destination = {(int)i->x, (int)i->y, (int)i->w, (int)i->h};

                pntr_draw_image_tint_rec(destinationImage, image, srcRect, destination.x, destination.y, tint);
            } break;

            case NK_COMMAND_CUSTOM: {
                const struct nk_command_custom *custom = (const struct nk_command_custom *)cmd;
                custom->callback(NULL, (short)(custom->x), (short)(custom->y), (unsigned short)(custom->w), (unsigned short)(custom->h), custom->callback_data);
            } break;

            default: {
                //TraceLog(LOG_WARNING, "NUKLEAR: Missing implementation %i", cmd->type);
            } break;
        }
    }

    if (scissorMode) {
        pntr_unload_image(destinationImage);
    }

    nk_clear(ctx);

    // Let Nuklear know that it may now process events.
    nk_input_begin(ctx);
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

PNTR_NUKLEAR_API struct nk_color pntr_color_to_nk_color(pntr_color color) {
    return nk_rgba(color.r, color.g, color.b, color.a);
}

PNTR_NUKLEAR_API inline pntr_color pntr_color_from_nk_colorf(struct nk_colorf color) {
    return pntr_new_color(
        (unsigned char)(color.r * 255.0f),
        (unsigned char)(color.g * 255.0f),
        (unsigned char)(color.b * 255.0f),
        (unsigned char)(color.a * 255.0f)
    );
}

PNTR_NUKLEAR_API struct nk_colorf pntr_color_to_nk_colorf(pntr_color color) {
    struct nk_colorf out;
    out.r = (float)color.r / 255.0f;
    out.g = (float)color.g / 255.0f;
    out.b = (float)color.b / 255.0f;
    out.a = (float)color.a / 255.0f;
    return out;
}

PNTR_NUKLEAR_API inline pntr_vector pntr_vector_from_nk_vec2i(struct nk_vec2i vector) {
    return PNTR_CLITERAL(pntr_vector) {
        vector.x,
        vector.y
    };
}

PNTR_NUKLEAR_API struct nk_image pntr_image_nk(pntr_image* image) {
	struct nk_image out;

    if (image != NULL) {
        out.handle.ptr = (void*)image;
        out.w = (nk_ushort)image->width;
        out.h = (nk_ushort)image->height;
        out.region[0] = 0;
        out.region[1] = 0;
        out.region[2] = out.w;
        out.region[3] = out.h;
    }
    else {
        out.handle.ptr = NULL;
        out.w = 0;
        out.h = 0;
        out.region[0] = 0;
        out.region[1] = 0;
        out.region[2] = 0;
        out.region[3] = 0;
    }

    return out;
}

#ifdef __cplusplus
}
#endif

#endif  // PNTR_NUKLEAR_IMPLEMENTATION_ONCE
#endif  // PNTR_NUKLEAR_IMPLEMENTATION
