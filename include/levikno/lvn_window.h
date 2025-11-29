#ifndef HG_LVN_WINDOW_H
#define HG_LVN_WINDOW_H

// [LAYOUT]: lvn_window.h
// ------------------------------------------------------------
//
// [SECTION]: Enums
// [SECTION]: Struct Declaration
// [SECTION]: Functions
// [SECTION]: Struct Implementation


#include "lvn_config.h"
#include "levikno.h"


// [SECTION]: Enums
// ------------------------------------------------------------

enum LvnEventType
{
    Lvn_EventType_None = 0,
    Lvn_EventType_KeyPressed,
    Lvn_EventType_KeyReleased,
    Lvn_EventType_KeyHold,
    Lvn_EventType_KeyTyped,
    Lvn_EventType_WindowCreated,
    Lvn_EventType_WindowDestroy,
    Lvn_EventType_WindowClose,
    Lvn_EventType_WindowResize,
    Lvn_EventType_WindowFramebufferResize,
    Lvn_EventType_WindowFocus,
    Lvn_EventType_WindowLostFocus,
    Lvn_EventType_WindowMoved,
    Lvn_EventType_MouseButtonPressed,
    Lvn_EventType_MouseButtonReleased,
    Lvn_EventType_MouseMoved,
    Lvn_EventType_MouseScrolled,
};

enum LvnEventCategory
{
    Lvn_EventCategory_Application   = (1U << 0),
    Lvn_EventCategory_Input         = (1U << 1),
    Lvn_EventCategory_Keyboard      = (1U << 2),
    Lvn_EventCategory_Mouse         = (1U << 3),
    Lvn_EventCategory_MouseButton   = (1U << 4),
    Lvn_EventCategory_Window        = (1U << 5),
};

// -- key code enums
enum LvnKeyCodes
{
    Lvn_KeyCode_Space           = 32,
    Lvn_KeyCode_Apostrophe      = 39,       /* ' */
    Lvn_KeyCode_Comma           = 44,       /* , */
    Lvn_KeyCode_Minus           = 45,       /* - */
    Lvn_KeyCode_Period          = 46,       /* . */
    Lvn_KeyCode_Slash           = 47,       /* / */
    Lvn_KeyCode_0               = 48,
    Lvn_KeyCode_1               = 49,
    Lvn_KeyCode_2               = 50,
    Lvn_KeyCode_3               = 51,
    Lvn_KeyCode_4               = 52,
    Lvn_KeyCode_5               = 53,
    Lvn_KeyCode_6               = 54,
    Lvn_KeyCode_7               = 55,
    Lvn_KeyCode_8               = 56,
    Lvn_KeyCode_9               = 57,
    Lvn_KeyCode_Semicolon       = 59,       /* ; */
    Lvn_KeyCode_Equal           = 61,       /* = */
    Lvn_KeyCode_A               = 65,
    Lvn_KeyCode_B               = 66,
    Lvn_KeyCode_C               = 67,
    Lvn_KeyCode_D               = 68,
    Lvn_KeyCode_E               = 69,
    Lvn_KeyCode_F               = 70,
    Lvn_KeyCode_G               = 71,
    Lvn_KeyCode_H               = 72,
    Lvn_KeyCode_I               = 73,
    Lvn_KeyCode_J               = 74,
    Lvn_KeyCode_K               = 75,
    Lvn_KeyCode_L               = 76,
    Lvn_KeyCode_M               = 77,
    Lvn_KeyCode_N               = 78,
    Lvn_KeyCode_O               = 79,
    Lvn_KeyCode_P               = 80,
    Lvn_KeyCode_Q               = 81,
    Lvn_KeyCode_R               = 82,
    Lvn_KeyCode_S               = 83,
    Lvn_KeyCode_T               = 84,
    Lvn_KeyCode_U               = 85,
    Lvn_KeyCode_V               = 86,
    Lvn_KeyCode_W               = 87,
    Lvn_KeyCode_X               = 88,
    Lvn_KeyCode_Y               = 89,
    Lvn_KeyCode_Z               = 90,
    Lvn_KeyCode_LeftBracket     = 91,       /* [ */
    Lvn_KeyCode_Backslash       = 92,       /* \ */
    Lvn_KeyCode_RightBracket    = 93,       /* ] */
    Lvn_KeyCode_GraveAccent     = 96,       /* ` */
    Lvn_KeyCode_World1          = 161,      /* non-US #1 */
    Lvn_KeyCode_World2          = 162,      /* non-US #2 */

    /* Function keys */
    Lvn_KeyCode_Escape          = 256,
    Lvn_KeyCode_Enter           = 257,
    Lvn_KeyCode_Tab             = 258,
    Lvn_KeyCode_Backspace       = 259,
    Lvn_KeyCode_Insert          = 260,
    Lvn_KeyCode_Delete          = 261,
    Lvn_KeyCode_Right           = 262,
    Lvn_KeyCode_Left            = 263,
    Lvn_KeyCode_Down            = 264,
    Lvn_KeyCode_Up              = 265,
    Lvn_KeyCode_PageUp          = 266,
    Lvn_KeyCode_PageDown        = 267,
    Lvn_KeyCode_Home            = 268,
    Lvn_KeyCode_End             = 269,
    Lvn_KeyCode_CapsLock        = 280,
    Lvn_KeyCode_ScrollLock      = 281,
    Lvn_KeyCode_NumLock         = 282,
    Lvn_KeyCode_PrintScreen     = 283,
    Lvn_KeyCode_Pause           = 284,
    Lvn_KeyCode_F1              = 290,
    Lvn_KeyCode_F2              = 291,
    Lvn_KeyCode_F3              = 292,
    Lvn_KeyCode_F4              = 293,
    Lvn_KeyCode_F5              = 294,
    Lvn_KeyCode_F6              = 295,
    Lvn_KeyCode_F7              = 296,
    Lvn_KeyCode_F8              = 297,
    Lvn_KeyCode_F9              = 298,
    Lvn_KeyCode_F10             = 299,
    Lvn_KeyCode_F11             = 300,
    Lvn_KeyCode_F12             = 301,
    Lvn_KeyCode_F13             = 302,
    Lvn_KeyCode_F14             = 303,
    Lvn_KeyCode_F15             = 304,
    Lvn_KeyCode_F16             = 305,
    Lvn_KeyCode_F17             = 306,
    Lvn_KeyCode_F18             = 307,
    Lvn_KeyCode_F19             = 308,
    Lvn_KeyCode_F20             = 309,
    Lvn_KeyCode_F21             = 310,
    Lvn_KeyCode_F22             = 311,
    Lvn_KeyCode_F23             = 312,
    Lvn_KeyCode_F24             = 313,
    Lvn_KeyCode_F25             = 314,
    Lvn_KeyCode_KP_0            = 320,
    Lvn_KeyCode_KP_1            = 321,
    Lvn_KeyCode_KP_2            = 322,
    Lvn_KeyCode_KP_3            = 323,
    Lvn_KeyCode_KP_4            = 324,
    Lvn_KeyCode_KP_5            = 325,
    Lvn_KeyCode_KP_6            = 326,
    Lvn_KeyCode_KP_7            = 327,
    Lvn_KeyCode_KP_8            = 328,
    Lvn_KeyCode_KP_9            = 329,
    Lvn_KeyCode_KP_Decimal      = 330,
    Lvn_KeyCode_KP_Divide       = 331,
    Lvn_KeyCode_KP_Multiply     = 332,
    Lvn_KeyCode_KP_Subtract     = 333,
    Lvn_KeyCode_KP_Add          = 334,
    Lvn_KeyCode_KP_Enter        = 335,
    Lvn_KeyCode_KP_Equal        = 336,
    Lvn_KeyCode_LeftShift       = 340,
    Lvn_KeyCode_LeftControl     = 341,
    Lvn_KeyCode_LeftAlt         = 342,
    Lvn_KeyCode_LeftSuper       = 343,
    Lvn_KeyCode_RightShift      = 344,
    Lvn_KeyCode_RightControl    = 345,
    Lvn_KeyCode_RightAlt        = 346,
    Lvn_KeyCode_RightSuper      = 347,
    Lvn_KeyCode_Menu            = 348,
};

enum LvnKeyMods
{
    Lvn_KeyMod_Control          = (1U << 0),
    Lvn_KeyMod_Alt              = (1U << 1),
    Lvn_KeyMod_Shift            = (1U << 3),
    Lvn_KeyMod_Super            = (1U << 4),
    Lvn_KeyMod_CapsLock         = (1U << 5),
    Lvn_KeyMod_NumLock          = (1U << 6),
};

// -- mouse button code enums
enum LvnMouseButtonCodes
{
    Lvn_MouseButton_1           = 0,
    Lvn_MouseButton_2           = 1,
    Lvn_MouseButton_3           = 2,
    Lvn_MouseButton_4           = 3,
    Lvn_MouseButton_5           = 4,
    Lvn_MouseButton_6           = 5,
    Lvn_MouseButton_7           = 6,
    Lvn_MouseButton_8           = 7,
    Lvn_MouseButton_Last        = Lvn_MouseButton_8,
    Lvn_MouseButton_Left        = Lvn_MouseButton_1,
    Lvn_MouseButton_Right       = Lvn_MouseButton_2,
    Lvn_MouseButton_Middle      = Lvn_MouseButton_3,
};

enum LvnMouseCursor
{
    Lvn_MouseCursor_Arrow,
    Lvn_MouseCursor_Ibeam,
    Lvn_MouseCursor_Crosshair,
    Lvn_MouseCursor_PointingHand,
    Lvn_MouseCursor_ResizeEW,
    Lvn_MouseCursor_ResizeNS,
    Lvn_MouseCursor_ResizeNWSE,
    Lvn_MouseCursor_ResizeNESW,
    Lvn_MouseCursor_ResizeAll,
    Lvn_MouseCursor_NotAllowed,
    Lvn_MouseCursor_HResize         = Lvn_MouseCursor_ResizeEW,
    Lvn_MouseCursor_VRrsize         = Lvn_MouseCursor_ResizeNS,
    Lvn_MouseCursor_Hand            = Lvn_MouseCursor_PointingHand,
};

enum LvnMouseInputMode
{
    Lvn_MouseInputMode_Normal,
    Lvn_MouseInputMode_Disable,
    Lvn_MouseInputMode_Hidden,
    Lvn_MouseInputMode_Captured,
};

enum LvnWindowApi
{
    Lvn_WindowApi_None = 0,
    Lvn_WindowApi_Win32,
    Lvn_WindowApi_Cocoa,
    Lvn_WindowApi_Wayland,
    Lvn_WindowApi_X11,
    Lvn_WindowApi_Glfw,

    Lvn_WindowApi_win32   = Lvn_WindowApi_Win32,
    Lvn_WindowApi_cocoa   = Lvn_WindowApi_Cocoa,
    Lvn_WindowApi_wayland = Lvn_WindowApi_Wayland,
    Lvn_WindowApi_x11     = Lvn_WindowApi_X11,
    Lvn_WindowApi_glfw    = Lvn_WindowApi_Glfw,
};


// [SECTION]: Struct Declaration
// ------------------------------------------------------------

struct LvnEvent;
struct LvnWindowContext;
struct LvnWindowContextCreateInfo;
struct LvnKeyHoldEvent;
struct LvnKeyPressedEvent;
struct LvnKeyReleasedEvent;
struct LvnKeyTypedEvent;
struct LvnMouseButtonPressedEvent;
struct LvnMouseButtonReleasedEvent;
struct LvnMouseMovedEvent;
struct LvnMouseScrolledEvent;
struct LvnRenderPass;
struct LvnWindow;
struct LvnWindowCloseEvent;
struct LvnWindowCreateInfo;
struct LvnWindowEvent;
struct LvnWindowFocusEvent;
struct LvnWindowFramebufferResizeEvent;
struct LvnWindowLostFocusEvent;
struct LvnWindowMovedEvent;
struct LvnWindowResizeEvent;


// [SECTION]: Functions
// ------------------------------------------------------------

namespace lvn
{
    // -- window functions
    LVN_API LvnResult                   initWindowContext(LvnWindowContextCreateInfo* createInfo);
    LVN_API void                        terminateWindowContext();
    LVN_API LvnWindowContext*           getWindowContext();
    LVN_API LvnWindowApi                getWindowApi();
    LVN_API const char*                 getWindowApiName();
    LVN_API LvnWindowApi                getNativeWindowApi();

    LVN_API LvnResult                   createWindow(LvnWindow** window, const LvnWindowCreateInfo* createInfo);
    LVN_API void                        destroyWindow(LvnWindow* window);
    LVN_API LvnWindowCreateInfo         configWindowInit(const char* title, int width, int height);

    LVN_API void                        windowUpdate(LvnWindow* window);
    LVN_API bool                        windowOpen(LvnWindow* window);
    LVN_API void                        windowPollEvents();
    LVN_API LvnPair<int>                windowGetDimensions(LvnWindow* window);
    LVN_API int                         windowGetWidth(LvnWindow* window);
    LVN_API int                         windowGetHeight(LvnWindow* window);
    LVN_API void                        windowSetEventCallback(LvnWindow* window, void (*callback)(LvnEvent*), void* userData);
    LVN_API void                        windowSetVSync(LvnWindow* window, bool enable);
    LVN_API bool                        windowGetVSync(LvnWindow* window);
    LVN_API void*                       windowGetNativeWindow(LvnWindow* window);
    LVN_API LvnRenderPass*              windowGetRenderPass(LvnWindow* window);
    LVN_API void                        windowSetContextCurrent(LvnWindow* window);

    // - Use these function within the call back function of LvnWindow (if set)
    LVN_API bool                        dispatchKeyHoldEvent(LvnEvent* event, bool(*func)(LvnKeyHoldEvent*, void*));
    LVN_API bool                        dispatchKeyPressedEvent(LvnEvent* event, bool(*func)(LvnKeyPressedEvent*, void*));
    LVN_API bool                        dispatchKeyReleasedEvent(LvnEvent* event, bool(*func)(LvnKeyReleasedEvent*, void*));
    LVN_API bool                        dispatchKeyTypedEvent(LvnEvent* event, bool(*func)(LvnKeyTypedEvent*, void*));
    LVN_API bool                        dispatchMouseButtonPressedEvent(LvnEvent* event, bool(*func)(LvnMouseButtonPressedEvent*, void*));
    LVN_API bool                        dispatchMouseButtonReleasedEvent(LvnEvent* event, bool(*func)(LvnMouseButtonReleasedEvent*, void*));
    LVN_API bool                        dispatchMouseMovedEvent(LvnEvent* event, bool(*func)(LvnMouseMovedEvent*, void*));
    LVN_API bool                        dispatchMouseScrolledEvent(LvnEvent* event, bool(*func)(LvnMouseScrolledEvent*, void*));
    LVN_API bool                        dispatchWindowCloseEvent(LvnEvent* event, bool(*func)(LvnWindowCloseEvent*, void*));
    LVN_API bool                        dispatchWindowFramebufferResizeEvent(LvnEvent* event, bool(*func)(LvnWindowFramebufferResizeEvent*, void*));
    LVN_API bool                        dispatchWindowFocusEvent(LvnEvent* event, bool(*func)(LvnWindowFocusEvent*, void*));
    LVN_API bool                        dispatchWindowLostFocusEvent(LvnEvent* event, bool(*func)(LvnWindowLostFocusEvent*, void*));
    LVN_API bool                        dispatchWindowMovedEvent(LvnEvent* event, bool(*func)(LvnWindowMovedEvent*, void*));
    LVN_API bool                        dispatchWindowResizeEvent(LvnEvent* event, bool(*func)(LvnWindowResizeEvent*, void*));

    // -- input functions
    // - Use to get user input (eg. keyboard, mouse, window input)

    LVN_API bool                        keyPressed(LvnWindow* window, int keycode);
    LVN_API bool                        keyReleased(LvnWindow* window, int keycode);
    LVN_API bool                        mouseButtonPressed(LvnWindow* window, int button);
    LVN_API bool                        mouseButtonReleased(LvnWindow* window, int button);

    LVN_API LvnPair<float>              mouseGetPos(LvnWindow* window);
    LVN_API void                        mouseGetPos(LvnWindow* window, float* xpos, float* ypos);
    LVN_API float                       mouseGetX(LvnWindow* window);
    LVN_API float                       mouseGetY(LvnWindow* window);
    LVN_API void                        mouseSetCursor(LvnWindow* window, LvnMouseCursor);
    LVN_API void                        mouseSetInputMode(LvnWindow* window, LvnMouseInputMode mode);

    LVN_API LvnPair<int>                windowGetPos(LvnWindow* window);
    LVN_API void                        windowGetPos(LvnWindow* window, int* xpos, int* ypos);
    LVN_API LvnPair<int>                windowGetSize(LvnWindow* window);
    LVN_API void                        windowGetSize(LvnWindow* window, int* width, int* height);
}


// [SECTION]: Struct Implementation
// ------------------------------------------------------------

// -- window struct impl
struct LvnKeyHoldEvent
{
    LvnEventType type;
    int category;
    const char* name;
    bool handled;

    int keyCode;
    bool repeat;
};

struct LvnKeyPressedEvent
{
    LvnEventType type;
    int category;
    const char* name;
    bool handled;

    int keyCode;
};

struct LvnKeyReleasedEvent
{
    LvnEventType type;
    int category;
    const char* name;
    bool handled;

    int keyCode;
};

struct LvnKeyTypedEvent
{
    LvnEventType type;
    int category;
    const char* name;
    bool handled;

    unsigned int key;
};

struct LvnMouseButtonPressedEvent
{
    LvnEventType type;
    int category;
    const char* name;
    bool handled;

    int buttonCode;
};

struct LvnMouseButtonReleasedEvent
{
    LvnEventType type;
    int category;
    const char* name;
    bool handled;

    int buttonCode;
};

struct LvnMouseMovedEvent
{
    LvnEventType type;
    int category;
    const char* name;
    bool handled;

    int x, y;
};

struct LvnMouseScrolledEvent
{
    LvnEventType type;
    int category;
    const char* name;
    bool handled;

    float x, y;
};

struct LvnWindowCloseEvent
{
    LvnEventType type;
    int category;
    const char* name;
    bool handled;
};

struct LvnWindowFocusEvent
{
    LvnEventType type;
    int category;
    const char* name;
    bool handled;
};

struct LvnWindowFramebufferResizeEvent
{
    LvnEventType type;
    int category;
    const char* name;
    bool handled;

    unsigned int width, height;
};

struct LvnWindowLostFocusEvent
{
    LvnEventType type;
    int category;
    const char* name;
    bool handled;
};

struct LvnWindowMovedEvent
{
    LvnEventType type;
    int category;
    const char* name;
    bool handled;

    int x, y;
};

struct LvnWindowResizeEvent
{
    LvnEventType type;
    int category;
    const char* name;
    bool handled;

    int width, height;
};

struct LvnWindowCreateInfo
{
    int width, height;                  // width and height of window
    LvnString title;                    // title of window
    int minWidth, minHeight;            // minimum width and height of window (set to 0 if not specified)
    int maxWidth, maxHeight;            // maximum width and height of window (set to -1 if not specified)
    bool fullscreen, resizable, vSync;  // sets window to fullscreen if true; enables window resizing if true; vSync controls window framerate, sets framerate to 60fps if true
    LvnImage* pIcons;                   // icon images used for window/app icon; pIcons can be stored in an array; pIcons will be ignored if set to null
    uint32_t iconCount;                 // iconCount is the number of icons in pIcons; if using only one icon, set iconCount to 1; if using an array of icons, set to length of array

    void (*eventCallBack)(LvnEvent*);   // set function ptr used as a callback to get events from this window
    void* userData;                     // pass a ptr of a variable or struct to use and get data during window callbacks
};

struct LvnWindowContextCreateInfo
{
    LvnWindowApi windowapi;
};

#endif /* !HG_LVN_GRAPHICS_H */
