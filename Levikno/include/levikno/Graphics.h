#ifndef HG_LEVIKNO_GRAPHICS_H
#define HG_LEVIKNO_GRAPHICS_H

#ifndef HG_LEVIKNO_DEFINE_CONFIG
#define HG_LEVIKNO_DEFINE_CONFIG

// Platform
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) 
	#ifndef LVN_PLATFORM_WINDOWS
		#define LVN_PLATFORM_WINDOWS
	#endif
	#ifdef LVN_PLATFORM_WINDOWS
		#ifdef LVN_SHARED_LIBRARY
			#define LVN_API __declspec(dllexport)
			#define LVN_API_IMPORT __declspec(dllimport)
		#else 
			#define LVN_API
		#endif
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

namespace lvn
{
	struct BatchCreateInfo;
	struct BatchRenderer;
	struct Camera;
	struct Charset;
	struct Cubemap;
	struct CubemapCreateInfo;
	struct DrawCommand;
	struct DrawList;
	struct Font;
	struct FontGlyph;
	struct FontMetrics;
	struct FrameBuffer;
	struct FrameBufferColorAttachment;
	struct FrameBufferDepthAttachment;
	struct FrameBufferCreateInfo;
	struct GraphicsContext;
	struct IndexBuffer;
	struct OrthographicCamera;
	struct PhysicalDevice;
	struct PhysicalDeviceInfo;
	struct RendererBackends;
	struct Shader;
	struct VertexArray;
	struct VertexArrayCreateInfo;
	struct VertexBuffer;
	struct VertexLayout;
	struct VertexLayoutLinkInfo;

	typedef Camera PerspectiveCamera;
	typedef Camera Camera3D;
	typedef OrthographicCamera Camera2D;
	

	/* [Data Types] */

	/* [Vectors] */
	template<typename T>
	struct vec2_t;
	template<typename T>
	struct vec3_t;
	template<typename T>
	struct vec4_t;

	/* [Matricies] */
	template<typename T>
	struct mat2x2_t;
	template<typename T>
	struct mat3x3_t;
	template<typename T>
	struct mat4x4_t;
	template<typename T>
	struct mat2x3_t;
	template<typename T>
	struct mat2x4_t;
	template<typename T>
	struct mat3x2_t;
	template<typename T>
	struct mat3x4_t;
	template<typename T>
	struct mat4x2_t;
	template<typename T>
	struct mat4x3_t;

	template<typename T>
	struct vec2_t
	{
		union { T x, r, s; };
		union { T y, g, t; };

		vec2_t() {}
		vec2_t(const T& n)
			: x(n), y(n) {}
		vec2_t(const T& nx, const T& ny)
			: x(nx), y(ny) {}
		vec2_t(const vec3_t<T>& v)
		{
			this->x = v.x;
			this->y = v.y;
		}
		vec2_t(const vec4_t<T>& v)
		{
			this->x = v.x;
			this->y = v.y;
		}

		vec2_t<T> operator+()
		{
			return { this->x, this->y };
		}
		vec2_t<T> operator-()
		{
			return { -this->x, -this->y };
		}
		vec2_t<T> operator+(const vec2_t<T>& v)
		{
			return { this->x + v.x, this->y + v.y };
		}
		vec2_t<T> operator-(const vec2_t<T>& v)
		{
			return { this->x - v.x, this->y - v.y };
		}
		vec2_t<T> operator*(const vec2_t<T>& v)
		{
			return { this->x * v.x, this->y * v.y };
		}
		vec2_t<T> operator/(const vec2_t<T>& v)
		{
			return { this->x / v.x, this->y / v.y };
		}
		vec2_t<T>& operator+=(const vec2_t<T>& v)
		{
			this->x += v.x;
			this->y += v.y;
			return *this;
		}
		vec2_t<T>& operator-=(const vec2_t<T>& v)
		{
			this->x -= v.x;
			this->y -= v.y;
			return *this;
		}
		vec2_t<T>& operator*=(const vec2_t<T>& v)
		{
			this->x *= v.x;
			this->y *= v.y;
			return *this;
		}
		vec2_t<T>& operator/=(const vec2_t<T>& v)
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
	struct vec3_t
	{
		union { T x, r, s; };
		union { T y, g, t; };
		union { T z, b, p; };

		vec3_t() {}
		vec3_t(const T& n)
			: x(n), y(n), z(n) {}
		vec3_t(const T& nx, const T& ny, const T& nz)
			: x(nx), y(ny), z(nz) {}
		vec3_t(const T& n_x, const vec2_t<T>& n_yz)
			: x(n_x), y(n_yz.y), z(n_yz.z) {}
		vec3_t(const vec2_t<T>& n_xy, const T& n_z)
			: x(n_xy.x), y(n_xy.y), z(n_z) {}
		vec3_t(const vec4_t<T>& v)
		{
			this->x = v.x;
			this->y = v.y;
			this->z = v.z;
		}

		vec3_t<T> operator+()
		{
			return { this->x, this->y, this->z };
		}
		vec3_t<T> operator-()
		{
			return { -this->x, -this->y, -this->z };
		}
		vec3_t<T> operator+(const vec3_t<T>& v)
		{
			return { this->x + v.x, this->y + v.y, this->z + v.z };
		}
		vec3_t<T> operator-(const vec3_t<T>& v)
		{
			return { this->x - v.x, this->y - v.y, this->z - v.z };
		}
		vec3_t<T> operator*(const vec3_t<T>& v)
		{
			return { this->x * v.x, this->y * v.y, this->z * v.z };
		}
		vec3_t<T> operator/(const vec3_t<T>& v)
		{
			return { this->x / v.x, this->y / v.y, this->z / v.z };
		}
		vec3_t<T>& operator+=(const vec3_t<T>& v)
		{
			this->x += v.x;
			this->y += v.y;
			this->z += v.z;
			return *this;
		}
		vec3_t<T>& operator-=(const vec3_t<T>& v)
		{
			this->x -= v.x;
			this->y -= v.y;
			this->z -= v.z;
			return *this;
		}
		vec3_t<T>& operator*=(const vec3_t<T>& v)
		{
			this->x *= v.x;
			this->y *= v.y;
			this->z *= v.z;
			return *this;
		}
		vec3_t<T>& operator/=(const vec3_t<T>& v)
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
	struct vec4_t
	{
		union { T x, r, s; };
		union { T y, g, t; };
		union { T z, b, p; };
		union { T w, a, q; };

		vec4_t() {}
		vec4_t(const T& n)
			: x(n), y(n), z(n), w(n) {}
		vec4_t(const T& nx, const T& ny, const T& nz, const T& nw)
			: x(nx), y(ny), z(nz), w(nw) {}
		vec4_t(const T& n_x, const T& n_y, const vec2_t<T>& n_zw)
			: x(n_x), y(n_y), z(n_zw.z), w(n_zw.w) {}
		vec4_t(const vec2_t<T>& n_xy, const T& n_z, const T& n_w)
			: x(n_xy.x), y(n_xy.y), z(n_z), w(n_w) {}
		vec4_t(const T& n_x, const vec2_t<T>& n_yz, const T& n_w)
			: x(n_x), y(n_yz.y), z(n_yz.z), w(n_w) {}
		vec4_t(const vec3_t<T>& n_xyz, const T& n_w)
			: x(n_xyz.x), y(n_xyz.y), z(n_xyz.z), w(n_w) {}
		vec4_t(const T& n_x, const vec3_t<T>& n_yzw)
			: x(n_x), y(n_yzw.y), z(n_yzw.z), w(n_yzw.w) {}

		vec4_t<T> operator+()
		{
			return { this->x, this->y, this->z, this->w };
		}
		vec4_t<T> operator-()
		{
			return { -this->x, -this->y, -this->z, -this->w };
		}
		vec4_t<T> operator+(const vec4_t<T>& v)
		{
			return { this->x + v.x, this->y + v.y, this->z + v.z, this->w + v.w };
		}
		vec4_t<T> operator-(const vec4_t<T>& v)
		{
			return { this->x - v.x, this->y - v.y, this->z - v.z, this->w - v.w };
		}
		vec4_t<T> operator*(const vec4_t<T>& v)
		{
			return { this->x * v.x, this->y * v.y, this->z * v.z, this->w * v.w };
		}
		vec4_t<T> operator/(const vec4_t<T>& v)
		{
			return { this->x / v.x, this->y / v.y, this->z / v.z, this->w / v.w };
		}
		vec4_t<T>& operator+=(const vec4_t<T>& v)
		{
			this->x += v.x;
			this->y += v.y;
			this->z += v.z;
			this->w += v.w;
			return *this;
		}
		vec4_t<T>& operator-=(const vec4_t<T>& v)
		{
			this->x -= v.x;
			this->y -= v.y;
			this->z -= v.z;
			this->w -= v.w;
			return *this;
		}
		vec4_t<T>& operator*=(const vec4_t<T>& v)
		{
			this->x *= v.x;
			this->y *= v.y;
			this->z *= v.z;
			this->w *= v.w;
			return *this;
		}
		vec4_t<T>& operator/=(const vec4_t<T>& v)
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
	struct mat2x2_t
	{
		vec2_t<T> value[2];

		static int length() { return 2; }

		mat2x2_t()
		{
			this->value[0] = { 1, 0 };
			this->value[1] = { 0, 1 };
		}
		mat2x2_t(const T& n)
		{
			this->value[0] = { n, 0 };
			this->value[1] = { 0, n };
		}
		mat2x2_t
		(
			const T& x0, const T& y0,
			const T& x1, const T& y1
		)
		{
			this->value[0] = { x0, y0 };
			this->value[1] = { x1, y1 };
		}
		mat2x2_t(const vec2_t<T>& v0, const vec2_t<T>& v1)
			: value{ v0, v1 } {}
		mat2x2_t(const mat2x2_t<T>& m)
			: value{ vec2_t<T>(m.value[0]), vec2_t<T>(m.value[1]) } {}
		mat2x2_t(const mat3x3_t<T>& m)
			: value{ vec2_t<T>(m.value[0]), vec2_t<T>(m.value[1]) } {}
		mat2x2_t(const mat4x4_t<T>& m)
			: value{ vec2_t<T>(m.value[0]), vec2_t<T>(m.value[1]) } {}
		mat2x2_t(const mat2x3_t<T>& m)
			: value{ vec2_t<T>(m.value[0]), vec2_t<T>(m.value[1]) } {}
		mat2x2_t(const mat2x4_t<T>& m)
			: value{ vec2_t<T>(m.value[0]), vec2_t<T>(m.value[1]) } {}
		mat2x2_t(const mat3x2_t<T>& m)
			: value{ vec2_t<T>(m.value[0]), vec2_t<T>(m.value[1]) } {}
		mat2x2_t(const mat3x4_t<T>& m)
			: value{ vec2_t<T>(m.value[0]), vec2_t<T>(m.value[1]) } {}
		mat2x2_t(const mat4x2_t<T>& m)
			: value{ vec2_t<T>(m.value[0]), vec2_t<T>(m.value[1]) } {}
		mat2x2_t(const mat4x3_t<T>& m)
			: value{ vec2_t<T>(m.value[0]), vec2_t<T>(m.value[1]) } {}

		vec2_t<T>& operator[](int i)
		{
			return this->value[i];
		}

		mat2x2_t<T> operator+()
		{
			return mat2x2_t<T>(
				this->value[0],
				this->value[1]);
		}
		mat2x2_t<T> operator-()
		{
			return mat2x2_t<T>(
				-this->value[0],
				-this->value[1]);
		}
		mat2x2_t<T> operator+(const mat2x2_t<T>& m)
		{
			return mat2x2_t<T>(
				this->value[0] + m.value[0],
				this->value[1] + m.value[1]);
		}
		mat2x2_t<T> operator-(const mat2x2_t<T>& m)
		{
			return mat2x2_t<T>(
				this->value[0] - m.value[0],
				this->value[1] - m.value[1]);
		}
		mat2x2_t<T> operator*(const T& n)
		{
			return mat2x2_t<T>(
				this->value[0] * n,
				this->value[1] * n);
		}
		mat2x2_t<T> operator*(const mat2x2_t<T>& m)
		{
			return mat2x2_t<T>(
				this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y,
				this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y,
				this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y,
				this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y);
		}
		mat3x2_t<T> operator*(const mat3x2_t<T>& m)
		{
			return mat3x2_t<T>(
				this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y,
				this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y,
				this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y,
				this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y,
				this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y,
				this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y);
		}
		mat4x2_t<T> operator*(const mat4x2_t<T>& m)
		{
			return mat4x2_t<T>(
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
	vec2_t<T> operator*(const mat2x2_t<T>& m, const vec2_t<T>& v)
	{
		return vec2_t<T>(
			m.value[0].x * v.x + m.value[1].x * v.y,
			m.value[0].y * v.x + m.value[1].y * v.y);
	}
	template<typename T>
	vec2_t<T> operator*(const vec2_t<T>& v, const mat2x2_t<T>& m)
	{
		return vec2_t<T>(
			v.x * m.value[0].x + v.y * m.value[0].y,
			v.x * m.value[1].x + v.y * m.value[1].y);
	}

	template<typename T>
	struct mat3x3_t
	{
		vec3_t<T> value[3];

		static int length() { return 3; }

		mat3x3_t()
		{
			this->value[0] = { 1, 0, 0 };
			this->value[1] = { 0, 1, 0 };
			this->value[2] = { 0, 0, 1 };
		}
		mat3x3_t(const T& n)
		{
			this->value[0] = { n, 0, 0 };
			this->value[1] = { 0, n, 0 };
			this->value[2] = { 0, 0, n };
		}
		mat3x3_t
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
		mat3x3_t(const vec3_t<T>& v0, const vec3_t<T>& v1, const vec3_t<T>& v2)
			: value{ v0, v1, v2 } {}
		mat3x3_t(const mat2x2_t<T>& m)
			: value{ vec3_t<T>(m.value[0], 0), vec3_t<T>(m.value[1], 0), vec3_t<T>(0, 0, 1) } {}
		mat3x3_t(const mat3x3_t<T>& m)
			: value{ vec3_t<T>(m.value[0]), vec3_t<T>(m.value[1]), vec3_t<T>(m.value[2]) } {}
		mat3x3_t(const mat4x4_t<T>& m)
			: value{ vec3_t<T>(m.value[0]), vec3_t<T>(m.value[1]), vec3_t<T>(m.value[2]) } {}
		mat3x3_t(const mat2x3_t<T>& m)
			: value{ vec3_t<T>(m.value[0]), vec3_t<T>(m.value[1]), vec3_t<T>(0, 0, 1) } {}
		mat3x3_t(const mat2x4_t<T>& m)
			: value{ vec3_t<T>(m.value[0]), vec3_t<T>(m.value[1]), vec3_t<T>(0, 0, 1) } {}
		mat3x3_t(const mat3x2_t<T>& m)
			: value{ vec3_t<T>(m.value[0], 0), vec3_t<T>(m.value[1], 0), vec3_t<T>(m.value[2], 1) } {}
		mat3x3_t(const mat3x4_t<T>& m)
			: value{ vec3_t<T>(m.value[0]), vec3_t<T>(m.value[1]), vec3_t<T>(m.value[2]) } {}
		mat3x3_t(const mat4x2_t<T>& m)
			: value{ vec3_t<T>(m.value[0], 0), vec3_t<T>(m.value[1], 0), vec3_t<T>(m.value[2], 1) } {}
		mat3x3_t(const mat4x3_t<T>& m)
			: value{ vec3_t<T>(m.value[0]), vec3_t<T>(m.value[1]), vec3_t<T>(m.value[2]) } {}

		vec3_t<T>& operator[](int i)
		{
			return this->value[i];
		}

		mat3x3_t<T> operator+()
		{
			return mat3x3_t<T>(
				this->value[0],
				this->value[1],
				this->value[2]);
		}
		mat3x3_t<T> operator-()
		{
			return mat3x3_t<T>(
				-this->value[0],
				-this->value[1],
				-this->value[2]);
		}
		mat3x3_t<T> operator+(const mat3x3_t<T>& m)
		{
			return mat3x3_t<T>(
				this->value[0] + m.value[0],
				this->value[1] + m.value[1],
				this->value[2] + m.value[2]);
		}
		mat3x3_t<T> operator-(const mat3x3_t<T>& m)
		{
			return mat3x3_t<T>(
				this->value[0] - m.value[0],
				this->value[1] - m.value[1],
				this->value[2] - m.value[2]);
		}
		mat3x3_t<T> operator*(const mat3x3_t<T>& m)
		{
			return mat3x3_t<T>(
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
		mat2x3_t<T> operator*(const mat2x3_t<T>& m)
		{
			return mat2x3_t<T>(
				this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z,
				this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z,
				this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y + this->value[2].z * m.value[0].z,
				this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z,
				this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z,
				this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y + this->value[2].z * m.value[1].z);
		}
		mat4x3_t<T> operator*(const mat4x3_t<T>& m)
		{
			return mat4x3_t<T>(
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
	vec3_t<T> operator*(const mat3x3_t<T>& m, const vec3_t<T>& v)
	{
		return vec3_t<T>(
			m.value[0].x * v.x + m.value[1].x * v.y + m.value[2].x * v.z,
			m.value[0].y * v.x + m.value[1].y * v.y + m.value[2].y * v.z,
			m.value[0].z * v.x + m.value[1].z * v.y + m.value[2].z * v.z);
	}
	template<typename T>
	vec3_t<T> operator*(const vec3_t<T>& v, const mat3x3_t<T>& m)
	{
		return vec3_t<T>(
			v.x * m.value[0].x + v.y * m.value[0].y + v.z * m.value[0].z,
			v.x * m.value[1].x + v.y * m.value[1].y + v.z * m.value[1].z,
			v.x * m.value[2].x + v.y * m.value[2].y + v.z * m.value[2].z);
	}

	template<typename T>
	struct mat4x4_t
	{
		vec4_t<T> value[4];

		static int length() { return 4; }

		mat4x4_t()
		{
			this->value[0] = { 1, 0, 0, 0 };
			this->value[1] = { 0, 1, 0, 0 };
			this->value[2] = { 0, 0, 1, 0 };
			this->value[3] = { 0, 0, 0, 1 };
		}
		mat4x4_t(const T& n)
		{
			this->value[0] = { n, 0, 0, 0 };
			this->value[1] = { 0, n, 0, 0 };
			this->value[2] = { 0, 0, n, 0 };
			this->value[3] = { 0, 0, 0, n };
		}
		mat4x4_t
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
		mat4x4_t(const vec4_t<T>& v0, const vec4_t<T>& v1, const vec4_t<T>& v2, const vec4_t<T>& v3)
			: value{ v0, v1, v2, v3 } {}
		mat4x4_t(const mat2x2_t<T>& m)
			: value{ vec4_t<T>(m.value[0], 0, 0), vec4_t<T>(m.value[1], 0, 0), vec4_t<T>(0, 0, 1, 0), vec4_t<T>(0, 0, 0, 1) } {}
		mat4x4_t(const mat3x3_t<T>& m)
			: value{ vec4_t<T>(m.value[0], 0), vec4_t<T>(m.value[1], 0), vec4_t<T>(m.value[2], 0), vec4_t<T>(0, 0, 0, 1) } {}
		mat4x4_t(const mat4x4_t<T>& m)
			: value{ vec4_t<T>(m.value[0]), vec4_t<T>(m.value[1]), vec4_t<T>(m.value[2]), vec4_t<T>(m.value[3]) } {}
		mat4x4_t(const mat2x3_t<T>& m)
			: value{ vec4_t<T>(m.value[0], 0), vec4_t<T>(m.value[1], 0), vec4_t<T>(0, 0, 1, 0), vec4_t<T>(0, 0, 0, 1) } {}
		mat4x4_t(const mat2x4_t<T>& m)
			: value{ vec4_t<T>(m.value[0]), vec4_t<T>(m.value[1]), vec4_t<T>(0, 0, 1, 0), vec4_t<T>(0, 0, 0, 1) } {}
		mat4x4_t(const mat3x2_t<T>& m)
			: value{ vec4_t<T>(m.value[0], 0, 0), vec4_t<T>(m.value[1], 0, 0), vec4_t<T>(m.value[2], 0, 0), vec4_t<T>(0, 0, 0, 1) } {}
		mat4x4_t(const mat3x4_t<T>& m)
			: value{ vec4_t<T>(m.value[0]), vec4_t<T>(m.value[1]), vec4_t<T>(m.value[2]), vec4_t<T>(0, 0, 0, 1) } {}
		mat4x4_t(const mat4x2_t<T>& m)
			: value{ vec4_t<T>(m.value[0], 0, 0), vec4_t<T>(m.value[1], 0, 0), vec4_t<T>(m.value[2], 1, 0), vec4_t<T>(m.value[3], 0, 1)} {}
		mat4x4_t(const mat4x3_t<T>& m)
			: value{ vec4_t<T>(m.value[0], 0), vec4_t<T>(m.value[1], 0), vec4_t<T>(m.value[2], 0), vec4_t<T>(m.value[3], 1)} {}

		vec4_t<T>& operator[](int i)
		{
			return this->value[i];
		}

		mat4x4_t<T> operator+()
		{
			return mat4x4_t<T>(
				this->value[0],
				this->value[1],
				this->value[2],
				this->value[3]);
		}
		mat4x4_t<T> operator-()
		{
			return mat4x4_t<T>(
				-this->value[0],
				-this->value[1],
				-this->value[2],
				-this->value[3]);
		}
		mat4x4_t<T> operator+(const mat4x4_t<T>& m)
		{
			return mat4x4_t<T>(
				this->value[0] + m.value[0],
				this->value[1] + m.value[1],
				this->value[2] + m.value[2],
				this->value[3] + m.value[3]);
		}
		mat4x4_t<T> operator-(const mat4x4_t<T>& m)
		{
			return mat4x4_t<T>(
				this->value[0] - m.value[0],
				this->value[1] - m.value[1],
				this->value[2] - m.value[2],
				this->value[3] - m.value[3]);
		}
		mat4x4_t<T> operator*(const mat4x4_t<T>& m)
		{
			return mat4x4_t<T>(
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
		mat2x4_t<T> operator*(const mat2x4_t<T>& m)
		{
			return mat2x4_t<T>(
				this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z + this->value[3].x * m.value[0].w,
				this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z + this->value[3].y * m.value[0].w,
				this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y + this->value[2].z * m.value[0].z + this->value[3].z * m.value[0].w,
				this->value[0].w * m.value[0].x + this->value[1].w * m.value[0].y + this->value[2].w * m.value[0].z + this->value[3].w * m.value[0].w,
				this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z + this->value[3].x * m.value[1].w,
				this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z + this->value[3].y * m.value[1].w,
				this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y + this->value[2].z * m.value[1].z + this->value[3].z * m.value[1].w,
				this->value[0].w * m.value[1].x + this->value[1].w * m.value[1].y + this->value[2].w * m.value[1].z + this->value[3].w * m.value[1].w);
		}
		mat3x4_t<T> operator*(const mat3x4_t<T>& m)
		{
			return mat3x4_t<T>(
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
	vec4_t<T> operator*(const mat4x4_t<T>& m, const vec4_t<T>& v)
	{
		return vec4_t<T>(
			m.value[0].x * v.x + m.value[1].x * v.y + m.value[2].x * v.z + m.value[3].x * v.w,
			m.value[0].y * v.x + m.value[1].y * v.y + m.value[2].y * v.z + m.value[3].y * v.w,
			m.value[0].z * v.x + m.value[1].z * v.y + m.value[2].z * v.z + m.value[3].z * v.w,
			m.value[0].w * v.x + m.value[1].w * v.y + m.value[2].w * v.z + m.value[3].w * v.w);
	}
	template<typename T>
	vec4_t<T> operator*(const vec4_t<T>& v, const mat4x4_t<T>& m)
	{
		return vec4_t<T>(
			v.x * m.value[0].x + v.y * m.value[0].y + v.z * m.value[0].z + v.w * m.value[0].w,
			v.x * m.value[1].x + v.y * m.value[1].y + v.z * m.value[1].z + v.w * m.value[1].w,
			v.x * m.value[2].x + v.y * m.value[2].y + v.z * m.value[2].z + v.w * m.value[2].w,
			v.x * m.value[3].x + v.y * m.value[3].y + v.z * m.value[3].z + v.w * m.value[3].w);
	}

	template<typename T>
	struct mat2x3_t
	{
		vec3_t<T> value[2];

		static int length() { return 2; }

		mat2x3_t()
		{
			this->value[0] = { 1, 0, 0 };
			this->value[1] = { 0, 1, 0 };
		}
		mat2x3_t(const T& n)
		{
			this->value[0] = { n, 0, 0 };
			this->value[1] = { 0, n, 0 };
		}
		mat2x3_t
		(
			const T& x0, const T& y0, const T& z0,
			const T& x1, const T& y1, const T& z1
		)
		{
			this->value[0] = { x0, y0, z0 };
			this->value[1] = { x1, y1, z1 };
		}
		mat2x3_t(const vec3_t<T>& v0, const vec3_t<T>& v1)
			: value{ v0, v1 } {}
		mat2x3_t(const mat2x2_t<T>& m)
			: value{ vec3_t<T>(m.value[0], 0), vec3_t<T>(m.value[1], 0) } {}
		mat2x3_t(const mat3x3_t<T>& m)
			: value{ vec3_t<T>(m.value[0]), vec3_t<T>(m.value[1]) } {}
		mat2x3_t(const mat4x4_t<T>& m)
			: value{ vec3_t<T>(m.value[0]), vec3_t<T>(m.value[1]) } {}
		mat2x3_t(const mat2x3_t<T>& m)
			: value{ vec3_t<T>(m.value[0]), vec3_t<T>(m.value[1]) } {}
		mat2x3_t(const mat2x4_t<T>& m)
			: value{ vec3_t<T>(m.value[0]), vec3_t<T>(m.value[1]) } {}
		mat2x3_t(const mat3x2_t<T>& m)
			: value{ vec3_t<T>(m.value[0], 0), vec3_t<T>(m.value[1], 0) } {}
		mat2x3_t(const mat3x4_t<T>& m)
			: value{ vec3_t<T>(m.value[0]), vec3_t<T>(m.value[1]) } {}
		mat2x3_t(const mat4x2_t<T>& m)
			: value{ vec3_t<T>(m.value[0], 0), vec3_t<T>(m.value[1], 0) } {}
		mat2x3_t(const mat4x3_t<T>& m)
			: value{ vec3_t<T>(m.value[0]), vec3_t<T>(m.value[1]) } {}

		vec3_t<T>& operator[](int i)
		{
			return this->value[i];
		}

		mat2x3_t<T> operator+()
		{
			return mat2x3_t<T>(
				this->value[0],
				this->value[1]);
		}
		mat2x3_t<T> operator-()
		{
			return mat2x3_t<T>(
				-this->value[0],
				-this->value[1]);
		}
		mat2x3_t<T> operator+(const mat2x3_t<T>& m)
		{
			return mat2x3_t<T>(
				this->value[0] + m.value[0],
				this->value[1] + m.value[1]);
		}
		mat2x3_t<T> operator-(const mat2x3_t<T>& m)
		{
			return mat2x3_t<T>(
				this->value[0] - m.value[0],
				this->value[1] - m.value[1]);
		}
		mat2x3_t<T> operator*(const mat2x2_t<T>& m)
		{
			return mat2x3_t<T>(
				this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y,
				this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y,
				this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y,
				this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y,
				this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y,
				this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y);
		}
		mat3x3_t<T> operator*(const mat3x2_t<T>& m)
		{
			return mat3x3_t<T>(
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
		mat4x3_t<T> operator*(const mat4x2_t<T>& m)
		{
			return mat4x3_t<T>(
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
	vec3_t<T> operator*(const mat2x3_t<T>& m, const vec2_t<T>& v)
	{
		return vec3_t<T>(
			m.value[0].x * v.x + m.value[1].x * v.y,
			m.value[0].y * v.x + m.value[1].y * v.y,
			m.value[0].z * v.x + m.value[1].z * v.y);
	}
	template<typename T>
	vec2_t<T> operator*(const vec3_t<T>& v, const mat2x3_t<T>& m)
	{
		return vec2_t<T>(
			v.x * m.value[0].x + v.y * m.value[0].y + v.z * m.value[0].z,
			v.x * m.value[1].x + v.y * m.value[1].y + v.z * m.value[1].z);
	}

	template<typename T>
	struct mat2x4_t
	{
		vec4_t<T> value[2];

		static int length() { return 2; }

		mat2x4_t()
		{
			this->value[0] = { 1, 0, 0, 0 };
			this->value[1] = { 0, 1, 0, 0 };
		}
		mat2x4_t(const T& n)
		{
			this->value[0] = { n, 0, 0, 0 };
			this->value[1] = { 0, n, 0, 0 };
		}
		mat2x4_t
		(
			const T& x0, const T& y0, const T& z0, const T& w0,
			const T& x1, const T& y1, const T& z1, const T& w1
		)
		{
			this->value[0] = { x0, y0, z0, w0 };
			this->value[1] = { x1, y1, z1, w1 };
		}
		mat2x4_t(const vec4_t<T>& v0, const vec4_t<T>& v1)
			: value{ v0, v1 } {}
		mat2x4_t(const mat2x2_t<T>& m)
			: value{ vec4_t<T>(m.value[0]), vec4_t<T>(m.value[1]) } {}
		mat2x4_t(const mat3x3_t<T>& m)
			: value{ vec4_t<T>(m.value[0], 0), vec4_t<T>(m.value[1], 0) } {}
		mat2x4_t(const mat4x4_t<T>& m)
			: value{ vec4_t<T>(m.value[0]), vec4_t<T>(m.value[1]) } {}
		mat2x4_t(const mat2x3_t<T>& m)
			: value{ vec4_t<T>(m.value[0], 0), vec4_t<T>(m.value[1], 0) } {}
		mat2x4_t(const mat2x4_t<T>& m)
			: value{ vec4_t<T>(m.value[0]), vec4_t<T>(m.value[1]) } {}
		mat2x4_t(const mat3x2_t<T>& m)
			: value{ vec4_t<T>(m.value[0], 0, 0), vec4_t<T>(m.value[1], 0, 0) } {}
		mat2x4_t(const mat3x4_t<T>& m)
			: value{ vec4_t<T>(m.value[0]), vec4_t<T>(m.value[1]) } {}
		mat2x4_t(const mat4x2_t<T>& m)
			: value{ vec4_t<T>(m.value[0], 0, 0), vec4_t<T>(m.value[1], 0, 0) } {}
		mat2x4_t(const mat4x3_t<T>& m)
			: value{ vec4_t<T>(m.value[0], 0), vec4_t<T>(m.value[1], 0) } {}

		vec4_t<T>& operator[](int i)
		{
			return this->value[i];
		}

		mat2x4_t<T> operator+()
		{
			return mat2x4_t<T>(
				this->value[0],
				this->value[1]);
		}
		mat2x4_t<T> operator-()
		{
			return mat2x4_t<T>(
				-this->value[0],
				-this->value[1]);
		}
		mat2x4_t<T> operator+(const mat2x4_t<T>& m)
		{
			return mat2x4_t<T>(
				this->value[0] + m.value[0],
				this->value[1] + m.value[1]);
		}
		mat2x4_t<T> operator-(const mat2x4_t<T>& m)
		{
			return mat2x4_t<T>(
				this->value[0] - m.value[0],
				this->value[1] - m.value[1]);
		}
		mat4x4_t<T> operator*(const mat4x2_t<T>& m)
		{
			return mat4x4_t<T>(
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
		mat2x4_t<T> operator*(const mat2x2_t<T>& m)
		{
			return mat2x4_t<T>(
				this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y,
				this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y,
				this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y,
				this->value[0].w * m.value[0].x + this->value[1].w * m.value[0].y,
				this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y,
				this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y,
				this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y,
				this->value[0].w * m.value[1].x + this->value[1].w * m.value[1].y);
		}
		mat3x4_t<T> operator*(const mat3x2_t<T>& m)
		{
			return mat3x4_t<T>(
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
	vec4_t<T> operator*(const mat2x4_t<T>& m, const vec2_t<T>& v)
	{
		return vec4_t<T>(
			m.value[0].x * v.x + m.value[1].x * v.y,
			m.value[0].y * v.x + m.value[1].y * v.y,
			m.value[0].z * v.x + m.value[1].z * v.y,
			m.value[0].w * v.x + m.value[1].w * v.y);
	}
	template<typename T>
	vec2_t<T> operator*(const vec4_t<T>& v, const mat2x4_t<T>& m)
	{
		return vec2_t<T>(
			v.x * m.value[0].x + v.y * m.value[0].y + v.z * m.value[0].z + v.w * m.value[0].w,
			v.x * m.value[1].x + v.y * m.value[1].y + v.z * m.value[1].z + v.w * m.value[1].w);
	}

	template<typename T>
	struct mat3x2_t
	{
		vec2_t<T> value[3];

		static int length() { return 3; }

		mat3x2_t()
		{
			this->value[0] = { 1, 0 };
			this->value[1] = { 0, 1 };
			this->value[2] = { 0, 0 };
		}
		mat3x2_t(const T& n)
		{
			this->value[0] = { n, 0 };
			this->value[1] = { 0, n };
			this->value[2] = { 0, 0 };
		}
		mat3x2_t
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
		mat3x2_t(const vec2_t<T>& v0, const vec2_t<T>& v1, const vec2_t<T>& v2)
			: value{ v0, v1, v2 } {}
		mat3x2_t(const mat2x2_t<T>& m)
			: value{ vec2_t<T>(m.value[0]), vec2_t<T>(m.value[1]), vec2_t<T>(0, 0) } {}
		mat3x2_t(const mat3x3_t<T>& m)
			: value{ vec2_t<T>(m.value[0]), vec2_t<T>(m.value[1]), vec2_t<T>(m.value[2]) } {}
		mat3x2_t(const mat4x4_t<T>& m)
			: value{ vec2_t<T>(m.value[0]), vec2_t<T>(m.value[1]), vec2_t<T>(m.value[2]) } {}
		mat3x2_t(const mat2x3_t<T>& m)
			: value{ vec2_t<T>(m.value[0]), vec2_t<T>(m.value[1]), vec2_t<T>(0, 0) } {}
		mat3x2_t(const mat2x4_t<T>& m)
			: value{ vec2_t<T>(m.value[0]), vec2_t<T>(m.value[1]), vec2_t<T>(0, 0) } {}
		mat3x2_t(const mat3x2_t<T>& m)
			: value{ vec2_t<T>(m.value[0]), vec2_t<T>(m.value[1]), vec2_t<T>(m.value[2]) } {}
		mat3x2_t(const mat3x4_t<T>& m)
			: value{ vec2_t<T>(m.value[0]), vec2_t<T>(m.value[1]), vec2_t<T>(m.value[2]) } {}
		mat3x2_t(const mat4x2_t<T>& m)
			: value{ vec2_t<T>(m.value[0]), vec2_t<T>(m.value[1]), vec2_t<T>(m.value[2]) } {}
		mat3x2_t(const mat4x3_t<T>& m)
			: value{ vec2_t<T>(m.value[0]), vec2_t<T>(m.value[1]), vec2_t<T>(m.value[2]) } {}

		vec2_t<T>& operator[](int i)
		{
			return this->value[i];
		}

		mat3x2_t<T> operator+()
		{
			return mat3x2_t<T>(
				this->value[0],
				this->value[1],
				this->value[2]);
		}
		mat3x2_t<T> operator-()
		{
			return mat3x2_t<T>(
				-this->value[0],
				-this->value[1],
				-this->value[2]);
		}
		mat3x2_t<T> operator+(const mat3x2_t<T>& m)
		{
			return mat3x2_t<T>(
				this->value[0] + m.value[0],
				this->value[1] + m.value[1],
				this->value[2] + m.value[2]);
		}
		mat3x2_t<T> operator-(const mat3x2_t<T>& m)
		{
			return mat3x2_t<T>(
				this->value[0] - m.value[0],
				this->value[1] - m.value[1],
				this->value[2] - m.value[2]);
		}
		mat3x2_t<T> operator*(const mat3x3_t<T>& m)
		{
			return mat3x2_t<T>(
				this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z,
				this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z,
				this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z,
				this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z,
				this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z,
				this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z);
		}
		mat4x2_t<T> operator*(const mat4x3_t<T>& m)
		{
			return mat4x2_t<T>(
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
	vec2_t<T> operator*(const mat3x2_t<T>& m, const vec3_t<T>& v)
	{
		return vec2_t<T>(
			m.value[0].x * v.x + m.value[1].x * v.y + m.value[2].x * v.z,
			m.value[0].y * v.x + m.value[1].y * v.y + m.value[2].y * v.z);
	}
	template<typename T>
	vec3_t<T> operator*(const vec2_t<T>& v, const mat3x2_t<T>& m)
	{
		return vec3_t<T>(
			v.x * m.value[0].x + v.y * m.value[0].y,
			v.x * m.value[1].x + v.y * m.value[1].y,
			v.x * m.value[2].x + v.y * m.value[2].y);
	}

	template<typename T>
	struct mat3x4_t
	{
		vec4_t<T> value[3];

		static int length() { return 3; }

		mat3x4_t()
		{
			this->value[0] = { 1, 0, 0, 0 };
			this->value[1] = { 0, 1, 0, 0 };
			this->value[2] = { 0, 0, 1, 0 };
		}
		mat3x4_t(const T& n)
		{
			this->value[0] = { n, 0, 0, 0 };
			this->value[1] = { 0, n, 0, 0 };
			this->value[2] = { 0, 0, n, 0 };
		}
		mat3x4_t
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
		mat3x4_t(const vec4_t<T>& v0, const vec4_t<T>& v1, const vec4_t<T>& v2)
			: value{ v0, v1, v2 } {}
		mat3x4_t(const mat2x2_t<T>& m)
			: value{ vec4_t<T>(m.value[0], 0, 0), vec4_t<T>(m.value[1], 0, 0), vec4_t<T>(0, 0, 1, 0) } {}
		mat3x4_t(const mat3x3_t<T>& m)
			: value{ vec4_t<T>(m.value[0], 0), vec4_t<T>(m.value[1], 0), vec4_t<T>(m.value[2], 0) } {}
		mat3x4_t(const mat4x4_t<T>& m)
			: value{ vec4_t<T>(m.value[0]), vec4_t<T>(m.value[1]), vec4_t<T>(m.value[2]) } {}
		mat3x4_t(const mat2x3_t<T>& m)
			: value{ vec4_t<T>(m.value[0], 0), vec4_t<T>(m.value[1], 0), vec4_t<T>(0, 0, 1, 0) } {}
		mat3x4_t(const mat2x4_t<T>& m)
			: value{ vec4_t<T>(m.value[0]), vec4_t<T>(m.value[1]), vec4_t<T>(0, 0, 1, 0) } {}
		mat3x4_t(const mat3x2_t<T>& m)
			: value{ vec4_t<T>(m.value[0], 0, 0), vec4_t<T>(m.value[1], 0, 0), vec4_t<T>(m.value[2], 1, 0) } {}
		mat3x4_t(const mat3x4_t<T>& m)
			: value{ vec4_t<T>(m.value[0]), vec4_t<T>(m.value[1]), vec4_t<T>(m.value[2]) } {}
		mat3x4_t(const mat4x2_t<T>& m)
			: value{ vec4_t<T>(m.value[0], 0, 0), vec4_t<T>(m.value[1], 0, 0), vec4_t<T>(m.value[2], 1, 0) } {}
		mat3x4_t(const mat4x3_t<T>& m)
			: value{ vec4_t<T>(m.value[0], 0), vec4_t<T>(m.value[1], 0), vec4_t<T>(m.value[2], 0) } {}

		vec4_t<T>& operator[](int i)
		{
			return this->value[i];
		}

		mat3x4_t<T> operator+()
		{
			return mat3x4_t<T>(
				this->value[0],
				this->value[1],
				this->value[2]);
		}
		mat3x4_t<T> operator-()
		{
			return mat3x4_t<T>(
				-this->value[0],
				-this->value[1],
				-this->value[2]);
		}
		mat3x4_t<T> operator+(const mat3x4_t<T>& m)
		{
			return mat3x4_t<T>(
				this->value[0] + m.value[0],
				this->value[1] + m.value[1],
				this->value[2] + m.value[2]);
		}
		mat3x4_t<T> operator-(const mat3x4_t<T>& m)
		{
			return mat3x4_t<T>(
				this->value[0] - m.value[0],
				this->value[1] - m.value[1],
				this->value[2] - m.value[2]);
		}
		mat4x4_t<T> operator*(const mat4x3_t<T>& m)
		{
			return mat4x4_t<T>(
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
		mat2x4_t<T> operator*(const mat2x3_t<T>& m)
		{
			return mat2x4_t<T>(
				this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].x + this->value[2].x * m.value[0].x,
				this->value[0].y * m.value[0].y + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].y,
				this->value[0].z * m.value[0].z + this->value[1].z * m.value[0].z + this->value[2].z * m.value[0].z,
				this->value[0].w * m.value[0].w + this->value[1].w * m.value[0].w + this->value[2].w * m.value[0].w,
				this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].x + this->value[2].x * m.value[1].x,
				this->value[0].y * m.value[1].y + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].y,
				this->value[0].z * m.value[1].z + this->value[1].z * m.value[1].z + this->value[2].z * m.value[1].z,
				this->value[0].w * m.value[1].w + this->value[1].w * m.value[1].w + this->value[2].w * m.value[1].w);
		}
		mat3x4_t<T> operator*(const mat3x3_t<T>& m)
		{
			return mat3x4_t<T>(
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
	vec4_t<T> operator*(const mat3x4_t<T>& m, const vec3_t<T>& v)
	{
		return vec4_t<T>(
			m.value[0].x * v.x + m.value[1].x * v.y + m.value[2].x * v.z,
			m.value[0].y * v.x + m.value[1].y * v.y + m.value[2].y * v.z,
			m.value[0].z * v.x + m.value[1].z * v.y + m.value[2].z * v.z,
			m.value[0].w * v.x + m.value[1].w * v.y + m.value[2].w * v.z);
	}
	template<typename T>
	vec3_t<T> operator*(const vec4_t<T>& v, const mat3x4_t<T>& m)
	{
		return vec3_t<T>(
			v.x * m.value[0].x + v.y * m.value[0].y + v.z * m.value[0].z + v.w * m.value[0].w,
			v.x * m.value[1].x + v.y * m.value[1].y + v.z * m.value[1].z + v.w * m.value[1].w,
			v.x * m.value[2].x + v.y * m.value[2].y + v.z * m.value[2].z + v.w * m.value[2].w);
	}

	template<typename T>
	struct mat4x2_t
	{
		vec2_t<T> value[4];

		static int length() { return 4; }

		mat4x2_t()
		{
			this->value[0] = { 1, 0 };
			this->value[1] = { 0, 1 };
			this->value[2] = { 0, 0 };
			this->value[3] = { 0, 0 };
		}
		mat4x2_t(const T& n)
		{
			this->value[0] = { n, 0 };
			this->value[1] = { 0, n };
			this->value[2] = { 0, 0 };
			this->value[3] = { 0, 0 };
		}
		mat4x2_t
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
		mat4x2_t(const vec2_t<T>& v0, const vec2_t<T>& v1, const vec2_t<T>& v2, const vec2_t<T>& v3)
			: value{ v0, v1, v2, v3 } {}
		mat4x2_t(const mat2x2_t<T>& m)
			: value{ vec2_t<T>(m.value[0]), vec2_t<T>(m.value[1]), vec2_t<T>(0, 0), vec2_t<T>(0, 0) } {}
		mat4x2_t(const mat3x3_t<T>& m)
			: value{ vec2_t<T>(m.value[0]), vec2_t<T>(m.value[1]), vec2_t<T>(m.value[2]), vec2_t<T>(0, 0) } {}
		mat4x2_t(const mat4x4_t<T>& m)
			: value{ vec2_t<T>(m.value[0]), vec2_t<T>(m.value[1]), vec2_t<T>(m.value[2]), vec2_t<T>(m.value[3]) } {}
		mat4x2_t(const mat2x3_t<T>& m)
			: value{ vec2_t<T>(m.value[0]), vec2_t<T>(m.value[1]), vec2_t<T>(0, 0), vec2_t<T>(0, 0) } {}
		mat4x2_t(const mat2x4_t<T>& m)
			: value{ vec2_t<T>(m.value[0]), vec2_t<T>(m.value[1]), vec2_t<T>(0, 0), vec2_t<T>(0, 0) } {}
		mat4x2_t(const mat3x2_t<T>& m)
			: value{ vec2_t<T>(m.value[0]), vec2_t<T>(m.value[1]), vec2_t<T>(m.value[2]), vec2_t<T>(0, 0) } {}
		mat4x2_t(const mat3x4_t<T>& m)
			: value{ vec2_t<T>(m.value[0]), vec2_t<T>(m.value[1]), vec2_t<T>(m.value[2]), vec2_t<T>(0, 0) } {}
		mat4x2_t(const mat4x2_t<T>& m)
			: value{ vec2_t<T>(m.value[0]), vec2_t<T>(m.value[1]), vec2_t<T>(m.value[2]), vec2_t<T>(m.value[3]) } {}
		mat4x2_t(const mat4x3_t<T>& m)
			: value{ vec2_t<T>(m.value[0]), vec2_t<T>(m.value[1]), vec2_t<T>(m.value[2]), vec2_t<T>(m.value[3]) } {}

		vec2_t<T>& operator[](int i)
		{
			return this->value[i];
		}

		mat4x2_t<T> operator+()
		{
			return mat4x2_t<T>(
				this->value[0],
				this->value[1],
				this->value[2],
				this->value[3]);
		}
		mat4x2_t<T> operator-()
		{
			return mat4x2_t<T>(
				-this->value[0],
				-this->value[1],
				-this->value[2],
				-this->value[3]);
		}
		mat4x2_t<T> operator+(const mat4x2_t<T>& m)
		{
			return mat4x2_t<T>(
				this->value[0] + m.value[0],
				this->value[1] + m.value[1],
				this->value[2] + m.value[2],
				this->value[3] + m.value[3]);
		}
		mat4x2_t<T> operator-(const mat4x2_t<T>& m)
		{
			return mat4x2_t<T>(
				this->value[0] - m.value[0],
				this->value[1] - m.value[1],
				this->value[2] - m.value[2],
				this->value[3] - m.value[3]);
		}
		mat2x2_t<T> operator*(const mat2x4_t<T>& m)
		{
			return mat2x2_t<T>(
				this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z + this->value[3].x * m.value[0].w,
				this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z + this->value[3].y * m.value[0].w,
				this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z + this->value[3].x * m.value[1].w,
				this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z + this->value[3].y * m.value[1].w);
		}
		mat3x2_t<T> operator*(const mat3x4_t<T>& m)
		{
			return mat3x2_t<T>(
				this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z + this->value[3].x * m.value[0].w,
				this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z + this->value[3].y * m.value[0].w,
				this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z + this->value[3].x * m.value[1].w,
				this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z + this->value[3].y * m.value[1].w,
				this->value[0].x * m.value[2].x + this->value[1].x * m.value[2].y + this->value[2].x * m.value[2].z + this->value[3].x * m.value[2].w,
				this->value[0].y * m.value[2].x + this->value[1].y * m.value[2].y + this->value[2].y * m.value[2].z + this->value[3].y * m.value[2].w);
		}
		mat4x2_t<T> operator*(const mat4x4_t<T>& m)
		{
			return mat4x2_t<T>(
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
	vec2_t<T> operator*(const mat4x2_t<T>& m, const vec4_t<T>& v)
	{
		return vec2_t<T>(
			m.value[0].x * v.x + m.value[1].x * v.y + m.value[2].x * v.z + m.value[3].x * v.w,
			m.value[0].y * v.x + m.value[1].y * v.y + m.value[2].y * v.z + m.value[3].y * v.w);
	}
	template<typename T>
	vec4_t<T> operator*(const vec2_t<T>& v, const mat4x2_t<T>& m)
	{
		return vec4_t<T>(
			v.x * m.value[0].x + v.y * m.value[0].y,
			v.x * m.value[1].x + v.y * m.value[1].y,
			v.x * m.value[2].x + v.y * m.value[2].y,
			v.x * m.value[3].x + v.y * m.value[3].y);
	}

	template<typename T>
	struct mat4x3_t
	{
		vec3_t<T> value[4];

		static int length() { return 4; }

		mat4x3_t()
		{
			this->value[0] = { 1, 0, 0 };
			this->value[1] = { 0, 1, 0 };
			this->value[2] = { 0, 0, 1 };
			this->value[3] = { 0, 0, 0 };
		}
		mat4x3_t(const T& n)
		{
			this->value[0] = { n, 0, 0 };
			this->value[1] = { 0, n, 0 };
			this->value[2] = { 0, 0, n };
			this->value[3] = { 0, 0, 0 };
		}
		mat4x3_t
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
		mat4x3_t(const vec3_t<T>& v0, const vec3_t<T>& v1, const vec3_t<T>& v2, const vec3_t<T>& v3)
			: value{ v0, v1, v2, v3 } {}
		mat4x3_t(const mat2x2_t<T>& m)
			: value{ vec3_t<T>(m.value[0], 0), vec3_t<T>(m.value[1], 0), vec3_t<T>(0, 0, 1), vec3_t<T>(0, 0, 0) } {}
		mat4x3_t(const mat3x3_t<T>& m)
			: value{ vec3_t<T>(m.value[0]), vec3_t<T>(m.value[1]), vec3_t<T>(m.value[2]), vec3_t<T>(0, 0, 0) } {}
		mat4x3_t(const mat4x4_t<T>& m)
			: value{ vec3_t<T>(m.value[0]), vec3_t<T>(m.value[1]), vec3_t<T>(m.value[2]), vec3_t<T>(m.value[3]) } {}
		mat4x3_t(const mat2x3_t<T>& m)
			: value{ vec3_t<T>(m.value[0]), vec3_t<T>(m.value[1]), vec3_t<T>(0, 0, 1), vec3_t<T>(0, 0, 0) } {}
		mat4x3_t(const mat2x4_t<T>& m)
			: value{ vec3_t<T>(m.value[0]), vec3_t<T>(m.value[1]), vec3_t<T>(0, 0, 1), vec3_t<T>(0, 0, 0) } {}
		mat4x3_t(const mat3x2_t<T>& m)
			: value{ vec3_t<T>(m.value[0], 0), vec3_t<T>(m.value[1], 0), vec3_t<T>(m.value[2], 1), vec3_t<T>(0, 0, 0) } {}
		mat4x3_t(const mat3x4_t<T>& m)
			: value{ vec3_t<T>(m.value[0]), vec3_t<T>(m.value[1]), vec3_t<T>(m.value[2]), vec3_t<T>(0, 0, 0) } {}
		mat4x3_t(const mat4x2_t<T>& m)
			: value{ vec3_t<T>(m.value[0], 0), vec3_t<T>(m.value[1], 0), vec3_t<T>(m.value[2], 1), vec3_t<T>(m.value[3], 0) } {}
		mat4x3_t(const mat4x3_t<T>& m)
			: value{ vec3_t<T>(m.value[0]), vec3_t<T>(m.value[1]), vec3_t<T>(m.value[2]), vec3_t<T>(m.value[3]) } {}

		vec3_t<T>& operator[](int i)
		{
			return this->value[i];
		}

		mat4x3_t<T> operator+()
		{
			return mat4x3_t<T>(
				this->value[0],
				this->value[1],
				this->value[2],
				this->value[3]);
		}
		mat4x3_t<T> operator-()
		{
			return mat4x3_t<T>(
				-this->value[0],
				-this->value[1],
				-this->value[2],
				-this->value[3]);
		}
		mat4x3_t<T> operator+(const mat4x3_t<T>& m)
		{
			return mat4x3_t<T>(
				this->value[0] + m.value[0],
				this->value[1] + m.value[1],
				this->value[2] + m.value[2],
				this->value[3] + m.value[3]);
		}
		mat4x3_t<T> operator-(const mat4x3_t<T>& m)
		{
			return mat4x3_t<T>(
				this->value[0] - m.value[0],
				this->value[1] - m.value[1],
				this->value[2] - m.value[2],
				this->value[3] - m.value[3]);
		}
		mat2x3_t<T> operator*(const mat2x4_t<T>& m)
		{
			return mat2x3_t<T>(
				this->value[0].x * m.value[0].x + this->value[1].x * m.value[0].y + this->value[2].x * m.value[0].z + this->value[3].x * m.value[0].w,
				this->value[0].y * m.value[0].x + this->value[1].y * m.value[0].y + this->value[2].y * m.value[0].z + this->value[3].y * m.value[0].w,
				this->value[0].z * m.value[0].x + this->value[1].z * m.value[0].y + this->value[2].z * m.value[0].z + this->value[3].z * m.value[0].w,
				this->value[0].x * m.value[1].x + this->value[1].x * m.value[1].y + this->value[2].x * m.value[1].z + this->value[3].x * m.value[1].w,
				this->value[0].y * m.value[1].x + this->value[1].y * m.value[1].y + this->value[2].y * m.value[1].z + this->value[3].y * m.value[1].w,
				this->value[0].z * m.value[1].x + this->value[1].z * m.value[1].y + this->value[2].z * m.value[1].z + this->value[3].z * m.value[1].w);
		}
		mat3x3_t<T> operator*(const mat3x4_t<T>& m)
		{
			return mat3x3_t<T>(
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
		mat4x3_t<T> operator*(const mat4x4_t<T>& m)
		{
			return mat4x3_t<T>(
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
	vec3_t<T> operator*(const mat4x3_t<T>& m, const vec4_t<T>& v)
	{
		return vec3_t<T>(
			m.value[0].x * v.x + m.value[1].x * v.y + m.value[2].x * v.z + m.value[3].x * v.w,
			m.value[0].y * v.x + m.value[1].y * v.y + m.value[2].y * v.z + m.value[3].y * v.w,
			m.value[0].z * v.x + m.value[1].z * v.y + m.value[2].z * v.z + m.value[3].z * v.w);
	}
	template<typename T>
	vec4_t<T> operator*(const vec3_t<T>& v, const mat4x3_t<T>& m)
	{
		return vec4_t<T>(
			v.x * m.value[0].x + v.y * m.value[0].y + v.z * m.value[0].z,
			v.x * m.value[1].x + v.y * m.value[1].y + v.z * m.value[1].z,
			v.x * m.value[2].x + v.y * m.value[2].y + v.z * m.value[2].z,
			v.x * m.value[3].x + v.y * m.value[3].y + v.z * m.value[3].z);
	}

	typedef vec2_t<float>				vec2;
	typedef vec3_t<float>				vec3;
	typedef vec4_t<float>				vec4;
	typedef vec2_t<int>					vec2i;
	typedef vec3_t<int>					vec3i;
	typedef vec4_t<int>					vec4i;
	typedef vec2_t<unsigned int>		vec2ui;
	typedef vec3_t<unsigned int>		vec3ui;
	typedef vec4_t<unsigned int>		vec4ui;
	typedef vec2_t<float>				vec2f;
	typedef vec3_t<float>				vec3f;
	typedef vec4_t<float>				vec4f;
	typedef vec2_t<double>				vec2d;
	typedef vec3_t<double>				vec3d;
	typedef vec4_t<double>				vec4d;
	typedef vec2_t<bool>				vec2b;
	typedef vec3_t<bool>				vec3b;
	typedef vec4_t<bool>				vec4b;

	typedef mat2x2_t<float>				mat2;
	typedef mat3x3_t<float>				mat3;
	typedef mat4x4_t<float>				mat4;
	typedef mat2x3_t<float>				mat2x3;
	typedef mat2x4_t<float>				mat2x4;
	typedef mat3x2_t<float>				mat3x2;
	typedef mat3x4_t<float>				mat3x4;
	typedef mat4x2_t<float>				mat4x2;
	typedef mat4x3_t<float>				mat4x3;
	typedef mat2x2_t<int>				mat2i;
	typedef mat3x3_t<int>				mat3i;
	typedef mat4x4_t<int>				mat4i;
	typedef mat2x3_t<int>				mat2x3i;
	typedef mat2x4_t<int>				mat2x4i;
	typedef mat3x2_t<int>				mat3x2i;
	typedef mat3x4_t<int>				mat3x4i;
	typedef mat4x2_t<int>				mat4x2i;
	typedef mat4x3_t<int>				mat4x3i;
	typedef mat2x2_t<unsigned int>		mat2ui;
	typedef mat3x3_t<unsigned int>		mat3ui;
	typedef mat4x4_t<unsigned int>		mat4ui;
	typedef mat2x3_t<unsigned int>		mat2x3ui;
	typedef mat2x4_t<unsigned int>		mat2x4ui;
	typedef mat3x2_t<unsigned int>		mat3x2ui;
	typedef mat3x4_t<unsigned int>		mat3x4ui;
	typedef mat4x2_t<unsigned int>		mat4x2ui;
	typedef mat4x3_t<unsigned int>		mat4x3ui;
	typedef mat2x2_t<float>				mat2f;
	typedef mat3x3_t<float>				mat3f;
	typedef mat4x4_t<float>				mat4f;
	typedef mat2x3_t<float>				mat2x3f;
	typedef mat2x4_t<float>				mat2x4f;
	typedef mat3x2_t<float>				mat3x2f;
	typedef mat3x4_t<float>				mat3x4f;
	typedef mat4x2_t<float>				mat4x2f;
	typedef mat4x3_t<float>				mat4x3f;
	typedef mat2x2_t<double>			mat2d;
	typedef mat3x3_t<double>			mat3d;
	typedef mat4x4_t<double>			mat4d;
	typedef mat2x3_t<double>			mat2x3d;
	typedef mat2x4_t<double>			mat2x4d;
	typedef mat3x2_t<double>			mat3x2d;
	typedef mat3x4_t<double>			mat3x4d;
	typedef mat4x2_t<double>			mat4x2d;
	typedef mat4x3_t<double>			mat4x3d;

	enum class FrameBufferColorFormat
	{
		None = 0,
		Auto = FrameBufferColorFormat::None,
		RGB, RGBA, RGBA8, RGBA16F, RGBA32F, RED_INT,
	};

	enum class FrameBufferDepthFormat
	{
		None = 0,
		Auto = FrameBufferDepthFormat::None,
		DEPTH_COMPONENT,
		DEPTH24STENCIL8
	};

	enum class RenderTopologyType
	{
		None = 0,
		Point,
		Line,
		LineStrip,
		Triangle,
		TriangleStrip,
	};

	enum class RenderCullFaceMode
	{
		Front,
		Back,
		Both,
		Disable
	};

	enum class RenderCullFrontFace
	{
		Clockwise,
		CounterClockwise,

		CW = Clockwise,
		CCW = CounterClockwise
	};

	enum class Sample
	{
		SampleCount_1_Bit = 0x00000001,
		SampleCount_2_Bit = 0x00000002,
		SampleCount_4_Bit = 0x00000004,
		SampleCount_8_Bit = 0x00000008,
		SampleCount_16_Bit = 0x00000010,
		SampleCount_32_Bit = 0x00000020,
		SampleCount_64_Bit = 0x00000040,
		SampleCount_Max_Bit = 0x7FFFFFFF
	};

	enum class ColorBlendFactor
	{
		Zero = 0,
		One = 1,
		SrcColor = 2,
		OneMinusSrcColor = 3,
		DstColor = 4,
		OneMinusDstColor = 5,
		SrcAlpha = 6,
		OneMinusSrcAlpha = 7,
		DstAlpha = 8,
		OneMinusDstAlpha = 9,
		ConstantColor = 10,
		OneMinusConstantColor = 11,
		ConstantAlpha = 12,
		OneMinusConstantAlpha = 13,
		SrcAlphaSaturate = 14,
		Src1Color = 15,
		OneMinusSrc1Color = 16,
		Src1_Alpha = 17,
		OneMinusSrc1Alpha = 18,
	};

	enum class ColorBlendOperation
	{
		Add = 0,
		Subtract = 1,
		ReverseSubtract = 2,
		Min = 3,
		Max = 4,
		ZeroExt = 1000148000,
		SrcExt = 1000148001,
		DstExt = 1000148002,
		SrcOverExt = 1000148003,
		DstOverExt = 1000148004,
		SrcInExt = 1000148005,
		DstInExt = 1000148006,
		SrcOutExt = 1000148007,
		DstOutExt = 1000148008,
		SrcAtopExt = 1000148009,
		DstAtopExt = 1000148010,
		XorExt = 1000148011,
		MultiplyExt = 1000148012,
		ScreenExt = 1000148013,
		OverlayExt = 1000148014,
		DarkenExt = 1000148015,
		LightenExt = 1000148016,
		ColorDodgeExt = 1000148017,
		ColorBurnExt = 1000148018,
		HardLightExt = 1000148019,
		SoftLightExt = 1000148020,
		DifferenceExt = 1000148021,
		ExclusionExt = 1000148022,
		InvertExt = 1000148023,
		InvertRGBExt = 1000148024,
		LinearDodgeExt = 1000148025,
		LinearBurnExt = 1000148026,
		VividLightExt = 1000148027,
		LinearLightExt = 1000148028,
		PinLightExt = 1000148029,
		HardMixExt = 1000148030,
		HSLHueExt = 1000148031,
		HSLSaturationExt = 1000148032,
		HSLColorExt = 1000148033,
		HSLLuminosityExt = 1000148034,
		PlusExt = 1000148035,
		PlusClampedExt = 1000148036,
		PlusClampedAlphaExt = 1000148037,
		PlusDarkerExt = 1000148038,
		MinusExt = 1000148039,
		MinusClampedExt = 1000148040,
		ContrastExt = 1000148041,
		InvertOVGExt = 1000148042,
		RedExt = 1000148043,
		GreenExt = 1000148044,
		BlueExt = 1000148045,
	};

	enum class CompareOperation
	{
		Never = 0,
		Less = 1,
		Equal = 2,
		LessOrEqual = 3,
		Greater = 4,
		NotEqual = 5,
		GreaterOrEqual = 6,
		Always = 7,
	};

	enum class GraphicsAPI
	{
		None = 0,
		opengl,
		vulkan,
	};

	enum class StencilOperation
	{
		Keep = 0,
		Zero = 1,
		Replace = 2,
		IncrementAndClamp = 3,
		DecrementAndClamp = 4,
		Invert = 5,
		IncrementAndWrap = 6,
		DecrementAndWrap = 7,
	};

	enum class PhysicalDeviceType
	{
		Other = 0,
		Integrated_GPU = 1,
		Discrete_GPU = 2,
		Virtual_GPU = 3,
		CPU = 4,
	};

	struct PhysicalDeviceInfo
	{
		const char* name;
		PhysicalDeviceType type;
		uint32_t apiVersion;
		uint32_t driverVersion;
	};

	struct PhysicalDevice
	{
		typedef void* PhysicalDeviceHandle;

		PhysicalDeviceInfo info;
		PhysicalDeviceHandle device;
	};

	struct RendererBackends
	{
		bool enableValidationLayers;
	};

	/* [Graphics API] */
	bool				createGraphicsContext(GraphicsAPI graphicsapi, RendererBackends* renderBackends);
	bool				terminateGraphicsContext();
	GraphicsAPI			getGraphicsAPI();
	const char*			getGraphicsAPIName();
	void				getPhysicalDevices(PhysicalDevice* pPhysicalDevices, uint32_t* deviceCount);

	void				renderClearColor(const float r, const float g, const float b, const float w);
	void				renderClear();
	void				renderDraw(uint32_t vertexCount);
	void				renderDrawIndexed(uint32_t indexCount);
	void				renderDrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstInstance);
	void				renderDrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t firstInstance);
	void				renderSetStencilReference(uint32_t reference);
	void				renderSetStencilMask(uint32_t compareMask, uint32_t writeMask);
	void				renderBeginNextFrame();
	void				renderDrawSubmit();
	void				renderBeginRenderPass();
	void				renderEndRenderPass();

	VertexBuffer*		createVertexBuffer(float* vertices, uint32_t size);
	IndexBuffer*		createIndexBuffer(uint32_t indices, uint32_t size);
	VertexArray*		createVertexArray();

	void				linkVertexArrayBuffers(VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, VertexLayoutLinkInfo* vertexLayouts);
	void				linkVertexArrayBuffers(VertexArrayCreateInfo* createInfo);

	void				bindVertexBuffer(VertexBuffer* vertexBuffer);
	void				bindIndexBuffer(VertexBuffer* vertexBuffer);
	void				bindVertexArray(VertexBuffer* vertexBuffer);

	void				destroyVertexBuffer(VertexBuffer* vertexBuffer);
	void				destroyIndexBuffer(VertexBuffer* vertexBuffer);
	void				destroyVertexArray(VertexBuffer* vertexBuffer);
}

#endif