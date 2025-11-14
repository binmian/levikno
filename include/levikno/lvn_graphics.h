#ifndef HG_LVN_GRAPHICS_H
#define HG_LVN_GRAPHICS_H

// [LAYOUT]:
// ------------------------------------------------------------
//
// [SECTION]: Enums
// [SECTION]: Struct Declaration
// [SECTION]: Functions
// [SECTION]: Struct Implementation


#include "lvn_config.h"
#include "levikno.h"
#include "lvn_lmath.h"


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

enum LvnGraphicsApi
{
    Lvn_GraphicsApi_None = 0,
    Lvn_GraphicsApi_OpenGL,
    Lvn_GraphicsApi_Vulkan,

    Lvn_GraphicsApi_opengl = Lvn_GraphicsApi_OpenGL,
    Lvn_GraphicsApi_vulkan = Lvn_GraphicsApi_Vulkan,
};

enum LvnPhysicalDeviceType
{
    Lvn_PhysicalDeviceType_Other           = 0,
    Lvn_PhysicalDeviceType_Integrated_GPU  = 1,
    Lvn_PhysicalDeviceType_Discrete_GPU    = 2,
    Lvn_PhysicalDeviceType_Virtual_GPU     = 3,
    Lvn_PhysicalDeviceType_CPU             = 4,

    Lvn_PhysicalDeviceType_Unknown = Lvn_PhysicalDeviceType_Other,
};

enum LvnBufferType
{
    Lvn_BufferType_Unknown  = 0,
    Lvn_BufferType_Vertex   = (1U << 0),
    Lvn_BufferType_Index    = (1U << 1),
    Lvn_BufferType_Uniform  = (1U << 2),
    Lvn_BufferType_Storage  = (1U << 3),
};
typedef uint32_t LvnBufferTypeFlagBits;

enum LvnBufferUsage
{
    Lvn_BufferUsage_Static,
    Lvn_BufferUsage_Dynamic,
    Lvn_BufferUsage_Resize,
};

enum LvnCullFaceMode
{
    Lvn_CullFaceMode_Front,
    Lvn_CullFaceMode_Back,
    Lvn_CullFaceMode_Both,
    Lvn_CullFaceMode_Disable,
};

enum LvnCullFrontFace
{
    Lvn_CullFrontFace_Clockwise,
    Lvn_CullFrontFace_CounterClockwise,

    Lvn_CullFrontFace_CW = Lvn_CullFrontFace_Clockwise,
    Lvn_CullFrontFace_CCW = Lvn_CullFrontFace_CounterClockwise,
};

enum LvnColorBlendFactor
{
    Lvn_ColorBlendFactor_Zero                   = 0,
    Lvn_ColorBlendFactor_One                    = 1,
    Lvn_ColorBlendFactor_SrcColor               = 2,
    Lvn_ColorBlendFactor_OneMinusSrcColor       = 3,
    Lvn_ColorBlendFactor_DstColor               = 4,
    Lvn_ColorBlendFactor_OneMinusDstColor       = 5,
    Lvn_ColorBlendFactor_SrcAlpha               = 6,
    Lvn_ColorBlendFactor_OneMinusSrcAlpha       = 7,
    Lvn_ColorBlendFactor_DstAlpha               = 8,
    Lvn_ColorBlendFactor_OneMinusDstAlpha       = 9,
    Lvn_ColorBlendFactor_ConstantColor          = 10,
    Lvn_ColorBlendFactor_OneMinusConstantColor  = 11,
    Lvn_ColorBlendFactor_ConstantAlpha          = 12,
    Lvn_ColorBlendFactor_OneMinusConstantAlpha  = 13,
    Lvn_ColorBlendFactor_SrcAlphaSaturate       = 14,
    Lvn_ColorBlendFactor_Src1Color              = 15,
    Lvn_ColorBlendFactor_OneMinusSrc1Color      = 16,
    Lvn_ColorBlendFactor_Src1_Alpha             = 17,
    Lvn_ColorBlendFactor_OneMinusSrc1Alpha      = 18,
};

enum LvnColorBlendOperation
{
    Lvn_ColorBlendOp_Add                 = 0,
    Lvn_ColorBlendOp_Subtract            = 1,
    Lvn_ColorBlendOp_ReverseSubtract     = 2,
    Lvn_ColorBlendOp_Min                 = 3,
    Lvn_ColorBlendOp_Max                 = 4,
};

enum LvnColorImageFormat
{
    Lvn_ColorImageFormat_None = 0,
    Lvn_ColorImageFormat_RGB,
    Lvn_ColorImageFormat_RGBA,
    Lvn_ColorImageFormat_RGBA8,
    Lvn_ColorImageFormat_RGBA16F,
    Lvn_ColorImageFormat_RGBA32F,
    Lvn_ColorImageFormat_SRGB,
    Lvn_ColorImageFormat_SRGBA,
    Lvn_ColorImageFormat_SRGBA8,
    Lvn_ColorImageFormat_SRGBA16F,
    Lvn_ColorImageFormat_SRGBA32F,
    Lvn_ColorImageFormat_RedInt,
};

enum LvnCompareOperation
{
    Lvn_CompareOp_Never          = 0,
    Lvn_CompareOp_Less           = 1,
    Lvn_CompareOp_Equal          = 2,
    Lvn_CompareOp_LessOrEqual    = 3,
    Lvn_CompareOp_Greater        = 4,
    Lvn_CompareOp_NotEqual       = 5,
    Lvn_CompareOp_GreaterOrEqual = 6,
    Lvn_CompareOp_Always         = 7,
};

enum LvnDepthImageFormat
{
    Lvn_DepthImageFormat_Depth16,
    Lvn_DepthImageFormat_Depth32,
    Lvn_DepthImageFormat_Depth24Stencil8,
    Lvn_DepthImageFormat_Depth32Stencil8,
};

enum LvnDescriptorType
{
    Lvn_DescriptorType_None = 0,
    Lvn_DescriptorType_ImageSampler,
    Lvn_DescriptorType_ImageSamplerBindless,
    Lvn_DescriptorType_UniformBuffer,
    Lvn_DescriptorType_StorageBuffer,
};

enum LvnSampleCount
{
    Lvn_SampleCount_1_Bit  = (1U << 0),
    Lvn_SampleCount_2_Bit  = (1U << 1),
    Lvn_SampleCount_4_Bit  = (1U << 2),
    Lvn_SampleCount_8_Bit  = (1U << 3),
    Lvn_SampleCount_16_Bit = (1U << 4),
    Lvn_SampleCount_32_Bit = (1U << 5),
    Lvn_SampleCount_64_Bit = (1U << 6),
};

enum LvnShaderStage
{
    Lvn_ShaderStage_All,
    Lvn_ShaderStage_Vertex,
    Lvn_ShaderStage_Fragment,
};

enum LvnStencilOperation
{
    Lvn_StencilOp_Keep              = 0,
    Lvn_StencilOp_Zero              = 1,
    Lvn_StencilOp_Replace           = 2,
    Lvn_StencilOp_IncrementAndClamp = 3,
    Lvn_StencilOp_DecrementAndClamp = 4,
    Lvn_StencilOp_Invert            = 5,
    Lvn_StencilOp_IncrementAndWrap  = 6,
    Lvn_StencilOp_DecrementAndWrap  = 7,
};

enum LvnTextureFilter
{
    Lvn_TextureFilter_Nearest,
    Lvn_TextureFilter_Linear,
};

enum LvnTextureFormat
{
    Lvn_TextureFormat_Unorm = 0,
    Lvn_TextureFormat_Srgb  = 1,
};

enum LvnTextureMode
{
    Lvn_TextureMode_Repeat,
    Lvn_TextureMode_MirrorRepeat,
    Lvn_TextureMode_ClampToEdge,
    Lvn_TextureMode_ClampToBorder,
};

enum LvnTopologyType
{
    Lvn_TopologyType_None = 0,
    Lvn_TopologyType_Point,
    Lvn_TopologyType_Line,
    Lvn_TopologyType_LineStrip,
    Lvn_TopologyType_Triangle,
    Lvn_TopologyType_TriangleStrip,
};

enum LvnAttributeFormat
{
    Lvn_AttributeFormat_Undefined = 0,
    Lvn_AttributeFormat_Scalar_f32,
    Lvn_AttributeFormat_Scalar_f64,
    Lvn_AttributeFormat_Scalar_i32,
    Lvn_AttributeFormat_Scalar_ui32,
    Lvn_AttributeFormat_Scalar_i8,
    Lvn_AttributeFormat_Scalar_ui8,
    Lvn_AttributeFormat_Vec2_f32,
    Lvn_AttributeFormat_Vec3_f32,
    Lvn_AttributeFormat_Vec4_f32,
    Lvn_AttributeFormat_Vec2_f64,
    Lvn_AttributeFormat_Vec3_f64,
    Lvn_AttributeFormat_Vec4_f64,
    Lvn_AttributeFormat_Vec2_i32,
    Lvn_AttributeFormat_Vec3_i32,
    Lvn_AttributeFormat_Vec4_i32,
    Lvn_AttributeFormat_Vec2_ui32,
    Lvn_AttributeFormat_Vec3_ui32,
    Lvn_AttributeFormat_Vec4_ui32,
    Lvn_AttributeFormat_Vec2_i8,
    Lvn_AttributeFormat_Vec3_i8,
    Lvn_AttributeFormat_Vec4_i8,
    Lvn_AttributeFormat_Vec2_ui8,
    Lvn_AttributeFormat_Vec3_ui8,
    Lvn_AttributeFormat_Vec4_ui8,
    Lvn_AttributeFormat_Vec2_n8,
    Lvn_AttributeFormat_Vec3_n8,
    Lvn_AttributeFormat_Vec4_n8,
    Lvn_AttributeFormat_Vec2_un8,
    Lvn_AttributeFormat_Vec3_un8,
    Lvn_AttributeFormat_Vec4_un8,
    Lvn_AttributeFormat_2_10_10_10_ile,
    Lvn_AttributeFormat_2_10_10_10_uile,
    Lvn_AttributeFormat_2_10_10_10_nle,
    Lvn_AttributeFormat_2_10_10_10_unle,
};

enum LvnInterpolationMode
{
    Lvn_InterpolationMode_Step,
    Lvn_InterpolationMode_Linear,
};

enum LvnAnimationPath
{
    Lvn_AnimationPath_Translation,
    Lvn_AnimationPath_Rotation,
    Lvn_AnimationPath_Scale,
};


// [SECTION]: Struct Declaration
// ------------------------------------------------------------

struct LvnAnimation;
struct LvnAnimationChannel;
struct LvnBuffer;
struct LvnBufferCreateInfo;
struct LvnCamera;
struct LvnCommandBuffer;
struct LvnCommandBufferCreateInfo;
struct LvnCommandPool;
struct LvnCubemap;
struct LvnCubemapCreateInfo;
struct LvnDescriptorBinding;
struct LvnDescriptorLayout;
struct LvnDescriptorLayoutCreateInfo;
struct LvnDescriptorSet;
struct LvnDescriptorUpdateInfo;
struct LvnEvent;
struct LvnFont;
struct LvnFontGlyph;
struct LvnFrameBuffer;
struct LvnFrameBufferColorAttachment;
struct LvnFrameBufferCreateInfo;
struct LvnFrameBufferDepthAttachment;
struct LvnGraphicsContext;
struct LvnGraphicsContextCreateInfo;
struct LvnImage;
struct LvnKeyHoldEvent;
struct LvnKeyPressedEvent;
struct LvnKeyReleasedEvent;
struct LvnKeyTypedEvent;
struct LvnMaterial;
struct LvnMesh;
struct LvnModel;
struct LvnMouseButtonPressedEvent;
struct LvnMouseButtonReleasedEvent;
struct LvnMouseMovedEvent;
struct LvnMouseScrolledEvent;
struct LvnNode;
struct LvnOrthoCamera;
struct LvnPhysicalDevice;
struct LvnPhysicalDeviceFeatures;
struct LvnPhysicalDeviceProperties;
struct LvnPipeline;
struct LvnPipelineColorBlend;
struct LvnPipelineColorBlendAttachment;
struct LvnPipelineColorWriteMask;
struct LvnPipelineCreateInfo;
struct LvnPipelineDepthStencil;
struct LvnPipelineFixedFunctions;
struct LvnPipelineInputAssembly;
struct LvnPipelineMultiSampling;
struct LvnPipelineRasterizer;
struct LvnPipelineScissor;
struct LvnPipelineStencilAttachment;
struct LvnPipelineViewport;
struct LvnPrimitive;
struct LvnRenderPass;
struct LvnSampler;
struct LvnSamplerCreateInfo;
struct LvnShader;
struct LvnShaderCreateInfo;
struct LvnShaderBinCreateInfo;
struct LvnSkin;
struct LvnTexture;
struct LvnTextureCreateInfo;
struct LvnUniformBufferInfo;
struct LvnVertexAttribute;
struct LvnVertexBindingDescription;
struct LvnWindow;
struct LvnWindowCloseEvent;
struct LvnWindowCreateInfo;
struct LvnWindowEvent;
struct LvnWindowFocusEvent;
struct LvnWindowFramebufferResizeEvent;
struct LvnWindowLostFocusEvent;
struct LvnWindowMovedEvent;
struct LvnWindowResizeEvent;

typedef LvnVec<4, uint8_t>             LvnColorImageData;

// [SECTION]: Functions
// ------------------------------------------------------------
// - Render functions with the prefix CmdDraw can only
//     be used during command recording
// - New graphics objects cannot be created or destroyed
//     during command recording

namespace lvn
{
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

    // -- window functions
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
    LVN_API LvnGraphicsApi              getGraphicsApi();
    LVN_API LvnResult                   initGraphicsContext(LvnGraphicsContextCreateInfo* createInfo);
    LVN_API void                        terminateGraphicsContext();
    LVN_API LvnGraphicsContext*         getGraphicsContext();


    // -- graphics functions
    LVN_API void                        getPhysicalDevices(LvnPhysicalDevice** pPhysicalDevices, uint32_t* deviceCount);
    LVN_API LvnPhysicalDeviceProperties getPhysicalDeviceProperties(LvnPhysicalDevice* physicalDevice);
    LVN_API LvnPhysicalDeviceFeatures   getPhysicalDeviceFeatures(LvnPhysicalDevice* physicalDevice);
    LVN_API LvnResult                   checkPhysicalDeviceSupport(LvnPhysicalDevice* physicalDevice);
    LVN_API LvnResult                   setPhysicalDevice(LvnPhysicalDevice* physicalDevice);

    LVN_API void                        renderCmdDraw(LvnCommandBuffer* cmdBuffer, uint32_t vertexCount);
    LVN_API void                        renderCmdDrawIndexed(LvnCommandBuffer* cmdBuffer, uint32_t indexCount);
    LVN_API void                        renderCmdDrawInstanced(LvnCommandBuffer* cmdBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance);
    LVN_API void                        renderCmdDrawIndexedInstanced(LvnCommandBuffer* cmdBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance);
    LVN_API void                        renderCmdSetStencilReference(uint32_t reference);
    LVN_API void                        renderCmdSetStencilMask(uint32_t compareMask, uint32_t writeMask);
    LVN_API void                        renderBeginNextFrame(LvnWindow* window, LvnCommandBuffer* cmdBuffer);                                                                          // begins the next frame of the window
    LVN_API void                        renderDrawSubmit(LvnWindow* window, LvnCommandBuffer* cmdBuffer);                                                                              // submits all draw commands recorded and presents to window
    LVN_API void                        renderBeginCommandRecording(LvnCommandBuffer* cmdBuffer);                                                                   // begins command buffer when recording draw commands start
    LVN_API void                        renderEndCommandRecording(LvnCommandBuffer* cmdBuffer);                                                                     // ends command buffer when finished recording draw commands
    LVN_API void                        renderCmdBeginRenderPass(LvnCommandBuffer* cmdBuffer, LvnWindow* window, float r, float g, float b, float a);                                  // begins renderpass when rendering starts
    LVN_API void                        renderCmdEndRenderPass(LvnCommandBuffer* cmdBuffer);                                                                        // ends renderpass when rendering has finished
    LVN_API void                        renderCmdBindPipeline(LvnCommandBuffer* cmdBuffer, LvnPipeline* pipeline);                                                  // bind a pipeline to begin shading during rendering
    LVN_API void                        renderCmdBindVertexBuffer(LvnCommandBuffer* cmdBuffer, uint32_t firstBinding, uint32_t bindingCount, LvnBuffer** pBuffers, uint64_t* pOffsets); // binds the vertex buffer within an LvnBuffer object
    LVN_API void                        renderCmdBindIndexBuffer(LvnCommandBuffer* cmdBuffer, LvnBuffer* buffer, uint64_t offset);                                  // binds the index buffer within an LvnBuffer object
    LVN_API void                        renderCmdBindDescriptorSets(LvnCommandBuffer* cmdBuffer, LvnPipeline* pipeline, uint32_t firstSetIndex, uint32_t descriptorSetCount, LvnDescriptorSet** pDescriptorSets); // bind multiple descriptor sets to the shader (if multiple sets are used), Note that descriptor sets must be in order to how the sets are ordered in the pipeline
    LVN_API void                        renderCmdBeginFrameBuffer(LvnCommandBuffer* cmdBuffer, LvnFrameBuffer* frameBuffer);                                        // begins the framebuffer for recording offscreen render calls, similar to beginning the render pass
    LVN_API void                        renderCmdEndFrameBuffer(LvnCommandBuffer* cmdBuffer, LvnFrameBuffer* frameBuffer);                                          // ends recording to the framebuffer

    LVN_API LvnResult                   createShaderFromSrc(LvnShader** shader, const LvnShaderCreateInfo* createInfo);                                   // create shader with the source code as input
    LVN_API LvnResult                   createShaderFromBin(LvnShader** shader, const LvnShaderBinCreateInfo* createInfo);                                // create shader with the source binary code as input
    LVN_API LvnResult                   createShaderFromFileBin(LvnShader** shader, const LvnShaderCreateInfo* createInfo);                               // create shader with the file paths to the binary files (.spv) as input
    LVN_API LvnResult                   createShaderFromFileSrc(LvnShader** shader, const LvnShaderCreateInfo* createInfo);                               // create shader with the file paths to the source files as input
    LVN_API LvnResult                   createDescriptorLayout(LvnDescriptorLayout** descriptorLayout, const LvnDescriptorLayoutCreateInfo* createInfo);  // create descriptor layout for the pipeline
    LVN_API LvnResult                   createPipeline(LvnPipeline** pipeline, const LvnPipelineCreateInfo* createInfo);                                  // create pipeline to describe rendering function
    LVN_API LvnResult                   createCommandPool(LvnCommandPool** cmdPool);                                                                      // create command pool to store command buffers
    LVN_API LvnResult                   createFrameBuffer(LvnFrameBuffer** frameBuffer, const LvnFrameBufferCreateInfo* createInfo);                      // create framebuffer to render images to
    LVN_API LvnResult                   createBuffer(LvnBuffer** buffer, const LvnBufferCreateInfo* createInfo);                                          // create a single buffer object that can hold both the vertex and index buffers
    LVN_API LvnResult                   createSampler(LvnSampler** sampler, const LvnSamplerCreateInfo* createInfo);                                      // create a sampler object to store texture sampler data
    LVN_API LvnResult                   createTexture(LvnTexture** texture, const LvnTextureCreateInfo* createInfo);                                      // create a texture object to store image data
    LVN_API LvnResult                   createCubemap(LvnCubemap** cubemap, const LvnCubemapCreateInfo* createInfo);                                      // create a cubemap texture object that holds the textures of the cubemap

    LVN_API void                        destroyShader(LvnShader* shader);                                                                                 // destroy shader module object
    LVN_API void                        destroyDescriptorLayout(LvnDescriptorLayout* descriptorLayout);                                                   // destroy descriptor layout
    LVN_API void                        destroyPipeline(LvnPipeline* pipeline);                                                                           // destroy pipeline object
    LVN_API void                        destroyCommandPool(LvnCommandPool* cmdPool);                                                                      // destroy command pool object
    LVN_API void                        destroyFrameBuffer(LvnFrameBuffer* frameBuffer);                                                                  // destroy framebuffer object
    LVN_API void                        destroyBuffer(LvnBuffer* buffer);                                                                                 // destory buffers object
    LVN_API void                        destroySampler(LvnSampler* sampler);                                                                              // destroy sampler object
    LVN_API void                        destroyTexture(LvnTexture* texture);                                                                              // destroy texture object
    LVN_API void                        destroyCubemap(LvnCubemap* cubemap);                                                                              // destroy cubemap object

    LVN_API uint32_t                    getAttributeFormatSize(LvnAttributeFormat format);
    LVN_API uint32_t                    getAttributeFormatComponentSize(LvnAttributeFormat format);
    LVN_API bool                        isAttributeFormatNormalizedType(LvnAttributeFormat format);
    LVN_API void                        pipelineFixedFuncSetConfig(LvnPipelineFixedFunctions* pipelineFixedFuncs);
    LVN_API LvnPipelineFixedFunctions   configPipelineFixedFuncInit();
    LVN_API LvnResult                   allocateCommandBuffers(LvnCommandPool* cmdPool, LvnCommandBuffer** pCmdBuffers, uint32_t count);                  // create command buffer to record render commands
    LVN_API LvnResult                   allocateDescriptorSets(LvnDescriptorLayout* descriptorLayout, LvnDescriptorSet** pDescriptorSet, uint32_t count); // create descriptor set to uplaod uniform data to pipeline

    LVN_API void                        bufferUpdateData(LvnBuffer* buffer, void* data, uint64_t size, uint64_t offset);
    LVN_API void                        bufferResize(LvnBuffer* buffer, uint64_t size);

    LVN_API LvnTexture*                 cubemapGetTextureData(LvnCubemap* cubemap);                                                                               // get the cubemap texture from the cubemap

    LVN_API void                        updateDescriptorSetData(LvnDescriptorSet* descriptorSet, LvnDescriptorUpdateInfo* pUpdateInfo, uint32_t count);           // update the descriptor content within a descroptor set

    LVN_API LvnTexture*                 frameBufferGetImage(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex);                                               // get the texture image data (render pass attachment) from the framebuffer via the attachment index
    LVN_API LvnRenderPass*              frameBufferGetRenderPass(LvnFrameBuffer* frameBuffer);                                                                    // get the render pass from the framebuffer
    LVN_API void                        frameBufferResize(LvnFrameBuffer* frameBuffer, uint32_t width, uint32_t height);                                          // update the width and height of the new framebuffer (updates the image data dimensions), Note: call only when the image dimensions need to be changed
    LVN_API void                        frameBufferSetClearColor(LvnFrameBuffer* frameBuffer, uint32_t attachmentIndex, float r, float g, float b, float a);      // set the background color for the framebuffer for offscreen rendering
    LVN_API LvnDepthImageFormat         findSupportedDepthImageFormat(LvnDepthImageFormat* pDepthImageFormats, uint32_t count);

    LVN_API LvnImage                    loadImageData(const char* filepath, int forceChannels = 0, bool flipVertically = false);
    LVN_API LvnImage                    loadImageDataMemory(const uint8_t* data, int length, int forceChannels = 0, bool flipVertically = false);
    LVN_API LvnImage                    loadImageDataThread(const LvnString filepath, int forceChannels = 0, bool flipVertically = false);
    LVN_API LvnImage                    loadImageDataMemoryThread(const uint8_t* data, int length, int forceChannels = 0, bool flipVertically = false);

    LVN_API LvnResult                   writeImagePng(const LvnImage& imageData, const char* filename);               // writes the image data into a png file with the filename/filepath
    LVN_API LvnResult                   writeImageJpg(const LvnImage& imageData, const char* filename, int quality);  // writes the image data into a jpg file with the filename/filepath and the jpg quality (from 0...100)
    LVN_API LvnResult                   writeImageBmp(const LvnImage& imageData, const char* filename);               // writes the image data into a bmp file with the filename/filepath

    LVN_API void                        imageFlipVertically(LvnImage& imageData);                                     // flips the image vertically
    LVN_API void                        imageFlipHorizontally(LvnImage& imageData);                                   // flips the image horizontally
    LVN_API void                        imageRotateCW(LvnImage& imageData);                                           // rotates the image clockwise (right)
    LVN_API void                        imageRotateCCW(LvnImage& imageData);                                          // rotates the image counter clockwise (left)

    LVN_API LvnImage                    imageGenColor(uint32_t width, uint32_t height, uint32_t channels, const LvnColorImageData& color);
    LVN_API LvnImage                    imageGenWhiteNoise(uint32_t width, uint32_t height, uint32_t channels);
    LVN_API LvnImage                    imageGenWhiteNoise(uint32_t width, uint32_t height, uint32_t channels, uint32_t seed);
    LVN_API LvnImage                    imageGenGrayScaleNoise(uint32_t width, uint32_t height, uint32_t channels);
    LVN_API LvnImage                    imageGenGrayScaleNoise(uint32_t width, uint32_t height, uint32_t channels, uint32_t seed);

    LVN_API LvnModel                    loadModel(const char* filepath);
    LVN_API void                        unloadModel(LvnModel* model);
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

// -- graphics struct impl
struct LvnPhysicalDeviceProperties
{
    LvnString name;
    LvnPhysicalDeviceType type;
    uint32_t apiVersion;
    uint32_t driverVersion;
    uint32_t vendorID;
};

struct LvnPhysicalDeviceFeatures
{
    bool robustBufferAccess;
    bool fullDrawIndexUint32;
    bool imageCubeArray;
    bool independentBlend;
    bool geometryShader;
    bool tessellationShader;
    bool sampleRateShading;
    bool dualSrcBlend;
    bool logicOp;
    bool multiDrawIndirect;
    bool drawIndirectFirstInstance;
    bool depthClamp;
    bool depthBiasClamp;
    bool fillModeNonSolid;
    bool depthBounds;
    bool wideLines;
    bool largePoints;
    bool alphaToOne;
    bool multiViewport;
    bool samplerAnisotropy;
    bool textureCompressionETC2;
    bool textureCompressionASTC_LDR;
    bool textureCompressionBC;
    bool occlusionQueryPrecise;
    bool pipelineStatisticsQuery;
    bool vertexPipelineStoresAndAtomics;
    bool fragmentStoresAndAtomics;
    bool shaderTessellationAndGeometryPointSize;
    bool shaderImageGatherExtended;
    bool shaderStorageImageExtendedFormats;
    bool shaderStorageImageMultisample;
    bool shaderStorageImageReadWithoutFormat;
    bool shaderStorageImageWriteWithoutFormat;
    bool shaderUniformBufferArrayDynamicIndexing;
    bool shaderSampledImageArrayDynamicIndexing;
    bool shaderStorageBufferArrayDynamicIndexing;
    bool shaderStorageImageArrayDynamicIndexing;
    bool shaderClipDistance;
    bool shaderCullDistance;
    bool shaderFloat64;
    bool shaderInt64;
    bool shaderInt16;
    bool shaderResourceResidency;
    bool shaderResourceMinLod;
    bool sparseBinding;
    bool sparseResidencyBuffer;
    bool sparseResidencyImage2D;
    bool sparseResidencyImage3D;
    bool sparseResidency2Samples;
    bool sparseResidency4Samples;
    bool sparseResidency8Samples;
    bool sparseResidency16Samples;
    bool sparseResidencyAliased;
    bool variableMultisampleRate;
    bool inheritedQueries;
};

struct LvnPipelineInputAssembly
{
    LvnTopologyType topology;
    bool primitiveRestartEnable;
};

// width and height are based on GLFW window framebuffer size
// Note: GLFW framebuffer size and window pixel coordinates may not be the same on different systems
// Set width and height to -1 if it does not need to be specified, width and height will then be automatically set to the framebuffer size
struct LvnPipelineViewport
{
    float x, y;
    float width, height;
    float minDepth, maxDepth;
};

struct LvnPipelineScissor
{
    struct { uint32_t x, y; } offset;
    struct { uint32_t width, height; } extent;
};

struct LvnPipelineRasterizer
{
    LvnCullFaceMode cullMode;
    LvnCullFrontFace frontFace;

    float lineWidth;
    float depthBiasConstantFactor;
    float depthBiasClamp;
    float depthBiasSlopeFactor;

    bool depthClampEnable;
    bool rasterizerDiscardEnable;
    bool depthBiasEnable;
};

struct LvnPipelineColorWriteMask
{
    bool colorComponentR;
    bool colorComponentG;
    bool colorComponentB;
    bool colorComponentA;
};

struct LvnPipelineMultiSampling
{
    LvnSampleCount rasterizationSamples;
    float minSampleShading;
    uint32_t* sampleMask;
    bool sampleShadingEnable;
    bool alphaToCoverageEnable;
    bool alphaToOneEnable;
};

struct LvnPipelineColorBlendAttachment
{
    LvnPipelineColorWriteMask colorWriteMask;
    LvnColorBlendFactor srcColorBlendFactor;
    LvnColorBlendFactor dstColorBlendFactor;
    LvnColorBlendOperation colorBlendOp;
    LvnColorBlendFactor srcAlphaBlendFactor;
    LvnColorBlendFactor dstAlphaBlendFactor;
    LvnColorBlendOperation alphaBlendOp;
    bool blendEnable;
};

struct LvnPipelineColorBlend
{
    LvnPipelineColorBlendAttachment* pColorBlendAttachments;
    uint32_t colorBlendAttachmentCount;
    float blendConstants[4];
    bool logicOpEnable;
};

struct LvnPipelineStencilAttachment
{
    LvnStencilOperation failOp;
    LvnStencilOperation passOp;
    LvnStencilOperation depthFailOp;
    LvnCompareOperation compareOp;
    uint32_t compareMask;
    uint32_t writeMask;
    uint32_t reference;
};

struct LvnPipelineDepthStencil
{
    LvnCompareOperation depthOpCompare;
    LvnPipelineStencilAttachment stencil;
    bool enableDepth, enableStencil;
};

struct LvnPipelineFixedFunctions
{
    LvnPipelineInputAssembly inputAssembly;
    LvnPipelineViewport viewport;
    LvnPipelineScissor scissor;
    LvnPipelineRasterizer rasterizer;
    LvnPipelineMultiSampling multisampling;
    LvnPipelineColorBlend colorBlend;
    LvnPipelineDepthStencil depthstencil;
};

struct LvnVertexBindingDescription
{
    uint32_t binding, stride;
};

struct LvnVertexAttribute
{
    uint32_t binding;
    uint32_t layout;
    LvnAttributeFormat format;
    uint64_t offset;
};

struct LvnDescriptorBinding
{
    uint32_t binding;
    LvnDescriptorType descriptorType;
    uint32_t descriptorCount;
    uint32_t maxAllocations;
    LvnShaderStage shaderStage;
};

struct LvnDescriptorLayoutCreateInfo
{
    LvnDescriptorBinding* pDescriptorBindings;
    uint32_t descriptorBindingCount;
    uint32_t maxSets;
};

struct LvnDescriptorUpdateInfo
{
    uint32_t binding;
    LvnDescriptorType descriptorType;
    uint32_t firstIndex;
    uint32_t descriptorCount;
    const LvnUniformBufferInfo* bufferInfo;
    const LvnTexture* const* pTextureInfos;
};

struct LvnPipelineCreateInfo
{
    LvnPipelineFixedFunctions* pipelineFixedFuncs;
    LvnVertexBindingDescription* pVertexBindingDescriptions;
    uint32_t vertexBindingDescriptionCount;
    LvnVertexAttribute* pVertexAttributes;
    uint32_t vertexAttributeCount;
    const LvnDescriptorLayout* const* pDescriptorLayouts;
    uint32_t descriptorLayoutCount;
    const LvnShader* shader;
    const LvnRenderPass* renderPass;
};

struct LvnShaderCreateInfo
{
    LvnString vertexSrc;
    LvnString fragmentSrc;
};

struct LvnShaderBinCreateInfo
{
    uint8_t* vertexBin;
    uint64_t vertexSize;
    uint8_t* fragmentBin;
    uint64_t fragmentSize;
};

struct LvnFrameBufferColorAttachment
{
    uint32_t index;
    LvnColorImageFormat format;
};

struct LvnFrameBufferDepthAttachment
{
    uint32_t index;
    LvnDepthImageFormat format;
};

struct LvnFrameBufferCreateInfo
{
    uint32_t width, height;
    LvnSampleCount sampleCount;
    LvnFrameBufferColorAttachment* pColorAttachments;
    uint32_t colorAttachmentCount;
    LvnFrameBufferDepthAttachment* depthAttachment;
    LvnTextureFilter textureFilter;
    LvnTextureMode textureMode;
};

struct LvnBufferCreateInfo
{
    LvnBufferTypeFlagBits type;
    LvnBufferUsage usage;
    uint64_t size;
    const void* data;
};

struct LvnUniformBufferInfo
{
    LvnBuffer* buffer;
    uint64_t range;
    uint64_t offset;
};

struct LvnImage
{
    LvnVector<uint8_t> pixels;
    uint32_t width, height, channels;
};

struct LvnSamplerCreateInfo
{
    LvnTextureFilter minFilter, magFilter;
    LvnTextureMode wrapS, wrapT, wrapR;
};

struct LvnTextureCreateInfo
{
    LvnImage imageData;
    LvnTextureFormat format;
    LvnSampler* sampler;
};

struct LvnTransform
{
    LvnVec3 translation;
    LvnQuat rotation;
    LvnVec3 scale;
};

struct LvnMaterial
{
    LvnVec3 baseColorFactor;
    LvnVec3 emissiveFactor;
    float metallicFactor;
    float roughnessFactor;

    LvnTexture* albedo;
    LvnTexture* metallicRoughnessOcclusion;
    LvnTexture* normal;
    LvnTexture* emissive;

    bool doubleSided;
};

struct LvnPrimitive
{
    LvnTopologyType topology;
    LvnMaterial material;
    uint32_t vertexCount;
    uint32_t indexCount;
    uint64_t indexOffset;

    LvnBuffer* buffer;
    LvnDescriptorSet* descriptorSet;
};

struct LvnMesh
{
    LvnVector<LvnPrimitive> primitives;
};

struct LvnNode
{
    int32_t parent;
    LvnVector<int32_t> children;

    int32_t mesh;
    int32_t skin;
    LvnTransform transform;
    LvnMat4 matrix;
};

struct LvnSkin
{
    LvnString name;
    LvnVector<LvnMat4> inverseBindMatrices;
    LvnVector<int32_t> joints;
    LvnBuffer* ssbo;
};

struct LvnAnimationChannel
{
    LvnAnimationPath path;
    LvnInterpolationMode interpolation;
    LvnVector<float> keyFrames;
    LvnVector<LvnVec4> outputs;
    int32_t node;
};

struct LvnAnimation
{
    LvnVector<LvnAnimationChannel> channels;
    float start;
    float end;
    float currentTime;
};

struct LvnModel
{
    LvnVector<int32_t> rootNodes;
    LvnVector<LvnNode> nodes;
    LvnVector<LvnMesh> meshes;
    LvnVector<LvnAnimation> animations;
    LvnVector<LvnSkin> skins;
    LvnVector<LvnBuffer*> buffers;
    LvnVector<LvnSampler*> samplers;
    LvnVector<LvnTexture*> textures;
    LvnMat4 matrix;
};

struct LvnCamera
{
    float aspectRatio;           // aspect ratio (width / height)
    float fov;                   // field of view
    float zNear;                 // near plane
    float zFar;                  // far plane
};

struct LvnOrthoCamera
{
    float right;                 // posx bound
    float left;                  // negx bound
    float top;                   // posy bound
    float bottom;                // negy bound
    float zNear;                 // far plane
    float zFar;                  // near plane
};

struct LvnCubemapCreateInfo
{
    LvnImage posx, negx, posy, negy, posz, negz;
};

struct LvnFontGlyph
{
    struct
    {
        float x0, y0, x1, y1;
    } uv;

    struct
    {
        float x, y;
    } size, bearing;

    uint32_t unicode;
    int advance;
};

struct LvnFont
{
    LvnImage atlas;
    float fontSize;

    LvnVector<uint32_t> codepoints;
    LvnVector<LvnFontGlyph> glyphs;
};

struct LvnGraphicsContextCreateInfo
{
    LvnWindowApi windowapi;
    LvnGraphicsApi graphicsapi;

    bool enableGraphicsApiDebugLogs;
    uint32_t maxFramesInFlight;
    LvnTextureFormat fbFormat;

};

#endif /* !HG_LVN_GRAPHICS_H */
