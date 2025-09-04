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

struct LvnWindowData
{                                        // [Same use with LvnWindowCreateinfo]
    int width, height;                   // width and height of window
    LvnString title;                     // title of window
    int minWidth, minHeight;             // minimum width and height of window
    int maxWidth, maxHeight;             // maximum width and height of window
    bool fullscreen, resizable, vSync;   // sets window to fullscreen; enables window resizing; vSync controls window framerate
    LvnWindowIconData* pIcons;           // icon images used for window/app icon
    uint32_t iconCount;                  // iconCount is the number of icons in pIcons
    void (*eventCallBackFn)(LvnEvent*);  // function ptr used as a callback to get events from this window
    void* userData;
};

struct LvnRenderPass
{
    void* nativeRenderPass;
};

struct LvnWindow
{
    LvnWindowData data;              // holds data of window (eg. width, height)
    void* nativeWindow;              // pointer to window api handle (eg. GLFWwindow)
    void* apiData;                   // used for graphics api related uses
    LvnRenderPass renderPass;        // pointer to native render pass for this window (vulkan)
    uint32_t topologyTypeEnum;       // topologyType used to render primitives (opengl)
    uint32_t vao;                    // vertex array object per pipeline object (opengl)
    uint32_t indexOffset;            // index offset when binding index buffer (opengl)
    LvnHashMap<uint32_t, uint32_t>* bindingDescriptions;
    LvnVector<uint8_t> cmdBuffer;    // command buffer to store draw commands in byte data
};

struct LvnWindowContext
{
    LvnWindowApi        windowapi;    /* window api enum */
    LvnGraphicsApi      graphicsBackend;

    LvnResult           (*implInitWindowContext)(LvnWindowContext*);
    void                (*implTerminateWindowContext)();

    LvnResult           (*createWindow)(LvnWindow*, const LvnWindowCreateInfo*);
    void                (*destroyWindow)(LvnWindow*);
    void                (*updateWindow)(LvnWindow*);
    bool                (*windowOpen)(LvnWindow*);
    void                (*windowPollEvents)();
    LvnPair<int>        (*getDimensions)(LvnWindow*);
    unsigned int        (*getWindowWidth)(LvnWindow*);
    unsigned int        (*getWindowHeight)(LvnWindow*);
    void                (*setWindowVSync)(LvnWindow*, bool);
    bool                (*getWindowVSync)(LvnWindow*);
    void                (*setWindowContextCurrent)(LvnWindow*);

    bool                (*keyPressed)(LvnWindow*, int);
    bool                (*keyReleased)(LvnWindow*, int);
    bool                (*mouseButtonPressed)(LvnWindow*, int);
    bool                (*mouseButtonReleased)(LvnWindow*, int);

    LvnPair<float>      (*getMousePos)(LvnWindow*);
    void                (*getMousePosPtr)(LvnWindow*, float*, float*);
    float               (*getMouseX)(LvnWindow*);
    float               (*getMouseY)(LvnWindow*);
    void                (*setMouseCursor)(LvnWindow*, LvnMouseCursor);
    void                (*SetMouseInputMode)(LvnWindow*, LvnMouseInputMode);

    LvnPair<int>        (*getWindowPos)(LvnWindow*);
    void                (*getWindowPosPtr)(LvnWindow*, int*, int*);
    LvnPair<int>        (*getWindowSize)(LvnWindow*);
    void                (*getWindowSizePtr)(LvnWindow*, int*, int*);
};

#endif /* !HG_LVN_WINDOW_INTERNAL_H */
