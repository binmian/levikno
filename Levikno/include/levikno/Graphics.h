#ifndef HG_LEVIKNO_GRAPHICS_H
#define HG_LEVIKNO_GRAPHICS_H

#ifndef HG_LEVIKNO_DEFINE_CONFIG
#define HG_LEVIKNO_DEFINE_CONFIG

// Platform
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) 
	#ifndef LVN_PLATFORM_WINDOWS
		#define LVN_PLATFORM_WINDOWS
	#endif
	#ifdef LVN_SHARED_LIBRARY_EXPORT
			#define LVN_API __declspec(dllexport)
	#elif LVN_SHARED_LIBRARY_IMPORT
			#define LVN_API __declspec(dllimport)
	#else 
		#define LVN_API
	#endif

#elif __APPLE__
	#define LVN_PLATFORM_APPLE

#elif __linux__
	#define LVN_PLATFORM_LINUX

#else
	#error "lvn does not support the current platform."
#endif

// Compiler
#ifdef _MSC_VER
	#define LVN_ASSERT_BREAK __debugbreak()
	#pragma warning (disable : 4267)
	#pragma warning (disable : 4244)
	#pragma warning (disable : 26495)

	#ifdef _DEBUG
		#ifndef LVN_DEBUG
			#define LVN_DEBUG
		#endif
	#endif
#else
	#define LVN_ASSERT_BREAK assert(false);
#endif

// Debug
#ifdef LVN_DEBUG
	#define LVN_ENABLE_ASSERTS 
#endif

#ifdef LVN_DISABLE_ASSERTS_KEEP_ERROR_MESSAGES
	#define LVN_DISABLE_ASSERTS
#endif

#if defined (LVN_DISABLE_ASSERTS)
	#define LVN_ASSERT(x, ...) { if(!(x)) { LVN_ERROR(LVN_LOG_FILE##__VA_ARGS__); } }
	#define LVN_CORE_ASSERT(x, ...) { if(!(x)) { LVN_CORE_ERROR(LVN_LOG_FILE##__VA_ARGS__); } }
#elif defined(LVN_ENABLE_ASSERTS)
	#define LVN_ASSERT(x, ...) { if(!(x)) { LVN_ERROR(LVN_LOG_FILE##__VA_ARGS__); LVN_ASSERT_BREAK; } }
	#define LVN_CORE_ASSERT(x, ...) { if(!(x)) { LVN_CORE_ERROR(LVN_LOG_FILE##__VA_ARGS__); LVN_ASSERT_BREAK; } }
#else
	#define LVN_ASSERT(x, ...)
	#define LVN_CORE_ASSERT(x, ...)
#endif

// Warnings
#ifdef LVN_DEBUG

#endif


#define LVN_TRUE 1
#define LVN_FALSE 0
#define LVN_NULL_HANDLE nullptr

#define LVN_UINT8_MAX  0xff
#define LVN_UINT16_MAX 0xffff
#define LVN_UINT32_MAX 0xffffffff
#define LVN_UINT64_MAX 0xffffffffffffffff

#define LVN_ALIGN(x) alignas(x)
#define LVN_SCAST(type, x) static_cast<type>(x)

#define LVN_MALLOC(type, size) static_cast<type>(malloc(size))
#define LVN_FREE(mem) free(mem)

#define LVN_FILE_NAME __FILE__
#define LVN_LINE __LINE__
#define LVN_FUNC_NAME __func__

#define LVN_STR(x) #x
#define LVN_STRINGIFY(x) LVN_STR(x)


#endif // !HG_LEVIKNO_DEFINE_CONFIG

#include <stdint.h>


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
	Lvn_ColorBlendFactor_Zero					= 0,
	Lvn_ColorBlendFactor_One					= 1,
	Lvn_ColorBlendFactor_SrcColor				= 2,
	Lvn_ColorBlendFactor_OneMinusSrcColor		= 3,
	Lvn_ColorBlendFactor_DstColor				= 4,
	Lvn_ColorBlendFactor_OneMinusDstColor		= 5,
	Lvn_ColorBlendFactor_SrcAlpha				= 6,
	Lvn_ColorBlendFactor_OneMinusSrcAlpha		= 7,
	Lvn_ColorBlendFactor_DstAlpha				= 8,
	Lvn_ColorBlendFactor_OneMinusDstAlpha		= 9,
	Lvn_ColorBlendFactor_ConstantColor			= 10,
	Lvn_ColorBlendFactor_OneMinusConstantColor	= 11,
	Lvn_ColorBlendFactor_ConstantAlpha			= 12,
	Lvn_ColorBlendFactor_OneMinusConstantAlpha	= 13,
	Lvn_ColorBlendFactor_SrcAlphaSaturate		= 14,
	Lvn_ColorBlendFactor_Src1Color				= 15,
	Lvn_ColorBlendFactor_OneMinusSrc1Color		= 16,
	Lvn_ColorBlendFactor_Src1_Alpha				= 17,
	Lvn_ColorBlendFactor_OneMinusSrc1Alpha		= 18,
};

enum LvnColorBlendOperation
{
	Lvn_ColorBlendOperation_Add					= 0,
	Lvn_ColorBlendOperation_Subtract			= 1,
	Lvn_ColorBlendOperation_ReverseSubtract		= 2,
	Lvn_ColorBlendOperation_Min					= 3,
	Lvn_ColorBlendOperation_Max					= 4,
	Lvn_ColorBlendOperation_ZeroExt				= 1000148000,
	Lvn_ColorBlendOperation_SrcExt				= 1000148001,
	Lvn_ColorBlendOperation_DstExt				= 1000148002,
	Lvn_ColorBlendOperation_SrcOverExt			= 1000148003,
	Lvn_ColorBlendOperation_DstOverExt			= 1000148004,
	Lvn_ColorBlendOperation_SrcInExt			= 1000148005,
	Lvn_ColorBlendOperation_DstInExt			= 1000148006,
	Lvn_ColorBlendOperation_SrcOutExt			= 1000148007,
	Lvn_ColorBlendOperation_DstOutExt			= 1000148008,
	Lvn_ColorBlendOperation_SrcAtopExt			= 1000148009,
	Lvn_ColorBlendOperation_DstAtopExt			= 1000148010,
	Lvn_ColorBlendOperation_XorExt				= 1000148011,
	Lvn_ColorBlendOperation_MultiplyExt			= 1000148012,
	Lvn_ColorBlendOperation_ScreenExt			= 1000148013,
	Lvn_ColorBlendOperation_OverlayExt			= 1000148014,
	Lvn_ColorBlendOperation_DarkenExt			= 1000148015,
	Lvn_ColorBlendOperation_LightenExt			= 1000148016,
	Lvn_ColorBlendOperation_ColorDodgeExt		= 1000148017,
	Lvn_ColorBlendOperation_ColorBurnExt		= 1000148018,
	Lvn_ColorBlendOperation_HardLightExt		= 1000148019,
	Lvn_ColorBlendOperation_SoftLightExt		= 1000148020,
	Lvn_ColorBlendOperation_DifferenceExt		= 1000148021,
	Lvn_ColorBlendOperation_ExclusionExt		= 1000148022,
	Lvn_ColorBlendOperation_InvertExt			= 1000148023,
	Lvn_ColorBlendOperation_InvertRGBExt		= 1000148024,
	Lvn_ColorBlendOperation_LinearDodgeExt		= 1000148025,
	Lvn_ColorBlendOperation_LinearBurnExt		= 1000148026,
	Lvn_ColorBlendOperation_VividLightExt		= 1000148027,
	Lvn_ColorBlendOperation_LinearLightExt		= 1000148028,
	Lvn_ColorBlendOperation_PinLightExt			= 1000148029,
	Lvn_ColorBlendOperation_HardMixExt			= 1000148030,
	Lvn_ColorBlendOperation_HSLHueExt			= 1000148031,
	Lvn_ColorBlendOperation_HSLSaturationExt	= 1000148032,
	Lvn_ColorBlendOperation_HSLColorExt			= 1000148033,
	Lvn_ColorBlendOperation_HSLLuminosityExt	= 1000148034,
	Lvn_ColorBlendOperation_PlusExt				= 1000148035,
	Lvn_ColorBlendOperation_PlusClampedExt		= 1000148036,
	Lvn_ColorBlendOperation_PlusClampedAlphaExt	= 1000148037,
	Lvn_ColorBlendOperation_PlusDarkerExt		= 1000148038,
	Lvn_ColorBlendOperation_MinusExt			= 1000148039,
	Lvn_ColorBlendOperation_MinusClampedExt		= 1000148040,
	Lvn_ColorBlendOperation_ContrastExt			= 1000148041,
	Lvn_ColorBlendOperation_InvertOVGExt		= 1000148042,
	Lvn_ColorBlendOperation_RedExt				= 1000148043,
	Lvn_ColorBlendOperation_GreenExt			= 1000148044,
	Lvn_ColorBlendOperation_BlueExt				= 1000148045,
};

enum LvnCompareOperation
{
	Lvn_CompareOperation_Never			= 0,
	Lvn_CompareOperation_Less			= 1,
	Lvn_CompareOperation_Equal			= 2,
	Lvn_CompareOperation_LessOrEqual	= 3,
	Lvn_CompareOperation_Greater		= 4,
	Lvn_CompareOperation_NotEqual		= 5,
	Lvn_CompareOperation_GreaterOrEqual = 6,
	Lvn_CompareOperation_Always			= 7,
};

enum LvnFrameBufferColorFormat
{
	Lvn_FrameBufferColorFormat_None = 0,
	Lvn_FrameBufferColorFormat_RGB,
	Lvn_FrameBufferColorFormat_RGBA,
	Lvn_FrameBufferColorFormat_RGBA8,
	Lvn_FrameBufferColorFormat_RGBA16F,
	Lvn_FrameBufferColorFormat_RGBA32F,
	Lvn_FrameBufferColorFormat_RedInt,
};

enum LvnFrameBufferDepthFormat
{
	Lvn_FrameBufferDepthFormat_None = 0,
	Lvn_FrameBufferDepthFormat_DepthComponent,
	Lvn_FrameBufferDepthFormat_Depth24Stencil8,
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
	Lvn_PhysicalDeviceType_Other = 0,
	Lvn_PhysicalDeviceType_Integrated_GPU = 1,
	Lvn_PhysicalDeviceType_Discrete_GPU = 2,
	Lvn_PhysicalDeviceType_Virtual_GPU = 3,
	Lvn_PhysicalDeviceType_CPU = 4,

	Lvn_PhysicalDeviceType_Unknown = Lvn_PhysicalDeviceType_Other,
};

enum LvnSampleCount
{
	Lvn_SampleCount_1_Bit = 0x00000001,
	Lvn_SampleCount_2_Bit = 0x00000002,
	Lvn_SampleCount_4_Bit = 0x00000004,
	Lvn_SampleCount_8_Bit = 0x00000008,
	Lvn_SampleCount_16_Bit = 0x00000010,
	Lvn_SampleCount_32_Bit = 0x00000020,
	Lvn_SampleCount_64_Bit = 0x00000040,
	Lvn_SampleCount_Max_Bit = 0x7FFFFFFF
};

enum LvnStencilOperation
{
	Lvn_StencilOperation_Keep = 0,
	Lvn_StencilOperation_Zero = 1,
	Lvn_StencilOperation_Replace = 2,
	Lvn_StencilOperation_IncrementAndClamp = 3,
	Lvn_StencilOperation_DecrementAndClamp = 4,
	Lvn_StencilOperation_Invert = 5,
	Lvn_StencilOperation_IncrementAndWrap = 6,
	Lvn_StencilOperation_DecrementAndWrap = 7,
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


struct LvnBatchCreateInfo;
struct LvnBatchRenderer;
struct LvnCamera;
struct LvnCharset;
struct LvnCubemap;
struct LvnCubemapCreateInfo;
struct LvnDrawCommand;
struct LvnDrawList;
struct LvnFont;
struct LvnFontGlyph;
struct LvnFontMetrics;
struct LvnFrameBuffer;
struct LvnFrameBufferColorAttachment;
struct LvnFrameBufferDepthAttachment;
struct LvnFrameBufferCreateInfo;
struct LvnGraphicsContext;
struct LvnIndexBuffer;
struct LvnOrthographicCamera;
struct LvnPhysicalDevice;
struct LvnPhysicalDeviceInfo;
struct LvnRendererBackends;
struct LvnShader;
struct LvnVertexArray;
struct LvnVertexArrayCreateInfo;
struct LvnVertexBuffer;
struct LvnVertexLayout;
struct LvnVertexLayoutLinkInfo;

typedef LvnCamera LvnPerspectiveCamera;
typedef LvnCamera LvnCamera3D;
typedef LvnOrthographicCamera LvnCamera2D;


/* [Data Types] */

/* [Vectors] */
template<typename T>
struct LvnVec2_t;
template<typename T>
struct LvnVec3_t;
template<typename T>
struct LvnVec4_t;

/* [Matricies] */
template<typename T>
struct LvnMat2x2_t;
template<typename T>
struct LvnMat3x3_t;
template<typename T>
struct LvnMat4x4_t;
template<typename T>
struct LvnMat2x3_t;
template<typename T>
struct LvnMat2x4_t;
template<typename T>
struct LvnMat3x2_t;
template<typename T>
struct LvnMat3x4_t;
template<typename T>
struct LvnMat4x2_t;
template<typename T>
struct LvnMat4x3_t;

template<typename T>
struct LvnVec2_t
{
	union { T x, r, s; };
	union { T y, g, t; };

	LvnVec2_t() {}
	LvnVec2_t(const T& n)
		: x(n), y(n) {}
	LvnVec2_t(const T& nx, const T& ny)
		: x(nx), y(ny) {}
	LvnVec2_t(const LvnVec3_t<T>& v)
	{
		this->x = v.x;
		this->y = v.y;
	}
	LvnVec2_t(const LvnVec4_t<T>& v)
	{
		this->x = v.x;
		this->y = v.y;
	}

	LvnVec2_t<T> operator+()
	{
		return { this->x, this->y };
	}
	LvnVec2_t<T> operator-()
	{
		return { -this->x, -this->y };
	}
	LvnVec2_t<T> operator+(const LvnVec2_t<T>& v)
	{
		return { this->x + v.x, this->y + v.y };
	}
	LvnVec2_t<T> operator-(const LvnVec2_t<T>& v)
	{
		return { this->x - v.x, this->y - v.y };
	}
	LvnVec2_t<T> operator*(const LvnVec2_t<T>& v)
	{
		return { this->x * v.x, this->y * v.y };
	}
	LvnVec2_t<T> operator/(const LvnVec2_t<T>& v)
	{
		return { this->x / v.x, this->y / v.y };
	}
	LvnVec2_t<T>& operator+=(const LvnVec2_t<T>& v)
	{
		this->x += v.x;
		this->y += v.y;
		return *this;
	}
	LvnVec2_t<T>& operator-=(const LvnVec2_t<T>& v)
	{
		this->x -= v.x;
		this->y -= v.y;
		return *this;
	}
	LvnVec2_t<T>& operator*=(const LvnVec2_t<T>& v)
	{
		this->x *= v.x;
		this->y *= v.y;
		return *this;
	}
	LvnVec2_t<T>& operator/=(const LvnVec2_t<T>& v)
	{
		this->x /= v.x;
		this->y /= v.y;
		return *this;
	}
	T& operator[](int i)
	{
		switch (i)
		{
		default:
		case 0:
			return x;
		case 1:
			return y;
		}
	}
};

template<typename T>
struct LvnVec3_t
{
	union { T x, r, s; };
	union { T y, g, t; };
	union { T z, b, p; };

	LvnVec3_t() {}
	LvnVec3_t(const T& n)
		: x(n), y(n), z(n) {}
	LvnVec3_t(const T& nx, const T& ny, const T& nz)
		: x(nx), y(ny), z(nz) {}
	LvnVec3_t(const T& n_x, const LvnVec2_t<T>& n_yz)
		: x(n_x), y(n_yz.y), z(n_yz.z) {}
	LvnVec3_t(const LvnVec2_t<T>& n_xy, const T& n_z)
		: x(n_xy.x), y(n_xy.y), z(n_z) {}
	LvnVec3_t(const LvnVec4_t<T>& v)
	{
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
	}

	LvnVec3_t<T> operator+()
	{
		return { this->x, this->y, this->z };
	}
	LvnVec3_t<T> operator-()
	{
		return { -this->x, -this->y, -this->z };
	}
	LvnVec3_t<T> operator+(const LvnVec3_t<T>& v)
	{
		return { this->x + v.x, this->y + v.y, this->z + v.z };
	}
	LvnVec3_t<T> operator-(const LvnVec3_t<T>& v)
	{
		return { this->x - v.x, this->y - v.y, this->z - v.z };
	}
	LvnVec3_t<T> operator*(const LvnVec3_t<T>& v)
	{
		return { this->x * v.x, this->y * v.y, this->z * v.z };
	}
	LvnVec3_t<T> operator/(const LvnVec3_t<T>& v)
	{
		return { this->x / v.x, this->y / v.y, this->z / v.z };
	}
	LvnVec3_t<T>& operator+=(const LvnVec3_t<T>& v)
	{
		this->x += v.x;
		this->y += v.y;
		this->z += v.z;
		return *this;
	}
	LvnVec3_t<T>& operator-=(const LvnVec3_t<T>& v)
	{
		this->x -= v.x;
		this->y -= v.y;
		this->z -= v.z;
		return *this;
	}
	LvnVec3_t<T>& operator*=(const LvnVec3_t<T>& v)
	{
		this->x *= v.x;
		this->y *= v.y;
		this->z *= v.z;
		return *this;
	}
	LvnVec3_t<T>& operator/=(const LvnVec3_t<T>& v)
	{
		this->x /= v.x;
		this->y /= v.y;
		this->z /= v.z;
		return *this;
	}
	T& operator[](int i)
	{
		switch (i)
		{
		default:
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return y;
		}
	}
};

template<typename T>
struct LvnVec4_t
{
	union { T x, r, s; };
	union { T y, g, t; };
	union { T z, b, p; };
	union { T w, a, q; };

	LvnVec4_t() {}
	LvnVec4_t(const T& n)
		: x(n), y(n), z(n), w(n) {}
	LvnVec4_t(const T& nx, const T& ny, const T& nz, const T& nw)
		: x(nx), y(ny), z(nz), w(nw) {}
	LvnVec4_t(const T& n_x, const T& n_y, const LvnVec2_t<T>& n_zw)
		: x(n_x), y(n_y), z(n_zw.z), w(n_zw.w) {}
	LvnVec4_t(const LvnVec2_t<T>& n_xy, const T& n_z, const T& n_w)
		: x(n_xy.x), y(n_xy.y), z(n_z), w(n_w) {}
	LvnVec4_t(const T& n_x, const LvnVec2_t<T>& n_yz, const T& n_w)
		: x(n_x), y(n_yz.y), z(n_yz.z), w(n_w) {}
	LvnVec4_t(const LvnVec3_t<T>& n_xyz, const T& n_w)
		: x(n_xyz.x), y(n_xyz.y), z(n_xyz.z), w(n_w) {}
	LvnVec4_t(const T& n_x, const LvnVec3_t<T>& n_yzw)
		: x(n_x), y(n_yzw.y), z(n_yzw.z), w(n_yzw.w) {}

	LvnVec4_t<T> operator+()
	{
		return { this->x, this->y, this->z, this->w };
	}
	LvnVec4_t<T> operator-()
	{
		return { -this->x, -this->y, -this->z, -this->w };
	}
	LvnVec4_t<T> operator+(const LvnVec4_t<T>& v)
	{
		return { this->x + v.x, this->y + v.y, this->z + v.z, this->w + v.w };
	}
	LvnVec4_t<T> operator-(const LvnVec4_t<T>& v)
	{
		return { this->x - v.x, this->y - v.y, this->z - v.z, this->w - v.w };
	}
	LvnVec4_t<T> operator*(const LvnVec4_t<T>& v)
	{
		return { this->x * v.x, this->y * v.y, this->z * v.z, this->w * v.w };
	}
	LvnVec4_t<T> operator/(const LvnVec4_t<T>& v)
	{
		return { this->x / v.x, this->y / v.y, this->z / v.z, this->w / v.w };
	}
	LvnVec4_t<T>& operator+=(const LvnVec4_t<T>& v)
	{
		this->x += v.x;
		this->y += v.y;
		this->z += v.z;
		this->w += v.w;
		return *this;
	}
	LvnVec4_t<T>& operator-=(const LvnVec4_t<T>& v)
	{
		this->x -= v.x;
		this->y -= v.y;
		this->z -= v.z;
		this->w -= v.w;
		return *this;
	}
	LvnVec4_t<T>& operator*=(const LvnVec4_t<T>& v)
	{
		this->x *= v.x;
		this->y *= v.y;
		this->z *= v.z;
		this->w *= v.w;
		return *this;
	}
	LvnVec4_t<T>& operator/=(const LvnVec4_t<T>& v)
	{
		this->x /= v.x;
		this->y /= v.y;
		this->z /= v.z;
		this->w /= v.w;
		return *this;
	}
	T& operator[](int i)
	{
		switch (i)
		{
		default:
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return y;
		case 3:
			return w;
		}
	}
};


template<typename T>
struct LvnMat2x2_t
{
	LvnVec2_t<T> value[2];

	static int length() { return 2; }

	LvnMat2x2_t()
	{
		this->value[0] = { 1, 0 };
		this->value[1] = { 0, 1 };
	}
	LvnMat2x2_t(const T& n)
	{
		this->value[0] = { n, 0 };
		this->value[1] = { 0, n };
	}
	LvnMat2x2_t
	(
		const T& x0, const T& y0,
		const T& x1, const T& y1
	)
	{
		this->value[0] = { x0, y0 };
		this->value[1] = { x1, y1 };
	}
	LvnMat2x2_t(const LvnVec2_t<T>& v0, const LvnVec2_t<T>& v1)
		: value{ v0, v1 } {}
	LvnMat2x2_t(const LvnMat2x2_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]) } {}
	LvnMat2x2_t(const LvnMat3x3_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]) } {}
	LvnMat2x2_t(const LvnMat4x4_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]) } {}
	LvnMat2x2_t(const LvnMat2x3_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]) } {}
	LvnMat2x2_t(const LvnMat2x4_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]) } {}
	LvnMat2x2_t(const LvnMat3x2_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]) } {}
	LvnMat2x2_t(const LvnMat3x4_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]) } {}
	LvnMat2x2_t(const LvnMat4x2_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]) } {}
	LvnMat2x2_t(const LvnMat4x3_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]) } {}

	LvnVec2_t<T>& operator[](int i)
	{
		return this->value[i];
	}

	LvnMat2x2_t<T> operator+()
	{
		return LvnMat2x2_t<T>(
			this->value[0],
			this->value[1]);
	}
	LvnMat2x2_t<T> operator-()
	{
		return LvnMat2x2_t<T>(
			-this->value[0],
			-this->value[1]);
	}
	LvnMat2x2_t<T> operator+(const LvnMat2x2_t<T>& m)
	{
		return LvnMat2x2_t<T>(
			this->value[0] + m.value[0],
			this->value[1] + m.value[1]);
	}
	LvnMat2x2_t<T> operator-(const LvnMat2x2_t<T>& m)
	{
		return LvnMat2x2_t<T>(
			this->value[0] - m.value[0],
			this->value[1] - m.value[1]);
	}
	LvnMat2x2_t<T> operator*(const T& n)
	{
		return LvnMat2x2_t<T>(
			this->value[0] * n,
			this->value[1] * n);
	}
	LvnMat2x2_t<T> operator*(const LvnMat2x2_t<T>& m)
	{
		return LvnMat2x2_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y);
	}
	LvnMat3x2_t<T> operator*(const LvnMat3x2_t<T>& m)
	{
		return LvnMat3x2_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y);
	}
	LvnMat4x2_t<T> operator*(const LvnMat4x2_t<T>& m)
	{
		return LvnMat4x2_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y,
			this->value[0].x * m.value[3].x + this->value[1].x * m.value[3].y,
			this->value[0].y * m.value[3].x + this->value[1].y * m.value[3].y);
	}
};

template<typename T>
LvnVec2_t<T> operator*(const LvnMat2x2_t<T>& m, const LvnVec2_t<T>& v)
{
	return LvnVec2_t<T>(
		m.value[0].x * v.x + m.value[1].x * v.y,
		m.value[0].y * v.x + m.value[1].y * v.y);
}
template<typename T>
LvnVec2_t<T> operator*(const LvnVec2_t<T>& v, const LvnMat2x2_t<T>& m)
{
	return LvnVec2_t<T>(
		v.x * m.value[0].x + v.y * m.value[0].y,
		v.x * m.value[1].x + v.y * m.value[1].y);
}

template<typename T>
struct LvnMat3x3_t
{
	LvnVec3_t<T> value[3];

	static int length() { return 3; }

	LvnMat3x3_t()
	{
		this->value[0] = { 1, 0, 0 };
		this->value[1] = { 0, 1, 0 };
		this->value[2] = { 0, 0, 1 };
	}
	LvnMat3x3_t(const T& n)
	{
		this->value[0] = { n, 0, 0 };
		this->value[1] = { 0, n, 0 };
		this->value[2] = { 0, 0, n };
	}
	LvnMat3x3_t
	(
		const T& x0, const T& y0, const T& z0,
		const T& x1, const T& y1, const T& z1,
		const T& x2, const T& y2, const T& z2
	)
	{
		this->value[0] = { x0, y0, z0 };
		this->value[1] = { x1, y1, z1 };
		this->value[2] = { x2, y2, z2 };
	}
	LvnMat3x3_t(const LvnVec3_t<T>& v0, const LvnVec3_t<T>& v1, const LvnVec3_t<T>& v2)
		: value{ v0, v1, v2 } {}
	LvnMat3x3_t(const LvnMat2x2_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0], 0), LvnVec3_t<T>(m.value[1], 0), LvnVec3_t<T>(0, 0, 1) } {}
	LvnMat3x3_t(const LvnMat3x3_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]), LvnVec3_t<T>(m.value[2]) } {}
	LvnMat3x3_t(const LvnMat4x4_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]), LvnVec3_t<T>(m.value[2]) } {}
	LvnMat3x3_t(const LvnMat2x3_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]), LvnVec3_t<T>(0, 0, 1) } {}
	LvnMat3x3_t(const LvnMat2x4_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]), LvnVec3_t<T>(0, 0, 1) } {}
	LvnMat3x3_t(const LvnMat3x2_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0], 0), LvnVec3_t<T>(m.value[1], 0), LvnVec3_t<T>(m.value[2], 1) } {}
	LvnMat3x3_t(const LvnMat3x4_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]), LvnVec3_t<T>(m.value[2]) } {}
	LvnMat3x3_t(const LvnMat4x2_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0], 0), LvnVec3_t<T>(m.value[1], 0), LvnVec3_t<T>(m.value[2], 1) } {}
	LvnMat3x3_t(const LvnMat4x3_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]), LvnVec3_t<T>(m.value[2]) } {}

	LvnVec3_t<T>& operator[](int i)
	{
		return this->value[i];
	}

	LvnMat3x3_t<T> operator+()
	{
		return LvnMat3x3_t<T>(
			this->value[0],
			this->value[1],
			this->value[2]);
	}
	LvnMat3x3_t<T> operator-()
	{
		return LvnMat3x3_t<T>(
			-this->value[0],
			-this->value[1],
			-this->value[2]);
	}
	LvnMat3x3_t<T> operator+(const LvnMat3x3_t<T>& m)
	{
		return LvnMat3x3_t<T>(
			this->value[0] + m.value[0],
			this->value[1] + m.value[1],
			this->value[2] + m.value[2]);
	}
	LvnMat3x3_t<T> operator-(const LvnMat3x3_t<T>& m)
	{
		return LvnMat3x3_t<T>(
			this->value[0] - m.value[0],
			this->value[1] - m.value[1],
			this->value[2] - m.value[2]);
	}
	LvnMat3x3_t<T> operator*(const LvnMat3x3_t<T>& m)
	{
		return LvnMat3x3_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y + this->value[2].z * m.value[0].z,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y + this->value[2].z * m.value[1].z,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z,
			this->value[0].z * m.value[2].x + this->value[1].z * m.value[2].y + this->value[2].z * m.value[2].z);
	}
	LvnMat2x3_t<T> operator*(const LvnMat2x3_t<T>& m)
	{
		return LvnMat2x3_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y + this->value[2].z * m.value[0].z,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y + this->value[2].z * m.value[1].z);
	}
	LvnMat4x3_t<T> operator*(const LvnMat4x3_t<T>& m)
	{
		return LvnMat4x3_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y + this->value[2].z * m.value[0].z,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y + this->value[2].z * m.value[1].z,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z,
			this->value[0].z * m.value[2].x + this->value[1].z * m.value[2].y + this->value[2].z * m.value[2].z,
			this->value[0].x * m.value[3].x + this->value[1].x * m.value[3].y + this->value[2].x * m.value[3].z,
			this->value[0].y * m.value[3].x + this->value[1].y * m.value[3].y + this->value[2].y * m.value[3].z,
			this->value[0].z * m.value[3].x + this->value[1].z * m.value[3].y + this->value[2].z * m.value[3].z);
	}
};

template<typename T>
LvnVec3_t<T> operator*(const LvnMat3x3_t<T>& m, const LvnVec3_t<T>& v)
{
	return LvnVec3_t<T>(
		m.value[0].x * v.x + m.value[1].x * v.y + m.value[2].x * v.z,
		m.value[0].y * v.x + m.value[1].y * v.y + m.value[2].y * v.z,
		m.value[0].z * v.x + m.value[1].z * v.y + m.value[2].z * v.z);
}
template<typename T>
LvnVec3_t<T> operator*(const LvnVec3_t<T>& v, const LvnMat3x3_t<T>& m)
{
	return LvnVec3_t<T>(
		v.x * m.value[0].x + v.y * m.value[0].y + v.z * m.value[0].z,
		v.x * m.value[1].x + v.y * m.value[1].y + v.z * m.value[1].z,
		v.x * m.value[2].x + v.y * m.value[2].y + v.z * m.value[2].z);
}

template<typename T>
struct LvnMat4x4_t
{
	LvnVec4_t<T> value[4];

	static int length() { return 4; }

	LvnMat4x4_t()
	{
		this->value[0] = { 1, 0, 0, 0 };
		this->value[1] = { 0, 1, 0, 0 };
		this->value[2] = { 0, 0, 1, 0 };
		this->value[3] = { 0, 0, 0, 1 };
	}
	LvnMat4x4_t(const T& n)
	{
		this->value[0] = { n, 0, 0, 0 };
		this->value[1] = { 0, n, 0, 0 };
		this->value[2] = { 0, 0, n, 0 };
		this->value[3] = { 0, 0, 0, n };
	}
	LvnMat4x4_t
	(
		const T& x0, const T& y0, const T& z0, const T& w0,
		const T& x1, const T& y1, const T& z1, const T& w1,
		const T& x2, const T& y2, const T& z2, const T& w2,
		const T& x3, const T& y3, const T& z3, const T& w3
	)
	{
		this->value[0] = { x0, y0, z0, w0 };
		this->value[1] = { x1, y1, z1, w1 };
		this->value[2] = { x2, y2, z2, w2 };
		this->value[3] = { x3, y3, z3, w3 };
	}
	LvnMat4x4_t(const LvnVec4_t<T>& v0, const LvnVec4_t<T>& v1, const LvnVec4_t<T>& v2, const LvnVec4_t<T>& v3)
		: value{ v0, v1, v2, v3 } {}
	LvnMat4x4_t(const LvnMat2x2_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0, 0), LvnVec4_t<T>(m.value[1], 0, 0), LvnVec4_t<T>(0, 0, 1, 0), LvnVec4_t<T>(0, 0, 0, 1) } {}
	LvnMat4x4_t(const LvnMat3x3_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0), LvnVec4_t<T>(m.value[1], 0), LvnVec4_t<T>(m.value[2], 0), LvnVec4_t<T>(0, 0, 0, 1) } {}
	LvnMat4x4_t(const LvnMat4x4_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0]), LvnVec4_t<T>(m.value[1]), LvnVec4_t<T>(m.value[2]), LvnVec4_t<T>(m.value[3]) } {}
	LvnMat4x4_t(const LvnMat2x3_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0), LvnVec4_t<T>(m.value[1], 0), LvnVec4_t<T>(0, 0, 1, 0), LvnVec4_t<T>(0, 0, 0, 1) } {}
	LvnMat4x4_t(const LvnMat2x4_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0]), LvnVec4_t<T>(m.value[1]), LvnVec4_t<T>(0, 0, 1, 0), LvnVec4_t<T>(0, 0, 0, 1) } {}
	LvnMat4x4_t(const LvnMat3x2_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0, 0), LvnVec4_t<T>(m.value[1], 0, 0), LvnVec4_t<T>(m.value[2], 0, 0), LvnVec4_t<T>(0, 0, 0, 1) } {}
	LvnMat4x4_t(const LvnMat3x4_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0]), LvnVec4_t<T>(m.value[1]), LvnVec4_t<T>(m.value[2]), LvnVec4_t<T>(0, 0, 0, 1) } {}
	LvnMat4x4_t(const LvnMat4x2_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0, 0), LvnVec4_t<T>(m.value[1], 0, 0), LvnVec4_t<T>(m.value[2], 1, 0), LvnVec4_t<T>(m.value[3], 0, 1) } {}
	LvnMat4x4_t(const LvnMat4x3_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0), LvnVec4_t<T>(m.value[1], 0), LvnVec4_t<T>(m.value[2], 0), LvnVec4_t<T>(m.value[3], 1) } {}

	LvnVec4_t<T>& operator[](int i)
	{
		return this->value[i];
	}

	LvnMat4x4_t<T> operator+()
	{
		return LvnMat4x4_t<T>(
			this->value[0],
			this->value[1],
			this->value[2],
			this->value[3]);
	}
	LvnMat4x4_t<T> operator-()
	{
		return LvnMat4x4_t<T>(
			-this->value[0],
			-this->value[1],
			-this->value[2],
			-this->value[3]);
	}
	LvnMat4x4_t<T> operator+(const LvnMat4x4_t<T>& m)
	{
		return LvnMat4x4_t<T>(
			this->value[0] + m.value[0],
			this->value[1] + m.value[1],
			this->value[2] + m.value[2],
			this->value[3] + m.value[3]);
	}
	LvnMat4x4_t<T> operator-(const LvnMat4x4_t<T>& m)
	{
		return LvnMat4x4_t<T>(
			this->value[0] - m.value[0],
			this->value[1] - m.value[1],
			this->value[2] - m.value[2],
			this->value[3] - m.value[3]);
	}
	LvnMat4x4_t<T> operator*(const LvnMat4x4_t<T>& m)
	{
		return LvnMat4x4_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z + this->value[3].x * m.value[0].w,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z + this->value[3].y * m.value[0].w,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y + this->value[2].z * m.value[0].z + this->value[3].z * m.value[0].w,
			this->value[0].w * m.value[0].x + this->value[1].w * m.value[0].y + this->value[2].w * m.value[0].z + this->value[3].w * m.value[0].w,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z + this->value[3].x * m.value[1].w,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z + this->value[3].y * m.value[1].w,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y + this->value[2].z * m.value[1].z + this->value[3].z * m.value[1].w,
			this->value[0].w * m.value[1].x + this->value[1].w * m.value[1].y + this->value[2].w * m.value[1].z + this->value[3].w * m.value[1].w,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z + this->value[3].x * m.value[2].w,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z + this->value[3].y * m.value[2].w,
			this->value[0].z * m.value[2].x + this->value[1].z * m.value[2].y + this->value[2].z * m.value[2].z + this->value[3].z * m.value[2].w,
			this->value[0].w * m.value[2].x + this->value[1].w * m.value[2].y + this->value[2].w * m.value[2].z + this->value[3].w * m.value[2].w,
			this->value[0].x * m.value[3].x + this->value[1].x * m.value[3].y + this->value[2].x * m.value[3].z + this->value[3].x * m.value[3].w,
			this->value[0].y * m.value[3].x + this->value[1].y * m.value[3].y + this->value[2].y * m.value[3].z + this->value[3].y * m.value[3].w,
			this->value[0].z * m.value[3].x + this->value[1].z * m.value[3].y + this->value[2].z * m.value[3].z + this->value[3].z * m.value[3].w,
			this->value[0].w * m.value[3].x + this->value[1].w * m.value[3].y + this->value[2].w * m.value[3].z + this->value[3].w * m.value[3].w);
	}
	LvnMat2x4_t<T> operator*(const LvnMat2x4_t<T>& m)
	{
		return LvnMat2x4_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z + this->value[3].x * m.value[0].w,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z + this->value[3].y * m.value[0].w,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y + this->value[2].z * m.value[0].z + this->value[3].z * m.value[0].w,
			this->value[0].w * m.value[0].x + this->value[1].w * m.value[0].y + this->value[2].w * m.value[0].z + this->value[3].w * m.value[0].w,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z + this->value[3].x * m.value[1].w,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z + this->value[3].y * m.value[1].w,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y + this->value[2].z * m.value[1].z + this->value[3].z * m.value[1].w,
			this->value[0].w * m.value[1].x + this->value[1].w * m.value[1].y + this->value[2].w * m.value[1].z + this->value[3].w * m.value[1].w);
	}
	LvnMat3x4_t<T> operator*(const LvnMat3x4_t<T>& m)
	{
		return LvnMat3x4_t<T>(
			this->value[0.x] * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z + this->value[3].x * m.value[0].w,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z + this->value[3].y * m.value[0].w,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y + this->value[2].z * m.value[0].z + this->value[3].z * m.value[0].w,
			this->value[0].w * m.value[0].x + this->value[1].w * m.value[0].y + this->value[2].w * m.value[0].z + this->value[3].w * m.value[0].w,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z + this->value[3].x * m.value[1].w,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z + this->value[3].y * m.value[1].w,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y + this->value[2].z * m.value[1].z + this->value[3].z * m.value[1].w,
			this->value[0].w * m.value[1].x + this->value[1].w * m.value[1].y + this->value[2].w * m.value[1].z + this->value[3].w * m.value[1].w,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z + this->value[3].x * m.value[2].w,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z + this->value[3].y * m.value[2].w,
			this->value[0].z * m.value[2].x + this->value[1].z * m.value[2].y + this->value[2].z * m.value[2].z + this->value[3].z * m.value[2].w,
			this->value[0].w * m.value[2].x + this->value[1].w * m.value[2].y + this->value[2].w * m.value[2].z + this->value[3].w * m.value[2].w);
	}
};

template<typename T>
LvnVec4_t<T> operator*(const LvnMat4x4_t<T>& m, const LvnVec4_t<T>& v)
{
	return LvnVec4_t<T>(
		m.value[0].x * v.x + m.value[1].x * v.y + m.value[2].x * v.z + m.value[3].x * v.w,
		m.value[0].y * v.x + m.value[1].y * v.y + m.value[2].y * v.z + m.value[3].y * v.w,
		m.value[0].z * v.x + m.value[1].z * v.y + m.value[2].z * v.z + m.value[3].z * v.w,
		m.value[0].w * v.x + m.value[1].w * v.y + m.value[2].w * v.z + m.value[3].w * v.w);
}
template<typename T>
LvnVec4_t<T> operator*(const LvnVec4_t<T>& v, const LvnMat4x4_t<T>& m)
{
	return LvnVec4_t<T>(
		v.x * m.value[0].x + v.y * m.value[0].y + v.z * m.value[0].z + v.w * m.value[0].w,
		v.x * m.value[1].x + v.y * m.value[1].y + v.z * m.value[1].z + v.w * m.value[1].w,
		v.x * m.value[2].x + v.y * m.value[2].y + v.z * m.value[2].z + v.w * m.value[2].w,
		v.x * m.value[3].x + v.y * m.value[3].y + v.z * m.value[3].z + v.w * m.value[3].w);
}

template<typename T>
struct LvnMat2x3_t
{
	LvnVec3_t<T> value[2];

	static int length() { return 2; }

	LvnMat2x3_t()
	{
		this->value[0] = { 1, 0, 0 };
		this->value[1] = { 0, 1, 0 };
	}
	LvnMat2x3_t(const T& n)
	{
		this->value[0] = { n, 0, 0 };
		this->value[1] = { 0, n, 0 };
	}
	LvnMat2x3_t
	(
		const T& x0, const T& y0, const T& z0,
		const T& x1, const T& y1, const T& z1
	)
	{
		this->value[0] = { x0, y0, z0 };
		this->value[1] = { x1, y1, z1 };
	}
	LvnMat2x3_t(const LvnVec3_t<T>& v0, const LvnVec3_t<T>& v1)
		: value{ v0, v1 } {}
	LvnMat2x3_t(const LvnMat2x2_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0], 0), LvnVec3_t<T>(m.value[1], 0) } {}
	LvnMat2x3_t(const LvnMat3x3_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]) } {}
	LvnMat2x3_t(const LvnMat4x4_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]) } {}
	LvnMat2x3_t(const LvnMat2x3_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]) } {}
	LvnMat2x3_t(const LvnMat2x4_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]) } {}
	LvnMat2x3_t(const LvnMat3x2_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0], 0), LvnVec3_t<T>(m.value[1], 0) } {}
	LvnMat2x3_t(const LvnMat3x4_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]) } {}
	LvnMat2x3_t(const LvnMat4x2_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0], 0), LvnVec3_t<T>(m.value[1], 0) } {}
	LvnMat2x3_t(const LvnMat4x3_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]) } {}

	LvnVec3_t<T>& operator[](int i)
	{
		return this->value[i];
	}

	LvnMat2x3_t<T> operator+()
	{
		return LvnMat2x3_t<T>(
			this->value[0],
			this->value[1]);
	}
	LvnMat2x3_t<T> operator-()
	{
		return LvnMat2x3_t<T>(
			-this->value[0],
			-this->value[1]);
	}
	LvnMat2x3_t<T> operator+(const LvnMat2x3_t<T>& m)
	{
		return LvnMat2x3_t<T>(
			this->value[0] + m.value[0],
			this->value[1] + m.value[1]);
	}
	LvnMat2x3_t<T> operator-(const LvnMat2x3_t<T>& m)
	{
		return LvnMat2x3_t<T>(
			this->value[0] - m.value[0],
			this->value[1] - m.value[1]);
	}
	LvnMat2x3_t<T> operator*(const LvnMat2x2_t<T>& m)
	{
		return LvnMat2x3_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y);
	}
	LvnMat3x3_t<T> operator*(const LvnMat3x2_t<T>& m)
	{
		return LvnMat3x3_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y,
			this->value[0].z * m.value[2].x + this->value[1].z * m.value[2].y);
	}
	LvnMat4x3_t<T> operator*(const LvnMat4x2_t<T>& m)
	{
		return LvnMat4x3_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y,
			this->value[0].z * m.value[2].x + this->value[1].z * m.value[2].y,
			this->value[0].x * m.value[3].x + this->value[1].x * m.value[3].y,
			this->value[0].y * m.value[3].x + this->value[1].y * m.value[3].y,
			this->value[0].z * m.value[3].x + this->value[1].z * m.value[3].y);
	}
};

template<typename T>
LvnVec3_t<T> operator*(const LvnMat2x3_t<T>& m, const LvnVec2_t<T>& v)
{
	return LvnVec3_t<T>(
		m.value[0].x * v.x + m.value[1].x * v.y,
		m.value[0].y * v.x + m.value[1].y * v.y,
		m.value[0].z * v.x + m.value[1].z * v.y);
}
template<typename T>
LvnVec2_t<T> operator*(const LvnVec3_t<T>& v, const LvnMat2x3_t<T>& m)
{
	return LvnVec2_t<T>(
		v.x * m.value[0].x + v.y * m.value[0].y + v.z * m.value[0].z,
		v.x * m.value[1].x + v.y * m.value[1].y + v.z * m.value[1].z);
}

template<typename T>
struct LvnMat2x4_t
{
	LvnVec4_t<T> value[2];

	static int length() { return 2; }

	LvnMat2x4_t()
	{
		this->value[0] = { 1, 0, 0, 0 };
		this->value[1] = { 0, 1, 0, 0 };
	}
	LvnMat2x4_t(const T& n)
	{
		this->value[0] = { n, 0, 0, 0 };
		this->value[1] = { 0, n, 0, 0 };
	}
	LvnMat2x4_t
	(
		const T& x0, const T& y0, const T& z0, const T& w0,
		const T& x1, const T& y1, const T& z1, const T& w1
	)
	{
		this->value[0] = { x0, y0, z0, w0 };
		this->value[1] = { x1, y1, z1, w1 };
	}
	LvnMat2x4_t(const LvnVec4_t<T>& v0, const LvnVec4_t<T>& v1)
		: value{ v0, v1 } {}
	LvnMat2x4_t(const LvnMat2x2_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0]), LvnVec4_t<T>(m.value[1]) } {}
	LvnMat2x4_t(const LvnMat3x3_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0), LvnVec4_t<T>(m.value[1], 0) } {}
	LvnMat2x4_t(const LvnMat4x4_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0]), LvnVec4_t<T>(m.value[1]) } {}
	LvnMat2x4_t(const LvnMat2x3_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0), LvnVec4_t<T>(m.value[1], 0) } {}
	LvnMat2x4_t(const LvnMat2x4_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0]), LvnVec4_t<T>(m.value[1]) } {}
	LvnMat2x4_t(const LvnMat3x2_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0, 0), LvnVec4_t<T>(m.value[1], 0, 0) } {}
	LvnMat2x4_t(const LvnMat3x4_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0]), LvnVec4_t<T>(m.value[1]) } {}
	LvnMat2x4_t(const LvnMat4x2_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0, 0), LvnVec4_t<T>(m.value[1], 0, 0) } {}
	LvnMat2x4_t(const LvnMat4x3_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0), LvnVec4_t<T>(m.value[1], 0) } {}

	LvnVec4_t<T>& operator[](int i)
	{
		return this->value[i];
	}

	LvnMat2x4_t<T> operator+()
	{
		return LvnMat2x4_t<T>(
			this->value[0],
			this->value[1]);
	}
	LvnMat2x4_t<T> operator-()
	{
		return LvnMat2x4_t<T>(
			-this->value[0],
			-this->value[1]);
	}
	LvnMat2x4_t<T> operator+(const LvnMat2x4_t<T>& m)
	{
		return LvnMat2x4_t<T>(
			this->value[0] + m.value[0],
			this->value[1] + m.value[1]);
	}
	LvnMat2x4_t<T> operator-(const LvnMat2x4_t<T>& m)
	{
		return LvnMat2x4_t<T>(
			this->value[0] - m.value[0],
			this->value[1] - m.value[1]);
	}
	LvnMat4x4_t<T> operator*(const LvnMat4x2_t<T>& m)
	{
		return LvnMat4x4_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y,
			this->value[0].w * m.value[0].x + this->value[1].w * m.value[0].y,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y,
			this->value[0].w * m.value[1].x + this->value[1].w * m.value[1].y,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y,
			this->value[0].z * m.value[2].x + this->value[1].z * m.value[2].y,
			this->value[0].w * m.value[2].x + this->value[1].w * m.value[2].y,
			this->value[0].x * m.value[3].x + this->value[1].x * m.value[3].y,
			this->value[0].y * m.value[3].x + this->value[1].y * m.value[3].y,
			this->value[0].z * m.value[3].x + this->value[1].z * m.value[3].y,
			this->value[0].w * m.value[3].x + this->value[1].w * m.value[3].y);
	}
	LvnMat2x4_t<T> operator*(const LvnMat2x2_t<T>& m)
	{
		return LvnMat2x4_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y,
			this->value[0].w * m.value[0].x + this->value[1].w * m.value[0].y,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y,
			this->value[0].w * m.value[1].x + this->value[1].w * m.value[1].y);
	}
	LvnMat3x4_t<T> operator*(const LvnMat3x2_t<T>& m)
	{
		return LvnMat3x4_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y,
			this->value[0].w * m.value[0].x + this->value[1].w * m.value[0].y,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y,
			this->value[0].w * m.value[1].x + this->value[1].w * m.value[1].y,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y,
			this->value[0].z * m.value[2].x + this->value[1].z * m.value[2].y,
			this->value[0].w * m.value[2].x + this->value[1].w * m.value[2].y);
	}
};

template<typename T>
LvnVec4_t<T> operator*(const LvnMat2x4_t<T>& m, const LvnVec2_t<T>& v)
{
	return LvnVec4_t<T>(
		m.value[0].x * v.x + m.value[1].x * v.y,
		m.value[0].y * v.x + m.value[1].y * v.y,
		m.value[0].z * v.x + m.value[1].z * v.y,
		m.value[0].w * v.x + m.value[1].w * v.y);
}
template<typename T>
LvnVec2_t<T> operator*(const LvnVec4_t<T>& v, const LvnMat2x4_t<T>& m)
{
	return LvnVec2_t<T>(
		v.x * m.value[0].x + v.y * m.value[0].y + v.z * m.value[0].z + v.w * m.value[0].w,
		v.x * m.value[1].x + v.y * m.value[1].y + v.z * m.value[1].z + v.w * m.value[1].w);
}

template<typename T>
struct LvnMat3x2_t
{
	LvnVec2_t<T> value[3];

	static int length() { return 3; }

	LvnMat3x2_t()
	{
		this->value[0] = { 1, 0 };
		this->value[1] = { 0, 1 };
		this->value[2] = { 0, 0 };
	}
	LvnMat3x2_t(const T& n)
	{
		this->value[0] = { n, 0 };
		this->value[1] = { 0, n };
		this->value[2] = { 0, 0 };
	}
	LvnMat3x2_t
	(
		const T& x0, const T& y0,
		const T& x1, const T& y1,
		const T& x2, const T& y2
	)
	{
		this->value[0] = { x0, y0 };
		this->value[1] = { x1, y1 };
		this->value[2] = { x2, y2 };
	}
	LvnMat3x2_t(const LvnVec2_t<T>& v0, const LvnVec2_t<T>& v1, const LvnVec2_t<T>& v2)
		: value{ v0, v1, v2 } {}
	LvnMat3x2_t(const LvnMat2x2_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(0, 0) } {}
	LvnMat3x2_t(const LvnMat3x3_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(m.value[2]) } {}
	LvnMat3x2_t(const LvnMat4x4_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(m.value[2]) } {}
	LvnMat3x2_t(const LvnMat2x3_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(0, 0) } {}
	LvnMat3x2_t(const LvnMat2x4_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(0, 0) } {}
	LvnMat3x2_t(const LvnMat3x2_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(m.value[2]) } {}
	LvnMat3x2_t(const LvnMat3x4_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(m.value[2]) } {}
	LvnMat3x2_t(const LvnMat4x2_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(m.value[2]) } {}
	LvnMat3x2_t(const LvnMat4x3_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(m.value[2]) } {}

	LvnVec2_t<T>& operator[](int i)
	{
		return this->value[i];
	}

	LvnMat3x2_t<T> operator+()
	{
		return LvnMat3x2_t<T>(
			this->value[0],
			this->value[1],
			this->value[2]);
	}
	LvnMat3x2_t<T> operator-()
	{
		return LvnMat3x2_t<T>(
			-this->value[0],
			-this->value[1],
			-this->value[2]);
	}
	LvnMat3x2_t<T> operator+(const LvnMat3x2_t<T>& m)
	{
		return LvnMat3x2_t<T>(
			this->value[0] + m.value[0],
			this->value[1] + m.value[1],
			this->value[2] + m.value[2]);
	}
	LvnMat3x2_t<T> operator-(const LvnMat3x2_t<T>& m)
	{
		return LvnMat3x2_t<T>(
			this->value[0] - m.value[0],
			this->value[1] - m.value[1],
			this->value[2] - m.value[2]);
	}
	LvnMat3x2_t<T> operator*(const LvnMat3x3_t<T>& m)
	{
		return LvnMat3x2_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z);
	}
	LvnMat4x2_t<T> operator*(const LvnMat4x3_t<T>& m)
	{
		return LvnMat4x2_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z,
			this->value[0].x * m.value[3].x + this->value[1].x * m.value[3].y + this->value[2].x * m.value[3].z,
			this->value[0].y * m.value[3].x + this->value[1].y * m.value[3].y + this->value[2].y * m.value[3].z);
	}
};

template<typename T>
LvnVec2_t<T> operator*(const LvnMat3x2_t<T>& m, const LvnVec3_t<T>& v)
{
	return LvnVec2_t<T>(
		m.value[0].x * v.x + m.value[1].x * v.y + m.value[2].x * v.z,
		m.value[0].y * v.x + m.value[1].y * v.y + m.value[2].y * v.z);
}
template<typename T>
LvnVec3_t<T> operator*(const LvnVec2_t<T>& v, const LvnMat3x2_t<T>& m)
{
	return LvnVec3_t<T>(
		v.x * m.value[0].x + v.y * m.value[0].y,
		v.x * m.value[1].x + v.y * m.value[1].y,
		v.x * m.value[2].x + v.y * m.value[2].y);
}

template<typename T>
struct LvnMat3x4_t
{
	LvnVec4_t<T> value[3];

	static int length() { return 3; }

	LvnMat3x4_t()
	{
		this->value[0] = { 1, 0, 0, 0 };
		this->value[1] = { 0, 1, 0, 0 };
		this->value[2] = { 0, 0, 1, 0 };
	}
	LvnMat3x4_t(const T& n)
	{
		this->value[0] = { n, 0, 0, 0 };
		this->value[1] = { 0, n, 0, 0 };
		this->value[2] = { 0, 0, n, 0 };
	}
	LvnMat3x4_t
	(
		const T& x0, const T& y0, const T& z0,
		const T& x1, const T& y1, const T& z1,
		const T& x2, const T& y2, const T& z2
	)
	{
		this->value[0] = { x0, y0, z0 };
		this->value[1] = { x1, y1, z1 };
		this->value[2] = { x2, y2, z2 };
	}
	LvnMat3x4_t(const LvnVec4_t<T>& v0, const LvnVec4_t<T>& v1, const LvnVec4_t<T>& v2)
		: value{ v0, v1, v2 } {}
	LvnMat3x4_t(const LvnMat2x2_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0, 0), LvnVec4_t<T>(m.value[1], 0, 0), LvnVec4_t<T>(0, 0, 1, 0) } {}
	LvnMat3x4_t(const LvnMat3x3_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0), LvnVec4_t<T>(m.value[1], 0), LvnVec4_t<T>(m.value[2], 0) } {}
	LvnMat3x4_t(const LvnMat4x4_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0]), LvnVec4_t<T>(m.value[1]), LvnVec4_t<T>(m.value[2]) } {}
	LvnMat3x4_t(const LvnMat2x3_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0), LvnVec4_t<T>(m.value[1], 0), LvnVec4_t<T>(0, 0, 1, 0) } {}
	LvnMat3x4_t(const LvnMat2x4_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0]), LvnVec4_t<T>(m.value[1]), LvnVec4_t<T>(0, 0, 1, 0) } {}
	LvnMat3x4_t(const LvnMat3x2_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0, 0), LvnVec4_t<T>(m.value[1], 0, 0), LvnVec4_t<T>(m.value[2], 1, 0) } {}
	LvnMat3x4_t(const LvnMat3x4_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0]), LvnVec4_t<T>(m.value[1]), LvnVec4_t<T>(m.value[2]) } {}
	LvnMat3x4_t(const LvnMat4x2_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0, 0), LvnVec4_t<T>(m.value[1], 0, 0), LvnVec4_t<T>(m.value[2], 1, 0) } {}
	LvnMat3x4_t(const LvnMat4x3_t<T>& m)
		: value{ LvnVec4_t<T>(m.value[0], 0), LvnVec4_t<T>(m.value[1], 0), LvnVec4_t<T>(m.value[2], 0) } {}

	LvnVec4_t<T>& operator[](int i)
	{
		return this->value[i];
	}

	LvnMat3x4_t<T> operator+()
	{
		return LvnMat3x4_t<T>(
			this->value[0],
			this->value[1],
			this->value[2]);
	}
	LvnMat3x4_t<T> operator-()
	{
		return LvnMat3x4_t<T>(
			-this->value[0],
			-this->value[1],
			-this->value[2]);
	}
	LvnMat3x4_t<T> operator+(const LvnMat3x4_t<T>& m)
	{
		return LvnMat3x4_t<T>(
			this->value[0] + m.value[0],
			this->value[1] + m.value[1],
			this->value[2] + m.value[2]);
	}
	LvnMat3x4_t<T> operator-(const LvnMat3x4_t<T>& m)
	{
		return LvnMat3x4_t<T>(
			this->value[0] - m.value[0],
			this->value[1] - m.value[1],
			this->value[2] - m.value[2]);
	}
	LvnMat4x4_t<T> operator*(const LvnMat4x3_t<T>& m)
	{
		return LvnMat4x4_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y + this->value[2].z * m.value[0].z,
			this->value[0].w * m.value[0].x + this->value[1].w * m.value[0].y + this->value[2].w * m.value[0].z,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y + this->value[2].z * m.value[1].z,
			this->value[0].w * m.value[1].x + this->value[1].w * m.value[1].y + this->value[2].w * m.value[1].z,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z,
			this->value[0].z * m.value[2].x + this->value[1].z * m.value[2].y + this->value[2].z * m.value[2].z,
			this->value[0].w * m.value[2].x + this->value[1].w * m.value[2].y + this->value[2].w * m.value[2].z,
			this->value[0].x * m.value[3].x + this->value[1].x * m.value[3].y + this->value[2].x * m.value[3].z,
			this->value[0].y * m.value[3].x + this->value[1].y * m.value[3].y + this->value[2].y * m.value[3].z,
			this->value[0].z * m.value[3].x + this->value[1].z * m.value[3].y + this->value[2].z * m.value[3].z,
			this->value[0].w * m.value[3].x + this->value[1].w * m.value[3].y + this->value[2].w * m.value[3].z);
	}
	LvnMat2x4_t<T> operator*(const LvnMat2x3_t<T>& m)
	{
		return LvnMat2x4_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].x + this->value[2].x * m.value[0].x,
			this->value[0].y * m.value[0].y + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].y,
			this->value[0].z * m.value[0].z + this->value[1].z * m.value[0].z + this->value[2].z * m.value[0].z,
			this->value[0].w * m.value[0].w + this->value[1].w * m.value[0].w + this->value[2].w * m.value[0].w,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].x + this->value[2].x * m.value[1].x,
			this->value[0].y * m.value[1].y + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].y,
			this->value[0].z * m.value[1].z + this->value[1].z * m.value[1].z + this->value[2].z * m.value[1].z,
			this->value[0].w * m.value[1].w + this->value[1].w * m.value[1].w + this->value[2].w * m.value[1].w);
	}
	LvnMat3x4_t<T> operator*(const LvnMat3x3_t<T>& m)
	{
		return LvnMat3x4_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y + this->value[2].z * m.value[0].z,
			this->value[0].w * m.value[0].x + this->value[1].w * m.value[0].y + this->value[2].w * m.value[0].z,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y + this->value[2].z * m.value[1].z,
			this->value[0].w * m.value[1].x + this->value[1].w * m.value[1].y + this->value[2].w * m.value[1].z,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z,
			this->value[0].z * m.value[2].x + this->value[1].z * m.value[2].y + this->value[2].z * m.value[2].z,
			this->value[0].w * m.value[2].x + this->value[1].w * m.value[2].y + this->value[2].w * m.value[2].z);
	}
};

template<typename T>
LvnVec4_t<T> operator*(const LvnMat3x4_t<T>& m, const LvnVec3_t<T>& v)
{
	return LvnVec4_t<T>(
		m.value[0].x * v.x + m.value[1].x * v.y + m.value[2].x * v.z,
		m.value[0].y * v.x + m.value[1].y * v.y + m.value[2].y * v.z,
		m.value[0].z * v.x + m.value[1].z * v.y + m.value[2].z * v.z,
		m.value[0].w * v.x + m.value[1].w * v.y + m.value[2].w * v.z);
}
template<typename T>
LvnVec3_t<T> operator*(const LvnVec4_t<T>& v, const LvnMat3x4_t<T>& m)
{
	return LvnVec3_t<T>(
		v.x * m.value[0].x + v.y * m.value[0].y + v.z * m.value[0].z + v.w * m.value[0].w,
		v.x * m.value[1].x + v.y * m.value[1].y + v.z * m.value[1].z + v.w * m.value[1].w,
		v.x * m.value[2].x + v.y * m.value[2].y + v.z * m.value[2].z + v.w * m.value[2].w);
}

template<typename T>
struct LvnMat4x2_t
{
	LvnVec2_t<T> value[4];

	static int length() { return 4; }

	LvnMat4x2_t()
	{
		this->value[0] = { 1, 0 };
		this->value[1] = { 0, 1 };
		this->value[2] = { 0, 0 };
		this->value[3] = { 0, 0 };
	}
	LvnMat4x2_t(const T& n)
	{
		this->value[0] = { n, 0 };
		this->value[1] = { 0, n };
		this->value[2] = { 0, 0 };
		this->value[3] = { 0, 0 };
	}
	LvnMat4x2_t
	(
		const T& x0, const T& y0,
		const T& x1, const T& y1,
		const T& x2, const T& y2,
		const T& x3, const T& y3
	)
	{
		this->value[0] = { x0, y0 };
		this->value[1] = { x1, y1 };
		this->value[2] = { x2, y2 };
		this->value[3] = { x3, y3 };
	}
	LvnMat4x2_t(const LvnVec2_t<T>& v0, const LvnVec2_t<T>& v1, const LvnVec2_t<T>& v2, const LvnVec2_t<T>& v3)
		: value{ v0, v1, v2, v3 } {}
	LvnMat4x2_t(const LvnMat2x2_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(0, 0), LvnVec2_t<T>(0, 0) } {}
	LvnMat4x2_t(const LvnMat3x3_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(m.value[2]), LvnVec2_t<T>(0, 0) } {}
	LvnMat4x2_t(const LvnMat4x4_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(m.value[2]), LvnVec2_t<T>(m.value[3]) } {}
	LvnMat4x2_t(const LvnMat2x3_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(0, 0), LvnVec2_t<T>(0, 0) } {}
	LvnMat4x2_t(const LvnMat2x4_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(0, 0), LvnVec2_t<T>(0, 0) } {}
	LvnMat4x2_t(const LvnMat3x2_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(m.value[2]), LvnVec2_t<T>(0, 0) } {}
	LvnMat4x2_t(const LvnMat3x4_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(m.value[2]), LvnVec2_t<T>(0, 0) } {}
	LvnMat4x2_t(const LvnMat4x2_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(m.value[2]), LvnVec2_t<T>(m.value[3]) } {}
	LvnMat4x2_t(const LvnMat4x3_t<T>& m)
		: value{ LvnVec2_t<T>(m.value[0]), LvnVec2_t<T>(m.value[1]), LvnVec2_t<T>(m.value[2]), LvnVec2_t<T>(m.value[3]) } {}

	LvnVec2_t<T>& operator[](int i)
	{
		return this->value[i];
	}

	LvnMat4x2_t<T> operator+()
	{
		return LvnMat4x2_t<T>(
			this->value[0],
			this->value[1],
			this->value[2],
			this->value[3]);
	}
	LvnMat4x2_t<T> operator-()
	{
		return LvnMat4x2_t<T>(
			-this->value[0],
			-this->value[1],
			-this->value[2],
			-this->value[3]);
	}
	LvnMat4x2_t<T> operator+(const LvnMat4x2_t<T>& m)
	{
		return LvnMat4x2_t<T>(
			this->value[0] + m.value[0],
			this->value[1] + m.value[1],
			this->value[2] + m.value[2],
			this->value[3] + m.value[3]);
	}
	LvnMat4x2_t<T> operator-(const LvnMat4x2_t<T>& m)
	{
		return LvnMat4x2_t<T>(
			this->value[0] - m.value[0],
			this->value[1] - m.value[1],
			this->value[2] - m.value[2],
			this->value[3] - m.value[3]);
	}
	LvnMat2x2_t<T> operator*(const LvnMat2x4_t<T>& m)
	{
		return LvnMat2x2_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z + this->value[3].x * m.value[0].w,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z + this->value[3].y * m.value[0].w,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z + this->value[3].x * m.value[1].w,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z + this->value[3].y * m.value[1].w);
	}
	LvnMat3x2_t<T> operator*(const LvnMat3x4_t<T>& m)
	{
		return LvnMat3x2_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z + this->value[3].x * m.value[0].w,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z + this->value[3].y * m.value[0].w,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z + this->value[3].x * m.value[1].w,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z + this->value[3].y * m.value[1].w,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z + this->value[3].x * m.value[2].w,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z + this->value[3].y * m.value[2].w);
	}
	LvnMat4x2_t<T> operator*(const LvnMat4x4_t<T>& m)
	{
		return LvnMat4x2_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z + this->value[3].x * m.value[0].w,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z + this->value[3].y * m.value[0].w,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z + this->value[3].x * m.value[1].w,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z + this->value[3].y * m.value[1].w,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z + this->value[3].x * m.value[2].w,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z + this->value[3].y * m.value[2].w,
			this->value[0].x * m.value[3].x + this->value[1].x * m.value[3].y + this->value[2].x * m.value[3].z + this->value[3].x * m.value[3].w,
			this->value[0].y * m.value[3].x + this->value[1].y * m.value[3].y + this->value[2].y * m.value[3].z + this->value[3].y * m.value[3].w);
	}
};

template<typename T>
LvnVec2_t<T> operator*(const LvnMat4x2_t<T>& m, const LvnVec4_t<T>& v)
{
	return LvnVec2_t<T>(
		m.value[0].x * v.x + m.value[1].x * v.y + m.value[2].x * v.z + m.value[3].x * v.w,
		m.value[0].y * v.x + m.value[1].y * v.y + m.value[2].y * v.z + m.value[3].y * v.w);
}
template<typename T>
LvnVec4_t<T> operator*(const LvnVec2_t<T>& v, const LvnMat4x2_t<T>& m)
{
	return LvnVec4_t<T>(
		v.x * m.value[0].x + v.y * m.value[0].y,
		v.x * m.value[1].x + v.y * m.value[1].y,
		v.x * m.value[2].x + v.y * m.value[2].y,
		v.x * m.value[3].x + v.y * m.value[3].y);
}

template<typename T>
struct LvnMat4x3_t
{
	LvnVec3_t<T> value[4];

	static int length() { return 4; }

	LvnMat4x3_t()
	{
		this->value[0] = { 1, 0, 0 };
		this->value[1] = { 0, 1, 0 };
		this->value[2] = { 0, 0, 1 };
		this->value[3] = { 0, 0, 0 };
	}
	LvnMat4x3_t(const T& n)
	{
		this->value[0] = { n, 0, 0 };
		this->value[1] = { 0, n, 0 };
		this->value[2] = { 0, 0, n };
		this->value[3] = { 0, 0, 0 };
	}
	LvnMat4x3_t
	(
		const T& x0, const T& y0, const T& z0,
		const T& x1, const T& y1, const T& z1,
		const T& x2, const T& y2, const T& z2,
		const T& x3, const T& y3, const T& z3
	)
	{
		this->value[0] = { x0, y0, z0 };
		this->value[1] = { x1, y1, z1 };
		this->value[2] = { x2, y2, z2 };
		this->value[3] = { x3, y3, z3 };
	}
	LvnMat4x3_t(const LvnVec3_t<T>& v0, const LvnVec3_t<T>& v1, const LvnVec3_t<T>& v2, const LvnVec3_t<T>& v3)
		: value{ v0, v1, v2, v3 } {}
	LvnMat4x3_t(const LvnMat2x2_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0], 0), LvnVec3_t<T>(m.value[1], 0), LvnVec3_t<T>(0, 0, 1), LvnVec3_t<T>(0, 0, 0) } {}
	LvnMat4x3_t(const LvnMat3x3_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]), LvnVec3_t<T>(m.value[2]), LvnVec3_t<T>(0, 0, 0) } {}
	LvnMat4x3_t(const LvnMat4x4_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]), LvnVec3_t<T>(m.value[2]), LvnVec3_t<T>(m.value[3]) } {}
	LvnMat4x3_t(const LvnMat2x3_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]), LvnVec3_t<T>(0, 0, 1), LvnVec3_t<T>(0, 0, 0) } {}
	LvnMat4x3_t(const LvnMat2x4_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]), LvnVec3_t<T>(0, 0, 1), LvnVec3_t<T>(0, 0, 0) } {}
	LvnMat4x3_t(const LvnMat3x2_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0], 0), LvnVec3_t<T>(m.value[1], 0), LvnVec3_t<T>(m.value[2], 1), LvnVec3_t<T>(0, 0, 0) } {}
	LvnMat4x3_t(const LvnMat3x4_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]), LvnVec3_t<T>(m.value[2]), LvnVec3_t<T>(0, 0, 0) } {}
	LvnMat4x3_t(const LvnMat4x2_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0], 0), LvnVec3_t<T>(m.value[1], 0), LvnVec3_t<T>(m.value[2], 1), LvnVec3_t<T>(m.value[3], 0) } {}
	LvnMat4x3_t(const LvnMat4x3_t<T>& m)
		: value{ LvnVec3_t<T>(m.value[0]), LvnVec3_t<T>(m.value[1]), LvnVec3_t<T>(m.value[2]), LvnVec3_t<T>(m.value[3]) } {}

	LvnVec3_t<T>& operator[](int i)
	{
		return this->value[i];
	}

	LvnMat4x3_t<T> operator+()
	{
		return LvnMat4x3_t<T>(
			this->value[0],
			this->value[1],
			this->value[2],
			this->value[3]);
	}
	LvnMat4x3_t<T> operator-()
	{
		return LvnMat4x3_t<T>(
			-this->value[0],
			-this->value[1],
			-this->value[2],
			-this->value[3]);
	}
	LvnMat4x3_t<T> operator+(const LvnMat4x3_t<T>& m)
	{
		return LvnMat4x3_t<T>(
			this->value[0] + m.value[0],
			this->value[1] + m.value[1],
			this->value[2] + m.value[2],
			this->value[3] + m.value[3]);
	}
	LvnMat4x3_t<T> operator-(const LvnMat4x3_t<T>& m)
	{
		return LvnMat4x3_t<T>(
			this->value[0] - m.value[0],
			this->value[1] - m.value[1],
			this->value[2] - m.value[2],
			this->value[3] - m.value[3]);
	}
	LvnMat2x3_t<T> operator*(const LvnMat2x4_t<T>& m)
	{
		return LvnMat2x3_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z + this->value[3].x * m.value[0].w,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z + this->value[3].y * m.value[0].w,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y + this->value[2].z * m.value[0].z + this->value[3].z * m.value[0].w,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z + this->value[3].x * m.value[1].w,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z + this->value[3].y * m.value[1].w,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y + this->value[2].z * m.value[1].z + this->value[3].z * m.value[1].w);
	}
	LvnMat3x3_t<T> operator*(const LvnMat3x4_t<T>& m)
	{
		return LvnMat3x3_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z + this->value[3].x * m.value[0].w,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z + this->value[3].y * m.value[0].w,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y + this->value[2].z * m.value[0].z + this->value[3].z * m.value[0].w,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z + this->value[3].x * m.value[1].w,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z + this->value[3].y * m.value[1].w,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y + this->value[2].z * m.value[1].z + this->value[3].z * m.value[1].w,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z + this->value[3].x * m.value[2].w,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z + this->value[3].y * m.value[2].w,
			this->value[0].z * m.value[2].x + this->value[1].z * m.value[2].y + this->value[2].z * m.value[2].z + this->value[3].z * m.value[2].w);
	}
	LvnMat4x3_t<T> operator*(const LvnMat4x4_t<T>& m)
	{
		return LvnMat4x3_t<T>(
			this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z + this->value[3].x * m.value[0].w,
			this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z + this->value[3].y * m.value[0].w,
			this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y + this->value[2].z * m.value[0].z + this->value[3].z * m.value[0].w,
			this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z + this->value[3].x * m.value[1].w,
			this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z + this->value[3].y * m.value[1].w,
			this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y + this->value[2].z * m.value[1].z + this->value[3].z * m.value[1].w,
			this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z + this->value[3].x * m.value[2].w,
			this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z + this->value[3].y * m.value[2].w,
			this->value[0].z * m.value[2].x + this->value[1].z * m.value[2].y + this->value[2].z * m.value[2].z + this->value[3].z * m.value[2].w,
			this->value[0].x * m.value[3].x + this->value[1].x * m.value[3].y + this->value[2].x * m.value[3].z + this->value[3].x * m.value[3].w,
			this->value[0].y * m.value[3].x + this->value[1].y * m.value[3].y + this->value[2].y * m.value[3].z + this->value[3].y * m.value[3].w,
			this->value[0].z * m.value[3].x + this->value[1].z * m.value[3].y + this->value[2].z * m.value[3].z + this->value[3].z * m.value[3].w);
	}
};

template<typename T>
LvnVec3_t<T> operator*(const LvnMat4x3_t<T>& m, const LvnVec4_t<T>& v)
{
	return LvnVec3_t<T>(
		m.value[0].x * v.x + m.value[1].x * v.y + m.value[2].x * v.z + m.value[3].x * v.w,
		m.value[0].y * v.x + m.value[1].y * v.y + m.value[2].y * v.z + m.value[3].y * v.w,
		m.value[0].z * v.x + m.value[1].z * v.y + m.value[2].z * v.z + m.value[3].z * v.w);
}
template<typename T>
LvnVec4_t<T> operator*(const LvnVec3_t<T>& v, const LvnMat4x3_t<T>& m)
{
	return LvnVec4_t<T>(
		v.x * m.value[0].x + v.y * m.value[0].y + v.z * m.value[0].z,
		v.x * m.value[1].x + v.y * m.value[1].y + v.z * m.value[1].z,
		v.x * m.value[2].x + v.y * m.value[2].y + v.z * m.value[2].z,
		v.x * m.value[3].x + v.y * m.value[3].y + v.z * m.value[3].z);
}

typedef LvnVec2_t<float>				LvnVec2;
typedef LvnVec3_t<float>				LvnVec3;
typedef LvnVec4_t<float>				LvnVec4;
typedef LvnVec2_t<int>					LvnVec2i;
typedef LvnVec3_t<int>					LvnVec3i;
typedef LvnVec4_t<int>					LvnVec4i;
typedef LvnVec2_t<unsigned int>			LvnVec2ui;
typedef LvnVec3_t<unsigned int>			LvnVec3ui;
typedef LvnVec4_t<unsigned int>			LvnVec4ui;
typedef LvnVec2_t<float>				LvnVec2f;
typedef LvnVec3_t<float>				LvnVec3f;
typedef LvnVec4_t<float>				LvnVec4f;
typedef LvnVec2_t<double>				LvnVec2d;
typedef LvnVec3_t<double>				LvnVec3d;
typedef LvnVec4_t<double>				LvnVec4d;
typedef LvnVec2_t<bool>					LvnVec2b;
typedef LvnVec3_t<bool>					LvnVec3b;
typedef LvnVec4_t<bool>					LvnVec4b;

typedef LvnMat2x2_t<float>				LvnMat2;
typedef LvnMat3x3_t<float>				LvnMat3;
typedef LvnMat4x4_t<float>				LvnMat4;
typedef LvnMat2x3_t<float>				LvnMat2x3;
typedef LvnMat2x4_t<float>				LvnMat2x4;
typedef LvnMat3x2_t<float>				LvnMat3x2;
typedef LvnMat3x4_t<float>				LvnMat3x4;
typedef LvnMat4x2_t<float>				LvnMat4x2;
typedef LvnMat4x3_t<float>				LvnMat4x3;
typedef LvnMat2x2_t<int>				LvnMat2i;
typedef LvnMat3x3_t<int>				LvnMat3i;
typedef LvnMat4x4_t<int>				LvnMat4i;
typedef LvnMat2x3_t<int>				LvnMat2x3i;
typedef LvnMat2x4_t<int>				LvnMat2x4i;
typedef LvnMat3x2_t<int>				LvnMat3x2i;
typedef LvnMat3x4_t<int>				LvnMat3x4i;
typedef LvnMat4x2_t<int>				LvnMat4x2i;
typedef LvnMat4x3_t<int>				LvnMat4x3i;
typedef LvnMat2x2_t<unsigned int>		LvnMat2ui;
typedef LvnMat3x3_t<unsigned int>		LvnMat3ui;
typedef LvnMat4x4_t<unsigned int>		LvnMat4ui;
typedef LvnMat2x3_t<unsigned int>		LvnMat2x3ui;
typedef LvnMat2x4_t<unsigned int>		LvnMat2x4ui;
typedef LvnMat3x2_t<unsigned int>		LvnMat3x2ui;
typedef LvnMat3x4_t<unsigned int>		LvnMat3x4ui;
typedef LvnMat4x2_t<unsigned int>		LvnMat4x2ui;
typedef LvnMat4x3_t<unsigned int>		LvnMat4x3ui;
typedef LvnMat2x2_t<float>				LvnMat2f;
typedef LvnMat3x3_t<float>				LvnMat3f;
typedef LvnMat4x4_t<float>				LvnMat4f;
typedef LvnMat2x3_t<float>				LvnMat2x3f;
typedef LvnMat2x4_t<float>				LvnMat2x4f;
typedef LvnMat3x2_t<float>				LvnMat3x2f;
typedef LvnMat3x4_t<float>				LvnMat3x4f;
typedef LvnMat4x2_t<float>				LvnMat4x2f;
typedef LvnMat4x3_t<float>				LvnMat4x3f;
typedef LvnMat2x2_t<double>				LvnMat2d;
typedef LvnMat3x3_t<double>				LvnMat3d;
typedef LvnMat4x4_t<double>				LvnMat4d;
typedef LvnMat2x3_t<double>				LvnMat2x3d;
typedef LvnMat2x4_t<double>				LvnMat2x4d;
typedef LvnMat3x2_t<double>				LvnMat3x2d;
typedef LvnMat3x4_t<double>				LvnMat3x4d;
typedef LvnMat4x2_t<double>				LvnMat4x2d;
typedef LvnMat4x3_t<double>				LvnMat4x3d;


struct LvnPhysicalDeviceInfo
{
	const char* name;
	LvnPhysicalDeviceType type;
	uint32_t apiVersion;
	uint32_t driverVersion;
};

struct LvnPhysicalDevice
{
	typedef void* LvnPhysicalDeviceHandle;

	LvnPhysicalDeviceInfo info;
	LvnPhysicalDeviceHandle device;
};

struct LvnRendererBackends
{
	bool enableValidationLayers;
	LvnPhysicalDevice* physicalDevice;
};

namespace lvn
{
	typedef LvnVec2_t<float>				vec2;
	typedef LvnVec3_t<float>				vec3;
	typedef LvnVec4_t<float>				vec4;
	typedef LvnVec2_t<int>					vec2i;
	typedef LvnVec3_t<int>					vec3i;
	typedef LvnVec4_t<int>					vec4i;
	typedef LvnVec2_t<unsigned int>			vec2ui;
	typedef LvnVec3_t<unsigned int>			vec3ui;
	typedef LvnVec4_t<unsigned int>			vec4ui;
	typedef LvnVec2_t<float>				vec2f;
	typedef LvnVec3_t<float>				vec3f;
	typedef LvnVec4_t<float>				vec4f;
	typedef LvnVec2_t<double>				vec2d;
	typedef LvnVec3_t<double>				vec3d;
	typedef LvnVec4_t<double>				vec4d;
	typedef LvnVec2_t<bool>					vec2b;
	typedef LvnVec3_t<bool>					vec3b;
	typedef LvnVec4_t<bool>					vec4b;

	typedef LvnMat2x2_t<float>				mat2;
	typedef LvnMat3x3_t<float>				mat3;
	typedef LvnMat4x4_t<float>				mat4;
	typedef LvnMat2x3_t<float>				mat2x3;
	typedef LvnMat2x4_t<float>				mat2x4;
	typedef LvnMat3x2_t<float>				mat3x2;
	typedef LvnMat3x4_t<float>				mat3x4;
	typedef LvnMat4x2_t<float>				mat4x2;
	typedef LvnMat4x3_t<float>				mat4x3;
	typedef LvnMat2x2_t<int>				mat2i;
	typedef LvnMat3x3_t<int>				mat3i;
	typedef LvnMat4x4_t<int>				mat4i;
	typedef LvnMat2x3_t<int>				mat2x3i;
	typedef LvnMat2x4_t<int>				mat2x4i;
	typedef LvnMat3x2_t<int>				mat3x2i;
	typedef LvnMat3x4_t<int>				mat3x4i;
	typedef LvnMat4x2_t<int>				mat4x2i;
	typedef LvnMat4x3_t<int>				mat4x3i;
	typedef LvnMat2x2_t<unsigned int>		mat2ui;
	typedef LvnMat3x3_t<unsigned int>		mat3ui;
	typedef LvnMat4x4_t<unsigned int>		mat4ui;
	typedef LvnMat2x3_t<unsigned int>		mat2x3ui;
	typedef LvnMat2x4_t<unsigned int>		mat2x4ui;
	typedef LvnMat3x2_t<unsigned int>		mat3x2ui;
	typedef LvnMat3x4_t<unsigned int>		mat3x4ui;
	typedef LvnMat4x2_t<unsigned int>		mat4x2ui;
	typedef LvnMat4x3_t<unsigned int>		mat4x3ui;
	typedef LvnMat2x2_t<float>				mat2f;
	typedef LvnMat3x3_t<float>				mat3f;
	typedef LvnMat4x4_t<float>				mat4f;
	typedef LvnMat2x3_t<float>				mat2x3f;
	typedef LvnMat2x4_t<float>				mat2x4f;
	typedef LvnMat3x2_t<float>				mat3x2f;
	typedef LvnMat3x4_t<float>				mat3x4f;
	typedef LvnMat4x2_t<float>				mat4x2f;
	typedef LvnMat4x3_t<float>				mat4x3f;
	typedef LvnMat2x2_t<double>				mat2d;
	typedef LvnMat3x3_t<double>				mat3d;
	typedef LvnMat4x4_t<double>				mat4d;
	typedef LvnMat2x3_t<double>				mat2x3d;
	typedef LvnMat2x4_t<double>				mat2x4d;
	typedef LvnMat3x2_t<double>				mat3x2d;
	typedef LvnMat3x4_t<double>				mat3x4d;
	typedef LvnMat4x2_t<double>				mat4x2d;
	typedef LvnMat4x3_t<double>				mat4x3d;

	/* [Graphics API] */
	LVN_API bool				createGraphicsContext(LvnGraphicsApi graphicsapi);
	LVN_API bool				terminateGraphicsContext();
	LVN_API LvnGraphicsApi		getGraphicsApi();
	LVN_API const char*			getGraphicsApiName();
	LVN_API void				getPhysicalDevices(LvnPhysicalDevice* pPhysicalDevices, uint32_t* deviceCount);
	LVN_API bool				renderInit(LvnRendererBackends* renderBackends);

	LVN_API void				renderClearColor(const float r, const float g, const float b, const float w);
	LVN_API void				renderClear();
	LVN_API void				renderDraw(uint32_t vertexCount);
	LVN_API void				renderDrawIndexed(uint32_t indexCount);
	LVN_API void				renderDrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance);
	LVN_API void				renderDrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance);
	LVN_API void				renderSetStencilReference(uint32_t reference);
	LVN_API void				renderSetStencilMask(uint32_t compareMask, uint32_t writeMask);
	LVN_API void				renderBeginNextFrame();
	LVN_API void				renderDrawSubmit();
	LVN_API void				renderBeginRenderPass();
	LVN_API void				renderEndRenderPass();

	LVN_API LvnVertexBuffer*	createVertexBuffer(float* vertices, uint32_t size);
	LVN_API LvnIndexBuffer*		createIndexBuffer(uint32_t indices, uint32_t size);
	LVN_API LvnVertexArray*		createVertexArray();

	LVN_API void				linkVertexArrayBuffers(LvnVertexBuffer* vertexBuffer, LvnIndexBuffer* indexBuffer, LvnVertexLayoutLinkInfo* vertexLayouts);
	LVN_API void				linkVertexArrayBuffers(LvnVertexArrayCreateInfo* createInfo);

	LVN_API void				bindVertexBuffer(LvnVertexBuffer* vertexBuffer);
	LVN_API void				bindIndexBuffer(LvnVertexBuffer* vertexBuffer);
	LVN_API void				bindVertexArray(LvnVertexBuffer* vertexBuffer);

	LVN_API void				destroyVertexBuffer(LvnVertexBuffer* vertexBuffer);
	LVN_API void				destroyIndexBuffer(LvnVertexBuffer* vertexBuffer);
	LVN_API void				destroyVertexArray(LvnVertexBuffer* vertexBuffer);
}

#endif