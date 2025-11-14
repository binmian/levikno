#include "lvn_impl_wl.h"

#include <cstring>
#include <dlfcn.h>

#include <sys/timerfd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>

#include <linux/input-event-codes.h>

static WaylandBackends* s_WlBackends = nullptr;

#include "wayland-client-protocol.h"
#include "wayland-client-protocol-code.h"

#include "xdg-shell-client-protocol.h"
#include "xdg-shell-client-protocol-code.h"



namespace lvn
{
namespace wls
{
    static WaylandBackends*   getWaylandBackends();
    static void               registryHandleGlobal(void* userData, struct wl_registry* registry, uint32_t name, const char* interface, uint32_t version);
    static void               registryHandleGlobalRemove(void* userData, struct wl_registry* registry, uint32_t name);
    static void               xdgPing(void *data, struct xdg_wm_base *xdg_wm_base, uint32_t serial);
    static void               xdgSurfaceHandleConfigure(void* userData, struct xdg_surface* surface, uint32_t serial);
    static void               xdgToplevelHandleConfigure(void* userData, struct xdg_toplevel* toplevel, int32_t width, int32_t height, struct wl_array* states);
    static void               xdgToplevelHandleClose(void* userData, struct xdg_toplevel* toplevel);
    static void               keyboardHandleKeymap(void* userData, struct wl_keyboard* keyboard, uint32_t format, int fd, uint32_t size);
    static void               keyboardHandleEnter(void* userData, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface* surface, struct wl_array* keys);
    static void               keyboardHandleLeave(void* userData, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface* surface);
    static void               keyboardHandleKey(void* userData, struct wl_keyboard* keyboard, uint32_t serial, uint32_t time, uint32_t scancode, uint32_t state);
    static void               keyboardHandleModifiers(void* userData, struct wl_keyboard* keyboard, uint32_t serial, uint32_t modsDepressed, uint32_t modsLatched, uint32_t modsLocked, uint32_t group);
    static void               keyboardHandleRepeatInfo(void* userData, struct wl_keyboard* keyboard, int32_t rate, int32_t delay);
    static void               seatHandleCapabilities(void* userData, struct wl_seat* seat, uint32_t caps);
    static void               seatHandleName(void* userData, struct wl_seat* seat, const char* name);
    static void               callBackDoneHandle(void* data, struct wl_callback* callback, uint32_t cbData);
    static void               outputGeometryHandle(void *data, struct wl_output* wl_output, int32_t x, int32_t y, int32_t physical_width, int32_t physical_height, int32_t subpixel, const char *make, const char *model, int32_t transform);
    static void               outputModeHandle(void* data, struct wl_output* wl_output, uint32_t flags, int32_t width, int32_t height, int32_t refresh);
    static void               outputDoneHandle(void* data, struct wl_output* wl_output);
    static void               outputScaleHandle(void* data, struct wl_output* wl_output, int32_t factor);
    static void               outputNameHandle(void* data, struct wl_output* wl_output, const char* name);
    static void               outputDescriptionHandle(void* data, struct wl_output* wl_output, const char* description);
    static int                createShmFile(off_t size);
    static void               createShmBuffer(uint8_t** pixels, wl_buffer** buffer, uint32_t width, uint32_t height);
    static xkb_keysym_t       composeSymbol(WaylandBackends* wlBackends, xkb_keysym_t sym);
    static void               inputText(WaylandBackends* wlBackends, LvnWindow* window, uint32_t scancode, bool repeat);
    static uint64_t           getPlatformTimeValue();
    static bool               pollKeyRepeatEvent(struct pollfd* fds, nfds_t count, double* timeout);
    static bool               resizeWindow(LvnWindow* window, int32_t width, int32_t height);

    static WaylandBackends* getWaylandBackends()
    {
        LVN_ASSERT(s_WlBackends != nullptr, "cannot get wayland backends, wayland backends was not created");
        return s_WlBackends;
    }

    static const struct wl_registry_listener s_RegistryListener =
    {
        wls::registryHandleGlobal,
        wls::registryHandleGlobalRemove,
    };

    static const struct xdg_wm_base_listener s_XdgBaseListener =
    {
        wls::xdgPing,
    };

    static const struct xdg_surface_listener s_XdgSurfaceListener =
    {
        wls::xdgSurfaceHandleConfigure
    };

    static const struct xdg_toplevel_listener s_XdgToplevelListener =
    {
        wls::xdgToplevelHandleConfigure,
        wls::xdgToplevelHandleClose
    };

    static const struct wl_keyboard_listener s_KeyboardListener =
    {
        wls::keyboardHandleKeymap,
        wls::keyboardHandleEnter,
        wls::keyboardHandleLeave,
        wls::keyboardHandleKey,
        wls::keyboardHandleModifiers,
        wls::keyboardHandleRepeatInfo,
    };

    static const struct wl_seat_listener s_SeatListener =
    {
        wls::seatHandleCapabilities,
        wls::seatHandleName,
    };

    static const struct wl_callback_listener s_CallbackListener =
    {
        wls::callBackDoneHandle,
    };

    static const struct wl_output_listener s_OutputListener =
    {
        wls::outputGeometryHandle,
        wls::outputModeHandle,
        wls::outputDoneHandle,
        wls::outputScaleHandle,
        wls::outputNameHandle,
        wls::outputDescriptionHandle,
    };

    static void registryHandleGlobal(void* userData, struct wl_registry* registry, uint32_t name, const char* interface, uint32_t version)
    {
        WaylandBackends* wlBackends = static_cast<WaylandBackends*>(userData);

        if (strcmp(interface, wl_compositor_interface.name) == 0)
            wlBackends->compositor = static_cast<wl_compositor*>(wl_registry_bind(registry, name, &wl_compositor_interface, lvn::min<uint32_t>(4, version)));
        else if (strcmp(interface, wl_shm_interface.name) == 0)
            wlBackends->shm = static_cast<wl_shm*>(wl_registry_bind( registry, name, &wl_shm_interface, 1));
        else if (strcmp(interface, xdg_wm_base_interface.name) == 0)
        {
            wlBackends->xdg_wm_base = static_cast<xdg_wm_base*>(wl_registry_bind(registry, name, &xdg_wm_base_interface, 1));
            xdg_wm_base_add_listener(s_WlBackends->xdg_wm_base, &s_XdgBaseListener, s_WlBackends);
        }
        else if (strcmp(interface, wl_seat_interface.name) == 0)
        {
            wlBackends->seat = static_cast<wl_seat*>(wl_registry_bind(registry, name, &wl_seat_interface, lvn::min<uint32_t>(4, version)));
            wl_seat_add_listener(wlBackends->seat, &s_SeatListener, wlBackends);
        }
        else if(strcmp(interface, wl_output_interface.name) == 0)
        {
            wlBackends->output = static_cast<wl_output*>(wl_registry_bind(registry, name, &wl_output_interface, lvn::min<uint32_t>(3, version)));
            wl_output_add_listener(wlBackends->output, &s_OutputListener, wlBackends);
        }
    }

    static void registryHandleGlobalRemove(void* userData, struct wl_registry* registry, uint32_t name)
    {

    }

    static void xdgPing(void *data, struct xdg_wm_base *xdg_wm_base, uint32_t serial)
    {
        xdg_wm_base_pong(xdg_wm_base, serial);
    }

    static void xdgSurfaceHandleConfigure(void* userData, struct xdg_surface* surface, uint32_t serial)
    {
        LvnWindow* window = static_cast<LvnWindow*>(userData);

        xdg_surface_ack_configure(surface, serial);

        int32_t width = window->nwdata.wl.pendingWidth;
        int32_t height = window->nwdata.wl.pendingHeight;

        if (wls::resizeWindow(window, width, height))
        {
            // LVN_CORE_INFO("window resize event");
        }
    }

    static void xdgToplevelHandleConfigure(void* userData, struct xdg_toplevel* toplevel, int32_t width, int32_t height, struct wl_array* states)
    {
        if (width * height == 0)
            return;

        LvnWindow* window = static_cast<LvnWindow*>(userData);
        WaylandBackends* wlBackends = wls::getWaylandBackends();

        if (window->width != width || window->height != height)
        {
            window->nwdata.wl.pendingWidth = width;
            window->nwdata.wl.pendingHeight = height;
        }
    }

    static void xdgToplevelHandleClose(void* userData, struct xdg_toplevel* toplevel)
    {
        LvnWindow* window = static_cast<LvnWindow*>(userData);
        window->windowOpen = false;
    }

    static void keyboardHandleKeymap(void* userData, struct wl_keyboard* keyboard, uint32_t format, int fd, uint32_t size)
    {
        WaylandBackends* wlBackends = static_cast<WaylandBackends*>(userData);

        if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1)
        {
            close(fd);
            return;
        }

        char* mapStr = static_cast<char*>(mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0));
        if (mapStr == MAP_FAILED)
        {
            close(fd);
            return;
        }

        struct xkb_keymap* keymap = xkb_keymap_new_from_string(wlBackends->xkb.context, mapStr, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
        munmap(mapStr, size);
        close(fd);

        if (!keymap)
        {
            LVN_CORE_ERROR("[wayland] failed to compile keymap");
            return;
        }

        struct xkb_state* state = xkb_state_new(keymap);
        if (!state)
        {
            LVN_CORE_ERROR("[wayland] failed to create XKB state");
            xkb_keymap_unref(keymap);
            return;
        }

        // Look up the preferred locale, falling back to "C" as default.
        const char* locale = getenv("LC_ALL");
        if (!locale)
            locale = getenv("LC_CTYPE");
        if (!locale)
            locale = getenv("LANG");
        if (!locale)
            locale = "C";

        struct xkb_compose_table* composeTable = xkb_compose_table_new_from_locale(wlBackends->xkb.context, locale, XKB_COMPOSE_COMPILE_NO_FLAGS);
        if (composeTable)
        {
            struct xkb_compose_state* composeState = xkb_compose_state_new(composeTable, XKB_COMPOSE_STATE_NO_FLAGS);
            xkb_compose_table_unref(composeTable);
            if (composeState)
                wlBackends->xkb.composeState = composeState;
            else
                LVN_CORE_ERROR("[wayland] failed to create XKB compose state");
        }
        else
            LVN_CORE_ERROR("[wayland] failed to create XKB compose table");

        xkb_keymap_unref(s_WlBackends->xkb.keymap);
        xkb_state_unref(s_WlBackends->xkb.state);
        s_WlBackends->xkb.keymap = keymap;
        s_WlBackends->xkb.state = state;

        s_WlBackends->xkb.controlIndex  = xkb_keymap_mod_get_index(s_WlBackends->xkb.keymap, "Control");
        s_WlBackends->xkb.altIndex      = xkb_keymap_mod_get_index(s_WlBackends->xkb.keymap, "Mod1");
        s_WlBackends->xkb.shiftIndex    = xkb_keymap_mod_get_index(s_WlBackends->xkb.keymap, "Shift");
        s_WlBackends->xkb.superIndex    = xkb_keymap_mod_get_index(s_WlBackends->xkb.keymap, "Mod4");
        s_WlBackends->xkb.capsLockIndex = xkb_keymap_mod_get_index(s_WlBackends->xkb.keymap, "Lock");
        s_WlBackends->xkb.numLockIndex  = xkb_keymap_mod_get_index(s_WlBackends->xkb.keymap, "Mod2");
    }

    static void keyboardHandleEnter(void* userData, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface* surface, struct wl_array* keys)
    {
        if (!surface)
            return;

        WaylandBackends* wlBackends = static_cast<WaylandBackends*>(userData);
        LvnWindow* window = static_cast<LvnWindow*>(wl_surface_get_user_data(surface));

        if (surface != window->nwdata.wl.surface)
            return;

        wlBackends->keyboardFocus = window;
    }

    static void keyboardHandleLeave(void* userData, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface* surface)
    {
        WaylandBackends* wlBackends = static_cast<WaylandBackends*>(userData);
        LvnWindow* window = wlBackends->keyboardFocus;

        if (!window)
            return;

        struct itimerspec timer = {0};
        timerfd_settime(wlBackends->keyRepeatTimerfd, 0, &timer, NULL);
    }

    static void keyboardHandleKey(void* userData, struct wl_keyboard* keyboard, uint32_t serial, uint32_t time, uint32_t scancode, uint32_t state)
    {
        WaylandBackends* wlBackends = static_cast<WaylandBackends*>(userData);
        LvnWindow* window = wlBackends->keyboardFocus;

        if (!window)
            return;

        const xkb_keycode_t keycode = scancode + 8;
        struct itimerspec timer = {0};

        if (state == WL_KEYBOARD_KEY_STATE_PRESSED)
        {
            wls::inputText(wlBackends, window, scancode, false);

            if (xkb_keymap_key_repeats(wlBackends->xkb.keymap, keycode) && wlBackends->keyRepeatRate > 0)
            {
                wlBackends->keyRepeatScancode = scancode;
                if (wlBackends->keyRepeatRate > 1)
                    timer.it_interval.tv_nsec = 1000000000 / wlBackends->keyRepeatRate;
                else
                    timer.it_interval.tv_sec = 1;

                timer.it_value.tv_sec = wlBackends->keyRepeatDelay / 1000;
                timer.it_value.tv_nsec = (wlBackends->keyRepeatDelay % 1000) * 1000000;
            }

            if (!window->eventCallBackFn)
                return;

            LvnEvent event{};
            event.type = Lvn_EventType_KeyPressed;
            event.category = Lvn_EventCategory_Input | Lvn_EventCategory_Keyboard;
            event.handled = false;
            event.data.code = wlBackends->keycodes[scancode];
            event.data.repeat = false;
            event.userData = window->userData;
            window->eventCallBackFn(&event);
        }
        else if (state == WL_KEYBOARD_KEY_STATE_RELEASED)
        {
            if (!window->eventCallBackFn)
                return;

            LvnEvent event{};
            event.type = Lvn_EventType_KeyReleased;
            event.category = Lvn_EventCategory_Input | Lvn_EventCategory_Keyboard;
            event.handled = false;
            event.data.code = wlBackends->keycodes[scancode];
            event.data.repeat = false;
            event.userData = window->userData;
            window->eventCallBackFn(&event);
        }

        timerfd_settime(wlBackends->keyRepeatTimerfd, 0, &timer, nullptr);
    }

    static void keyboardHandleModifiers(void* userData, struct wl_keyboard* keyboard, uint32_t serial, uint32_t modsDepressed, uint32_t modsLatched, uint32_t modsLocked, uint32_t group)
    {
        WaylandBackends* wlBackends = static_cast<WaylandBackends*>(userData);

        if (!wlBackends->xkb.keymap)
            return;

        xkb_state_update_mask(wlBackends->xkb.state, modsDepressed, modsLatched, modsLocked, 0, 0, group);

        wlBackends->xkb.modifiers = 0;

        if (xkb_state_mod_index_is_active(wlBackends->xkb.state, wlBackends->xkb.controlIndex, XKB_STATE_MODS_EFFECTIVE) == 1)
            wlBackends->xkb.modifiers |= Lvn_KeyMod_Control;
        if (xkb_state_mod_index_is_active(wlBackends->xkb.state, wlBackends->xkb.altIndex, XKB_STATE_MODS_EFFECTIVE) == 1)
            wlBackends->xkb.modifiers |= Lvn_KeyMod_Alt;
        if (xkb_state_mod_index_is_active(wlBackends->xkb.state, wlBackends->xkb.shiftIndex, XKB_STATE_MODS_EFFECTIVE) == 1)
            wlBackends->xkb.modifiers |= Lvn_KeyMod_Shift;
        if (xkb_state_mod_index_is_active(wlBackends->xkb.state, wlBackends->xkb.superIndex, XKB_STATE_MODS_EFFECTIVE) == 1)
            wlBackends->xkb.modifiers |= Lvn_KeyMod_Super;
        if (xkb_state_mod_index_is_active(wlBackends->xkb.state, wlBackends->xkb.capsLockIndex, XKB_STATE_MODS_EFFECTIVE) == 1)
            wlBackends->xkb.modifiers |= Lvn_KeyMod_CapsLock;
        if (xkb_state_mod_index_is_active(wlBackends->xkb.state, wlBackends->xkb.numLockIndex, XKB_STATE_MODS_EFFECTIVE) == 1)
            wlBackends->xkb.modifiers |= Lvn_KeyMod_NumLock;
    }

    static void keyboardHandleRepeatInfo(void* userData, struct wl_keyboard* keyboard, int32_t rate, int32_t delay)
    {
        WaylandBackends* wlBackends = static_cast<WaylandBackends*>(userData);

        if (keyboard != wlBackends->keyboard)
            return;

        wlBackends->keyRepeatRate = rate;
        wlBackends->keyRepeatDelay = delay;
    }

    static void seatHandleCapabilities(void* userData, struct wl_seat* seat, uint32_t caps)
    {
        WaylandBackends* wlBackends = static_cast<WaylandBackends*>(userData);

        if (caps & WL_SEAT_CAPABILITY_KEYBOARD && !wlBackends->keyboard)
        {
            wlBackends->keyboard = wl_seat_get_keyboard(seat);
            wl_keyboard_add_listener(wlBackends->keyboard, &s_KeyboardListener, wlBackends);
        }
        else if (!(caps & WL_SEAT_CAPABILITY_KEYBOARD) && wlBackends->keyboard)
        {
            wl_keyboard_destroy(wlBackends->keyboard);
            wlBackends->keyboard = nullptr;
        }
    }

    static void seatHandleName(void* userData, struct wl_seat* seat, const char* name)
    {

    }

    static void callBackDoneHandle(void* data, struct wl_callback* callback, uint32_t cbData)
    {
        LvnWindow* window = static_cast<LvnWindow*>(data);

        wl_callback_destroy(callback);
        callback = wl_surface_frame(window->nwdata.wl.surface);
        wl_callback_add_listener(callback, &s_CallbackListener, window);

        memset(window->nwdata.wl.pixels, 150, window->width * window->height * 4);
        wl_surface_attach(window->nwdata.wl.surface, window->nwdata.wl.buffer, 0, 0);
        wl_surface_damage_buffer(window->nwdata.wl.surface, 0, 0, window->width, window->height);
        wl_surface_commit(window->nwdata.wl.surface);
    }

    static void outputGeometryHandle(void* data,
             struct wl_output* wl_output,
             int32_t x,
             int32_t y,
             int32_t physical_width,
             int32_t physical_height,
             int32_t subpixel,
             const char* make,
             const char* model,
             int32_t transform)
    {

    }

    static void outputModeHandle(void* data, struct wl_output* wl_output, uint32_t flags, int32_t width, int32_t height, int32_t refresh)
    {

    }

    static void outputDoneHandle(void* data, struct wl_output* wl_output)
    {

    }

    static void outputScaleHandle(void* data, struct wl_output* wl_output, int32_t factor)
    {
        WaylandBackends* wlBackends = static_cast<WaylandBackends*>(data);

        if (factor > 0)
            wlBackends->scale = factor;
        else
            wlBackends->scale = 1;
    }

    static void outputNameHandle(void* data, struct wl_output* wl_output, const char* name)
    {

    }

    static void outputDescriptionHandle(void* data, struct wl_output* wl_output, const char* description)
    {

    }

    static int createShmFile(off_t size)
    {
        static const char name[] = "/lvn-shared-XXXXXX";
        int fd = memfd_create("lvn-shared", MFD_CLOEXEC | MFD_ALLOW_SEALING);
        if (fd >= 0)
            fcntl(fd, F_ADD_SEALS, F_SEAL_SHRINK | F_SEAL_SEAL);
        else
        {
            const char* path = getenv("XDG_RUNTIME_DIR");
            if (!path)
                return -1;

            char* buff = (char*)lvn::memAlloc(strlen(path) + sizeof(name));
            strcpy(buff, path);
            strcat(buff, name);

            fd = mkostemp(buff, O_CLOEXEC);
            if (fd >= 0)
                unlink(buff);

            lvn::memFree(buff);

            if (fd < 0)
                return -1;
        }

        int ret = posix_fallocate(fd, 0, size);

        if (ret != 0)
        {
            close(fd);
            return -1;
        }

        return fd;
    }

    static void createShmBuffer(uint8_t** pixels, wl_buffer** buffer, uint32_t width, uint32_t height)
    {
        LVN_ASSERT(pixels != nullptr && buffer != nullptr, "pixels or buffer was nullptr");

        WaylandBackends* wlBackends = wls::getWaylandBackends();

        int32_t stride = width * 4;
        int32_t size = stride * height;

        int fd = wls::createShmFile(size);

        *pixels = (uint8_t*)mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        struct wl_shm_pool* pool = wl_shm_create_pool(wlBackends->shm, fd, size);

        *buffer = wl_shm_pool_create_buffer(pool, 0, width, height, stride, WL_SHM_FORMAT_ARGB8888);

        wl_shm_pool_destroy(pool);
        close(fd);
    }

    static xkb_keysym_t composeSymbol(WaylandBackends* wlBackends, xkb_keysym_t sym)
    {
        if (sym == XKB_KEY_NoSymbol || !wlBackends->xkb.composeState)
            return sym;
        if (xkb_compose_state_feed(wlBackends->xkb.composeState, sym) != XKB_COMPOSE_FEED_ACCEPTED)
            return sym;

        switch (xkb_compose_state_get_status(wlBackends->xkb.composeState))
        {
            case XKB_COMPOSE_COMPOSED:
                return xkb_compose_state_get_one_sym(wlBackends->xkb.composeState);
            case XKB_COMPOSE_COMPOSING:
            case XKB_COMPOSE_CANCELLED:
                return XKB_KEY_NoSymbol;
            case XKB_COMPOSE_NOTHING:
            default:
                return sym;
        }
    }

    static void inputText(WaylandBackends* wlBackends, LvnWindow* window, uint32_t scancode, bool repeat)
    {
        const xkb_keysym_t* keysyms;
        const xkb_keycode_t keycode = scancode + 8;

        if (!window->eventCallBackFn)
            return;

        if (xkb_state_key_get_syms(wlBackends->xkb.state, keycode, &keysyms) == 1)
        {
            const xkb_keysym_t keysym = wls::composeSymbol(wlBackends, keysyms[0]);
            const uint32_t codepoint = xkb_keysym_to_utf32(keysym);

            if (codepoint < 32 || (codepoint > 126 && codepoint < 160))
                return;

            if (codepoint != 0)
            {
                const uint32_t mods = wlBackends->xkb.modifiers;
                const bool hasModifier = mods & (Lvn_KeyMod_Control | Lvn_KeyMod_Alt);

                if (hasModifier)
                    return;

                LvnEvent event{};
                event.type = Lvn_EventType_KeyTyped;
                event.category = Lvn_EventCategory_Input | Lvn_EventCategory_Keyboard;
                event.handled = false;
                event.data.ucode = codepoint;
                event.data.repeat = false;
                event.userData = window->userData;
                window->eventCallBackFn(&event);
            }
        }
    }

    static uint64_t getPlatformTimeValue()
    {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);

        const uint64_t frequency = 1000000000;
        return (uint64_t)ts.tv_sec * frequency + (uint64_t)ts.tv_nsec;
    }

    static bool pollKeyRepeatEvent(struct pollfd* fds, nfds_t count, double* timeout)
    {
        for (;;)
        {
            if (timeout)
            {
                LVN_CORE_DEBUG("place 1");
                uint64_t tFirst = wls::getPlatformTimeValue();

                LVN_CORE_DEBUG("place 2");
                const time_t seconds = (time_t)(*timeout);
                const long nanoseconds = (long)((*timeout - seconds) * 1e9);

                LVN_CORE_DEBUG("place 3");
                const struct timespec ts = { seconds, nanoseconds };
                const int result = ppoll(fds, count, &ts, nullptr);

                LVN_CORE_DEBUG("place 4");
                *timeout -= (wls::getPlatformTimeValue() - tFirst) / (double)(1e9);

                LVN_CORE_DEBUG("ppoll");

                if (result > 0)
                    return true;
                else if (result == -1)
                    return false;
                else if (*timeout <= 0.0)
                    return false;
            }
            else
            {
                LVN_CORE_DEBUG("poll");
                const int result = poll(fds, count, -1);
                if (result > 0)
                    return true;
                else if (result == -1)
                    return false;
            }

        }
    }

    static bool resizeWindow(LvnWindow* window, int32_t width, int32_t height)
    {
        if (window->width == width && window->height == height)
        {
            // LVN_CORE_INFO("its working");
            return false;
        }
        else {
            // LVN_CORE_WARN("its not working");
        }

        if (window->nwdata.wl.pixels)
            munmap(window->nwdata.wl.pixels, window->width * window->height * 4);
        if (window->nwdata.wl.buffer)
            wl_buffer_destroy(window->nwdata.wl.buffer);

        window->width = width;
        window->height = height;

        wls::createShmBuffer(&window->nwdata.wl.pixels, &window->nwdata.wl.buffer, window->width, window->height);

        int count = 0;
        for (int y = 0; y < window->height; y++) {
            for (int x = 0; x < window->width; x++) {
                for (int c = 0; c < 4; c++) {
                    if (count % 2 == 0)
                        window->nwdata.wl.pixels[y * window->width * 4 + x * 4 + c] = 255;
                    else
                        window->nwdata.wl.pixels[y * window->width * 4 + x * 4 + c] = 0;
                }
                count++;
            }
            count++;
        }

        LVN_CORE_ERROR("w:%d,h:%d", window->width, window->height);

        // memset(window->nwdata.wl.pixels, 150, window->width * window->height * 4);
        wl_surface_attach(window->nwdata.wl.surface, window->nwdata.wl.buffer, 0, 0);
        wl_surface_damage_buffer(window->nwdata.wl.surface, 0, 0, window->width, window->height);
        wl_surface_commit(window->nwdata.wl.surface);

        return true;
    }
} /* namespace wls */

LvnResult implWaylandInitWindowContext(LvnGraphicsContext* graphicsctx)
{
    if (s_WlBackends)
    {
        LVN_CORE_WARN("wl already initialized!");
        return Lvn_Result_AlreadyCalled;
    }

    s_WlBackends = lvn::memNew<WaylandBackends>();

    // load wayland-client library symbols
    s_WlBackends->client.handle = dlopen("libwayland-client.so.0", RTLD_LAZY | RTLD_LOCAL);
    if (!s_WlBackends->client.handle)
    {
        LVN_CORE_ERROR("[wayland] failed to load libwayland-client shared library");
        return Lvn_Result_Failure;
    }

    s_WlBackends->client.display_connect = (PFN_wl_display_connect)dlsym(s_WlBackends->client.handle, "wl_display_connect");
    s_WlBackends->client.display_disconnect = (PFN_wl_display_disconnect)dlsym(s_WlBackends->client.handle, "wl_display_disconnect");
    s_WlBackends->client.display_flush = (PFN_wl_display_flush)dlsym(s_WlBackends->client.handle, "wl_display_flush");
    s_WlBackends->client.display_cancel_read = (PFN_wl_display_cancel_read)dlsym(s_WlBackends->client.handle, "wl_display_cancel_read");
    s_WlBackends->client.display_dispatch = (PFN_wl_display_dispatch)dlsym(s_WlBackends->client.handle, "wl_display_dispatch");
    s_WlBackends->client.display_dispatch_pending = (PFN_wl_display_dispatch_pending)dlsym(s_WlBackends->client.handle, "wl_display_dispatch_pending");
    s_WlBackends->client.display_read_events = (PFN_wl_display_read_events)dlsym(s_WlBackends->client.handle, "wl_display_read_events");
    s_WlBackends->client.display_roundtrip = (PFN_wl_display_roundtrip)dlsym(s_WlBackends->client.handle, "wl_display_roundtrip");
    s_WlBackends->client.display_get_fd = (PFN_wl_display_get_fd)dlsym(s_WlBackends->client.handle, "wl_display_get_fd");
    s_WlBackends->client.display_prepare_read = (PFN_wl_display_prepare_read)dlsym(s_WlBackends->client.handle, "wl_display_prepare_read");
    s_WlBackends->client.proxy_marshal = (PFN_wl_proxy_marshal)dlsym(s_WlBackends->client.handle, "wl_proxy_marshal");
    s_WlBackends->client.proxy_add_listener = (PFN_wl_proxy_add_listener)dlsym(s_WlBackends->client.handle, "wl_proxy_add_listener");
    s_WlBackends->client.proxy_destroy = (PFN_wl_proxy_destroy)dlsym(s_WlBackends->client.handle, "wl_proxy_destroy");
    s_WlBackends->client.proxy_marshal_constructor = (PFN_wl_proxy_marshal_constructor)dlsym(s_WlBackends->client.handle, "wl_proxy_marshal_constructor");
    s_WlBackends->client.proxy_marshal_constructor_versioned = (PFN_wl_proxy_marshal_constructor_versioned)dlsym(s_WlBackends->client.handle, "wl_proxy_marshal_constructor_versioned");
    s_WlBackends->client.proxy_get_user_data = (PFN_wl_proxy_get_user_data)dlsym(s_WlBackends->client.handle, "wl_proxy_get_user_data");
    s_WlBackends->client.proxy_set_user_data = (PFN_wl_proxy_set_user_data)dlsym(s_WlBackends->client.handle, "wl_proxy_set_user_data");
    s_WlBackends->client.proxy_get_tag = (PFN_wl_proxy_get_tag)dlsym(s_WlBackends->client.handle, "wl_proxy_get_tag");
    s_WlBackends->client.proxy_set_tag = (PFN_wl_proxy_set_tag)dlsym(s_WlBackends->client.handle, "wl_proxy_set_tag");
    s_WlBackends->client.proxy_get_version = (PFN_wl_proxy_get_version)dlsym(s_WlBackends->client.handle, "wl_proxy_get_version");
    s_WlBackends->client.proxy_marshal_flags = (PFN_wl_proxy_marshal_flags)dlsym(s_WlBackends->client.handle, "wl_proxy_marshal_flags");

    if (!s_WlBackends->client.display_connect ||
        !s_WlBackends->client.display_disconnect ||
        !s_WlBackends->client.display_flush ||
        !s_WlBackends->client.display_cancel_read ||
        !s_WlBackends->client.display_dispatch ||
        !s_WlBackends->client.display_dispatch_pending ||
        !s_WlBackends->client.display_read_events ||
        !s_WlBackends->client.display_roundtrip ||
        !s_WlBackends->client.display_get_fd ||
        !s_WlBackends->client.display_prepare_read ||
        !s_WlBackends->client.proxy_marshal ||
        !s_WlBackends->client.proxy_add_listener ||
        !s_WlBackends->client.proxy_destroy ||
        !s_WlBackends->client.proxy_marshal_constructor ||
        !s_WlBackends->client.proxy_marshal_constructor_versioned ||
        !s_WlBackends->client.proxy_get_user_data ||
        !s_WlBackends->client.proxy_set_user_data ||
        !s_WlBackends->client.proxy_get_tag ||
        !s_WlBackends->client.proxy_set_tag)
    {
        LVN_CORE_ERROR("[wayland] failed to load wayland funciton pointer symbols");
        return Lvn_Result_Failure;
    }

    s_WlBackends->display = wl_display_connect(nullptr);
    if (!s_WlBackends->display)
    {
        LVN_CORE_ERROR("[wayland] failed to connect to wayland display server");
        return Lvn_Result_Failure;
    }

    // load wayland-egl library symbols
    s_WlBackends->egl.handle = dlopen("libwayland-egl.so.1", RTLD_LAZY | RTLD_LOCAL);
    if (!s_WlBackends->egl.handle)
    {
        LVN_CORE_ERROR("[wayland] failed to load libwayland-egl shared library");
        return Lvn_Result_Failure;
    }

    s_WlBackends->egl.window_create = (PFN_wl_egl_window_create)dlsym(s_WlBackends->egl.handle, "wl_egl_window_create");
    s_WlBackends->egl.window_destroy = (PFN_wl_egl_window_destroy)dlsym(s_WlBackends->egl.handle, "wl_egl_window_destroy");
    s_WlBackends->egl.window_resize = (PFN_wl_egl_window_resize)dlsym(s_WlBackends->egl.handle, "wl_egl_window_resize");

    if (!s_WlBackends->egl.window_create ||
        !s_WlBackends->egl.window_destroy ||
        !s_WlBackends->egl.window_resize)
    {
        LVN_CORE_ERROR("[wayland] failed to load wayland-egl function pointer symbols");
        return Lvn_Result_Failure;
    }

    s_WlBackends->registry = wl_display_get_registry(s_WlBackends->display);
    if (!s_WlBackends->registry)
    {
        LVN_CORE_ERROR("[wayland] failed to get registry from wayland display");
        return Lvn_Result_Failure;
    }

    wl_registry_add_listener(s_WlBackends->registry, &wls::s_RegistryListener, s_WlBackends);

    wl_display_roundtrip(s_WlBackends->display);

    // load xkbcommon library symbols
    s_WlBackends->xkb.handle = dlopen("libxkbcommon.so.0", RTLD_LAZY | RTLD_LOCAL);
    if (!s_WlBackends->xkb.handle)
    {
        LVN_CORE_ERROR("[wayland] failed to load libxkbcommon shared library");
        return Lvn_Result_Failure;
    }

    s_WlBackends->xkb.context_new = (PFN_xkb_context_new)dlsym(s_WlBackends->xkb.handle, "xkb_context_new");
    s_WlBackends->xkb.context_unref = (PFN_xkb_context_unref)dlsym(s_WlBackends->xkb.handle, "xkb_context_unref");
    s_WlBackends->xkb.keymap_new_from_string = (PFN_xkb_keymap_new_from_string)dlsym(s_WlBackends->xkb.handle, "xkb_keymap_new_from_string");
    s_WlBackends->xkb.keymap_unref = (PFN_xkb_keymap_unref)dlsym(s_WlBackends->xkb.handle, "xkb_keymap_unref");
    s_WlBackends->xkb.keymap_mod_get_index = (PFN_xkb_keymap_mod_get_index)dlsym(s_WlBackends->xkb.handle, "xkb_keymap_mod_get_index");
    s_WlBackends->xkb.keymap_key_repeats = (PFN_xkb_keymap_key_repeats)dlsym(s_WlBackends->xkb.handle, "xkb_keymap_key_repeats");
    s_WlBackends->xkb.state_new = (PFN_xkb_state_new)dlsym(s_WlBackends->xkb.handle, "xkb_state_new");
    s_WlBackends->xkb.state_unref = (PFN_xkb_state_unref)dlsym(s_WlBackends->xkb.handle, "xkb_state_unref");
    s_WlBackends->xkb.state_key_get_syms = (PFN_xkb_state_key_get_syms)dlsym(s_WlBackends->xkb.handle, "xkb_state_key_get_syms");
    s_WlBackends->xkb.state_update_mask = (PFN_xkb_state_update_mask)dlsym(s_WlBackends->xkb.handle, "xkb_state_update_mask");
    s_WlBackends->xkb.state_mod_index_is_active = (PFN_xkb_state_mod_index_is_active)dlsym(s_WlBackends->xkb.handle, "xkb_state_mod_index_is_active");
    s_WlBackends->xkb.keysym_to_utf32 = (PFN_xkb_keysym_to_utf32)dlsym(s_WlBackends->xkb.handle, "xkb_keysym_to_utf32");
    s_WlBackends->xkb.compose_table_new_from_locale = (PFN_xkb_compose_table_new_from_locale)dlsym(s_WlBackends->xkb.handle, "xkb_compose_table_new_from_locale");
    s_WlBackends->xkb.compose_table_unref = (PFN_xkb_compose_table_unref)dlsym(s_WlBackends->xkb.handle, "xkb_compose_table_unref");
    s_WlBackends->xkb.compose_state_new = (PFN_xkb_compose_state_new)dlsym(s_WlBackends->xkb.handle, "xkb_compose_state_new");
    s_WlBackends->xkb.compose_state_unref = (PFN_xkb_compose_state_unref)dlsym(s_WlBackends->xkb.handle, "xkb_compose_state_unref");
    s_WlBackends->xkb.compose_state_feed = (PFN_xkb_compose_state_feed)dlsym(s_WlBackends->xkb.handle, "xkb_compose_state_feed");
    s_WlBackends->xkb.compose_state_get_status = (PFN_xkb_compose_state_get_status)dlsym(s_WlBackends->xkb.handle, "xkb_compose_state_get_status");
    s_WlBackends->xkb.compose_state_get_one_sym = (PFN_xkb_compose_state_get_one_sym)dlsym(s_WlBackends->xkb.handle, "xkb_compose_state_get_one_sym");

    if (!s_WlBackends->xkb.context_new ||
        !s_WlBackends->xkb.context_unref ||
        !s_WlBackends->xkb.keymap_new_from_string ||
        !s_WlBackends->xkb.keymap_unref ||
        !s_WlBackends->xkb.keymap_mod_get_index ||
        !s_WlBackends->xkb.keymap_key_repeats ||
        !s_WlBackends->xkb.state_new ||
        !s_WlBackends->xkb.state_unref ||
        !s_WlBackends->xkb.state_key_get_syms ||
        !s_WlBackends->xkb.state_update_mask ||
        !s_WlBackends->xkb.state_mod_index_is_active ||
        !s_WlBackends->xkb.keysym_to_utf32 ||
        !s_WlBackends->xkb.compose_table_new_from_locale ||
        !s_WlBackends->xkb.compose_table_unref ||
        !s_WlBackends->xkb.compose_state_new ||
        !s_WlBackends->xkb.compose_state_unref ||
        !s_WlBackends->xkb.compose_state_feed ||
        !s_WlBackends->xkb.compose_state_get_status ||
        !s_WlBackends->xkb.compose_state_get_one_sym)
    {
        LVN_CORE_ERROR("[wayland] failed to load xkbcommon function pointer symbols");
        return Lvn_Result_Failure;
    }

    s_WlBackends->xkb.context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    if (!s_WlBackends->xkb.context)
    {
        LVN_CORE_ERROR("[wayland] failed to create xkb context");
        return Lvn_Result_Failure;
    }

    s_WlBackends->keycodes[KEY_GRAVE]      = Lvn_KeyCode_GraveAccent;
    s_WlBackends->keycodes[KEY_1]          = Lvn_KeyCode_1;
    s_WlBackends->keycodes[KEY_2]          = Lvn_KeyCode_2;
    s_WlBackends->keycodes[KEY_3]          = Lvn_KeyCode_3;
    s_WlBackends->keycodes[KEY_4]          = Lvn_KeyCode_4;
    s_WlBackends->keycodes[KEY_5]          = Lvn_KeyCode_5;
    s_WlBackends->keycodes[KEY_6]          = Lvn_KeyCode_6;
    s_WlBackends->keycodes[KEY_7]          = Lvn_KeyCode_7;
    s_WlBackends->keycodes[KEY_8]          = Lvn_KeyCode_8;
    s_WlBackends->keycodes[KEY_9]          = Lvn_KeyCode_9;
    s_WlBackends->keycodes[KEY_0]          = Lvn_KeyCode_0;
    s_WlBackends->keycodes[KEY_SPACE]      = Lvn_KeyCode_Space;
    s_WlBackends->keycodes[KEY_MINUS]      = Lvn_KeyCode_Minus;
    s_WlBackends->keycodes[KEY_EQUAL]      = Lvn_KeyCode_Equal;
    s_WlBackends->keycodes[KEY_Q]          = Lvn_KeyCode_Q;
    s_WlBackends->keycodes[KEY_W]          = Lvn_KeyCode_W;
    s_WlBackends->keycodes[KEY_E]          = Lvn_KeyCode_E;
    s_WlBackends->keycodes[KEY_R]          = Lvn_KeyCode_R;
    s_WlBackends->keycodes[KEY_T]          = Lvn_KeyCode_T;
    s_WlBackends->keycodes[KEY_Y]          = Lvn_KeyCode_Y;
    s_WlBackends->keycodes[KEY_U]          = Lvn_KeyCode_U;
    s_WlBackends->keycodes[KEY_I]          = Lvn_KeyCode_I;
    s_WlBackends->keycodes[KEY_O]          = Lvn_KeyCode_O;
    s_WlBackends->keycodes[KEY_P]          = Lvn_KeyCode_P;
    s_WlBackends->keycodes[KEY_LEFTBRACE]  = Lvn_KeyCode_LeftBracket;
    s_WlBackends->keycodes[KEY_RIGHTBRACE] = Lvn_KeyCode_RightBracket;
    s_WlBackends->keycodes[KEY_A]          = Lvn_KeyCode_A;
    s_WlBackends->keycodes[KEY_S]          = Lvn_KeyCode_S;
    s_WlBackends->keycodes[KEY_D]          = Lvn_KeyCode_D;
    s_WlBackends->keycodes[KEY_F]          = Lvn_KeyCode_F;
    s_WlBackends->keycodes[KEY_G]          = Lvn_KeyCode_G;
    s_WlBackends->keycodes[KEY_H]          = Lvn_KeyCode_H;
    s_WlBackends->keycodes[KEY_J]          = Lvn_KeyCode_J;
    s_WlBackends->keycodes[KEY_K]          = Lvn_KeyCode_K;
    s_WlBackends->keycodes[KEY_L]          = Lvn_KeyCode_L;
    s_WlBackends->keycodes[KEY_SEMICOLON]  = Lvn_KeyCode_Semicolon;
    s_WlBackends->keycodes[KEY_APOSTROPHE] = Lvn_KeyCode_Apostrophe;
    s_WlBackends->keycodes[KEY_Z]          = Lvn_KeyCode_Z;
    s_WlBackends->keycodes[KEY_X]          = Lvn_KeyCode_X;
    s_WlBackends->keycodes[KEY_C]          = Lvn_KeyCode_C;
    s_WlBackends->keycodes[KEY_V]          = Lvn_KeyCode_V;
    s_WlBackends->keycodes[KEY_B]          = Lvn_KeyCode_B;
    s_WlBackends->keycodes[KEY_N]          = Lvn_KeyCode_N;
    s_WlBackends->keycodes[KEY_M]          = Lvn_KeyCode_M;
    s_WlBackends->keycodes[KEY_COMMA]      = Lvn_KeyCode_Comma;
    s_WlBackends->keycodes[KEY_DOT]        = Lvn_KeyCode_Period;
    s_WlBackends->keycodes[KEY_SLASH]      = Lvn_KeyCode_Slash;
    s_WlBackends->keycodes[KEY_BACKSLASH]  = Lvn_KeyCode_Backslash;
    s_WlBackends->keycodes[KEY_ESC]        = Lvn_KeyCode_Escape;
    s_WlBackends->keycodes[KEY_TAB]        = Lvn_KeyCode_Tab;
    s_WlBackends->keycodes[KEY_LEFTSHIFT]  = Lvn_KeyCode_LeftShift;
    s_WlBackends->keycodes[KEY_RIGHTSHIFT] = Lvn_KeyCode_RightShift;
    s_WlBackends->keycodes[KEY_LEFTCTRL]   = Lvn_KeyCode_LeftControl;
    s_WlBackends->keycodes[KEY_RIGHTCTRL]  = Lvn_KeyCode_RightControl;
    s_WlBackends->keycodes[KEY_LEFTALT]    = Lvn_KeyCode_LeftAlt;
    s_WlBackends->keycodes[KEY_RIGHTALT]   = Lvn_KeyCode_RightAlt;
    s_WlBackends->keycodes[KEY_LEFTMETA]   = Lvn_KeyCode_LeftSuper;
    s_WlBackends->keycodes[KEY_RIGHTMETA]  = Lvn_KeyCode_RightSuper;
    s_WlBackends->keycodes[KEY_COMPOSE]    = Lvn_KeyCode_Menu;
    s_WlBackends->keycodes[KEY_NUMLOCK]    = Lvn_KeyCode_NumLock;
    s_WlBackends->keycodes[KEY_CAPSLOCK]   = Lvn_KeyCode_CapsLock;
    s_WlBackends->keycodes[KEY_PRINT]      = Lvn_KeyCode_PrintScreen;
    s_WlBackends->keycodes[KEY_SCROLLLOCK] = Lvn_KeyCode_ScrollLock;
    s_WlBackends->keycodes[KEY_PAUSE]      = Lvn_KeyCode_Pause;
    s_WlBackends->keycodes[KEY_DELETE]     = Lvn_KeyCode_Delete;
    s_WlBackends->keycodes[KEY_BACKSPACE]  = Lvn_KeyCode_Backspace;
    s_WlBackends->keycodes[KEY_ENTER]      = Lvn_KeyCode_Enter;
    s_WlBackends->keycodes[KEY_HOME]       = Lvn_KeyCode_Home;
    s_WlBackends->keycodes[KEY_END]        = Lvn_KeyCode_End;
    s_WlBackends->keycodes[KEY_PAGEUP]     = Lvn_KeyCode_PageUp;
    s_WlBackends->keycodes[KEY_PAGEDOWN]   = Lvn_KeyCode_PageDown;
    s_WlBackends->keycodes[KEY_INSERT]     = Lvn_KeyCode_Insert;
    s_WlBackends->keycodes[KEY_LEFT]       = Lvn_KeyCode_Left;
    s_WlBackends->keycodes[KEY_RIGHT]      = Lvn_KeyCode_Right;
    s_WlBackends->keycodes[KEY_DOWN]       = Lvn_KeyCode_Down;
    s_WlBackends->keycodes[KEY_UP]         = Lvn_KeyCode_Up;
    s_WlBackends->keycodes[KEY_F1]         = Lvn_KeyCode_F1;
    s_WlBackends->keycodes[KEY_F2]         = Lvn_KeyCode_F2;
    s_WlBackends->keycodes[KEY_F3]         = Lvn_KeyCode_F3;
    s_WlBackends->keycodes[KEY_F4]         = Lvn_KeyCode_F4;
    s_WlBackends->keycodes[KEY_F5]         = Lvn_KeyCode_F5;
    s_WlBackends->keycodes[KEY_F6]         = Lvn_KeyCode_F6;
    s_WlBackends->keycodes[KEY_F7]         = Lvn_KeyCode_F7;
    s_WlBackends->keycodes[KEY_F8]         = Lvn_KeyCode_F8;
    s_WlBackends->keycodes[KEY_F9]         = Lvn_KeyCode_F9;
    s_WlBackends->keycodes[KEY_F10]        = Lvn_KeyCode_F10;
    s_WlBackends->keycodes[KEY_F11]        = Lvn_KeyCode_F11;
    s_WlBackends->keycodes[KEY_F12]        = Lvn_KeyCode_F12;
    s_WlBackends->keycodes[KEY_F13]        = Lvn_KeyCode_F13;
    s_WlBackends->keycodes[KEY_F14]        = Lvn_KeyCode_F14;
    s_WlBackends->keycodes[KEY_F15]        = Lvn_KeyCode_F15;
    s_WlBackends->keycodes[KEY_F16]        = Lvn_KeyCode_F16;
    s_WlBackends->keycodes[KEY_F17]        = Lvn_KeyCode_F17;
    s_WlBackends->keycodes[KEY_F18]        = Lvn_KeyCode_F18;
    s_WlBackends->keycodes[KEY_F19]        = Lvn_KeyCode_F19;
    s_WlBackends->keycodes[KEY_F20]        = Lvn_KeyCode_F20;
    s_WlBackends->keycodes[KEY_F21]        = Lvn_KeyCode_F21;
    s_WlBackends->keycodes[KEY_F22]        = Lvn_KeyCode_F22;
    s_WlBackends->keycodes[KEY_F23]        = Lvn_KeyCode_F23;
    s_WlBackends->keycodes[KEY_F24]        = Lvn_KeyCode_F24;
    s_WlBackends->keycodes[KEY_KPSLASH]    = Lvn_KeyCode_KP_Divide;
    s_WlBackends->keycodes[KEY_KPASTERISK] = Lvn_KeyCode_KP_Multiply;
    s_WlBackends->keycodes[KEY_KPMINUS]    = Lvn_KeyCode_KP_Subtract;
    s_WlBackends->keycodes[KEY_KPPLUS]     = Lvn_KeyCode_KP_Add;
    s_WlBackends->keycodes[KEY_KP0]        = Lvn_KeyCode_KP_0;
    s_WlBackends->keycodes[KEY_KP1]        = Lvn_KeyCode_KP_1;
    s_WlBackends->keycodes[KEY_KP2]        = Lvn_KeyCode_KP_2;
    s_WlBackends->keycodes[KEY_KP3]        = Lvn_KeyCode_KP_3;
    s_WlBackends->keycodes[KEY_KP4]        = Lvn_KeyCode_KP_4;
    s_WlBackends->keycodes[KEY_KP5]        = Lvn_KeyCode_KP_5;
    s_WlBackends->keycodes[KEY_KP6]        = Lvn_KeyCode_KP_6;
    s_WlBackends->keycodes[KEY_KP7]        = Lvn_KeyCode_KP_7;
    s_WlBackends->keycodes[KEY_KP8]        = Lvn_KeyCode_KP_8;
    s_WlBackends->keycodes[KEY_KP9]        = Lvn_KeyCode_KP_9;
    s_WlBackends->keycodes[KEY_KPDOT]      = Lvn_KeyCode_KP_Decimal;
    s_WlBackends->keycodes[KEY_KPEQUAL]    = Lvn_KeyCode_KP_Equal;
    s_WlBackends->keycodes[KEY_KPENTER]    = Lvn_KeyCode_KP_Enter;
    s_WlBackends->keycodes[KEY_102ND]      = Lvn_KeyCode_World2;

    // create key repeat fd
    s_WlBackends->keyRepeatTimerfd = -1;
    if (wl_seat_get_version(s_WlBackends->seat) >= WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION)
        s_WlBackends->keyRepeatTimerfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);

    // set wayland function pointer functions
    graphicsctx->windowapi = Lvn_WindowApi_Wayland;
    graphicsctx->createWindow = lvn::implWaylandCreateWindow;
    graphicsctx->destroyWindow = lvn::implWaylandDestroyWindow;
    graphicsctx->updateWindow = lvn::implWaylandUpdateWindow;
    graphicsctx->windowOpen = lvn::implWaylandWindowOpen;
    graphicsctx->windowPollEvents = lvn::implWaylandWindowPollEvents;
    graphicsctx->getDimensions = lvn::implWaylandGetDimensions;
    graphicsctx->getWindowWidth = lvn::implWaylandGetWindowWidth;
    graphicsctx->getWindowHeight = lvn::implWaylandGetWindowHeight;
    graphicsctx->setWindowVSync = lvn::implWaylandSetWindowVSync;
    graphicsctx->getWindowVSync = lvn::implWaylandGetWindowVSync;
    graphicsctx->getNativeWindow = lvn::implWaylandGetNativeWindow;
    graphicsctx->setWindowContextCurrent = lvn::implWaylandSetWindowContextCurrent;
    graphicsctx->getWindowRenderPass = lvn::implWaylandGetWindowRenderPass;
    graphicsctx->getNativeWindowApi = lvn::implWaylandGetNativeWindowApi;
    graphicsctx->keyPressed = lvn::implWaylandKeyPressed;
    graphicsctx->keyReleased = lvn::implWaylandKeyReleased;
    graphicsctx->mouseButtonPressed = lvn::implWaylandMouseButtonPressed;
    graphicsctx->mouseButtonReleased = lvn::implWaylandMouseButtonReleased;
    graphicsctx->getMousePos = lvn::implWaylandGetMousePos;
    graphicsctx->getMousePosPtr = lvn::implWaylandGetMousePosPtr;
    graphicsctx->getMouseX = lvn::implWaylandGetMouseX;
    graphicsctx->getMouseY = lvn::implWaylandGetMouseY;
    graphicsctx->setMouseCursor = lvn::implWaylandSetMouseCursor;
    graphicsctx->SetMouseInputMode = lvn::implWaylandSetMouseInputMode;
    graphicsctx->getWindowPos = lvn::implWaylandGetWindowPos;
    graphicsctx->getWindowPosPtr = lvn::implWaylandGetWindowPosPtr;
    graphicsctx->getWindowSize = lvn::implWaylandGetWindowSize;
    graphicsctx->getWindowSizePtr = lvn::implWaylandGetWindowSizePtr;

    return Lvn_Result_Success;
}

void implWaylandTerminateWindowContext()
{
    if (!s_WlBackends)
        return;

    // file desciptors
    if (s_WlBackends->keyRepeatTimerfd >= 0)
        close(s_WlBackends->keyRepeatTimerfd);

    // xkb
    if (s_WlBackends->xkb.composeState)
        xkb_compose_state_unref(s_WlBackends->xkb.composeState);
    if (s_WlBackends->xkb.keymap)
        xkb_keymap_unref(s_WlBackends->xkb.keymap);
    if (s_WlBackends->xkb.state)
        xkb_state_unref(s_WlBackends->xkb.state);
    if (s_WlBackends->xkb.context)
        xkb_context_unref(s_WlBackends->xkb.context);

    // wayland
    if (s_WlBackends->output)
        wl_output_release(s_WlBackends->output);
    if (s_WlBackends->keyboard)
        wl_keyboard_destroy(s_WlBackends->keyboard);
    if (s_WlBackends->seat)
        wl_seat_release(s_WlBackends->seat);
    if (s_WlBackends->xdg_wm_base)
        xdg_wm_base_destroy(s_WlBackends->xdg_wm_base);
    if (s_WlBackends->registry)
        wl_registry_destroy(s_WlBackends->registry);

    if (s_WlBackends->display)
    {
        wl_display_flush(s_WlBackends->display);
        wl_display_disconnect(s_WlBackends->display);
    }

    // shared library handles
    if (s_WlBackends->xkb.handle)
        dlclose(s_WlBackends->xkb.handle);
    if (s_WlBackends->egl.handle)
        dlclose(s_WlBackends->egl.handle);
    if (s_WlBackends->client.handle)
        dlclose(s_WlBackends->client.handle);

    lvn::memDelete<WaylandBackends>(s_WlBackends);
    s_WlBackends = nullptr;
}

LvnResult implWaylandCreateWindow(LvnWindow* window, const LvnWindowCreateInfo* createInfo)
{
    WaylandBackends* wlBackends = wls::getWaylandBackends();

    wl_surface* surface = wl_compositor_create_surface(wlBackends->compositor);
    if (!surface)
    {
        LVN_CORE_ERROR("[wayland] failed to create surface for window (%p)", window);
        return Lvn_Result_Failure;
    }
    window->nwdata.wl.surface = surface;

    uint8_t* pixels;
    wls::createShmBuffer(&pixels, &window->nwdata.wl.buffer, window->width, window->height);
    if (!window->nwdata.wl.buffer)
    {
        LVN_CORE_ERROR("[wayland] failed to create buffer for window (%p)", window);
        return Lvn_Result_Failure;
    }
    if (!pixels)
    {
        LVN_CORE_ERROR("[wayland] failed to create pool pixel data for window (%p)", window);
        return Lvn_Result_Failure;
    }
    window->nwdata.wl.pixels = pixels;

    // window->nwdata.wl.callback = wl_surface_frame(window->nwdata.wl.surface);
    // wl_callback_add_listener(window->nwdata.wl.callback, &s_CallbackListener, window);

    // xdg
    window->nwdata.wl.xdg.surface = xdg_wm_base_get_xdg_surface(wlBackends->xdg_wm_base, surface);
    if (!window->nwdata.wl.xdg.surface)
    {
        LVN_CORE_ERROR("[wayland] failed to create xdg-surface for window (%p)", window);
        return Lvn_Result_Failure;
    }

    xdg_surface_add_listener(window->nwdata.wl.xdg.surface, &wls::s_XdgSurfaceListener, window);


    window->nwdata.wl.xdg.toplevel = xdg_surface_get_toplevel(window->nwdata.wl.xdg.surface);
    if (!window->nwdata.wl.xdg.toplevel)
    {
        LVN_CORE_ERROR("[wayland] failed to create xdg-toplevel for window (%p)", window);
        return Lvn_Result_Failure;
    }

    xdg_toplevel_add_listener(window->nwdata.wl.xdg.toplevel, &wls::s_XdgToplevelListener, window);
    xdg_toplevel_set_title(window->nwdata.wl.xdg.toplevel, window->title.c_str());

    memset(pixels, 150, window->width * window->height * 4);
    wl_surface_attach(surface, window->nwdata.wl.buffer, 0, 0);
    wl_surface_damage(surface, 0, 0, UINT32_MAX, UINT32_MAX);
    wl_surface_commit(surface);

    wl_surface_set_user_data(window->nwdata.wl.surface, window);

    wl_display_roundtrip(wlBackends->display);

    return Lvn_Result_Success;
}

void implWaylandDestroyWindow(LvnWindow* window)
{
    if (window->nwdata.wl.xdg.toplevel)
        xdg_toplevel_destroy(window->nwdata.wl.xdg.toplevel);
    if (window->nwdata.wl.xdg.surface)
        xdg_surface_destroy(window->nwdata.wl.xdg.surface);

    if (window->nwdata.wl.callback)
        wl_callback_destroy(window->nwdata.wl.callback);
    if (window->nwdata.wl.pixels)
        munmap(window->nwdata.wl.pixels, window->width * window->height * 4);
    if (window->nwdata.wl.buffer)
        wl_buffer_destroy(window->nwdata.wl.buffer);
    if (window->nwdata.wl.surface)
        wl_surface_destroy(window->nwdata.wl.surface);
}


void implWaylandUpdateWindow(LvnWindow* window)
{
    wl_display_dispatch(s_WlBackends->display);

    WaylandBackends* wlBackends = wls::getWaylandBackends();

    // struct pollfd fds =
    // {
    //     wlBackends->keyRepeatTimerfd, POLLIN,
    // };
    //
    // double timeout = 0.0;
    // if (!wls::pollKeyRepeatEvent(&fds, 1, &timeout))
    // {
    //     // wl_display_cancel_read(wlBackends->display);
    //     return;
    // }
    //
    // if (fds.revents & POLLIN)
    // {
    //     uint64_t repeats;
    //
    //     if (read(wlBackends->keyRepeatTimerfd, &repeats, sizeof(repeats)) == 8)
    //     {
    //         for (uint64_t i = 0; i < repeats; i++)
    //         {
    //             wls::inputText(wlBackends, window, wlBackends->keyRepeatScancode, true);
    //         }
    //     }
    // }

    wls::resizeWindow(window, window->nwdata.wl.pendingWidth, window->nwdata.wl.pendingHeight);

}

bool implWaylandWindowOpen(LvnWindow* window)
{
    return window->windowOpen;
}

void implWaylandWindowPollEvents()
{
}

LvnPair<int> implWaylandGetDimensions(LvnWindow* window)
{

    return LvnPair<int>{0,0};
}

unsigned int implWaylandGetWindowWidth(LvnWindow* window)
{

    return 0;
}

unsigned int implWaylandGetWindowHeight(LvnWindow* window)
{

    return 0;
}

void implWaylandSetWindowVSync(LvnWindow* window, bool enable)
{

}

bool implWaylandGetWindowVSync(LvnWindow* window)
{

    return false;
}

void* implWaylandGetNativeWindow(LvnWindow* window)
{

    return nullptr;
}

void implWaylandSetWindowContextCurrent(LvnWindow* window)
{

}

LvnRenderPass* implWaylandGetWindowRenderPass(LvnWindow* window)
{

    return nullptr;
}


bool implWaylandKeyPressed(LvnWindow* window, int keycode)
{

    return false;
}

bool implWaylandKeyReleased(LvnWindow* window, int keycode)
{

    return false;
}

bool implWaylandMouseButtonPressed(LvnWindow* window, int button)
{

    return false;
}

bool implWaylandMouseButtonReleased(LvnWindow* window, int button)
{

    return false;
}


LvnPair<float> implWaylandGetMousePos(LvnWindow* window)
{

    return LvnPair<float>{0,0};
}

void implWaylandGetMousePosPtr(LvnWindow* window, float* xpos, float* ypos)
{

}

float implWaylandGetMouseX(LvnWindow* window)
{

    return 0;
}

float implWaylandGetMouseY(LvnWindow* window)
{

    return 0;
}

void implWaylandSetMouseCursor(LvnWindow* window, LvnMouseCursor cursor)
{

}

void implWaylandSetMouseInputMode(LvnWindow* window, LvnMouseInputMode mode)
{

}


LvnPair<int> implWaylandGetWindowPos(LvnWindow* window)
{

    return LvnPair<int>{0,0};
}

void implWaylandGetWindowPosPtr(LvnWindow* window, int* xpos, int* ypos)
{

}

LvnPair<int> implWaylandGetWindowSize(LvnWindow* window)
{

    return LvnPair<int>{0,0};
}

void implWaylandGetWindowSizePtr(LvnWindow* window, int* width, int* height)
{

}


LvnWindowApi implWaylandGetNativeWindowApi()
{
    return Lvn_WindowApi_Wayland;
}

} /* namespace lvn */
