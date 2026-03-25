#include "GhosttyKit.h"
#include <string.h>

// Stub implementations — the real GhosttyKit.xcframework is required at runtime.
// These stubs exist solely so the package compiles without the binary artifact.

ghostty_init_result_e ghostty_init(int argc, const char * const *argv) {
    (void)argc; (void)argv;
    return GHOSTTY_INIT_FAILED;
}

ghostty_app_t ghostty_app_new(ghostty_runtime_config_s *config, ghostty_config_t app_config) {
    (void)config; (void)app_config;
    return NULL;
}
void ghostty_app_free(ghostty_app_t app)                              { (void)app; }
void ghostty_app_tick(ghostty_app_t app)                              { (void)app; }
void ghostty_app_update_config(ghostty_app_t app, ghostty_config_t c) { (void)app; (void)c; }

ghostty_config_t ghostty_config_new(void)                             { return NULL; }
void ghostty_config_free(ghostty_config_t c)                          { (void)c; }
void ghostty_config_finalize(ghostty_config_t c)                      { (void)c; }
void ghostty_config_load_default_files(ghostty_config_t c)            { (void)c; }

ghostty_surface_config_s ghostty_surface_config_new(void) {
    ghostty_surface_config_s cfg;
    memset(&cfg, 0, sizeof(cfg));
    return cfg;
}

ghostty_surface_t ghostty_surface_new(ghostty_app_t app, ghostty_surface_config_s *config) {
    (void)app; (void)config;
    return NULL;
}
void ghostty_surface_free(ghostty_surface_t s)                                                              { (void)s; }
bool ghostty_surface_key(ghostty_surface_t s, ghostty_input_key_s e)                                       { (void)s; (void)e; return false; }
bool ghostty_surface_key_is_binding(ghostty_surface_t s, ghostty_input_key_s e, ghostty_binding_flags_e *f) { (void)s; (void)e; if (f) *f = GHOSTTY_BINDING_FLAGS_NONE; return false; }
ghostty_input_mods_e ghostty_surface_key_translation_mods(ghostty_surface_t s, ghostty_input_mods_e m)     { (void)s; return m; }
void ghostty_surface_text(ghostty_surface_t s, const char *t, unsigned long l)                             { (void)s; (void)t; (void)l; }
void ghostty_surface_preedit(ghostty_surface_t s, const char *t, unsigned long l)                          { (void)s; (void)t; (void)l; }
bool ghostty_surface_binding_action(ghostty_surface_t s, const char *a, unsigned long l)                   { (void)s; (void)a; (void)l; return false; }
bool ghostty_surface_mouse_button(ghostty_surface_t s, ghostty_input_mouse_state_e st, ghostty_input_mouse_button_e b, ghostty_input_mods_e m) { (void)s; (void)st; (void)b; (void)m; return false; }
void ghostty_surface_mouse_pos(ghostty_surface_t s, double x, double y, ghostty_input_mods_e m)            { (void)s; (void)x; (void)y; (void)m; }
void ghostty_surface_mouse_scroll(ghostty_surface_t s, double dx, double dy, ghostty_input_scroll_mods_t m){ (void)s; (void)dx; (void)dy; (void)m; }
void ghostty_surface_set_focus(ghostty_surface_t s, bool f)                                                 { (void)s; (void)f; }
void ghostty_surface_set_size(ghostty_surface_t s, uint32_t w, uint32_t h)                                  { (void)s; (void)w; (void)h; }
void ghostty_surface_set_content_scale(ghostty_surface_t s, double sx, double sy)                           { (void)s; (void)sx; (void)sy; }
void ghostty_surface_set_display_id(ghostty_surface_t s, uint32_t d)                                        { (void)s; (void)d; }
void ghostty_surface_ime_point(ghostty_surface_t s, double *x, double *y, double *w, double *h)             { (void)s; (void)x; (void)y; (void)w; (void)h; }
void ghostty_surface_complete_clipboard_request(ghostty_surface_t s, const char *t, void *st, bool c)       { (void)s; (void)t; (void)st; (void)c; }
