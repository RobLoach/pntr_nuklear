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

/**
 * Loads Nuklear's built-in ProggyClean font as a pntr_font.
 *
 * Requires PNTR_ENABLE_TTF to be defined.
 *
 * @param fontSize The desired font size in pixels.
 *
 * @return The loaded font, or NULL on failure.
 *
 * @see pntr_load_nuklear()
 * @see pntr_unload_font()
 */
PNTR_NUKLEAR_API pntr_font* pntr_load_nuklear_font(float fontSize);

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
#endif

PNTR_NUKLEAR_API struct nk_context* pntr_load_nuklear(pntr_font* font) {
    if (font == NULL) {
        return NULL;
    }

    // Build the memory.
    struct nk_context* ctx = (struct nk_context*)pntr_load_memory(sizeof(struct nk_context));
    struct nk_user_font* userFont = (struct nk_user_font*)pntr_load_memory(sizeof(struct nk_user_font));

    // Allocator
    struct nk_allocator allocator;
    allocator.alloc = pntr_nuklear_alloc;
    allocator.free = pntr_nuklear_free;

    // Set up the font, and find the tallest character.
    #define PNTR_LOAD_NUKLEAR_ALPHABET_START (33)
    #define PNTR_LOAD_NUKLEAR_ALPHABET_LEN (172 - PNTR_LOAD_NUKLEAR_ALPHABET_START)
    char theAlphabet[PNTR_LOAD_NUKLEAR_ALPHABET_LEN];
    for (int i = 0; i < PNTR_LOAD_NUKLEAR_ALPHABET_LEN; i++) {
        theAlphabet[i] = (char)(i + PNTR_LOAD_NUKLEAR_ALPHABET_START);
    }

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
        nk_input_button(ctx, NK_BUTTON_X1, mouseX, mouseY, pntr_app_mouse_button_down(app, PNTR_APP_MOUSE_BUTTON_X1));
        nk_input_button(ctx, NK_BUTTON_X2, mouseX, mouseY, pntr_app_mouse_button_down(app, PNTR_APP_MOUSE_BUTTON_X2));
    #endif
}

#ifndef PNTR_NUKLEAR_MAX_POLYGON_POINTS
#define PNTR_NUKLEAR_MAX_POLYGON_POINTS 64
#endif

/**
 * Draw a filled polygon using Nuklear values.
 *
 * @see nk_rawfb_fill_polygon()
 * @see https://github.com/Immediate-Mode-UI/Nuklear/blob/master/demo/rawfb/nuklear_rawfb.h
 *
 * @internal
 */
PNTR_NUKLEAR_API void pntr_nuklear_draw_polygon_fill(pntr_image* dst, const struct nk_vec2i *pnts, int count, pntr_color col) {
    if (count == 0) return;
    if (count > PNTR_NUKLEAR_MAX_POLYGON_POINTS)
        count = PNTR_NUKLEAR_MAX_POLYGON_POINTS;
    pntr_vector points[PNTR_NUKLEAR_MAX_POLYGON_POINTS];
    for (int i = 0; i < count; i++) {
        points[i].x = pnts[i].x;
        points[i].y = pnts[i].y;
    }
    pntr_draw_polygon_fill(dst, points, count, col);
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

                // TODO: Add tint to pntr_draw_image_scaled_rec()
                //pntr_color tint = pntr_nk_color_to_color(i->col);
                pntr_rectangle srcRect = {
                    .x = i->img.region[0],
                    .y = i->img.region[1],
                    .width = i->img.region[2],
                    .height = i->img.region[3]
                };
                pntr_draw_image_scaled_rec(dst, image, srcRect, i->x, i->y, (float)i->w / (float)srcRect.width, (float)i->h / (float)srcRect.height, 0, 0, PNTR_FILTER_BILINEAR);
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

// Self-contained ProggyClean font loader. Embeds the compressed TTF data from Nuklear
// (MIT license, https://github.com/bluescan/proggyfonts) with private decode/decompress
// helpers, so no NK_INCLUDE_DEFAULT_FONT or NK_INCLUDE_FONT_BAKING defines are required.

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverlength-strings"
static const char _pntr_nk_font_b85[] =
    "7])#######hV0qs'/###[),##/l:$#Q6>##5[n42>c-TH`->>#/e>11NNV=Bv(*:.F?uu#(gRU.o0XGH`$vhLG1hxt9?W`#,5LsCp#-i>.r$<$6pD>Lb';9Crc6tgXmKVeU2cD4Eo3R/"
    "2*>]b(MC;$jPfY.;h^`IWM9<Lh2TlS+f-s$o6Q<BWH`YiU.xfLq$N;$0iR/GX:U(jcW2p/W*q?-qmnUCI;jHSAiFWM.R*kU@C=GH?a9wp8f$e.-4^Qg1)Q-GL(lf(r/7GrRgwV%MS=C#"
    "`8ND>Qo#t'X#(v#Y9w0#1D$CIf;W'#pWUPXOuxXuU(H9M(1<q-UE31#^-V'8IRUo7Qf./L>=Ke$$'5F%)]0^#0X@U.a<r:QLtFsLcL6##lOj)#.Y5<-R&KgLwqJfLgN&;Q?gI^#DY2uL"
    "i@^rMl9t=cWq6##weg>$FBjVQTSDgEKnIS7EM9>ZY9w0#L;>>#Mx&4Mvt//L[MkA#W@lK.N'[0#7RL_&#w+F%HtG9M#XL`N&.,GM4Pg;-<nLENhvx>-VsM.M0rJfLH2eTM`*oJMHRC`N"
    "kfimM2J,W-jXS:)r0wK#@Fge$U>`w'N7G#$#fB#$E^$#:9:hk+eOe--6x)F7*E%?76%^GMHePW-Z5l'&GiF#$956:rS?dA#fiK:)Yr+`&#0j@'DbG&#^$PG.Ll+DNa<XCMKEV*N)LN/N"
    "*b=%Q6pia-Xg8I$<MR&,VdJe$<(7G;Ckl'&hF;;$<_=X(b.RS%%)###MPBuuE1V:v&cX&#2m#(&cV]`k9OhLMbn%s$G2,B$BfD3X*sp5#l,$R#]x_X1xKX%b5U*[r5iMfUo9U`N99hG)"
    "tm+/Us9pG)XPu`<0s-)WTt(gCRxIg(%6sfh=ktMKn3j)<6<b5Sk_/0(^]AaN#(p/L>&VZ>1i%h1S9u5o@YaaW$e+b<TWFn/Z:Oh(Cx2$lNEoN^e)#CFY@@I;BOQ*sRwZtZxRcU7uW6CX"
    "ow0i(?$Q[cjOd[P4d)]>ROPOpxTO7Stwi1::iB1q)C_=dV26J;2,]7op$]uQr@_V7$q^%lQwtuHY]=DX,n3L#0PHDO4f9>dC@O>HBuKPpP*E,N+b3L#lpR/MrTEH.IAQk.a>D[.e;mc."
    "x]Ip.PH^'/aqUO/$1WxLoW0[iLA<QT;5HKD+@qQ'NQ(3_PLhE48R.qAPSwQ0/WK?Z,[x?-J;jQTWA0X@KJ(_Y8N-:/M74:/-ZpKrUss?d#dZq]DAbkU*JqkL+nwX@@47`5>w=4h(9.`G"
    "CRUxHPeR`5Mjol(dUWxZa(>STrPkrJiWx`5U7F#.g*jrohGg`cg:lSTvEY/EV_7H4Q9[Z%cnv;JQYZ5q.l7Zeas:HOIZOB?G<Nald$qs]@]L<J7bR*>gv:[7MI2k).'2($5FNP&EQ(,)"
    "U]W]+fh18.vsai00);D3@4ku5P?DP8aJt+;qUM]=+b'8@;mViBKx0DE[-auGl8:PJ&Dj+M6OC]O^((##]`0i)drT;-7X`=-H3[igUnPG-NZlo.#k@h#=Ork$m>a>$-?Tm$UV(?#P6YY#"
    "'/###xe7q.73rI3*pP/$1>s9)W,JrM7SN]'/4C#v$U`0#V.[0>xQsH$fEmPMgY2u7Kh(G%siIfLSoS+MK2eTM$=5,M8p`A.;_R%#u[K#$x4AG8.kK/HSB==-'Ie/QTtG?-.*^N-4B/ZM"
    "_3YlQC7(p7q)&](`6_c)$/*JL(L-^(]$wIM`dPtOdGA,U3:w2M-0<q-]L_?^)1vw'.,MRsqVr.L;aN&#/EgJ)PBc[-f>+WomX2u7lqM2iEumMTcsF?-aT=Z-97UEnXglEn1K-bnEO`gu"
    "Ft(c%=;Am_Qs@jLooI&NX;]0#j4#F14;gl8-GQpgwhrq8'=l_f-b49'UOqkLu7-##oDY2L(te+Mch&gLYtJ,MEtJfLh'x'M=$CS-ZZ%P]8bZ>#S?YY#%Q&q'3^Fw&?D)UDNrocM3A76/"
    "/oL?#h7gl85[qW/NDOk%16ij;+:1a'iNIdb-ou8.P*w,v5#EI$TWS>Pot-R*H'-SEpA:g)f+O$%%`kA#G=8RMmG1&O`>to8bC]T&$,n.LoO>29sp3dt-52U%VM#q7'DHpg+#Z9%H[K<L"
    "%a2E-grWVM3@2=-k22tL]4$##6We'8UJCKE[d_=%wI;'6X-GsLX4j^SgJ$##R*w,vP3wK#iiW&#*h^D&R?jp7+/u&#(AP##XU8c$fSYW-J95_-Dp[g9wcO&#M-h1OcJlc-*vpw0xUX&#"
    "OQFKNX@QI'IoPp7nb,QU//MQ&ZDkKP)X<WSVL(68uVl&#c'[0#(s1X&xm$Y%B7*K:eDA323j998GXbA#pwMs-jgD$9QISB-A_(aN4xoFM^@C58D0+Q+q3n0#3U1InDjF682-SjMXJK)("
    "h$hxua_K]ul92%'BOU&#BRRh-slg8KDlr:%L71Ka:.A;%YULjDPmL<LYs8i#XwJOYaKPKc1h:'9Ke,g)b),78=I39B;xiY$bgGw-&.Zi9InXDuYa%G*f2Bq7mn9^#p1vv%#(Wi-;/Z5h"
    "o;#2:;%d&#x9v68C5g?ntX0X)pT`;%pB3q7mgGN)3%(P8nTd5L7GeA-GL@+%J3u2:(Yf>et`e;)f#Km8&+DC$I46>#Kr]]u-[=99tts1.qb#q72g1WJO81q+eN'03'eM>&1XxY-caEnO"
    "j%2n8)),?ILR5^.Ibn<-X-Mq7[a82Lq:F&#ce+S9wsCK*x`569E8ew'He]h:sI[2LM$[guka3ZRd6:t%IG:;$%YiJ:Nq=?eAw;/:nnDq0(CYcMpG)qLN4$##&J<j$UpK<Q4a1]MupW^-"
    "sj_$%[HK%'F####QRZJ::Y3EGl4'@%FkiAOg#p[##O`gukTfBHagL<LHw%q&OV0##F=6/:chIm0@eCP8X]:kFI%hl8hgO@RcBhS-@Qb$%+m=hPDLg*%K8ln(wcf3/'DW-$.lR?n[nCH-"
    "eXOONTJlh:.RYF%3'p6sq:UIMA945&^HFS87@$EP2iG<-lCO$%c`uKGD3rC$x0BL8aFn--`ke%#HMP'vh1/R&O_J9'um,.<tx[@%wsJk&bUT2`0uMv7gg#qp/ij.L56'hl;.s5CUrxjO"
    "M7-##.l+Au'A&O:-T72L]P`&=;ctp'XScX*rU.>-XTt,%OVU4)S1+R-#dg0/Nn?Ku1^0f$B*P:Rowwm-`0PKjYDDM'3]d39VZHEl4,.j']Pk-M.h^&:0FACm$maq-&sgw0t7/6(^xtk%"
    "LuH88Fj-ekm>GA#_>568x6(OFRl-IZp`&b,_P'$M<Jnq79VsJW/mWS*PUiq76;]/NM_>hLbxfc$mj`,O;&%W2m`Zh:/)Uetw:aJ%]K9h:TcF]u_-Sj9,VK3M.*'&0D[Ca]J9gp8,kAW]"
    "%(?A%R$f<->Zts'^kn=-^@c4%-pY6qI%J%1IGxfLU9CP8cbPlXv);C=b),<2mOvP8up,UVf3839acAWAW-W?#ao/^#%KYo8fRULNd2.>%m]UK:n%r$'sw]J;5pAoO_#2mO3n,'=H5(et"
    "Hg*`+RLgv>=4U8guD$I%D:W>-r5V*%j*W:Kvej.Lp$<M-SGZ':+Q_k+uvOSLiEo(<aD/K<CCc`'Lx>'?;++O'>()jLR-^u68PHm8ZFWe+ej8h:9r6L*0//c&iH&R8pRbA#Kjm%upV1g:"
    "a_#Ur7FuA#(tRh#.Y5K+@?3<-8m0$PEn;J:rh6?I6uG<-`wMU'ircp0LaE_OtlMb&1#6T.#FDKu#1Lw%u%+GM+X'e?YLfjM[VO0MbuFp7;>Q&#WIo)0@F%q7c#4XAXN-U&VB<HFF*qL("
    "$/V,;(kXZejWO`<[5?\?ewY(*9=%wDc;,u<'9t3W-(H1th3+G]ucQ]kLs7df($/*JL]@*t7Bu_G3_7mp7<iaQjO@.kLg;x3B0lqp7Hf,^Ze7-##@/c58Mo(3;knp0%)A7?-W+eI'o8)b<"
    "nKnw'Ho8C=Y>pqB>0ie&jhZ[?iLR@@_AvA-iQC(=ksRZRVp7`.=+NpBC%rh&3]R:8XDmE5^V8O(x<<aG/1N$#FX$0V5Y6x'aErI3I$7x%E`v<-BY,)%-?Psf*l?%C3.mM(=/M0:JxG'?"
    "7WhH%o'a<-80g0NBxoO(GH<dM]n.+%q@jH?f.UsJ2Ggs&4<-e47&Kl+f//9@`b+?.TeN_&B8Ss?v;^Trk;f#YvJkl&w$]>-+k?'(<S:68tq*WoDfZu';mM?8X[ma8W%*`-=;D.(nc7/;"
    ")g:T1=^J$&BRV(-lTmNB6xqB[@0*o.erM*<SWF]u2=st-*(6v>^](H.aREZSi,#1:[IXaZFOm<-ui#qUq2$##Ri;u75OK#(RtaW-K-F`S+cF]uN`-KMQ%rP/Xri.LRcB##=YL3BgM/3M"
    "D?@f&1'BW-)Ju<L25gl8uhVm1hL$##*8###'A3/LkKW+(^rWX?5W_8g)a(m&K8P>#bmmWCMkk&#TR`C,5d>g)F;t,4:@_l8G/5h4vUd%&%950:VXD'QdWoY-F$BtUwmfe$YqL'8(PWX("
    "P?^@Po3$##`MSs?DWBZ/S>+4%>fX,VWv/w'KD`LP5IbH;rTV>n3cEK8U#bX]l-/V+^lj3;vlMb&[5YQ8#pekX9JP3XUC72L,,?+Ni&co7ApnO*5NK,((W-i:$,kp'UDAO(G0Sq7MVjJs"
    "bIu)'Z,*[>br5fX^:FPAWr-m2KgL<LUN098kTF&#lvo58=/vjDo;.;)Ka*hLR#/k=rKbxuV`>Q_nN6'8uTG&#1T5g)uLv:873UpTLgH+#FgpH'_o1780Ph8KmxQJ8#H72L4@768@Tm&Q"
    "h4CB/5OvmA&,Q&QbUoi$a_%3M01H)4x7I^&KQVgtFnV+;[Pc>[m4k//,]1?#`VY[Jr*3&&slRfLiVZJ:]?=K3Sw=[$=uRB?3xk48@aeg<Z'<$#4H)6,>e0jT6'N#(q%.O=?2S]u*(m<-"
    "V8J'(1)G][68hW$5'q[GC&5j`TE?m'esFGNRM)j,ffZ?-qx8;->g4t*:CIP/[Qap7/9'#(1sao7w-.qNUdkJ)tCF&#B^;xGvn2r9FEPFFFcL@.iFNkTve$m%#QvQS8U@)2Z+3K:AKM5i"
    "sZ88+dKQ)W6>J%CL<KE>`.d*(B`-n8D9oK<Up]c$X$(,)M8Zt7/[rdkqTgl-0cuGMv'?>-XV1q['-5k'cAZ69e;D_?$ZPP&s^+7])$*$#@QYi9,5P&#9r+$%CE=68>K8r0=dSC%%(@p7"
    ".m7jilQ02'0-VWAg<a/''3u.=4L$Y)6k/K:_[3=&jvL<L0C/2'v:^;-DIBW,B4E68:kZ;%?8(Q8BH=kO65BW?xSG&#@uU,DS*,?.+(o(#1vCS8#CHF>TlGW'b)Tq7VT9q^*^$$.:&N@@"
    "$&)WHtPm*5_rO0&e%K&#-30j(E4#'Zb.o/(Tpm$>K'f@[PvFl,hfINTNU6u'0pao7%XUp9]5.>%h`8_=VYbxuel.NTSsJfLacFu3B'lQSu/m6-Oqem8T+oE--$0a/k]uj9EwsG>%veR*"
    "hv^BFpQj:K'#SJ,sB-'#](j.Lg92rTw-*n%@/;39rrJF,l#qV%OrtBeC6/,;qB3ebNW[?,Hqj2L.1NP&GjUR=1D8QaS3Up&@*9wP?+lo7b?@%'k4`p0Z$22%K3+iCZj?XJN4Nm&+YF]u"
    "@-W$U%VEQ/,,>>#)D<h#`)h0:<Q6909ua+&VU%n2:cG3FJ-%@Bj-DgLr`Hw&HAKjKjseK</xKT*)B,N9X3]krc12t'pgTV(Lv-tL[xg_%=M_q7a^x?7Ubd>#%8cY#YZ?=,`Wdxu/ae&#"
    "w6)R89tI#6@s'(6Bf7a&?S=^ZI_kS&ai`&=tE72L_D,;^R)7[$s<Eh#c&)q.MXI%#v9ROa5FZO%sF7q7Nwb&#ptUJ:aqJe$Sl68%.D###EC><?-aF&#RNQv>o8lKN%5/$(vdfq7+ebA#"
    "u1p]ovUKW&Y%q]'>$1@-[xfn$7ZTp7mM,G,Ko7a&Gu%G[RMxJs[0MM%wci.LFDK)(<c`Q8N)jEIF*+?P2a8g%)$q]o2aH8C&<SibC/q,(e:v;-b#6[$NtDZ84Je2KNvB#$P5?tQ3nt(0"
    "d=j.LQf./Ll33+(;q3L-w=8dX$#WF&uIJ@-bfI>%:_i2B5CsR8&9Z&#=mPEnm0f`<&c)QL5uJ#%u%lJj+D-r;BoF&#4DoS97h5g)E#o:&S4weDF,9^Hoe`h*L+_a*NrLW-1pG_&2UdB8"
    "6e%B/:=>)N4xeW.*wft-;$'58-ESqr<b?UI(_%@[P46>#U`'6AQ]m&6/`Z>#S?YY#Vc;r7U2&326d=w&H####?TZ`*4?&.MK?LP8Vxg>$[QXc%QJv92.(Db*B)gb*BM9dM*hJMAo*c&#"
    "b0v=Pjer]$gG&JXDf->'StvU7505l9$AFvgYRI^&<^b68?j#q9QX4SM'RO#&sL1IM.rJfLUAj221]d##DW=m83u5;'bYx,*Sl0hL(W;;$doB&O/TQ:(Z^xBdLjL<Lni;''X.`$#8+1GD"
    ":k$YUWsbn8ogh6rxZ2Z9]%nd+>V#*8U_72Lh+2Q8Cj0i:6hp&$C/:p(HK>T8Y[gHQ4`4)'$Ab(Nof%V'8hL&#<NEdtg(n'=S1A(Q1/I&4([%dM`,Iu'1:_hL>SfD07&6D<fp8dHM7/g+"
    "tlPN9J*rKaPct&?'uBCem^jn%9_K)<,C5K3s=5g&GmJb*[SYq7K;TRLGCsM-$$;S%:Y@r7AK0pprpL<Lrh,q7e/%KWK:50I^+m'vi`3?%Zp+<-d+$L-Sv:@.o19n$s0&39;kn;S%BSq*"
    "$3WoJSCLweV[aZ'MQIjO<7;X-X;&+dMLvu#^UsGEC9WEc[X(wI7#2.(F0jV*eZf<-Qv3J-c+J5AlrB#$p(H68LvEA'q3n0#m,[`*8Ft)FcYgEud]CWfm68,(aLA$@EFTgLXoBq/UPlp7"
    ":d[/;r_ix=:TF`S5H-b<LI&HY(K=h#)]Lk$K14lVfm:x$H<3^Ql<M`$OhapBnkup'D#L$Pb_`N*g]2e;X/Dtg,bsj&K#2[-:iYr'_wgH)NUIR8a1n#S?Yej'h8^58UbZd+^FKD*T@;6A"
    "7aQC[K8d-(v6GI$x:T<&'Gp5Uf>@M.*J:;$-rv29'M]8qMv-tLp,'886iaC=Hb*YJoKJ,(j%K=H`K.v9HggqBIiZu'QvBT.#=)0ukruV&.)3=(^1`o*Pj4<-<aN((^7('#Z0wK#5GX@7"
    "u][`*S^43933A4rl][`*O4CgLEl]v$1Q3AeF37dbXk,.)vj#x'd`;qgbQR%FW,2(?LO=s%Sc68%NP'##Aotl8x=BE#j1UD([3$M(]UI2LX3RpKN@;/#f'f/&_mt&F)XdF<9t4)Qa.*kT"
    "LwQ'(TTB9.xH'>#MJ+gLq9-##@HuZPN0]u:h7.T..G:;$/Usj(T7`Q8tT72LnYl<-qx8;-HV7Q-&Xdx%1a,hC=0u+HlsV>nuIQL-5<N?)NBS)QN*_I,?&)2'IM%L3I)X((e/dl2&8'<M"
    ":^#M*Q+[T.Xri.LYS3v%fF`68h;b-X[/En'CR.q7E)p'/kle2HM,u;^%OKC-N+Ll%F9CF<Nf'^#t2L,;27W:0O@6##U6W7:$rJfLWHj$#)woqBefIZ.PK<b*t7ed;p*_m;4ExK#h@&]>"
    "_>@kXQtMacfD.m-VAb8;IReM3$wf0''hra*so568'Ip&vRs849'MRYSp%:t:h5qSgwpEr$B>Q,;s(C#$)`svQuF$##-D,##,g68@2[T;.XSdN9Qe)rpt._K-#5wF)sP'##p#C0c%-Gb%"
    "hd+<-j'Ai*x&&HMkT]C'OSl##5RG[JXaHN;d'uA#x._U;.`PU@(Z3dt4r152@:v,'R.Sj'w#0<-;kPI)FfJ&#AYJ&#//)>-k=m=*XnK$>=)72L]0I%>.G690a:$##<,);?;72#?x9+d;"
    "^V'9;jY@;)br#q^YQpx:X#Te$Z^'=-=bGhLf:D6&bNwZ9-ZD#n^9HhLMr5G;']d&6'wYmTFmL<LD)F^%[tC'8;+9E#C$g%#5Y>q9wI>P(9mI[>kC-ekLC/R&CH+s'B;K-M6$EB%is00:"
    "+A4[7xks.LrNk0&E)wILYF@2L'0Nb$+pv<(2.768/FrY&h$^3i&@+G%JT'<-,v`3;_)I9M^AE]CN?Cl2AZg+%4iTpT3<n-&%H%b<FDj2M<hH=&Eh<2Len$b*aTX=-8QxN)k11IM1c^j%"
    "9s<L<NFSo)B?+<-(GxsF,^-Eh@$4dXhN$+#rxK8'je'D7k`e;)2pYwPA'_p9&@^18ml1^[@g4t*[JOa*[=Qp7(qJ_oOL^('7fB&Hq-:sf,sNj8xq^>$U4O]GKx'm9)b@p7YsvK3w^YR-"
    "CdQ*:Ir<($u&)#(&?L9Rg3H)4fiEp^iI9O8KnTj,]H?D*r7'M;PwZ9K0E^k&-cpI;.p/6_vwoFMV<->#%Xi.LxVnrU(4&8/P+:hLSKj$#U%]49t'I:rgMi'FL@a:0Y-uA[39',(vbma*"
    "hU%<-SRF`Tt:542R_VV$p@[p8DV[A,?1839FWdF<TddF<9Ah-6&9tWoDlh]&1SpGMq>Ti1O*H&#(AL8[_P%.M>v^-))qOT*F5Cq0`Ye%+$B6i:7@0IX<N+T+0MlMBPQ*Vj>SsD<U4JHY"
    "8kD2)2fU/M#$e.)T4,_=8hLim[&);?UkK'-x?'(:siIfL<$pFM`i<?%W(mGDHM%>iWP,##P`%/L<eXi:@Z9C.7o=@(pXdAO/NLQ8lPl+HPOQa8wD8=^GlPa8TKI1CjhsCTSLJM'/Wl>-"
    "S(qw%sf/@%#B6;/U7K]uZbi^Oc^2n<bhPmUkMw>%t<)'mEVE''n`WnJra$^TKvX5B>;_aSEK',(hwa0:i4G?.Bci.(X[?b*($,=-n<.Q%`(X=?+@Am*Js0&=3bh8K]mL<LoNs'6,'85`"
    "0?t/'_U59@]ddF<#LdF<eWdF<OuN/45rY<-L@&#+fm>69=Lb,OcZV/);TTm8VI;?%OtJ<(b4mq7M6:u?KRdF<gR@2L=FNU-<b[(9c/ML3m;Z[$oF3g)GAWqpARc=<ROu7cL5l;-[A]%/"
    "+fsd;l#SafT/f*W]0=O'$(Tb<[)*@e775R-:Yob%g*>l*:xP?Yb.5)%w_I?7uk5JC+FS(m#i'k.'a0i)9<7b'fs'59hq$*5Uhv##pi^8+hIEBF`nvo`;'l0.^S1<-wUK2/Coh58KKhLj"
    "M=SO*rfO`+qC`W-On.=AJ56>>i2@2LH6A:&5q`?9I3@@'04&p2/LVa*T-4<-i3;M9UvZd+N7>b*eIwg:CC)c<>nO&#<IGe;__.thjZl<%w(Wk2xmp4Q@I#I9,DF]u7-P=.-_:YJ]aS@V"
    "?6*C()dOp7:WL,b&3Rg/.cmM9&r^>$(>.Z-I&J(Q0Hd5Q%7Co-b`-c<N(6r@ip+AurK<m86QIth*#v;-OBqi+L7wDE-Ir8K['m+DDSLwK&/.?-V%U_%3:qKNu$_b*B-kp7NaD'QdWQPK"
    "Yq[@>P)hI;*_F]u`Rb[.j8_Q/<&>uu+VsH$sM9TA%?)(vmJ80),P7E>)tjD%2L=-t#fK[%`v=Q8<FfNkgg^oIbah*#8/Qt$F&:K*-(N/'+1vMB,u()-a.VUU*#[e%gAAO(S>WlA2);Sa"
    ">gXm8YB`1d@K#n]76-a$U,mF<fX]idqd)<3,]J7JmW4`6]uks=4-72L(jEk+:bJ0M^q-8Dm_Z?0olP1C9Sa&H[d&c$ooQUj]Exd*3ZM@-WGW2%s',B-_M%>%Ul:#/'xoFM9QX-$.QN'>"
    "[%$Z$uF6pA6Ki2O5:8w*vP1<-1`[G,)-m#>0`P&#eb#.3i)rtB61(o'$?X3B</R90;eZ]%Ncq;-Tl]#F>2Qft^ae_5tKL9MUe9b*sLEQ95C&`=G?@Mj=wh*'3E>=-<)Gt*Iw)'QG:`@I"
    "wOf7&]1i'S01B+Ev/Nac#9S;=;YQpg_6U`*kVY39xK,[/6Aj7:'1Bm-_1EYfa1+o&o4hp7KN_Q(OlIo@S%;jVdn0'1<Vc52=u`3^o-n1'g4v58Hj&6_t7$##?M)c<$bgQ_'SY((-xkA#"
    "Y(,p'H9rIVY-b,'%bCPF7.J<Up^,(dU1VY*5#WkTU>h19w,WQhLI)3S#f$2(eb,jr*b;3Vw]*7NH%$c4Vs,eD9>XW8?N]o+(*pgC%/72LV-u<Hp,3@e^9UB1J+ak9-TN/mhKPg+AJYd$"
    "MlvAF_jCK*.O-^(63adMT->W%iewS8W6m2rtCpo'RS1R84=@paTKt)>=%&1[)*vp'u+x,VrwN;&]kuO9JDbg=pO$J*.jVe;u'm0dr9l,<*wMK*Oe=g8lV_KEBFkO'oU]^=[-792#ok,)"
    "i]lR8qQ2oA8wcRCZ^7w/Njh;?.stX?Q1>S1q4Bn$)K1<-rGdO'$Wr.Lc.CG)$/*JL4tNR/,SVO3,aUw'DJN:)Ss;wGn9A32ijw%FL+Z0Fn.U9;reSq)bmI32U==5ALuG&#Vf1398/pVo"
    "1*c-(aY168o<`JsSbk-,1N;$>0:OUas(3:8Z972LSfF8eb=c-;>SPw7.6hn3m`9^Xkn(r.qS[0;T%&Qc=+STRxX'q1BNk3&*eu2;&8q$&x>Q#Q7^Tf+6<(d%ZVmj2bDi%.3L2n+4W'$P"
    "iDDG)g,r%+?,$@?uou5tSe2aN_AQU*<h`e-GI7)?OK2A.d7_c)?wQ5AS@DL3r#7fSkgl6-++D:'A,uq7SvlB$pcpH'q3n0#_%dY#xCpr-l<F0NR@-##FEV6NTF6##$l84N1w?AO>'IAO"
    "URQ##V^Fv-XFbGM7Fl(N<3DhLGF%q.1rC$#:T__&Pi68%0xi_&[qFJ(77j_&JWoF.V735&T,[R*:xFR*K5>>#`bW-?4Ne_&6Ne_&6Ne_&n`kr-#GJcM6X;uM6X;uM(.a..^2TkL%oR(#"
    ";u.T%fAr%4tJ8&><1=GHZ_+m9/#H1F^R#SC#*N=BA9(D?v[UiFY>>^8p,KKF.W]L29uLkLlu/+4T<XoIB&hx=T1PcDaB&;HH+-AFr?(m9HZV)FKS8JCw;SD=6[^/DZUL`EUDf]GGlG&>"
    "w$)F./^n3+rlo+DB;5sIYGNk+i1t-69Jg--0pao7Sm#K)pdHW&;LuDNH@H>#/X-TI(;P>#,Gc>#0Su>#4`1?#8lC?#<xU?#@.i?#D:%@#HF7@#LRI@#P_[@#Tkn@#Xw*A#]-=A#a9OA#"
    "d<F&#*;G##.GY##2Sl##6`($#:l:$#>xL$#B.`$#F:r$#JF.%#NR@%#R_R%#Vke%#Zww%#_-4&#3^Rh%Sflr-k'MS.o?.5/sWel/wpEM0%3'/1)K^f1-d>G21&v(35>V`39V7A4=onx4"
    "A1OY5EI0;6Ibgr6M$HS7Q<)58C5w,;WoA*#[%T*#`1g*#d=#+#hI5+#lUG+#pbY+#tnl+#x$),#&1;,#*=M,#.I`,#2Ur,#6b.-#;w[H#iQtA#m^0B#qjBB#uvTB##-hB#'9$C#+E6C#"
    "/QHC#3^ZC#7jmC#;v)D#?,<D#C8ND#GDaD#KPsD#O]/E#g1A5#KA*1#gC17#MGd;#8(02#L-d3#rWM4#Hga1#,<w0#T.j<#O#'2#CYN1#qa^:#_4m3#o@/=#eG8=#t8J5#`+78#4uI-#"
    "m3B2#SB[8#Q0@8#i[*9#iOn8#1Nm;#^sN9#qh<9#:=x-#P;K2#$%X9#bC+.#Rg;<#mN=.#MTF.#RZO.#2?)4#Y#(/#[)1/#b;L/#dAU/#0Sv;#lY$0#n`-0#sf60#(F24#wrH0#%/e0#"
    "TmD<#%JSMFove:CTBEXI:<eh2g)B,3h2^G3i;#d3jD>)4kMYD4lVu`4m`:&5niUA5@(A5BA1]PBB:xlBCC=2CDLXMCEUtiCf&0g2'tN?PGT4CPGT4CPGT4CPGT4CPGT4CPGT4CPGT4CP"
    "GT4CPGT4CPGT4CPGT4CPGT4CPGT4CP-qekC`.9kEg^+F$kwViFJTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5o,^<-28ZI'O?;xp"
    "O?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xp;7q-#lLYI:xvD=#";
#pragma GCC diagnostic pop

#define _PNTR_NK_IN2(i,x) (((unsigned int)(i)[x] << 8)  + (i)[(x)+1])
#define _PNTR_NK_IN3(i,x) (((unsigned int)(i)[x] << 16) + _PNTR_NK_IN2(i,(x)+1))
#define _PNTR_NK_IN4(i,x) (((unsigned int)(i)[x] << 24) + _PNTR_NK_IN3(i,(x)+1))

typedef struct { unsigned char *barrier, *barrier2, *barrier3, *barrier4, *dout; } _pntr_nk_dc;

static void _pntr_nk_match(_pntr_nk_dc *c, unsigned char *data, unsigned int len) {
    if (c->dout + len > c->barrier || data < c->barrier4) { c->dout += len; return; }
    while (len--) *c->dout++ = *data++;
}
static void _pntr_nk_lit(_pntr_nk_dc *c, unsigned char *data, unsigned int len) {
    if (c->dout + len > c->barrier || data < c->barrier2) { c->dout += len; return; }
    PNTR_MEMCPY(c->dout, data, (size_t)len);
    c->dout += len;
}
static unsigned char* _pntr_nk_token(_pntr_nk_dc *c, unsigned char *i) {
    if (*i >= 0x20) {
        if      (*i >= 0x80) _pntr_nk_match(c, c->dout - i[1] - 1, (unsigned int)i[0] - 0x80 + 1), i += 2;
        else if (*i >= 0x40) _pntr_nk_match(c, c->dout - (_PNTR_NK_IN2(i,0) - 0x4000 + 1), (unsigned int)i[2] + 1), i += 3;
        else                 _pntr_nk_lit(c, i + 1, (unsigned int)i[0] - 0x20 + 1), i += 1 + (i[0] - 0x20 + 1);
    } else {
        if      (*i >= 0x18) _pntr_nk_match(c, c->dout - (unsigned int)(_PNTR_NK_IN3(i,0) - 0x180000 + 1), (unsigned int)i[3] + 1), i += 4;
        else if (*i >= 0x10) _pntr_nk_match(c, c->dout - (unsigned int)(_PNTR_NK_IN3(i,0) - 0x100000 + 1), (unsigned int)_PNTR_NK_IN2(i,3) + 1), i += 5;
        else if (*i >= 0x08) _pntr_nk_lit(c, i + 2, (unsigned int)_PNTR_NK_IN2(i,0) - 0x0800 + 1), i += 2 + (_PNTR_NK_IN2(i,0) - 0x0800 + 1);
        else if (*i == 0x07) _pntr_nk_lit(c, i + 3, (unsigned int)_PNTR_NK_IN2(i,1) + 1), i += 3 + (_PNTR_NK_IN2(i,1) + 1);
        else if (*i == 0x06) _pntr_nk_match(c, c->dout - (unsigned int)(_PNTR_NK_IN3(i,1) + 1), (unsigned int)i[4] + 1), i += 5;
        else if (*i == 0x04) _pntr_nk_match(c, c->dout - (unsigned int)(_PNTR_NK_IN3(i,1) + 1), (unsigned int)_PNTR_NK_IN2(i,4) + 1), i += 6;
    }
    return i;
}
static unsigned int _pntr_nk_adler32(unsigned char *buf, unsigned int len) {
    unsigned long s1 = 1, s2 = 0, bl = len % 5552, i;
    while (len) {
        for (i = 0; i + 7 < bl; i += 8) {
            s1+=buf[0];s2+=s1; s1+=buf[1];s2+=s1; s1+=buf[2];s2+=s1; s1+=buf[3];s2+=s1;
            s1+=buf[4];s2+=s1; s1+=buf[5];s2+=s1; s1+=buf[6];s2+=s1; s1+=buf[7];s2+=s1;
            buf += 8;
        }
        for (; i < bl; ++i) { s1 += *buf++; s2 += s1; }
        s1 %= 65521; s2 %= 65521;
        len -= (unsigned int)bl;
        bl = 5552;
    }
    return (unsigned int)(s2 << 16) + (unsigned int)s1;
}
static unsigned int _pntr_nk_decompress_len(const unsigned char *i) {
    return ((unsigned int)i[8]<<24)|((unsigned int)i[9]<<16)|((unsigned int)i[10]<<8)|i[11];
}
static unsigned int _pntr_nk_decompress(unsigned char *out, unsigned char *i, unsigned int len) {
    _pntr_nk_dc c;
    unsigned int olen;
    if (_PNTR_NK_IN4(i,0) != 0x57bC0000 || _PNTR_NK_IN4(i,4) != 0) return 0;
    olen = _pntr_nk_decompress_len(i);
    c.barrier2 = i; c.barrier3 = i + len;
    c.barrier = out + olen; c.barrier4 = out; c.dout = out;
    i += 16;
    for (;;) {
        unsigned char *old = i;
        i = _pntr_nk_token(&c, i);
        if (i == old) {
            if (*i == 0x05 && i[1] == 0xfa && c.dout == out + olen &&
                _pntr_nk_adler32(out, olen) == (unsigned int)_PNTR_NK_IN4(i,2))
                return olen;
            return 0;
        }
        if (c.dout > out + olen) return 0;
    }
}
static void _pntr_nk_decode85(unsigned char *dst, const unsigned char *src) {
    while (*src) {
        unsigned int c0 = (unsigned int)(*src >= '\\' ? *src-36 : *src-35); src++;
        unsigned int c1 = (unsigned int)(*src >= '\\' ? *src-36 : *src-35); src++;
        unsigned int c2 = (unsigned int)(*src >= '\\' ? *src-36 : *src-35); src++;
        unsigned int c3 = (unsigned int)(*src >= '\\' ? *src-36 : *src-35); src++;
        unsigned int c4 = (unsigned int)(*src >= '\\' ? *src-36 : *src-35); src++;
        unsigned int tmp = c0 + 85*(c1 + 85*(c2 + 85*(c3 + 85*c4)));
        dst[0]=(unsigned char)(tmp&0xFF); dst[1]=(unsigned char)((tmp>>8)&0xFF);
        dst[2]=(unsigned char)((tmp>>16)&0xFF); dst[3]=(unsigned char)((tmp>>24)&0xFF);
        dst += 4;
    }
}

PNTR_NUKLEAR_API pntr_font* pntr_load_nuklear_font(float fontSize) {
    int compressed_size = (((int)nk_strlen(_pntr_nk_font_b85) + 4) / 5) * 4;
    unsigned char* compressed = (unsigned char*)pntr_load_memory((size_t)compressed_size);
    if (compressed == NULL) return NULL;
    _pntr_nk_decode85(compressed, (const unsigned char*)_pntr_nk_font_b85);

    unsigned int decompressed_size = _pntr_nk_decompress_len(compressed);
    unsigned char* ttf = (unsigned char*)pntr_load_memory((size_t)decompressed_size);
    if (ttf == NULL) { pntr_unload_memory(compressed); return NULL; }

    _pntr_nk_decompress(ttf, compressed, (unsigned int)compressed_size);
    pntr_unload_memory(compressed);

    pntr_font* font = pntr_load_font_ttf_from_memory(ttf, decompressed_size, (int)fontSize);
    pntr_unload_memory(ttf);
    return font;
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
