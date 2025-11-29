#ifndef HG_LVN_WINDOW_INTERNAL_H
#define HG_LVN_WINDOW_INTERNAL_H

#include "lvn_window.h"
#include "levikno_internal.h"


struct LvnEvent
{
    LvnEventType type;
    int category;
    const char* name;
    bool handled;
    void* userData;

    struct Data
    {
        union
        {
            int x;
            double xd;
        };
        union
        {
            int y;
            double yd;
        };

        int code;
        unsigned int ucode;
        bool repeat;
    } data;
};

struct LvnNativeWindowData
{
    struct
    {
        void* nativeWindow;
    } win32;
    struct
    {
        void* nativeWindow;
    } cocoa;
    struct
    {
        struct wl_surface* surface;
        struct wl_buffer* buffer;
        struct wl_callback* callback;
        uint8_t* pixels;
        void* display;
        int pendingWidth, pendingHeight;

        struct
        {
            struct xdg_surface* surface;
            struct xdg_toplevel* toplevel;
        } xdg;
    } wl;
    struct
    {
        unsigned long nativeWindow;
        bool xcbSupport;
        bool xlibSupport;
    } x11;
};

struct LvnRenderPass
{
    void* nativeRenderPass;
};

struct LvnWindow
{
    int width, height;                   // width and height of window
    LvnString title;                     // title of window
    int minWidth, minHeight;             // minimum width and height of window
    int maxWidth, maxHeight;             // maximum width and height of window
    bool fullscreen, resizable, vSync;   // sets window to fullscreen; enables window resizing; vSync controls window framerate
    LvnVector<LvnImage> icons;           // icon images used for window
    void (*eventCallBackFn)(LvnEvent*);  // function ptr used as a callback to get events from this window
    void* userData;

    void* nativeWindow;                  // pointer to native window handle depending on window backend (eg. x11, wayland), if using glfw, GLFWwindow handle is used for this
    int nativeId;                        // window id for x11 windows
    LvnNativeWindowData nwdata;

    bool windowOpen;
    void* apiData;                       // graphics api related data tied to the window
    LvnRenderPass renderPass;            // vulkan render pass needed to retreive render pass pointer
};

struct LvnWindowContext
{
    LvnWindowApi                windowapi;

    LvnResult                   (*createWindow)(LvnWindow*, const LvnWindowCreateInfo*);
    void                        (*destroyWindow)(LvnWindow*);
    void                        (*updateWindow)(LvnWindow*);
    bool                        (*windowOpen)(LvnWindow*);
    void                        (*windowPollEvents)();
    LvnPair<int>                (*getDimensions)(LvnWindow*);
    unsigned int                (*getWindowWidth)(LvnWindow*);
    unsigned int                (*getWindowHeight)(LvnWindow*);
    void                        (*setWindowVSync)(LvnWindow*, bool);
    bool                        (*getWindowVSync)(LvnWindow*);
    void*                       (*getNativeWindow)(LvnWindow*);
    void                        (*setWindowContextCurrent)(LvnWindow*);
    LvnRenderPass*              (*getWindowRenderPass)(LvnWindow*);
    LvnWindowApi                (*getNativeWindowApi)();

    bool                        (*keyPressed)(LvnWindow*, int);
    bool                        (*keyReleased)(LvnWindow*, int);
    bool                        (*mouseButtonPressed)(LvnWindow*, int);
    bool                        (*mouseButtonReleased)(LvnWindow*, int);

    LvnPair<float>              (*getMousePos)(LvnWindow*);
    void                        (*getMousePosPtr)(LvnWindow*, float*, float*);
    float                       (*getMouseX)(LvnWindow*);
    float                       (*getMouseY)(LvnWindow*);
    void                        (*setMouseCursor)(LvnWindow*, LvnMouseCursor);
    void                        (*SetMouseInputMode)(LvnWindow*, LvnMouseInputMode);

    LvnPair<int>                (*getWindowPos)(LvnWindow*);
    void                        (*getWindowPosPtr)(LvnWindow*, int*, int*);
    LvnPair<int>                (*getWindowSize)(LvnWindow*);
    void                        (*getWindowSizePtr)(LvnWindow*, int*, int*);
};


namespace lvn
{
    const char* getWindowApiNameEnum(LvnWindowApi api);
}

#endif /* !HG_LVN_WINDOW_INTERNAL_H */
