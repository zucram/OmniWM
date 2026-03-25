#ifndef GHOSTTYKIT_H
#define GHOSTTYKIT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// ---------------------------------------------------------------------------
// Opaque pointer types
// ---------------------------------------------------------------------------
typedef struct ghostty_app_opaque    *ghostty_app_t;
typedef struct ghostty_config_opaque *ghostty_config_t;
typedef struct ghostty_surface_opaque *ghostty_surface_t;

// ---------------------------------------------------------------------------
// Enums
// ---------------------------------------------------------------------------

// Key action — plain enum, rawValue: Int32 in Swift (only compared, not init'd from UInt32)
typedef enum {
    GHOSTTY_ACTION_PRESS   = 0,
    GHOSTTY_ACTION_RELEASE = 1,
    GHOSTTY_ACTION_REPEAT  = 2
} ghostty_input_action_e;

// Modifier flags — MUST have uint32_t underlying type so Swift rawValue is UInt32
// (Swift code does: ghostty_input_mods_e(rawValue: someUInt32))
typedef enum ghostty_input_mods_e : uint32_t {
    GHOSTTY_MODS_NONE  = 0,
    GHOSTTY_MODS_SHIFT = 1 << 0,
    GHOSTTY_MODS_CTRL  = 1 << 1,
    GHOSTTY_MODS_ALT   = 1 << 2,
    GHOSTTY_MODS_SUPER = 1 << 3,
    GHOSTTY_MODS_CAPS  = 1 << 4,
    GHOSTTY_MODS_SHIFT_RIGHT = 1 << 5,
    GHOSTTY_MODS_CTRL_RIGHT  = 1 << 6,
    GHOSTTY_MODS_ALT_RIGHT   = 1 << 7,
    GHOSTTY_MODS_SUPER_RIGHT = 1 << 8
} ghostty_input_mods_e;

typedef enum ghostty_binding_flags_e : uint32_t {
    GHOSTTY_BINDING_FLAGS_NONE = 0
} ghostty_binding_flags_e;

typedef enum {
    GHOSTTY_MOUSE_LEFT   = 0,
    GHOSTTY_MOUSE_RIGHT  = 1,
    GHOSTTY_MOUSE_MIDDLE = 2
} ghostty_input_mouse_button_e;

typedef enum {
    GHOSTTY_MOUSE_RELEASE = 0,
    GHOSTTY_MOUSE_PRESS   = 1
} ghostty_input_mouse_state_e;

typedef enum {
    GHOSTTY_CLIPBOARD_STANDARD  = 0,
    GHOSTTY_CLIPBOARD_SELECTION = 1
} ghostty_clipboard_e;

typedef enum {
    GHOSTTY_SUCCESS     = 0,
    GHOSTTY_INIT_FAILED = 1
} ghostty_init_result_e;

typedef uint32_t ghostty_input_scroll_mods_t;

// ---------------------------------------------------------------------------
// Platform
// ---------------------------------------------------------------------------
typedef enum {
    GHOSTTY_PLATFORM_MACOS   = 0,
    GHOSTTY_PLATFORM_LINUX   = 1,
    GHOSTTY_PLATFORM_WINDOWS = 2
} ghostty_platform_tag_e;

typedef struct {
    void * __nullable nsview;
} ghostty_platform_macos_s;

typedef union {
    ghostty_platform_macos_s macos;
} ghostty_platform_u;

// ---------------------------------------------------------------------------
// Clipboard data (array element passed to write_clipboard_cb)
// ---------------------------------------------------------------------------
typedef struct {
    const char * __nullable mime;
    const char * __nullable data;
} ghostty_clipboard_data_s;

// ---------------------------------------------------------------------------
// Key event
// ---------------------------------------------------------------------------
typedef struct {
    ghostty_input_action_e  action;
    ghostty_input_mods_e    mods;
    ghostty_input_mods_e    consumed_mods;
    uint32_t                keycode;
    bool                    composing;
    uint32_t                unshifted_codepoint;
    const char * __nullable text;
} ghostty_input_key_s;

// ---------------------------------------------------------------------------
// Surface configuration
// ---------------------------------------------------------------------------
typedef struct {
    ghostty_platform_tag_e  platform_tag;
    ghostty_platform_u      platform;
    double                  scale_factor;
    void * __nullable       userdata;
} ghostty_surface_config_s;

// ---------------------------------------------------------------------------
// Runtime configuration
// ---------------------------------------------------------------------------
typedef struct {
    void * __nullable userdata;
    bool              supports_selection_clipboard;
    void (* __nullable wakeup_cb)(void * __nullable userdata);
    bool (* __nullable action_cb)(void * __nullable app, void * __nullable surface, void * __nullable action);
    bool (* __nullable read_clipboard_cb)(void * __nullable userdata, ghostty_clipboard_e location, void * __nullable state);
    void (* __nullable confirm_read_clipboard_cb)(void * __nullable userdata, void * __nullable surface, void * __nullable request, void * __nullable state);
    void (* __nullable write_clipboard_cb)(void * __nullable userdata, ghostty_clipboard_e location, const ghostty_clipboard_data_s * __nullable content, size_t len, bool confirm);
    void (* __nullable close_surface_cb)(void * __nullable userdata, bool process_alive);
} ghostty_runtime_config_s;

// ---------------------------------------------------------------------------
// Functions
// ---------------------------------------------------------------------------
ghostty_init_result_e ghostty_init(int argc, const char * __nullable const * __nullable argv);

ghostty_app_t __nullable ghostty_app_new(ghostty_runtime_config_s * __nonnull config, ghostty_config_t __nullable app_config);
void ghostty_app_free(ghostty_app_t __nonnull app);
void ghostty_app_tick(ghostty_app_t __nonnull app);
void ghostty_app_update_config(ghostty_app_t __nonnull app, ghostty_config_t __nonnull config);

ghostty_config_t __nullable ghostty_config_new(void);
void ghostty_config_free(ghostty_config_t __nullable config);
void ghostty_config_finalize(ghostty_config_t __nullable config);
void ghostty_config_load_default_files(ghostty_config_t __nullable config);

ghostty_surface_config_s ghostty_surface_config_new(void);
ghostty_surface_t __nullable ghostty_surface_new(ghostty_app_t __nullable app, ghostty_surface_config_s * __nonnull config);
void ghostty_surface_free(ghostty_surface_t __nonnull surface);
bool ghostty_surface_key(ghostty_surface_t __nonnull surface, ghostty_input_key_s event);
bool ghostty_surface_key_is_binding(
    ghostty_surface_t __nonnull surface,
    ghostty_input_key_s event,
    ghostty_binding_flags_e * __nullable flags
);
ghostty_input_mods_e ghostty_surface_key_translation_mods(
    ghostty_surface_t __nonnull surface,
    ghostty_input_mods_e mods
);
void ghostty_surface_text(ghostty_surface_t __nonnull surface, const char * __nullable text, unsigned long len);
void ghostty_surface_preedit(ghostty_surface_t __nonnull surface, const char * __nullable text, unsigned long len);
bool ghostty_surface_binding_action(ghostty_surface_t __nonnull surface, const char * __nullable action, unsigned long len);
bool ghostty_surface_mouse_button(ghostty_surface_t __nonnull surface, ghostty_input_mouse_state_e state, ghostty_input_mouse_button_e button, ghostty_input_mods_e mods);
void ghostty_surface_mouse_pos(ghostty_surface_t __nonnull surface, double x, double y, ghostty_input_mods_e mods);
void ghostty_surface_mouse_scroll(ghostty_surface_t __nonnull surface, double dx, double dy, ghostty_input_scroll_mods_t mods);
void ghostty_surface_set_focus(ghostty_surface_t __nonnull surface, bool focused);
void ghostty_surface_set_size(ghostty_surface_t __nonnull surface, uint32_t width, uint32_t height);
void ghostty_surface_set_content_scale(ghostty_surface_t __nonnull surface, double sx, double sy);
void ghostty_surface_set_display_id(ghostty_surface_t __nonnull surface, uint32_t display_id);
void ghostty_surface_ime_point(ghostty_surface_t __nonnull surface, double * __nullable x, double * __nullable y, double * __nullable w, double * __nullable h);
void ghostty_surface_complete_clipboard_request(ghostty_surface_t __nonnull surface, const char * __nullable text, void * __nullable state, bool confirmed);

#endif /* GHOSTTYKIT_H */
