#ifndef HG_LVN_WL_H
#define HG_LVN_WL_H

#include "lvn_graphics_internal.h"

#include "wayland-client-core.h"

#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-compose.h>

typedef struct wl_display*           (*PFN_wl_display_connect)(const char*);
typedef void                         (*PFN_wl_display_disconnect)(struct wl_display*);

typedef int                          (*PFN_wl_display_flush)(struct wl_display*);
typedef void                         (*PFN_wl_display_cancel_read)(struct wl_display*);
typedef int                          (*PFN_wl_display_dispatch)(struct wl_display*);
typedef int                          (*PFN_wl_display_dispatch_pending)(struct wl_display*);
typedef int                          (*PFN_wl_display_read_events)(struct wl_display*);
typedef int                          (*PFN_wl_display_roundtrip)(struct wl_display*);
typedef int                          (*PFN_wl_display_get_fd)(struct wl_display*);
typedef int                          (*PFN_wl_display_prepare_read)(struct wl_display*);
typedef int                          (*PFN_wl_proxy_add_listener)(struct wl_proxy*,void(**)(void),void*);
typedef void                         (*PFN_wl_proxy_destroy)(struct wl_proxy*);
typedef void                         (*PFN_wl_proxy_marshal)(struct wl_proxy*,uint32_t,...);
typedef struct wl_proxy*             (*PFN_wl_proxy_marshal_constructor)(struct wl_proxy*,uint32_t,const struct wl_interface*,...);
typedef struct wl_proxy*             (*PFN_wl_proxy_marshal_constructor_versioned)(struct wl_proxy*,uint32_t,const struct wl_interface*,uint32_t,...);
typedef struct wl_proxy*             (*PFN_wl_proxy_marshal_flags)(struct wl_proxy*,uint32_t,const struct wl_interface*,uint32_t,uint32_t,...);
typedef void*                        (*PFN_wl_proxy_get_user_data)(struct wl_proxy*);
typedef void                         (*PFN_wl_proxy_set_user_data)(struct wl_proxy*,void*);
typedef void                         (*PFN_wl_proxy_set_tag)(struct wl_proxy*,const char*const*);
typedef const char* const*           (*PFN_wl_proxy_get_tag)(struct wl_proxy*);
typedef uint32_t                     (*PFN_wl_proxy_get_version)(struct wl_proxy*);

typedef struct wl_egl_window*        (*PFN_wl_egl_window_create)(struct wl_surface*, int, int);
typedef void                         (*PFN_wl_egl_window_destroy)(struct wl_egl_window*);
typedef void                         (*PFN_wl_egl_window_resize)(struct wl_egl_window*, int, int, int, int);

typedef struct xkb_context*          (*PFN_xkb_context_new)(enum xkb_context_flags);
typedef void                         (*PFN_xkb_context_unref)(struct xkb_context*);
typedef struct xkb_keymap*           (*PFN_xkb_keymap_new_from_string)(struct xkb_context*, const char*, enum xkb_keymap_format, enum xkb_keymap_compile_flags);
typedef void                         (*PFN_xkb_keymap_unref)(struct xkb_keymap*);
typedef xkb_mod_index_t              (*PFN_xkb_keymap_mod_get_index)(struct xkb_keymap*, const char*);
typedef struct xkb_state*            (*PFN_xkb_state_new)(struct xkb_keymap*);
typedef void                         (*PFN_xkb_state_unref)(struct xkb_state*);
typedef int                          (*PFN_xkb_state_key_get_syms)(struct xkb_state*, xkb_keycode_t, const xkb_keysym_t**);
typedef enum xkb_state_component     (*PFN_xkb_state_update_mask)(struct xkb_state*, xkb_mod_mask_t, xkb_mod_mask_t, xkb_mod_mask_t, xkb_layout_index_t, xkb_layout_index_t, xkb_layout_index_t);
typedef int                          (*PFN_xkb_state_mod_index_is_active)(struct xkb_state*,xkb_mod_index_t,enum xkb_state_component);
typedef uint32_t                     (*PFN_xkb_keysym_to_utf32)(xkb_keysym_t);


typedef struct xkb_compose_table*    (*PFN_xkb_compose_table_new_from_locale)(struct xkb_context*, const char*, enum xkb_compose_compile_flags);
typedef void                         (*PFN_xkb_compose_table_unref)(struct xkb_compose_table*);
typedef struct xkb_compose_state*    (*PFN_xkb_compose_state_new)(struct xkb_compose_table*, enum xkb_compose_state_flags);
typedef void                         (*PFN_xkb_compose_state_unref)(struct xkb_compose_state*);
typedef enum xkb_compose_feed_result (*PFN_xkb_compose_state_feed)(struct xkb_compose_state*, xkb_keysym_t);
typedef enum xkb_compose_status      (*PFN_xkb_compose_state_get_status)(struct xkb_compose_state*);
typedef xkb_keysym_t                 (*PFN_xkb_compose_state_get_one_sym)(struct xkb_compose_state*);


#define wl_display_connect s_WlBackends->client.display_connect
#define wl_display_disconnect s_WlBackends->client.display_disconnect
#define wl_display_flush s_WlBackends->client.display_flush
#define wl_display_cancel_read s_WlBackends->client.display_cancel_read
#define wl_display_dispatch s_WlBackends->client.display_dispatch
#define wl_display_dispatch_pending s_WlBackends->client.display_dispatch_pending
#define wl_display_read_events s_WlBackends->client.display_read_events
#define wl_display_roundtrip s_WlBackends->client.display_roundtrip
#define wl_display_get_fd s_WlBackends->client.display_get_fd
#define wl_display_prepare_read s_WlBackends->client.display_prepare_read
#define wl_proxy_add_listener s_WlBackends->client.proxy_add_listener
#define wl_proxy_destroy s_WlBackends->client.proxy_destroy
#define wl_proxy_marshal s_WlBackends->client.proxy_marshal
#define wl_proxy_marshal_constructor s_WlBackends->client.proxy_marshal_constructor
#define wl_proxy_marshal_constructor_versioned s_WlBackends->client.proxy_marshal_constructor_versioned
#define wl_proxy_marshal_flags s_WlBackends->client.proxy_marshal_flags
#define wl_proxy_get_user_data s_WlBackends->client.proxy_get_user_data
#define wl_proxy_set_user_data s_WlBackends->client.proxy_set_user_data
#define wl_proxy_get_tag s_WlBackends->client.proxy_get_tag
#define wl_proxy_set_tag s_WlBackends->client.proxy_set_tag
#define wl_proxy_get_version s_WlBackends->client.proxy_get_version

#define xkb_context_new s_WlBackends->xkb.context_new
#define xkb_context_unref s_WlBackends->xkb.context_unref
#define xkb_keymap_new_from_string s_WlBackends->xkb.keymap_new_from_string
#define xkb_keymap_unref s_WlBackends->xkb.keymap_unref
#define xkb_keymap_mod_get_index s_WlBackends->xkb.keymap_mod_get_index
#define xkb_state_new s_WlBackends->xkb.state_new
#define xkb_state_unref s_WlBackends->xkb.state_unref
#define xkb_state_key_get_syms s_WlBackends->xkb.state_key_get_syms
#define xkb_state_update_mask s_WlBackends->xkb.state_update_mask
#define xkb_state_mod_index_is_active s_WlBackends->xkb.state_mod_index_is_active
#define xkb_keysym_to_utf32 s_WlBackends->xkb.keysym_to_utf32

#define xkb_compose_table_new_from_locale s_WlBackends->xkb.compose_table_new_from_locale
#define xkb_compose_table_unref s_WlBackends->xkb.compose_table_unref
#define xkb_compose_state_new s_WlBackends->xkb.compose_state_new
#define xkb_compose_state_unref s_WlBackends->xkb.compose_state_unref
#define xkb_compose_state_feed s_WlBackends->xkb.compose_state_feed
#define xkb_compose_state_get_status s_WlBackends->xkb.compose_state_get_status
#define xkb_compose_state_get_one_sym s_WlBackends->xkb.compose_state_get_one_sym

struct WaylandBackends
{
    struct wl_display*          display;
    struct wl_registry*         registry;
    struct wl_compositor*       compositor;
    struct wl_shm*              shm;
    struct xdg_wm_base*         xdg_wm_base;
    struct wl_seat*             seat;
    struct wl_keyboard*         keyboard;

    uint16_t                    keycodes[256];
    LvnWindow*                  keyboardFocus;

    struct
    {
        void* handle;

        PFN_wl_display_connect display_connect;
        PFN_wl_display_disconnect display_disconnect;
        PFN_wl_display_flush display_flush;
        PFN_wl_display_cancel_read display_cancel_read;
        PFN_wl_display_dispatch display_dispatch;
        PFN_wl_display_dispatch_pending display_dispatch_pending;
        PFN_wl_display_read_events display_read_events;
        PFN_wl_display_roundtrip display_roundtrip;
        PFN_wl_display_get_fd display_get_fd;
        PFN_wl_display_prepare_read display_prepare_read;
        PFN_wl_proxy_marshal proxy_marshal;
        PFN_wl_proxy_add_listener proxy_add_listener;
        PFN_wl_proxy_destroy proxy_destroy;
        PFN_wl_proxy_marshal_constructor proxy_marshal_constructor;
        PFN_wl_proxy_marshal_constructor_versioned proxy_marshal_constructor_versioned;
        PFN_wl_proxy_get_user_data proxy_get_user_data;
        PFN_wl_proxy_set_user_data proxy_set_user_data;
        PFN_wl_proxy_set_tag proxy_set_tag;
        PFN_wl_proxy_get_tag proxy_get_tag;
        PFN_wl_proxy_get_version proxy_get_version;
        PFN_wl_proxy_marshal_flags proxy_marshal_flags;
    } client;

    struct
    {
        void* handle;
        PFN_wl_egl_window_create window_create;
        PFN_wl_egl_window_destroy window_destroy;
        PFN_wl_egl_window_resize window_resize;
    } egl;

    struct
    {
        struct xkb_context* context;
        struct xkb_compose_state* composeState;
        struct xkb_keymap* keymap;
        struct xkb_state* state;

        xkb_mod_index_t controlIndex;
        xkb_mod_index_t altIndex;
        xkb_mod_index_t shiftIndex;
        xkb_mod_index_t superIndex;
        xkb_mod_index_t capsLockIndex;
        xkb_mod_index_t numLockIndex;

        uint32_t modifiers;

        void* handle;
        PFN_xkb_context_new context_new;
        PFN_xkb_context_unref context_unref;
        PFN_xkb_keymap_new_from_string keymap_new_from_string;
        PFN_xkb_keymap_unref keymap_unref;
        PFN_xkb_keymap_mod_get_index keymap_mod_get_index;
        PFN_xkb_state_new state_new;
        PFN_xkb_state_unref state_unref;
        PFN_xkb_state_key_get_syms state_key_get_syms;
        PFN_xkb_state_update_mask state_update_mask;
        PFN_xkb_state_mod_index_is_active state_mod_index_is_active;
        PFN_xkb_keysym_to_utf32 keysym_to_utf32;

        PFN_xkb_compose_table_new_from_locale compose_table_new_from_locale;
        PFN_xkb_compose_table_unref compose_table_unref;
        PFN_xkb_compose_state_new compose_state_new;
        PFN_xkb_compose_state_unref compose_state_unref;
        PFN_xkb_compose_state_feed compose_state_feed;
        PFN_xkb_compose_state_get_status compose_state_get_status;
        PFN_xkb_compose_state_get_one_sym compose_state_get_one_sym;
    } xkb;
};

namespace lvn
{
    LvnResult        implWaylandInitWindowContext(LvnGraphicsContext* graphicsctx);
    void             implWaylandTerminateWindowContext();

    LvnResult        implWaylandCreateWindow(LvnWindow* window, const LvnWindowCreateInfo* createInfo);
    void             implWaylandDestroyWindow(LvnWindow* window);

    void             implWaylandUpdateWindow(LvnWindow* window);
    bool             implWaylandWindowOpen(LvnWindow* window);
    void             implWaylandWindowPollEvents();
    LvnPair<int>     implWaylandGetDimensions(LvnWindow* window);
    unsigned int     implWaylandGetWindowWidth(LvnWindow* window);
    unsigned int     implWaylandGetWindowHeight(LvnWindow* window);
    void             implWaylandSetWindowVSync(LvnWindow* window, bool enable);
    bool             implWaylandGetWindowVSync(LvnWindow* window);
    void*            implWaylandGetNativeWindow(LvnWindow* window);
    void             implWaylandSetWindowContextCurrent(LvnWindow* window);
    LvnRenderPass*   implWaylandGetWindowRenderPass(LvnWindow* window);

    bool             implWaylandKeyPressed(LvnWindow* window, int keycode);
    bool             implWaylandKeyReleased(LvnWindow* window, int keycode);
    bool             implWaylandMouseButtonPressed(LvnWindow* window, int button);
    bool             implWaylandMouseButtonReleased(LvnWindow* window, int button);

    LvnPair<float>   implWaylandGetMousePos(LvnWindow* window);
    void             implWaylandGetMousePosPtr(LvnWindow* window, float* xpos, float* ypos);
    float            implWaylandGetMouseX(LvnWindow* window);
    float            implWaylandGetMouseY(LvnWindow* window);
    void             implWaylandSetMouseCursor(LvnWindow* window, LvnMouseCursor cursor);
    void             implWaylandSetMouseInputMode(LvnWindow* window, LvnMouseInputMode mode);

    LvnPair<int>     implWaylandGetWindowPos(LvnWindow* window);
    void             implWaylandGetWindowPosPtr(LvnWindow* window, int* xpos, int* ypos);
    LvnPair<int>     implWaylandGetWindowSize(LvnWindow* window);
    void             implWaylandGetWindowSizePtr(LvnWindow* window, int* width, int* height);

    LvnWindowApi     implWaylandGetNativeWindowApi();
}

#endif /* !HG_LVN_WL_H */
