/**********************************************************************************************
*
*   pntr_nuklear - Nuklear immediate-mode graphical user interface for pntr.
*
*   Copyright 2026 Rob Loach (@RobLoach)
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
#define NK_BUTTON_TRIGGER_ON_RELEASE

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
PNTR_NUKLEAR_API pntr_color pntr_nk_color_to_color(struct nk_color color);
PNTR_NUKLEAR_API struct nk_color pntr_color_to_nk_color(pntr_color color);
PNTR_NUKLEAR_API pntr_vector pntr_nk_vec2i_to_vector(struct nk_vec2i vector);
PNTR_NUKLEAR_API pntr_color pntr_nk_colorf_to_color(struct nk_colorf color);

/**
 * Converts the given pntr_color to a Nuklear floating point color.
 *
 * @param color The color to convert.
 *
 * @return The equivalent struct nk_colorf.
 *
 * @see pntr_nk_colorf_to_color()
 */
PNTR_NUKLEAR_API struct nk_colorf pntr_color_to_nk_colorf(pntr_color color);

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

/**
 * Draw a filled polygon from an array of Nuklear points.
 *
 * @param dst The destination image to draw to.
 * @param pnts The points making up the polygon.
 * @param count How many points are in the given array. At most
 *   PNTR_NUKLEAR_MAX_POLYGON_POINTS points are drawn.
 * @param col The color to fill the polygon with.
 */
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
#ifndef NK_BUTTON_TRIGGER_ON_RELEASE
#define NK_BUTTON_TRIGGER_ON_RELEASE
#endif
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
    return (float)pntr_measure_text_ex(pntrFont, text, len).x;
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
        (void)nk_inv_sqrt;
    }
}

#ifdef PNTR_APP_API
/**
 * Nuklear callback; Paste the current clipboard.
 *
 * @internal
 */
static void
pntr_nuklear_clipboard_paste(nk_handle usr, struct nk_text_edit *edit)
{
    const char* text = pntr_app_clipboard((pntr_app*)usr.ptr);
    if (text != NULL) {
        nk_textedit_paste(edit, text, PNTR_STRLEN(text));
    }
}

/**
 * Nuklear callback; Copy the given text.
 *
 * @internal
 */
static void
pntr_nuklear_clipboard_copy(nk_handle usr, const char *text, int len)
{
    pntr_app_set_clipboard((pntr_app*)usr.ptr, text, len);
}

/**
 * Fallback translation from a pntr_app key press to a printable character, assuming a US QWERTY layout.
 *
 * Override `PNTR_NUKLEAR_KEY_CHAR(app, key, shift)` before including pntr_nuklear.h to
 * delegate to a layout-aware platform API (AZERTY, QWERTZ, Dvorak, etc.).
 * Returns 0 when the key does not produce a printable character.
 *
 * @internal
 */
static char pntr_nuklear_default_key_char(pntr_app_key key, bool shift) {
    if (key >= PNTR_APP_KEY_A && key <= PNTR_APP_KEY_Z) {
        return shift ? (char)key : (char)(key + 32);
    }
    if (key >= PNTR_APP_KEY_KP_0 && key <= PNTR_APP_KEY_KP_9) {
        return (char)(key - PNTR_APP_KEY_KP_0 + '0');
    }
    switch (key) {
        case PNTR_APP_KEY_SPACE:         return ' ';
        case PNTR_APP_KEY_APOSTROPHE:    return shift ? '"' : '\'';
        case PNTR_APP_KEY_COMMA:         return shift ? '<' : ',';
        case PNTR_APP_KEY_MINUS:         return shift ? '_' : '-';
        case PNTR_APP_KEY_PERIOD:        return shift ? '>' : '.';
        case PNTR_APP_KEY_SLASH:         return shift ? '?' : '/';
        case PNTR_APP_KEY_0:             return shift ? ')' : '0';
        case PNTR_APP_KEY_1:             return shift ? '!' : '1';
        case PNTR_APP_KEY_2:             return shift ? '@' : '2';
        case PNTR_APP_KEY_3:             return shift ? '#' : '3';
        case PNTR_APP_KEY_4:             return shift ? '$' : '4';
        case PNTR_APP_KEY_5:             return shift ? '%' : '5';
        case PNTR_APP_KEY_6:             return shift ? '^' : '6';
        case PNTR_APP_KEY_7:             return shift ? '&' : '7';
        case PNTR_APP_KEY_8:             return shift ? '*' : '8';
        case PNTR_APP_KEY_9:             return shift ? '(' : '9';
        case PNTR_APP_KEY_SEMICOLON:     return shift ? ':' : ';';
        case PNTR_APP_KEY_EQUAL:         return shift ? '+' : '=';
        case PNTR_APP_KEY_LEFT_BRACKET:  return shift ? '{' : '[';
        case PNTR_APP_KEY_BACKSLASH:     return shift ? '|' : '\\';
        case PNTR_APP_KEY_RIGHT_BRACKET: return shift ? '}' : ']';
        case PNTR_APP_KEY_GRAVE_ACCENT:  return shift ? '~' : '`';
        case PNTR_APP_KEY_KP_DECIMAL:    return '.';
        case PNTR_APP_KEY_KP_DIVIDE:     return '/';
        case PNTR_APP_KEY_KP_MULTIPLY:   return '*';
        case PNTR_APP_KEY_KP_SUBTRACT:   return '-';
        case PNTR_APP_KEY_KP_ADD:        return '+';
        default:                         return 0;
    }
}

#ifndef PNTR_NUKLEAR_KEY_CHAR
#define PNTR_NUKLEAR_KEY_CHAR(app, key, shift) pntr_nuklear_default_key_char((key), (shift))
#endif

#ifndef PNTR_NUKLEAR_DOUBLE_CLICK_TIME
#define PNTR_NUKLEAR_DOUBLE_CLICK_TIME 0.3f
#endif
#endif

PNTR_NUKLEAR_API struct nk_context* pntr_load_nuklear(pntr_font* font) {
    if (font == NULL) {
        return NULL;
    }

    // Build the memory.
    struct nk_context* ctx = (struct nk_context*)pntr_load_memory(sizeof(struct nk_context));
    struct nk_user_font* userFont = (struct nk_user_font*)pntr_load_memory(sizeof(struct nk_user_font));

    // Both allocations have to be checked before either is used. userFont is written to
    // below, and ctx is handed to nk_init(), so a failed allocation would otherwise be
    // dereferenced rather than reported through the documented NULL return.
    // pntr_unload_memory() ignores NULL, so whichever one succeeded is still released.
    if (ctx == NULL || userFont == NULL) {
        pntr_unload_memory(userFont);
        pntr_unload_memory(ctx);
        return NULL;
    }

    // Allocator
    struct nk_allocator allocator;
    allocator.alloc = pntr_nuklear_alloc;
    allocator.free = pntr_nuklear_free;

    // Set up the font, and find the tallest character.
    #define PNTR_LOAD_NUKLEAR_ALPHABET_START (33)
    #define PNTR_LOAD_NUKLEAR_ALPHABET_LEN (172 - PNTR_LOAD_NUKLEAR_ALPHABET_START)
    char theAlphabet[PNTR_LOAD_NUKLEAR_ALPHABET_LEN + 1];
    for (int i = 0; i < PNTR_LOAD_NUKLEAR_ALPHABET_LEN; i++) {
        theAlphabet[i] = (char)(i + PNTR_LOAD_NUKLEAR_ALPHABET_START);
    }
    theAlphabet[PNTR_LOAD_NUKLEAR_ALPHABET_LEN] = '\0';

    pntr_vector size = pntr_measure_text_ex(font, theAlphabet, PNTR_LOAD_NUKLEAR_ALPHABET_LEN);
    userFont->height = (float)size.y;
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
        // Set up the clipboard if needed.
        if (ctx->clip.userdata.ptr == NULL) {
            ctx->clip.userdata.ptr = app;
            ctx->clip.copy = pntr_nuklear_clipboard_copy;
            ctx->clip.paste = pntr_nuklear_clipboard_paste;
        }

        // Delta Time
        ctx->delta_time_seconds = pntr_app_delta_time(app);

        // Keyboard
        bool shift = pntr_app_key_down(app, PNTR_APP_KEY_LEFT_SHIFT) || pntr_app_key_down(app, PNTR_APP_KEY_RIGHT_SHIFT);
        bool control = pntr_app_key_down(app, PNTR_APP_KEY_LEFT_CONTROL) || pntr_app_key_down(app, PNTR_APP_KEY_RIGHT_CONTROL);
        nk_input_key(ctx, NK_KEY_ALT, pntr_app_key_down(app, PNTR_APP_KEY_LEFT_ALT) || pntr_app_key_down(app, PNTR_APP_KEY_RIGHT_ALT));
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
        //nk_input_key(ctx, NK_KEY_TEXT_REPLACE_MODE, pntr_app_key_down(app, PNTR_APP_KEY_ESCAPE));
        nk_input_key(ctx, NK_KEY_TEXT_RESET_MODE, pntr_app_key_down(app, PNTR_APP_KEY_ESCAPE));
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

        nk_input_key(ctx, NK_KEY_F1, pntr_app_key_down(app, PNTR_APP_KEY_F1));
        nk_input_key(ctx, NK_KEY_F2, pntr_app_key_down(app, PNTR_APP_KEY_F2));
        nk_input_key(ctx, NK_KEY_F3, pntr_app_key_down(app, PNTR_APP_KEY_F3));
        nk_input_key(ctx, NK_KEY_F4, pntr_app_key_down(app, PNTR_APP_KEY_F4));
        nk_input_key(ctx, NK_KEY_F5, pntr_app_key_down(app, PNTR_APP_KEY_F5));
        nk_input_key(ctx, NK_KEY_F6, pntr_app_key_down(app, PNTR_APP_KEY_F6));
        nk_input_key(ctx, NK_KEY_F7, pntr_app_key_down(app, PNTR_APP_KEY_F7));
        nk_input_key(ctx, NK_KEY_F8, pntr_app_key_down(app, PNTR_APP_KEY_F8));
        nk_input_key(ctx, NK_KEY_F9, pntr_app_key_down(app, PNTR_APP_KEY_F9));
        nk_input_key(ctx, NK_KEY_F10, pntr_app_key_down(app, PNTR_APP_KEY_F10));
        nk_input_key(ctx, NK_KEY_F11, pntr_app_key_down(app, PNTR_APP_KEY_F11));
        nk_input_key(ctx, NK_KEY_F12, pntr_app_key_down(app, PNTR_APP_KEY_F12));

        // Keyboard text input. Translation is layout-aware when
        // PNTR_NUKLEAR_KEY_CHAR is overridden; otherwise falls back to US QWERTY.
        if (!control) {
            for (int k = PNTR_APP_KEY_FIRST; k < PNTR_APP_KEY_LAST; k++) {
                if (pntr_app_key_pressed(app, (pntr_app_key)k)) {
                    char c = PNTR_NUKLEAR_KEY_CHAR(app, (pntr_app_key)k, shift);
                    if (c != 0) {
                        nk_input_char(ctx, c);
                    }
                }
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
        nk_input_button(ctx, NK_BUTTON_X1, mouseX, mouseY, pntr_app_mouse_button_down(app, PNTR_APP_MOUSE_BUTTON_X1));
        nk_input_button(ctx, NK_BUTTON_X2, mouseX, mouseY, pntr_app_mouse_button_down(app, PNTR_APP_MOUSE_BUTTON_X2));

        // Double Click
        {
            static float dclick_timer = 1.0f;
            dclick_timer += pntr_app_delta_time(app);
            if (pntr_app_mouse_button_pressed(app, PNTR_APP_MOUSE_BUTTON_LEFT)) {
                if (dclick_timer < PNTR_NUKLEAR_DOUBLE_CLICK_TIME) {
                    nk_input_button(ctx, NK_BUTTON_DOUBLE, mouseX, mouseY, nk_true);
                    dclick_timer = 1.0f;
                } else {
                    dclick_timer = 0.0f;
                }
            }
            if (!pntr_app_mouse_button_down(app, PNTR_APP_MOUSE_BUTTON_LEFT)) {
                nk_input_button(ctx, NK_BUTTON_DOUBLE, mouseX, mouseY, nk_false);
            }
        }
    #endif
}

/**
 * Maximum number of vertices for polygon and polyline commands.
 * Define before including pntr_nuklear.h to raise the limit, e.g.:
 *   #define PNTR_NUKLEAR_MAX_POLYGON_POINTS 128
 * Debug builds will assert when a polygon exceeds this limit.
 */
#ifndef PNTR_NUKLEAR_MAX_POLYGON_POINTS
#define PNTR_NUKLEAR_MAX_POLYGON_POINTS 64
#endif

/**
 * Draw a filled polygon using Nuklear values.
 *
 * @see nk_rawfb_fill_polygon()
 * @see https://github.com/Immediate-Mode-UI/Nuklear/blob/master/demo/rawfb/nuklear_rawfb.h
 */
PNTR_NUKLEAR_API void pntr_nuklear_draw_polygon_fill(pntr_image* dst, const struct nk_vec2i *pnts, int count, pntr_color col) {
    if (count == 0) return;
    NK_ASSERT(count <= PNTR_NUKLEAR_MAX_POLYGON_POINTS);
    if (count > PNTR_NUKLEAR_MAX_POLYGON_POINTS)
        count = PNTR_NUKLEAR_MAX_POLYGON_POINTS;
    pntr_vector points[PNTR_NUKLEAR_MAX_POLYGON_POINTS];
    for (int i = 0; i < count; i++) {
        points[i].x = pnts[i].x;
        points[i].y = pnts[i].y;
    }
    pntr_draw_polygon_fill(dst, points, count, col);
}

/**
 * Apply a Nuklear scissor to the image, restricted to the clip the caller had set.
 *
 * pntr_image_set_clip() only intersects against the image bounds, so handing it a
 * Nuklear scissor on its own would throw away whatever clip the caller had in place
 * and let the GUI paint outside of it. Intersecting here keeps the caller's viewport
 * authoritative. nk_panel_end() pushes nk_null_rect as its "no clipping" signal,
 * which intersects down to exactly the caller's clip.
 *
 * pntr's own rectangle intersection is internal to its implementation, and
 * pntr_nuklear.h may be compiled in a translation unit that only has pntr.h's
 * declarations, so the intersection is done here.
 *
 * @param dst The image to set the clip region of.
 * @param clip The caller's clip region, as captured by pntr_draw_nuklear().
 * @param x The scissor's X coordinate.
 * @param y The scissor's Y coordinate.
 * @param width The scissor's width.
 * @param height The scissor's height.
 *
 * @internal
 */
static void _pntr_nuklear_set_clip(pntr_image* dst, pntr_rectangle clip, int x, int y, int width, int height) {
    // Nuklear scissors are built from a short and an unsigned short, and the caller's
    // clip is already bounded by the image, so neither edge can overflow an int.
    int left = (x > clip.x) ? x : clip.x;
    int top = (y > clip.y) ? y : clip.y;
    int right = (x + width < clip.x + clip.width) ? x + width : clip.x + clip.width;
    int bottom = (y + height < clip.y + clip.height) ? y + height : clip.y + clip.height;

    if (right <= left || bottom <= top) {
        // The scissor misses the caller's clip entirely. pntr_image_set_clip()
        // ignores an empty rectangle, so clip to nothing directly.
        dst->clip.x = clip.x;
        dst->clip.y = clip.y;
        dst->clip.width = 0;
        dst->clip.height = 0;
        return;
    }

    pntr_image_set_clip(dst, left, top, right - left, bottom - top);
}

PNTR_NUKLEAR_API void pntr_draw_nuklear(pntr_image* dst, struct nk_context* ctx) {
    if (dst == NULL || ctx == NULL) {
        return;
    }

    // Finish processing events as we'll now draw the context.
    nk_input_end(ctx);

    // Nuklear emits scissor commands as it walks its windows, and leaves the last
    // one in place. Remember the caller's clip region so it can be handed back
    // once every command has been drawn.
    pntr_rectangle clip = pntr_image_get_clip(dst);

    // Iterate through each drawing command.
    const struct nk_command *cmd;

    nk_foreach(cmd, ctx) {
        switch (cmd->type) {
            case NK_COMMAND_NOP: {
                break;
            }

            case NK_COMMAND_SCISSOR: {
                const struct nk_command_scissor *s =(const struct nk_command_scissor*)cmd;
                _pntr_nuklear_set_clip(dst, clip, s->x, s->y, s->w, s->h);
            } break;

            case NK_COMMAND_LINE: {
                const struct nk_command_line *l = (const struct nk_command_line *)cmd;
                pntr_draw_line_thick(dst,
                    l->begin.x, l->begin.y,
                    l->end.x, l->end.y,
                    (int)l->line_thickness,
                    pntr_nk_color_to_color(l->color)
                );
            } break;

            case NK_COMMAND_CURVE: {
                const struct nk_command_curve *q = (const struct nk_command_curve *)cmd;
                #ifndef PNTR_NUKLEAR_CURVE_SEGMENTS
                    #define PNTR_NUKLEAR_CURVE_SEGMENTS 22
                #endif
                pntr_draw_line_curve_thick(dst,
                    pntr_nk_vec2i_to_vector(q->begin),
                    pntr_nk_vec2i_to_vector(q->ctrl[0]),
                    pntr_nk_vec2i_to_vector(q->ctrl[1]),
                    pntr_nk_vec2i_to_vector(q->end),
                    PNTR_NUKLEAR_CURVE_SEGMENTS,
                    (int)q->line_thickness,
                    pntr_nk_color_to_color(q->color)
                );
            } break;

            case NK_COMMAND_RECT: {
                const struct nk_command_rect *r = (const struct nk_command_rect *)cmd;
                pntr_color color = pntr_nk_color_to_color(r->color);
                int rounding = (int)r->rounding;
                pntr_draw_rectangle_thick_rounded(dst,
                    (int)r->x, (int)r->y,
                    (int)r->w, (int)r->h,
                    rounding, rounding, rounding, rounding,
                    (int)r->line_thickness,
                    color
                );
            } break;

            case NK_COMMAND_RECT_FILLED: {
                const struct nk_command_rect_filled *r = (const struct nk_command_rect_filled *)cmd;
                pntr_draw_rectangle_rounded_fill(dst, (int)r->x, (int)r->y, (int)r->w, (int)r->h, (int)r->rounding, pntr_nk_color_to_color(r->color));
            } break;

            case NK_COMMAND_RECT_MULTI_COLOR: {
                const struct nk_command_rect_multi_color* rectangle = (const struct nk_command_rect_multi_color *)cmd;
                pntr_draw_rectangle_gradient_rec(dst,
                    PNTR_CLITERAL(pntr_rectangle) {(int)rectangle->x, (int)rectangle->y, (int)rectangle->w, (int)rectangle->h},
                    pntr_nk_color_to_color(rectangle->left),
                    pntr_nk_color_to_color(rectangle->top),
                    pntr_nk_color_to_color(rectangle->bottom),
                    pntr_nk_color_to_color(rectangle->right)
                );
            } break;

            case NK_COMMAND_CIRCLE: {
                const struct nk_command_circle *c = (const struct nk_command_circle *)cmd;
                pntr_color color = pntr_nk_color_to_color(c->color);
                if (c->w == c->h) {
                    pntr_draw_circle_thick(dst, c->x + c->w / 2, c->y + c->h / 2, c->w / 2 + 1, (int)c->line_thickness, color);
                }
                else {
                    pntr_draw_ellipse_thick(dst, c->x + c->w / 2, c->y + c->h / 2, c->w / 2 + 1, c->h / 2 + 1, (int)c->line_thickness, color);
                }
            } break;

            case NK_COMMAND_CIRCLE_FILLED: {
                const struct nk_command_circle_filled *c = (const struct nk_command_circle_filled *)cmd;
                pntr_color color = pntr_nk_color_to_color(c->color);
                if (c->w == c->h) {
                    pntr_draw_circle_fill(dst, c->x + c->w / 2, c->y + c->h / 2, c->w / 2 + 1, color);
                }
                else {
                    pntr_draw_ellipse_fill(dst, c->x + c->w / 2, c->y + c->h / 2, c->w / 2 + 1, c->h / 2 + 1, color);
                }
            } break;

            case NK_COMMAND_ARC: {
                const struct nk_command_arc *a = (const struct nk_command_arc*)cmd;
                float startAngle = a->a[0] * 180.0f / PNTR_PI;
                float endAngle = a->a[1] * 180.0f / PNTR_PI;
                pntr_draw_arc_thick(dst, a->cx, a->cy, a->r, startAngle, endAngle, a->r * 3, (int)a->line_thickness, pntr_nk_color_to_color(a->color));
            } break;

            case NK_COMMAND_ARC_FILLED: {
                const struct nk_command_arc_filled *a = (const struct nk_command_arc_filled*)cmd;
                pntr_color color = pntr_nk_color_to_color(a->color);

                float startAngle = a->a[0] * 180.0f / PNTR_PI;
                float endAngle = a->a[1] * 180.0f / PNTR_PI;

                pntr_draw_arc_fill(dst, (int)a->cx, (int)a->cy, a->r, startAngle, endAngle, a->r * 2, color);
            } break;

            case NK_COMMAND_TRIANGLE: {
                const struct nk_command_triangle *t = (const struct nk_command_triangle*)cmd;
                pntr_color color = pntr_nk_color_to_color(t->color);
                pntr_draw_triangle_thick(dst, t->b.x, t->b.y, t->a.x, t->a.y, t->c.x, t->c.y, (int)t->line_thickness, color);
            } break;

            case NK_COMMAND_TRIANGLE_FILLED: {
                const struct nk_command_triangle_filled *t = (const struct nk_command_triangle_filled*)cmd;
                pntr_color color = pntr_nk_color_to_color(t->color);
                pntr_draw_triangle_fill(dst, t->b.x, t->b.y, t->a.x, t->a.y, t->c.x, t->c.y, color);
            } break;

            case NK_COMMAND_POLYGON: {
                const struct nk_command_polygon *p = (const struct nk_command_polygon*)cmd;
                pntr_color color = pntr_nk_color_to_color(p->color);
                NK_ASSERT(p->point_count <= PNTR_NUKLEAR_MAX_POLYGON_POINTS);
                int count = (p->point_count < PNTR_NUKLEAR_MAX_POLYGON_POINTS) ? p->point_count : PNTR_NUKLEAR_MAX_POLYGON_POINTS;
                pntr_vector points[PNTR_NUKLEAR_MAX_POLYGON_POINTS];
                for (int i = 0; i < count; i++) {
                    points[i].x = p->points[i].x;
                    points[i].y = p->points[i].y;
                }
                pntr_draw_polygon_thick(dst, points, count, (int)p->line_thickness, color);
            } break;

            case NK_COMMAND_POLYGON_FILLED: {
                const struct nk_command_polygon_filled *p = (const struct nk_command_polygon_filled*)cmd;
                pntr_color color = pntr_nk_color_to_color(p->color);
                NK_ASSERT(p->point_count <= PNTR_NUKLEAR_MAX_POLYGON_POINTS);
                int count = (p->point_count < PNTR_NUKLEAR_MAX_POLYGON_POINTS) ? p->point_count : PNTR_NUKLEAR_MAX_POLYGON_POINTS;
                pntr_vector points[PNTR_NUKLEAR_MAX_POLYGON_POINTS];
                for (int i = 0; i < count; i++) {
                    points[i].x = p->points[i].x;
                    points[i].y = p->points[i].y;
                }
                pntr_draw_polygon_fill(dst, points, count, color);
            } break;

            case NK_COMMAND_POLYLINE: {
                const struct nk_command_polyline *p = (const struct nk_command_polyline *)cmd;
                pntr_color color = pntr_nk_color_to_color(p->color);
                NK_ASSERT(p->point_count <= PNTR_NUKLEAR_MAX_POLYGON_POINTS);
                int count = (p->point_count < PNTR_NUKLEAR_MAX_POLYGON_POINTS) ? p->point_count : PNTR_NUKLEAR_MAX_POLYGON_POINTS;
                pntr_vector points[PNTR_NUKLEAR_MAX_POLYGON_POINTS];
                for (int i = 0; i < count; i++) {
                    points[i].x = p->points[i].x;
                    points[i].y = p->points[i].y;
                }
                pntr_draw_polyline_thick(dst, points, count, (int)p->line_thickness, color);
            } break;

            case NK_COMMAND_TEXT: {
                const struct nk_command_text *text = (const struct nk_command_text*)cmd;
                // Don't draw the text background by default.
                #ifdef PNTR_NUKLEAR_DRAW_TEXT_BACKGROUND
                if (text->background.a > 0) {
                    pntr_draw_rectangle_fill(dst, text->x, text->y, text->w, text->h, pntr_nk_color_to_color(text->background));
                }
                #endif
                pntr_font* font = (pntr_font*)text->font->userdata.ptr;
                pntr_draw_text(dst, font, (const char*)text->string, text->x, text->y, pntr_nk_color_to_color(text->foreground));
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

                pntr_color tint = pntr_nk_color_to_color(i->col);
                pntr_rectangle srcRect = {
                    .x = i->img.region[0],
                    .y = i->img.region[1],
                    .width = i->img.region[2],
                    .height = i->img.region[3]
                };

                // nk_image_ptr(), nk_image_handle() and nk_image_id() all zero the
                // region, so fall back to the whole source image when it is
                // degenerate. Without this the region would scale by zero.
                if (srcRect.width <= 0) {
                    srcRect.x = 0;
                    srcRect.width = image->width;
                }
                if (srcRect.height <= 0) {
                    srcRect.y = 0;
                    srcRect.height = image->height;
                }

                pntr_rectangle dstRect = {
                    .x = i->x,
                    .y = i->y,
                    .width = i->w,
                    .height = i->h
                };

                // There is nothing to resample at a 1:1 scale, so skip the
                // bilinear pass when the source and destination match.
                pntr_filter filter = (dstRect.width == srcRect.width && dstRect.height == srcRect.height)
                    ? PNTR_FILTER_NEARESTNEIGHBOR
                    : PNTR_FILTER_BILINEAR;

                pntr_draw_image_dest_rec(dst, image, srcRect, dstRect, filter, tint);
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

    // Give the caller their clip region back.
    pntr_image_set_clip_rec(dst, clip);

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

PNTR_NUKLEAR_API inline pntr_color pntr_nk_color_to_color(struct nk_color color) {
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

PNTR_NUKLEAR_API inline pntr_color pntr_nk_colorf_to_color(struct nk_colorf color) {
    return pntr_nk_color_to_color(nk_rgba_f(color.r, color.g, color.b, color.a));
}

PNTR_NUKLEAR_API struct nk_colorf pntr_color_to_nk_colorf(pntr_color color) {
    return nk_color_cf(pntr_color_to_nk_color(color));
}

PNTR_NUKLEAR_API inline pntr_vector pntr_nk_vec2i_to_vector(struct nk_vec2i vector) {
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
