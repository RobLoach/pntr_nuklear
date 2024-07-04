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
#define NK_SQRT PNTR_SQRTF

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
#define PNTR_APP_TYPE pntr_app
#else
#define PNTR_APP_TYPE void
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
 * Update the Nuklear state based on the given pntr_app.
 *
 * The pntr_app integration is optional, and only used if pntr_app is included before pntr_nuklear.
 *
 * @see https://github.com/robloach/pntr_app
 *
 * @param ctx The nuklear context to handle the event.
 * @param app The pntr_app structure to process.
 */
PNTR_NUKLEAR_API void pntr_nuklear_update(struct nk_context* ctx, PNTR_APP_TYPE* app);

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
PNTR_NUKLEAR_API void pntr_nuklear_draw_polygon_fill(pntr_image* dst, const struct nk_vec2i *pnts, int count, pntr_color col);

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

// Add pntr_app configurations if needed.
#ifdef PNTR_APP_API
#define NK_KEYSTATE_BASED_INPUT
#endif

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
static float _pntr_nuklear_text_width(nk_handle font, float height, const char* text, int len) {
    NK_UNUSED(height);
    pntr_font* pntrFont = (pntr_font*)font.ptr;
    return pntr_measure_text_ex(pntrFont, text, len).x;
}

static void* pntr_nuklear_alloc(nk_handle handle, void *old, nk_size size) {
    NK_UNUSED(handle);
    NK_UNUSED(old);
    return pntr_load_memory((size_t)size);
}

static void pntr_nuklear_free(nk_handle handle, void *old) {
    NK_UNUSED(handle);
    if (old != NULL) {
        pntr_unload_memory(old);

        // TODO: Fix nk_inv_sqrt() not being used?
        nk_inv_sqrt(0.0f);
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

    // Set up the font, and find the tallest character.
    #define PNTR_LOAD_NUKLEAR_ALPHABET_START (33)
    #define PNTR_LOAD_NUKLEAR_ALPHABET_LEN (172 - PNTR_LOAD_NUKLEAR_ALPHABET_START)
    char theAlphabet[PNTR_LOAD_NUKLEAR_ALPHABET_LEN];
    for (int i = 0; i <= PNTR_LOAD_NUKLEAR_ALPHABET_LEN; i++) {
        theAlphabet[i] = i + PNTR_LOAD_NUKLEAR_ALPHABET_START;
    }

    pntr_vector size = pntr_measure_text_ex(font, theAlphabet, PNTR_LOAD_NUKLEAR_ALPHABET_LEN);
    userFont->height = size.y;
    userFont->width = _pntr_nuklear_text_width;
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

    // Unload the nuklear context.
    nk_free(ctx);

    // Unload the memory
    pntr_unload_memory(ctx);
}

PNTR_NUKLEAR_API void pntr_nuklear_update(struct nk_context* ctx, PNTR_APP_TYPE* app) {
    if (ctx == NULL || app == NULL) {
        return;
    }

    #ifndef PNTR_APP_API
    return;
    #else
        // Delta Time
        ctx->delta_time_seconds = pntr_app_delta_time(app);

        // Keyboard
        bool shift = pntr_app_key_down(app, PNTR_APP_KEY_LEFT_SHIFT) || pntr_app_key_down(app, PNTR_APP_KEY_RIGHT_SHIFT);
        bool control = pntr_app_key_down(app, PNTR_APP_KEY_LEFT_CONTROL) || pntr_app_key_down(app, PNTR_APP_KEY_RIGHT_CONTROL);
        nk_input_key(ctx, NK_KEY_SHIFT, shift);
        nk_input_key(ctx, NK_KEY_CTRL, control);
        nk_input_key(ctx, NK_KEY_DEL, pntr_app_key_down(app, PNTR_APP_KEY_DELETE));
        nk_input_key(ctx, NK_KEY_ENTER, pntr_app_key_down(app, PNTR_APP_KEY_ENTER) || pntr_app_key_down(app, PNTR_APP_KEY_KP_ENTER));
        nk_input_key(ctx, NK_KEY_TAB, pntr_app_key_down(app, PNTR_APP_KEY_TAB));
        nk_input_key(ctx, NK_KEY_BACKSPACE, pntr_app_key_down(app, PNTR_APP_KEY_BACKSPACE));
        nk_input_key(ctx, NK_KEY_COPY, control && pntr_app_key_pressed(app, PNTR_APP_KEY_C));
        nk_input_key(ctx, NK_KEY_CUT, control && pntr_app_key_pressed(app, PNTR_APP_KEY_X));
        nk_input_key(ctx, NK_KEY_PASTE, control && pntr_app_key_pressed(app, PNTR_APP_KEY_V));
        nk_input_key(ctx, NK_KEY_UP, pntr_app_key_down(app, PNTR_APP_KEY_UP));
        nk_input_key(ctx, NK_KEY_DOWN, pntr_app_key_down(app, PNTR_APP_KEY_DOWN));
        nk_input_key(ctx, NK_KEY_LEFT, pntr_app_key_down(app, PNTR_APP_KEY_LEFT));
        nk_input_key(ctx, NK_KEY_RIGHT, pntr_app_key_down(app, PNTR_APP_KEY_RIGHT));
        //nk_input_key(ctx, NK_KEY_TEXT_INSERT_MODE, pntr_app_key_down(app, PNTR_APP_KEY_ENTER));
        //nk_input_key(ctx, NK_KEY_TEXT_REPLACE_MODE, pntr_app_key_down(app, PNTR_APP_KEY_ENTER));
        //nk_input_key(ctx, NK_KEY_TEXT_RESET_MODE, pntr_app_key_down(app, PNTR_APP_KEY_ENTER));
        nk_input_key(ctx, NK_KEY_TEXT_LINE_START, !control && pntr_app_key_down(app, PNTR_APP_KEY_HOME));
        nk_input_key(ctx, NK_KEY_TEXT_LINE_END, !control && pntr_app_key_down(app, PNTR_APP_KEY_END));
        nk_input_key(ctx, NK_KEY_TEXT_START, control && pntr_app_key_down(app, PNTR_APP_KEY_HOME));
        nk_input_key(ctx, NK_KEY_TEXT_END, control && pntr_app_key_down(app, PNTR_APP_KEY_END));
        nk_input_key(ctx, NK_KEY_TEXT_UNDO, control && pntr_app_key_down(app, PNTR_APP_KEY_Z));
        nk_input_key(ctx, NK_KEY_TEXT_REDO, control && pntr_app_key_down(app, PNTR_APP_KEY_Y));
        nk_input_key(ctx, NK_KEY_TEXT_SELECT_ALL, control && pntr_app_key_down(app, PNTR_APP_KEY_A));
        nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT, control && pntr_app_key_down(app, PNTR_APP_KEY_LEFT));
        nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT, control && pntr_app_key_down(app, PNTR_APP_KEY_RIGHT));
        nk_input_key(ctx, NK_KEY_SCROLL_START, control && pntr_app_key_down(app, PNTR_APP_KEY_PAGE_UP));
        nk_input_key(ctx, NK_KEY_SCROLL_END, control && pntr_app_key_down(app, PNTR_APP_KEY_PAGE_DOWN));
        nk_input_key(ctx, NK_KEY_SCROLL_DOWN, pntr_app_key_down(app, PNTR_APP_KEY_PAGE_DOWN));
        nk_input_key(ctx, NK_KEY_SCROLL_UP, pntr_app_key_down(app, PNTR_APP_KEY_PAGE_UP));

        // Keyboard text input
        if (!control) {
            for (int i = PNTR_APP_KEY_A; i <= PNTR_APP_KEY_Z; i++) {
                if (pntr_app_key_pressed(app, i)) {
                    if (!shift) {
                        nk_input_char(ctx, (char)(i + 32));
                    }
                    else {
                        nk_input_char(ctx, (char)i);
                    }
                }
            }
            if (pntr_app_key_pressed(app, PNTR_APP_KEY_SPACE)) {
                nk_input_char(ctx, ' ');
            }
            if (pntr_app_key_pressed(app, PNTR_APP_KEY_APOSTROPHE)) {
                if (shift) {
                    nk_input_char(ctx, '\"');
                }
                else {
                    nk_input_char(ctx, '\'');
                }
            }
            if (pntr_app_key_pressed(app, PNTR_APP_KEY_COMMA)) {
                if (shift) {
                    nk_input_char(ctx, '<');
                }
                else {
                    nk_input_char(ctx, ',');
                }
            }
            if (pntr_app_key_pressed(app, PNTR_APP_KEY_MINUS)) {
                if (shift) {
                    nk_input_char(ctx, '_');
                }
                else {
                    nk_input_char(ctx, '-');
                }
            }
            if (pntr_app_key_pressed(app, PNTR_APP_KEY_PERIOD)) {
                if (shift) {
                    nk_input_char(ctx, '>');
                }
                else {
                    nk_input_char(ctx, '.');
                }
            }
            if (pntr_app_key_pressed(app, PNTR_APP_KEY_SLASH)) {
                if (shift) {
                    nk_input_char(ctx, '?');
                }
                else {
                    nk_input_char(ctx, '/');
                }
            }
            if (pntr_app_key_pressed(app, PNTR_APP_KEY_0)) {
                if (shift) {
                    nk_input_char(ctx, ')');
                }
                else {
                    nk_input_char(ctx, '0');
                }
            }
            if (pntr_app_key_pressed(app, PNTR_APP_KEY_1)) {
                if (shift) {
                    nk_input_char(ctx, '!');
                }
                else {
                    nk_input_char(ctx, '1');
                }
            }
            if (pntr_app_key_pressed(app, PNTR_APP_KEY_2)) {
                if (shift) {
                    nk_input_char(ctx, '@');
                }
                else {
                    nk_input_char(ctx, '2');
                }
            }
            if (pntr_app_key_pressed(app, PNTR_APP_KEY_3)) {
                if (shift) {
                    nk_input_char(ctx, '#');
                }
                else {
                    nk_input_char(ctx, '3');
                }
            }
            if (pntr_app_key_pressed(app, PNTR_APP_KEY_4)) {
                if (shift) {
                    nk_input_char(ctx, '$');
                }
                else {
                    nk_input_char(ctx, '4');
                }
            }
            if (pntr_app_key_pressed(app, PNTR_APP_KEY_5)) {
                if (shift) {
                    nk_input_char(ctx, '%');
                }
                else {
                    nk_input_char(ctx, '5');
                }
            }
            if (pntr_app_key_pressed(app, PNTR_APP_KEY_6)) {
                if (shift) {
                    nk_input_char(ctx, '^');
                }
                else {
                    nk_input_char(ctx, '6');
                }
            }
            if (pntr_app_key_pressed(app, PNTR_APP_KEY_7)) {
                if (shift) {
                    nk_input_char(ctx, '&');
                }
                else {
                    nk_input_char(ctx, '7');
                }
            }
            if (pntr_app_key_pressed(app, PNTR_APP_KEY_8)) {
                if (shift) {
                    nk_input_char(ctx, '*');
                }
                else {
                    nk_input_char(ctx, '8');
                }
            }
            if (pntr_app_key_pressed(app, PNTR_APP_KEY_9)) {
                if (shift) {
                    nk_input_char(ctx, '(');
                }
                else {
                    nk_input_char(ctx, '9');
                }
            }
            if (pntr_app_key_pressed(app, PNTR_APP_KEY_SEMICOLON)) {
                if (shift) {
                    nk_input_char(ctx, ':');
                }
                else {
                    nk_input_char(ctx, ';');
                }
            }
            if (pntr_app_key_pressed(app, PNTR_APP_KEY_EQUAL)) {
                if (shift) {
                    nk_input_char(ctx, '+');
                }
                else {
                    nk_input_char(ctx, '=');
                }
            }
            if (pntr_app_key_pressed(app, PNTR_APP_KEY_LEFT_BRACKET)) {
                if (shift) {
                    nk_input_char(ctx, '{');
                }
                else {
                    nk_input_char(ctx, '[');
                }
            }
            if (pntr_app_key_pressed(app, PNTR_APP_KEY_BACKSLASH)) {
                if (shift) {
                    nk_input_char(ctx, '|');
                }
                else {
                    nk_input_char(ctx, '\\');
                }
            }
            if (pntr_app_key_pressed(app, PNTR_APP_KEY_RIGHT_BRACKET)) {
                if (shift) {
                    nk_input_char(ctx, '}');
                }
                else {
                    nk_input_char(ctx, ']');
                }
            }
            if (pntr_app_key_pressed(app, PNTR_APP_KEY_GRAVE_ACCENT)) {
                if (shift) {
                    nk_input_char(ctx, '~');
                }
                else {
                    nk_input_char(ctx, '`');
                }
            }

            for (int i = PNTR_APP_KEY_KP_0; i <= PNTR_APP_KEY_KP_9; i++) {
                if (pntr_app_key_pressed(app, i)) {
                    nk_input_char(ctx, (char)(i - PNTR_APP_KEY_KP_0 + '0'));
                }
            }
            if (pntr_app_key_pressed(app, PNTR_APP_KEY_KP_DECIMAL)) {
                nk_input_char(ctx, '.');
            }
            if (pntr_app_key_pressed(app, PNTR_APP_KEY_KP_DIVIDE)) {
                nk_input_char(ctx, '/');
            }
            if (pntr_app_key_pressed(app, PNTR_APP_KEY_KP_MULTIPLY)) {
                nk_input_char(ctx, '*');
            }
            if (pntr_app_key_pressed(app, PNTR_APP_KEY_KP_SUBTRACT)) {
                nk_input_char(ctx, '-');
            }
            if (pntr_app_key_pressed(app, PNTR_APP_KEY_KP_ADD)) {
                nk_input_char(ctx, '+');
            }
        }

        // Mouse
        int mouseX = pntr_app_mouse_x(app);
        int mouseY = pntr_app_mouse_y(app);
        nk_input_motion(ctx, mouseX, mouseY);
        nk_input_scroll(ctx, nk_vec2(0.0f, pntr_app_mouse_wheel(app)));
        nk_input_button(ctx, NK_BUTTON_LEFT, mouseX, mouseY, pntr_app_mouse_button_down(app, PNTR_APP_MOUSE_BUTTON_LEFT));
        nk_input_button(ctx, NK_BUTTON_MIDDLE, mouseX, mouseY, pntr_app_mouse_button_down(app, PNTR_APP_MOUSE_BUTTON_MIDDLE));
        nk_input_button(ctx, NK_BUTTON_RIGHT, mouseX, mouseY, pntr_app_mouse_button_down(app, PNTR_APP_MOUSE_BUTTON_RIGHT));
    #endif
}

/**
 * Draw a filled polygon using Nuklear values.
 *
 * @see nk_rawfb_fill_polygon()
 * @see https://github.com/Immediate-Mode-UI/Nuklear/blob/master/demo/rawfb/nuklear_rawfb.h
 *
 * @internal
 */
PNTR_NUKLEAR_API void pntr_nuklear_draw_polygon_fill(pntr_image* dst, const struct nk_vec2i *pnts, int count, pntr_color col) {
    int i = 0;
    #ifndef PNTR_NUKLEAR_POLYGON_FILL_MAX_POINTS
    #define PNTR_NUKLEAR_POLYGON_FILL_MAX_POINTS 64
    #endif
    int left = 10000, top = 10000, bottom = 0, right = 0;
    int nodes, nodeX[PNTR_NUKLEAR_POLYGON_FILL_MAX_POINTS], pixelX, pixelY, j, swap ;

    if (count == 0) return;
    if (count > PNTR_NUKLEAR_POLYGON_FILL_MAX_POINTS)
        count = PNTR_NUKLEAR_POLYGON_FILL_MAX_POINTS;

    /* Get polygon dimensions */
    for (i = 0; i < count; i++) {
        if (left > pnts[i].x)
            left = pnts[i].x;
        if (right < pnts[i].x)
            right = pnts[i].x;
        if (top > pnts[i].y)
            top = pnts[i].y;
        if (bottom < pnts[i].y)
            bottom = pnts[i].y;
    } bottom++; right++;

    /* Polygon scanline algorithm released under public-domain by Darel Rex Finley, 2007 */
    /*  Loop through the rows of the image. */
    for (pixelY = top; pixelY < bottom; pixelY ++) {
        nodes = 0; /*  Build a list of nodes. */
        j = count - 1;
        for (i = 0; i < count; i++) {
            if (((pnts[i].y < pixelY) && (pnts[j].y >= pixelY)) ||
                ((pnts[j].y < pixelY) && (pnts[i].y >= pixelY))) {
                nodeX[nodes++]= (int)((float)pnts[i].x
                     + ((float)pixelY - (float)pnts[i].y) / ((float)pnts[j].y - (float)pnts[i].y)
                     * ((float)pnts[j].x - (float)pnts[i].x));
            } j = i;
        }

        /*  Sort the nodes, via a simple “Bubble” sort. */
        i = 0;
        while (i < nodes - 1) {
            if (nodeX[i] > nodeX[i+1]) {
                swap = nodeX[i];
                nodeX[i] = nodeX[i+1];
                nodeX[i+1] = swap;
                if (i) i--;
            } else i++;
        }
        /*  Fill the pixels between node pairs. */
        for (i = 0; i < nodes; i += 2) {
            if (nodeX[i+0] >= right) break;
            if (nodeX[i+1] > left) {
                if (nodeX[i+0] < left) nodeX[i+0] = left ;
                if (nodeX[i+1] > right) nodeX[i+1] = right;
                for (pixelX = nodeX[i]; pixelX < nodeX[i + 1]; pixelX++)
                    //nk_rawfb_ctx_setpixel(rawfb, pixelX, pixelY, col);
                    pntr_draw_point(dst, pixelX, pixelY, col);

            }
        }
    }
    #undef PNTR_NUKLEAR_POLYGON_FILL_MAX_POINTS
}

/**
 * @internal
 */
PNTR_NUKLEAR_API void pntr_nuklear_draw_stroke_curve(pntr_image* rawfb,
    const struct nk_vec2i p1, const struct nk_vec2i p2,
    const struct nk_vec2i p3, const struct nk_vec2i p4,
    const unsigned int num_segments, const unsigned short line_thickness,
    pntr_color col)
{
    NK_UNUSED(line_thickness); // TODO: Use line thickness.
    unsigned int i_step, segments;
    float t_step;
    struct nk_vec2i last = p1;

    segments = PNTR_MAX(num_segments, 1);
    t_step = 1.0f/(float)segments;
    for (i_step = 1; i_step <= segments; ++i_step) {
        float t = t_step * (float)i_step;
        float u = 1.0f - t;
        float w1 = u*u*u;
        float w2 = 3*u*u*t;
        float w3 = 3*u*t*t;
        float w4 = t * t *t;
        float x = w1 * p1.x + w2 * p2.x + w3 * p3.x + w4 * p4.x;
        float y = w1 * p1.y + w2 * p2.y + w3 * p3.y + w4 * p4.y;
        // TODO: Add line_thickness to pntr_nuklear_draw_stroke_curve
        pntr_draw_line(rawfb, last.x, last.y, (int)x, (int)y, col);
        last.x = (short)x; last.y = (short)y;
    }
}

PNTR_NUKLEAR_API void pntr_draw_nuklear(pntr_image* dst, struct nk_context* ctx) {
    if (dst == NULL || ctx == NULL) {
        return;
    }

    // Finish processing events as we'll now draw the context.
    nk_input_end(ctx);

    // Iterate through each drawing command.
    const struct nk_command *cmd;

    nk_foreach(cmd, ctx) {
        switch (cmd->type) {
            case NK_COMMAND_NOP: {
                break;
            }

            case NK_COMMAND_SCISSOR: {
                const struct nk_command_scissor *s =(const struct nk_command_scissor*)cmd;
                pntr_image_set_clip(dst, s->x, s->y, s->w, s->h);
            } break;

            case NK_COMMAND_LINE: {
                // TODO: Add NK_COMMAND_LINE line thickness
                const struct nk_command_line *l = (const struct nk_command_line *)cmd;
                pntr_draw_line(dst, l->begin.x, l->begin.y, l->end.x, l->end.y, pntr_color_from_nk_color(l->color));
            } break;

            case NK_COMMAND_CURVE: {
                const struct nk_command_curve *q = (const struct nk_command_curve *)cmd;
                pntr_nuklear_draw_stroke_curve(dst, q->begin, q->ctrl[0], q->ctrl[1],
                    q->end, 22, q->line_thickness, pntr_color_from_nk_color(q->color));
            } break;

            case NK_COMMAND_RECT: {
                // TODO: NK_COMMAND_RECT Add Rounding to the rectangle
                const struct nk_command_rect *r = (const struct nk_command_rect *)cmd;
                pntr_color color = pntr_color_from_nk_color(r->color);
                // Skip rounded rectangles for now
                if (r->rounding == 0) {
                    pntr_draw_rectangle_thick(dst, (int)r->x, (int)r->y, (int)r->w, (int)r->h, (int)r->line_thickness, color);
                }
                else {
                    // TODO: Add (int)r->line_thickness
                    int rounding = (int)r->rounding;
                    pntr_draw_rectangle_rounded(dst, (int)r->x, (int)r->y, (int)r->w, (int)r->h, rounding, rounding, rounding, rounding, color);
                }
            } break;

            case NK_COMMAND_RECT_FILLED: {
                // TODO: NK_COMMAND_RECT Add Rounding to the rectangle filled
                const struct nk_command_rect_filled *r = (const struct nk_command_rect_filled *)cmd;
                pntr_color color = pntr_color_from_nk_color(r->color);
                if (r->rounding == 0) {
                    pntr_draw_rectangle_fill(dst, r->x, r->y, r->w, r->h, color);
                }
                else {
                    pntr_draw_rectangle_rounded_fill(dst, (int)r->x, (int)r->y, (int)r->w, (int)r->h, (int)r->rounding, color);
                }
            } break;

            case NK_COMMAND_RECT_MULTI_COLOR: {
                const struct nk_command_rect_multi_color* rectangle = (const struct nk_command_rect_multi_color *)cmd;
                pntr_draw_rectangle_gradient_rec(dst,
                    PNTR_CLITERAL(pntr_rectangle) {(int)rectangle->x, (int)rectangle->y, (int)rectangle->w, (int)rectangle->h},
                    pntr_color_from_nk_color(rectangle->left),
                    pntr_color_from_nk_color(rectangle->top),
                    pntr_color_from_nk_color(rectangle->bottom),
                    pntr_color_from_nk_color(rectangle->right)
                );
            } break;

            case NK_COMMAND_CIRCLE: {
                const struct nk_command_circle *c = (const struct nk_command_circle *)cmd;
                pntr_color color = pntr_color_from_nk_color(c->color);
                if (c->w == c->h) {
                    pntr_draw_circle(dst, c->x + c->w / 2, c->y + c->h / 2, c->w / 2 + 1, color);
                }
                else {
                    pntr_draw_ellipse(dst, c->x + c->w / 2, c->y + c->h / 2, c->w / 2 + 1, c->h / 2 + 1, color);
                }
            } break;

            case NK_COMMAND_CIRCLE_FILLED: {
                const struct nk_command_circle_filled *c = (const struct nk_command_circle_filled *)cmd;
                pntr_color color = pntr_color_from_nk_color(c->color);
                if (c->w == c->h) {
                    pntr_draw_circle_fill(dst, c->x + c->w / 2, c->y + c->h / 2, c->w / 2 + 1, color);
                }
                else {
                    pntr_draw_ellipse_fill(dst, c->x + c->w / 2, c->y + c->h / 2, c->w / 2 + 1, c->h / 2 + 1, color);
                }
            } break;

            case NK_COMMAND_ARC: {
                // TODO: Add line thickness to NK_COMMAND_ARC
                const struct nk_command_arc *a = (const struct nk_command_arc*)cmd;
                pntr_color color = pntr_color_from_nk_color(a->color);

                float startAngle = a->a[0] * 180.0f / PNTR_PI;
                float endAngle = a->a[1] * 180.0f / PNTR_PI;

                pntr_draw_arc(dst, (int)a->cx, (int)a->cy, a->r, startAngle, endAngle, a->r * 3, color);
                pntr_draw_line(dst, a->cx, a->cy, a->cx + PNTR_COSF(a->a[0]) * a->r, a->cy + PNTR_SINF(a->a[0]) * a->r, color);
                pntr_draw_line(dst, a->cx, a->cy, a->cx + PNTR_COSF(a->a[1]) * a->r, a->cy + PNTR_SINF(a->a[1]) * a->r, color);
            } break;

            case NK_COMMAND_ARC_FILLED: {
                const struct nk_command_arc_filled *a = (const struct nk_command_arc_filled*)cmd;
                pntr_color color = pntr_color_from_nk_color(a->color);

                float startAngle = a->a[0] * 180.0f / PNTR_PI;
                float endAngle = a->a[1] * 180.0f / PNTR_PI;

                pntr_draw_arc_fill(dst, (int)a->cx, (int)a->cy, a->r, startAngle, endAngle, a->r * 2, color);
            } break;

            case NK_COMMAND_TRIANGLE: {
                const struct nk_command_triangle *t = (const struct nk_command_triangle*)cmd;
                // TODO: Add Line Thickness to NK_COMMAND_TRIANGLE
                pntr_color color = pntr_color_from_nk_color(t->color);
                pntr_draw_triangle(dst, t->b.x, t->b.y, t->a.x, t->a.y, t->c.x, t->c.y, color);
            } break;

            case NK_COMMAND_TRIANGLE_FILLED: {
                const struct nk_command_triangle_filled *t = (const struct nk_command_triangle_filled*)cmd;
                pntr_color color = pntr_color_from_nk_color(t->color);
                pntr_draw_triangle_fill(dst, t->b.x, t->b.y, t->a.x, t->a.y, t->c.x, t->c.y, color);
            } break;

            case NK_COMMAND_POLYGON: {
                // TODO: Add line thickness to NK_COMMAND_POLYGON
                const struct nk_command_polygon *p = (const struct nk_command_polygon*)cmd;
                pntr_color color = pntr_color_from_nk_color(p->color);
                for (int i = 1; i < p->point_count; i++) {
                    pntr_draw_line(dst, p->points[i - 1].x, p->points[i - 1].y, p->points[i].x, p->points[i].y, color);
                }
                pntr_draw_line(dst, p->points[p->point_count - 1].x, p->points[p->point_count - 1].y, p->points[0].x, p->points[0].y, color);
            } break;

            case NK_COMMAND_POLYGON_FILLED: {
                const struct nk_command_polygon_filled *p = (const struct nk_command_polygon_filled*)cmd;
                pntr_nuklear_draw_polygon_fill(dst, p->points, p->point_count, pntr_color_from_nk_color(p->color));
            } break;

            case NK_COMMAND_POLYLINE: {
                // TODO: Add line thickness to NK_COMMAND_POLYLINE
                const struct nk_command_polyline *p = (const struct nk_command_polyline *)cmd;
                for (int i = 0; i < p->point_count - 1; i++) {
                    pntr_color color = pntr_color_from_nk_color(p->color);
                    pntr_draw_line(dst, p->points[i].x, p->points[i].y, p->points[i + 1].x, p->points[i + 1].y, color);
                }
            } break;

            case NK_COMMAND_TEXT: {
                const struct nk_command_text *text = (const struct nk_command_text*)cmd;
                pntr_color color = pntr_color_from_nk_color(text->foreground);
                pntr_font* font = (pntr_font*)text->font->userdata.ptr;
                pntr_draw_text(dst, font, (const char*)text->string, text->x, text->y, color);
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

                pntr_draw_image_tint_rec(dst, image, srcRect, destination.x, destination.y, tint);
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
    return nk_rgba(
        pntr_color_r(color),
        pntr_color_g(color),
        pntr_color_b(color),
        pntr_color_a(color)
    );
}

PNTR_NUKLEAR_API inline pntr_color pntr_color_from_nk_colorf(struct nk_colorf color) {
    return pntr_color_from_nk_color(nk_rgba_f(color.r, color.g, color.b, color.a));
}

PNTR_NUKLEAR_API struct nk_colorf pntr_color_to_nk_colorf(pntr_color color) {
    return nk_color_cf(pntr_color_to_nk_color(color));
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
        nk_zero(&out, sizeof(struct nk_image));
    }

    return out;
}

#ifdef __cplusplus
}
#endif

#endif  // PNTR_NUKLEAR_IMPLEMENTATION_ONCE
#endif  // PNTR_NUKLEAR_IMPLEMENTATION
