#ifndef HG_LVN_LMATH_H
#define HG_LVN_LMATH_H

// [LAYOUT]: lvn_lmath
// ------------------------------------------------------------
// - math library for vectors, matrices, quaternians
//
// [SECTION]: Enums
// [SECTION]: Struct Declaration
// [SECTION]: Functions
// [SECTION]: Struct Implementation


#include "lvn_config.h"

#include <limits>
#include <cmath>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#define LVN_PI M_PI

using std::abs;
using std::acos;
using std::asin;
using std::atan;
using std::atan2;
using std::cos;
using std::sin;
using std::tan;
using std::cosh;
using std::sinh;
using std::tanh;
using std::exp;
using std::frexp;
using std::ldexp;
using std::log;
using std::log10;
using std::modf;
using std::pow;
using std::sqrt;
using std::ceil;
using std::fabs;
using std::floor;
using std::fmod;


// [SECTION]: Enums
// ------------------------------------------------------------

enum LvnClipRegion
{
    Lvn_ClipRegion_ApiSpecific,
    Lvn_ClipRegion_LeftHandZeroToOne,
    Lvn_ClipRegion_LeftHandNegOneToOne,
    Lvn_ClipRegion_RightHandZeroToOne,
    Lvn_ClipRegion_RightHandNegOneToOne,

    Lvn_ClipRegion_LHZO = Lvn_ClipRegion_LeftHandZeroToOne,
    Lvn_ClipRegion_LHNO = Lvn_ClipRegion_LeftHandNegOneToOne,
    Lvn_ClipRegion_RHZO = Lvn_ClipRegion_RightHandZeroToOne,
    Lvn_ClipRegion_RHNO = Lvn_ClipRegion_RightHandNegOneToOne,
};


// [SECTION]: Struct Declaration
// ------------------------------------------------------------

template <typename T> struct LvnFloatType;
template <> struct LvnFloatType<float> { using type = float; };
template <> struct LvnFloatType<double> { using type = double; };
template <typename T> using LvnFloatType_t = typename LvnFloatType<T>::type;

typedef int length_t;

template<length_t L, typename T>
struct LvnVec;

template<typename T>
struct LvnVec<2, T>;
template<typename T>
struct LvnVec<3, T>;
template<typename T>
struct LvnVec<4, T>;

template<length_t R, length_t C, typename T>
struct LvnMat;
template<typename T>
struct LvnMat<2, 2, T>;
template<typename T>
struct LvnMat<3, 3, T>;
template<typename T>
struct LvnMat<4, 4, T>;
template<typename T>
struct LvnMat<2, 3, T>;
template<typename T>
struct LvnMat<2, 4, T>;
template<typename T>
struct LvnMat<3, 2, T>;
template<typename T>
struct LvnMat<3, 4, T>;
template<typename T>
struct LvnMat<4, 2, T>;
template<typename T>
struct LvnMat<4, 3, T>;

template<typename T>
struct LvnQuat_t;

typedef LvnVec<2, float>               LvnVec2;
typedef LvnVec<3, float>               LvnVec3;
typedef LvnVec<4, float>               LvnVec4;
typedef LvnVec<2, int>                 LvnVec2i;
typedef LvnVec<3, int>                 LvnVec3i;
typedef LvnVec<4, int>                 LvnVec4i;
typedef LvnVec<2, unsigned int>        LvnVec2ui;
typedef LvnVec<3, unsigned int>        LvnVec3ui;
typedef LvnVec<4, unsigned int>        LvnVec4ui;
typedef LvnVec<2, double>              LvnVec2d;
typedef LvnVec<3, double>              LvnVec3d;
typedef LvnVec<4, double>              LvnVec4d;
typedef LvnVec<2, float>               LvnVec2f;
typedef LvnVec<3, float>               LvnVec3f;
typedef LvnVec<4, float>               LvnVec4f;
typedef LvnVec<2, bool>                LvnVec2b;
typedef LvnVec<3, bool>                LvnVec3b;
typedef LvnVec<4, bool>                LvnVec4b;

typedef LvnMat<2, 2, float>            LvnMat2;
typedef LvnMat<3, 3, float>            LvnMat3;
typedef LvnMat<4, 4, float>            LvnMat4;
typedef LvnMat<2, 3, float>            LvnMat2x3;
typedef LvnMat<2, 4, float>            LvnMat2x4;
typedef LvnMat<3, 2, float>            LvnMat3x2;
typedef LvnMat<3, 4, float>            LvnMat3x4;
typedef LvnMat<4, 2, float>            LvnMat4x2;
typedef LvnMat<4, 3, float>            LvnMat4x3;
typedef LvnMat<2, 2, int>              LvnMat2i;
typedef LvnMat<3, 3, int>              LvnMat3i;
typedef LvnMat<4, 4, int>              LvnMat4i;
typedef LvnMat<2, 3, int>              LvnMat2x3i;
typedef LvnMat<2, 4, int>              LvnMat2x4i;
typedef LvnMat<3, 2, int>              LvnMat3x2i;
typedef LvnMat<3, 4, int>              LvnMat3x4i;
typedef LvnMat<4, 2, int>              LvnMat4x2i;
typedef LvnMat<4, 3, int>              LvnMat4x3i;
typedef LvnMat<2, 2, unsigned int>     LvnMat2ui;
typedef LvnMat<3, 3, unsigned int>     LvnMat3ui;
typedef LvnMat<4, 4, unsigned int>     LvnMat4ui;
typedef LvnMat<2, 3, unsigned int>     LvnMat2x3ui;
typedef LvnMat<2, 4, unsigned int>     LvnMat2x4ui;
typedef LvnMat<3, 2, unsigned int>     LvnMat3x2ui;
typedef LvnMat<3, 4, unsigned int>     LvnMat3x4ui;
typedef LvnMat<4, 2, unsigned int>     LvnMat4x2ui;
typedef LvnMat<4, 3, unsigned int>     LvnMat4x3ui;
typedef LvnMat<2, 2, double>           LvnMat2d;
typedef LvnMat<3, 3, double>           LvnMat3d;
typedef LvnMat<4, 4, double>           LvnMat4d;
typedef LvnMat<2, 3, double>           LvnMat2x3d;
typedef LvnMat<2, 4, double>           LvnMat2x4d;
typedef LvnMat<3, 2, double>           LvnMat3x2d;
typedef LvnMat<3, 4, double>           LvnMat3x4d;
typedef LvnMat<4, 2, double>           LvnMat4x2d;
typedef LvnMat<4, 3, double>           LvnMat4x3d;
typedef LvnMat<2, 2, float>            LvnMat2f;
typedef LvnMat<3, 3, float>            LvnMat3f;
typedef LvnMat<4, 4, float>            LvnMat4f;
typedef LvnMat<2, 3, float>            LvnMat2x3f;
typedef LvnMat<2, 4, float>            LvnMat2x4f;
typedef LvnMat<3, 2, float>            LvnMat3x2f;
typedef LvnMat<3, 4, float>            LvnMat3x4f;
typedef LvnMat<4, 2, float>            LvnMat4x2f;
typedef LvnMat<4, 3, float>            LvnMat4x3f;

typedef LvnQuat_t<float>               LvnQuat;
typedef LvnQuat_t<int>                 LvnQuati;
typedef LvnQuat_t<unsigned int>        LvnQuatui;
typedef LvnQuat_t<float>               LvnQuatf;
typedef LvnQuat_t<double>              LvnQuatd;


// [SECTION]: Functions
// ------------------------------------------------------------

namespace lvn
{
    typedef LvnVec<2, float>               vec2;
    typedef LvnVec<3, float>               vec3;
    typedef LvnVec<4, float>               vec4;
    typedef LvnVec<2, int>                 vec2i;
    typedef LvnVec<3, int>                 vec3i;
    typedef LvnVec<4, int>                 vec4i;
    typedef LvnVec<2, unsigned int>        vec2ui;
    typedef LvnVec<3, unsigned int>        vec3ui;
    typedef LvnVec<4, unsigned int>        vec4ui;
    typedef LvnVec<2, double>              vec2d;
    typedef LvnVec<3, double>              vec3d;
    typedef LvnVec<4, double>              vec4d;
    typedef LvnVec<2, float>               vec2f;
    typedef LvnVec<3, float>               vec3f;
    typedef LvnVec<4, float>               vec4f;
    typedef LvnVec<2, bool>                vec2b;
    typedef LvnVec<3, bool>                vec3b;
    typedef LvnVec<4, bool>                vec4b;

    typedef LvnMat<2, 2, float>            mat2;
    typedef LvnMat<3, 3, float>            mat3;
    typedef LvnMat<4, 4, float>            mat4;
    typedef LvnMat<2, 3, float>            mat2x3;
    typedef LvnMat<2, 4, float>            mat2x4;
    typedef LvnMat<3, 2, float>            mat3x2;
    typedef LvnMat<3, 4, float>            mat3x4;
    typedef LvnMat<4, 2, float>            mat4x2;
    typedef LvnMat<4, 3, float>            mat4x3;
    typedef LvnMat<2, 2, int>              mat2i;
    typedef LvnMat<3, 3, int>              mat3i;
    typedef LvnMat<4, 4, int>              mat4i;
    typedef LvnMat<2, 3, int>              mat2x3i;
    typedef LvnMat<2, 4, int>              mat2x4i;
    typedef LvnMat<3, 2, int>              mat3x2i;
    typedef LvnMat<3, 4, int>              mat3x4i;
    typedef LvnMat<4, 2, int>              mat4x2i;
    typedef LvnMat<4, 3, int>              mat4x3i;
    typedef LvnMat<2, 2, unsigned int>     mat2ui;
    typedef LvnMat<3, 3, unsigned int>     mat3ui;
    typedef LvnMat<4, 4, unsigned int>     mat4ui;
    typedef LvnMat<2, 3, unsigned int>     mat2x3ui;
    typedef LvnMat<2, 4, unsigned int>     mat2x4ui;
    typedef LvnMat<3, 2, unsigned int>     mat3x2ui;
    typedef LvnMat<3, 4, unsigned int>     mat3x4ui;
    typedef LvnMat<4, 2, unsigned int>     mat4x2ui;
    typedef LvnMat<4, 3, unsigned int>     mat4x3ui;
    typedef LvnMat<2, 2, double>           mat2d;
    typedef LvnMat<3, 3, double>           mat3d;
    typedef LvnMat<4, 4, double>           mat4d;
    typedef LvnMat<2, 3, double>           mat2x3d;
    typedef LvnMat<2, 4, double>           mat2x4d;
    typedef LvnMat<3, 2, double>           mat3x2d;
    typedef LvnMat<3, 4, double>           mat3x4d;
    typedef LvnMat<4, 2, double>           mat4x2d;
    typedef LvnMat<4, 3, double>           mat4x3d;
    typedef LvnMat<2, 2, float>            mat2f;
    typedef LvnMat<3, 3, float>            mat3f;
    typedef LvnMat<4, 4, float>            mat4f;
    typedef LvnMat<2, 3, float>            mat2x3f;
    typedef LvnMat<2, 4, float>            mat2x4f;
    typedef LvnMat<3, 2, float>            mat3x2f;
    typedef LvnMat<3, 4, float>            mat3x4f;
    typedef LvnMat<4, 2, float>            mat4x2f;
    typedef LvnMat<4, 3, float>            mat4x3f;

    typedef LvnQuat_t<float>               quat;
    typedef LvnQuat_t<int>                 quati;
    typedef LvnQuat_t<unsigned int>        quatui;
    typedef LvnQuat_t<float>               quatf;
    typedef LvnQuat_t<double>              quatd;

    LVN_API inline LvnClipRegion i_ClipRegion = Lvn_ClipRegion_RHNO;

    LVN_API inline LvnClipRegion getClipRegionEnum()
    {
        return i_ClipRegion;
    }

    LVN_API inline void setClipRegionEnum(LvnClipRegion region)
    {
        i_ClipRegion = region;
    }

    // -- math functions
    template <typename T>
    LVN_API T               min(const T& n1, const T& n2) { return n1 < n2 ? n1 : n2; }
    template <typename T>
    LVN_API T               max(const T& n1, const T& n2) { return n1 > n2 ? n1 : n2; }
    template <typename T>
    LVN_API T               clamp(const T& val, const T& low, const T& high) { return lvn::max(lvn::min(val, high), low); }
    template <typename T>
    LVN_API LvnVec<2,T>     midpoint(const T& x1, const T& y1, const T& x2, const T& y2) { return LvnVec<2, T>((x1 + x2) / static_cast<T>(2), (y1 + y2) / static_cast<T>(2)); }
    template <typename T>
    LVN_API LvnVec<3,T>     midpoint(const T& x1, const T& y1, const T& z1, const T& x2, const T& y2, const T& z2) { return LvnVec<3, T>((x1 + x2) / static_cast<T>(2), (y1 + y2) / static_cast<T>(2), (z1 + z2) / static_cast<T>(2)); }
    template <typename T>
    LVN_API LvnVec<4,T>     midpoint(const T& x1, const T& y1, const T& z1, const T& w1, const T& x2, const T& y2, const T& z2, const T& w2) { return LvnVec<4, T>((x1 + x2) / static_cast<T>(2), (y1 + y2) / static_cast<T>(2), (z1 + z2) / static_cast<T>(2), (w1 + w2) / static_cast<T>(2)); }
    template <typename T>
    LVN_API LvnVec<2,T>     midpoint(const LvnVec<2,T>& v1, const LvnVec<2,T>& v2) { return LvnVec<2,T>((v1.x + v2.x) / static_cast<T>(2), (v1.y + v2.y) / static_cast<T>(2)); }
    template <typename T>
    LVN_API LvnVec<3,T>     midpoint(const LvnVec<3,T>& v1, const LvnVec<3,T>& v2) { return LvnVec<3,T>((v1.x + v2.x) / static_cast<T>(2), (v1.y + v2.y) / static_cast<T>(2), (v1.z + v2.z) / static_cast<T>(2)); }
    template <typename T>
    LVN_API LvnVec<4,T>     midpoint(const LvnVec<4,T>& v1, const LvnVec<4,T>& v2) { return LvnVec<4,T>((v1.x + v2.x) / static_cast<T>(2), (v1.y + v2.y) / static_cast<T>(2), (v1.z + v2.z) / static_cast<T>(2), (v1.w + v2.w) / static_cast<T>(2)); }
    template <typename T>
    LVN_API T               distance(const T& x1, const T& y1, const T& x2, const T& y2) { return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)); }
    template <typename T>
    LVN_API T               distance(const T& x1, const T& y1, const T& z1, const T& x2, const T& y2, const T& z2) { return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2)); }
    template <typename T>
    LVN_API T               distance(const T& x1, const T& y1, const T& z1, const T& w1, const T& x2, const T& y2, const T& z2, const T& w2) { return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2)+(w1-w2)*(w1-w2)); }
    template <typename T>
    LVN_API T               distance(const LvnVec<2, T>& v1, const LvnVec<2, T>& v2) { return sqrt((v1.x-v2.x)*(v1.x-v2.x)+(v1.y-v2.y)*(v1.y-v2.y)); }
    template <typename T>
    LVN_API T               distance(const LvnVec<3, T>& v1, const LvnVec<3, T>& v2) { return sqrt((v1.x-v2.x)*(v1.x-v2.x)+(v1.y-v2.y)*(v1.y-v2.y)+(v1.z-v2.z)*(v1.z-v2.z)); }
    template <typename T>
    LVN_API T               distance(const LvnVec<4, T>& v1, const LvnVec<4, T>& v2) { return sqrt((v1.x-v2.x)*(v1.x-v2.x)+(v1.y-v2.y)*(v1.y-v2.y)+(v1.z-v2.z)*(v1.z-v2.z)+(v1.w-v2.w)*(v1.w-v2.w)); }
    template <typename T>
    LVN_API bool            within(T num, T within, T range) { return num <= (within + range) && num >= (within - range); }
    template <typename T>
    LVN_API bool            within(T num, T within, T lowerRange, T upperRange) { return num <= (within + upperRange) && num >= (within - lowerRange); }

    template <typename T>
    LVN_API LvnFloatType_t<T> radians(T deg)
    {
        return deg * static_cast<T>(0.0174532925199); // deg * (PI / 180)
    }

    template <typename T>
    LVN_API LvnFloatType_t<T> degrees(T rad)
    {
        return rad * static_cast<T>(57.2957795131); // rad * (180 / PI)
    }

    template <typename T>
    LVN_API LvnFloatType_t<T> clampAngle(T rad)
    {
        T angle = fmod(rad, static_cast<T>(2 * LVN_PI));
        if (angle < 0) { angle += static_cast<T>(2 * LVN_PI); }
        return angle;
    }

    template <typename T>
    LVN_API LvnFloatType_t<T> clampAngleDeg(T deg)
    {
        T angle = fmod(deg, static_cast<T>(360));
        if (angle < 0) { angle += static_cast<T>(360); }
        return angle;
    }

    template <typename T>
    LVN_API LvnVec<2, T> normalize(const LvnVec<2, T>& v)
    {
        T u = static_cast<T>(1) / sqrt(v.x * v.x + v.y * v.y);
        return LvnVec<2, T>(v.x * u, v.y * u);
    }

    template <typename T>
    LVN_API LvnVec<3, T> normalize(const LvnVec<3, T>& v)
    {
        T u = static_cast<T>(1) / sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        return LvnVec<3, T>(v.x * u, v.y * u, v.z * u);
    }

    template <typename T>
    LVN_API LvnVec<4, T> normalize(const LvnVec<4, T>& v)
    {
        T u = static_cast<T>(1) / sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
        return LvnVec<4, T>(v.x * u, v.y * u, v.z * u, v.w * u);
    }

    template <typename T>
    LVN_API LvnQuat_t<T> normalize(const LvnQuat_t<T>& quat)
    {
        const T qw = quat.w;
        const T qx = quat.x;
        const T qy = quat.y;
        const T qz = quat.z;

        const float n = static_cast<T>(1) / sqrt(qx * qx + qy * qy + qz * qz + qw * qw);

        return LvnQuat_t<T>(qw * n, qx * n, qy * n, qz * n);
    }

    template <typename T>
    LVN_API T mag(LvnVec<2, T> v)
    {
        return sqrt(v.x * v.x + v.y * v.y);
    }

    template <typename T>
    LVN_API T mag(LvnVec<3, T> v)
    {
        return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }

    template <typename T>
    LVN_API T mag(LvnVec<4, T> v)
    {
        return sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
    }

    template <typename T>
    LVN_API T mag2(LvnVec<2, T> v)
    {
        return v.x * v.x + v.y * v.y;
    }

    template <typename T>
    LVN_API T mag2(LvnVec<3, T> v)
    {
        return v.x * v.x + v.y * v.y + v.z * v.z;
    }

    template <typename T>
    LVN_API T mag2(LvnVec<4, T> v)
    {
        return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
    }

    template <typename T>
    LVN_API T mag2(const LvnQuat_t<T>& q)
    {
        return q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z;
    }

    template <typename T>
    LVN_API LvnVec<3, T> cross(const LvnVec<3, T>& v1, const LvnVec<3, T>& v2)
    {
        const T cx = v1.y * v2.z - v1.z * v2.y;
        const T cy = v1.z * v2.x - v1.x * v2.z;
        const T cz = v1.x * v2.y - v1.y * v2.x;
        return LvnVec<3, T>(cx, cy, cz);
    }

    template <typename T>
    LVN_API T dot(const LvnVec<2, T>& v1, const LvnVec<2, T>& v2)
    {
        return v1.x * v2.x + v1.y * v2.y;
    }

    template <typename T>
    LVN_API T dot(const LvnVec<3, T>& v1, const LvnVec<3, T>& v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    template <typename T>
    LVN_API T dot(const LvnVec<4, T>& v1, const LvnVec<4, T>& v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
    }

    template <typename T>
    LVN_API T dot(const LvnQuat_t<T>& q1, const LvnQuat_t<T>& q2)
    {
        return q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
    }

    template <typename T>
    LVN_API T angle(const LvnVec<3, T>& v1, const LvnVec<3, T>& v2)
    {
        return acos(lvn::clamp(lvn::dot(v1, v2), T(-1), T(1)));
    }

    template <typename T>
    LVN_API LvnQuat_t<T> conjugate(const LvnQuat_t<T>& q)
    {
        return LvnQuat_t<T>(q.w, -q.x, -q.y, -q.z);
    }

    template <typename T>
    LVN_API LvnMat<2, 2, T> transpose(const LvnMat<2, 2, T>& m)
    {
        LvnMat<2, 2, T> result;
        result[0][0] = m[0][0];
        result[0][1] = m[1][0];
        result[1][0] = m[0][1];
        result[1][1] = m[1][1];
        return result;
    }

    template <typename T>
    LVN_API LvnMat<3, 3, T> transpose(const LvnMat<3, 3, T>& m)
    {
        LvnMat<3, 3, T> result;
        result[0][0] = m[0][0];
        result[0][1] = m[1][0];
        result[0][2] = m[2][0];
        result[1][0] = m[0][1];
        result[1][1] = m[1][1];
        result[1][2] = m[2][1];
        result[2][0] = m[0][2];
        result[2][1] = m[1][2];
        result[2][2] = m[2][2];
        return result;
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> transpose(const LvnMat<4, 4, T>& m)
    {
        LvnMat<4, 4, T> result;
        result[0][0] = m[0][0];
        result[0][1] = m[1][0];
        result[0][2] = m[2][0];
        result[0][3] = m[3][0];
        result[1][0] = m[0][1];
        result[1][1] = m[1][1];
        result[1][2] = m[2][1];
        result[1][3] = m[3][1];
        result[2][0] = m[0][2];
        result[2][1] = m[1][2];
        result[2][2] = m[2][2];
        result[2][3] = m[3][2];
        result[3][0] = m[0][3];
        result[3][1] = m[1][3];
        result[3][2] = m[2][3];
        result[3][3] = m[3][3];
        return result;
    }

    template <typename T>
    LVN_API LvnMat<3, 2, T> transpose(const LvnMat<2, 3, T>& m)
    {
        LvnMat<3, 2, T> result;
        result[0][0] = m[0][0];
        result[0][1] = m[1][0];
        result[1][0] = m[0][1];
        result[1][1] = m[1][1];
        result[2][0] = m[0][2];
        result[2][1] = m[1][2];
        return result;
    }

    template <typename T>
    LVN_API LvnMat<4, 2, T> transpose(const LvnMat<2, 4, T>& m)
    {
        LvnMat<4, 2, T> result;
        result[0][0] = m[0][0];
        result[0][1] = m[1][0];
        result[1][0] = m[0][1];
        result[1][1] = m[1][1];
        result[2][0] = m[0][2];
        result[2][1] = m[1][2];
        result[3][0] = m[0][3];
        result[3][1] = m[1][3];
        return result;
    }

    template <typename T>
    LVN_API LvnMat<2, 3, T> transpose(const LvnMat<3, 2, T>& m)
    {
        LvnMat<2, 3, T> result;
        result[0][0] = m[0][0];
        result[0][1] = m[1][0];
        result[0][2] = m[2][0];
        result[1][0] = m[0][1];
        result[1][1] = m[1][1];
        result[1][2] = m[2][1];
        return result;
    }

    template <typename T>
    LVN_API LvnMat<4, 3, T> transpose(const LvnMat<3, 4, T>& m)
    {
        LvnMat<4, 3, T> result;
        result[0][0] = m[0][0];
        result[0][1] = m[1][0];
        result[0][2] = m[2][0];
        result[1][0] = m[0][1];
        result[1][1] = m[1][1];
        result[1][2] = m[2][1];
        result[2][0] = m[0][2];
        result[2][1] = m[1][2];
        result[2][2] = m[2][2];
        result[3][0] = m[0][3];
        result[3][1] = m[1][3];
        result[3][2] = m[2][3];
        return result;
    }

    template <typename T>
    LVN_API LvnMat<2, 4, T> transpose(const LvnMat<4, 2, T>& m)
    {
        LvnMat<2, 4, T> result;
        result[0][0] = m[0][0];
        result[0][1] = m[1][0];
        result[0][2] = m[2][0];
        result[0][3] = m[3][0];
        result[1][0] = m[0][1];
        result[1][1] = m[1][1];
        result[1][2] = m[2][1];
        result[1][3] = m[3][1];
        return result;
    }

    template <typename T>
    LVN_API LvnMat<3, 4, T> transpose(const LvnMat<4, 3, T>& m)
    {
        LvnMat<3, 4, T> result;
        result[0][0] = m[0][0];
        result[0][1] = m[1][0];
        result[0][2] = m[2][0];
        result[0][3] = m[3][0];
        result[1][0] = m[0][1];
        result[1][1] = m[1][1];
        result[1][2] = m[2][1];
        result[1][3] = m[3][1];
        result[2][0] = m[0][2];
        result[2][1] = m[1][2];
        result[2][2] = m[2][2];
        result[2][3] = m[3][2];
        return result;
    }

    template <typename T>
    LVN_API T determinant(const LvnMat<2, 2, T>& m)
    {
        return m[0][0] * m[1][1] - m[1][0] * m[0][1];
    }

    template <typename T>
    LVN_API T determinant(const LvnMat<3, 3, T>& m)
    {
        return + m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])
               - m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2])
               + m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]);
    }

    template <typename T>
    LVN_API T determinant(const LvnMat<4, 4, T>& m)
    {
        T sub00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
        T sub01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
        T sub02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
        T sub03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
        T sub04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
        T sub05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];

        LvnVec<4, T> detCof(
            + (m[1][1] * sub00 - m[1][2] * sub01 + m[1][3] * sub02),
            - (m[1][0] * sub00 - m[1][2] * sub03 + m[1][3] * sub04),
            + (m[1][0] * sub01 - m[1][1] * sub03 + m[1][3] * sub05),
            - (m[1][0] * sub02 - m[1][1] * sub04 + m[1][2] * sub05));

        return m[0][0] * detCof[0] + m[0][1] * detCof[1] +
               m[0][2] * detCof[2] + m[0][3] * detCof[3];
    }

    template <typename T>
    LVN_API LvnMat<2, 2, T> inverse(const LvnMat<2, 2, T>& m)
    {
        T oneOverDeterminant = static_cast<T>(1) / (
            + m[0][0] * m[1][1]
            - m[1][0] * m[0][1]);

        LvnMat<2, 2, T> inverse(
            + m[1][1] * oneOverDeterminant,
            - m[0][1] * oneOverDeterminant,
            - m[1][0] * oneOverDeterminant,
            + m[0][0] * oneOverDeterminant);

        return inverse;
    }

    template <typename T>
    LVN_API LvnMat<3, 3, T> inverse(const LvnMat<3, 3, T>& m)
    {
        T oneOverDeterminant = static_cast<T>(1) / (
            + m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])
            - m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2])
            + m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]));

        LvnMat<3, 3, T> inverse;
        inverse[0][0] = + (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * oneOverDeterminant;
        inverse[1][0] = - (m[1][0] * m[2][2] - m[2][0] * m[1][2]) * oneOverDeterminant;
        inverse[2][0] = + (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * oneOverDeterminant;
        inverse[0][1] = - (m[0][1] * m[2][2] - m[2][1] * m[0][2]) * oneOverDeterminant;
        inverse[1][1] = + (m[0][0] * m[2][2] - m[2][0] * m[0][2]) * oneOverDeterminant;
        inverse[2][1] = - (m[0][0] * m[2][1] - m[2][0] * m[0][1]) * oneOverDeterminant;
        inverse[0][2] = + (m[0][1] * m[1][2] - m[1][1] * m[0][2]) * oneOverDeterminant;
        inverse[1][2] = - (m[0][0] * m[1][2] - m[1][0] * m[0][2]) * oneOverDeterminant;
        inverse[2][2] = + (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * oneOverDeterminant;

        return inverse;
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> inverse(const LvnMat<4, 4, T>& m)
    {
        T coef00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
        T coef02 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
        T coef03 = m[1][2] * m[2][3] - m[2][2] * m[1][3];

        T coef04 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
        T coef06 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
        T coef07 = m[1][1] * m[2][3] - m[2][1] * m[1][3];

        T coef08 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
        T coef10 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
        T coef11 = m[1][1] * m[2][2] - m[2][1] * m[1][2];

        T coef12 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
        T coef14 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
        T coef15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];

        T coef16 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
        T coef18 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
        T coef19 = m[1][0] * m[2][2] - m[2][0] * m[1][2];

        T coef20 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
        T coef22 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
        T coef23 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

        LvnVec<4, T> fac0(coef00, coef00, coef02, coef03);
        LvnVec<4, T> fac1(coef04, coef04, coef06, coef07);
        LvnVec<4, T> fac2(coef08, coef08, coef10, coef11);
        LvnVec<4, T> fac3(coef12, coef12, coef14, coef15);
        LvnVec<4, T> fac4(coef16, coef16, coef18, coef19);
        LvnVec<4, T> fac5(coef20, coef20, coef22, coef23);

        LvnVec<4, T> vec0(m[1][0], m[0][0], m[0][0], m[0][0]);
        LvnVec<4, T> vec1(m[1][1], m[0][1], m[0][1], m[0][1]);
        LvnVec<4, T> vec2(m[1][2], m[0][2], m[0][2], m[0][2]);
        LvnVec<4, T> vec3(m[1][3], m[0][3], m[0][3], m[0][3]);

        LvnVec<4, T> inv0(vec1 * fac0 - vec2 * fac1 + vec3 * fac2);
        LvnVec<4, T> inv1(vec0 * fac0 - vec2 * fac3 + vec3 * fac4);
        LvnVec<4, T> inv2(vec0 * fac1 - vec1 * fac3 + vec3 * fac5);
        LvnVec<4, T> inv3(vec0 * fac2 - vec1 * fac4 + vec2 * fac5);

        LvnVec<4, T> signA(+1, -1, +1, -1);
        LvnVec<4, T> signB(-1, +1, -1, +1);
        LvnMat<4, 4, T> inverse(inv0 * signA, inv1 * signB, inv2 * signA, inv3 * signB);

        LvnVec<4, T> row0(inverse[0][0], inverse[1][0], inverse[2][0], inverse[3][0]);

        LvnVec<4, T> dot0(m[0] * row0);
        T dot1 = (dot0.x + dot0.y) + (dot0.z + dot0.w);

        T oneOverDeterminant = static_cast<T>(1) / dot1;

        return inverse * oneOverDeterminant;
    }

    template <typename T>
    LVN_API LvnQuat_t<T> inverse(const LvnQuat_t<T>& q)
    {
        return lvn::conjugate(q) / lvn::dot(q, q);
    }

    template <typename T>
    LVN_API T lerp(const T& start, const T& end, float t)
    {
        return start + t * (end - start);
    }

    template <typename T>
    LVN_API LvnVec<2, T> lerp(const LvnVec<2, T>& start, const LvnVec<2, T>& end, float t)
    {
        return LvnVec<2, T>(lerp(start.x, end.x, t), lerp(start.y, end.y, t));
    }

    template <typename T>
    LVN_API LvnVec<3, T> lerp(const LvnVec<3, T>& start, const LvnVec<3, T>& end, float t)
    {
        return LvnVec<3, T>(lerp(start.x, end.x, t), lerp(start.y, end.y, t), lerp(start.z, end.z, t));
    }

    template <typename T>
    LVN_API LvnVec<4, T> lerp(const LvnVec<4, T>& start, const LvnVec<4, T>& end, float t)
    {
        return LvnVec<4, T>(lerp(start.x, end.x, t), lerp(start.y, end.y, t), lerp(start.z, end.z, t), lerp(start.w, end.w, t));
    }

    template <typename T>
    LVN_API LvnQuat_t<T> slerp(const LvnQuat_t<T>& q1, const LvnQuat_t<T>& q2, float t)
    {
        LvnQuat_t<T> q2s = q2;

        T cosTheta = dot(q1, q2);

        if (cosTheta < static_cast<T>(0))
        {
            q2s = -q2;
            cosTheta = -cosTheta;
        }

        if(cosTheta > static_cast<T>(1) - std::numeric_limits<T>::epsilon())
        {
            return lvn::normalize(LvnQuat_t<T>(
                lvn::lerp(q1.w, q2s.w, t),
                lvn::lerp(q1.x, q2s.x, t),
                lvn::lerp(q1.y, q2s.y, t),
                lvn::lerp(q1.z, q2s.z, t)));
        }
        else
        {
            T angle = acos(cosTheta);
            return (sin((static_cast<T>(1) - t) * angle) * q1 + sin(t * angle) * q2s) / sin(angle);
        }
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> orthoRHZO(T left, T right, T bottom, T top, T zNear, T zFar)
    {
        LvnMat<4, 4, T> matrix(static_cast<T>(1));
        matrix[0][0] = static_cast<T>(2) / (right - left);
        matrix[1][1] = static_cast<T>(2) / (top - bottom);
        matrix[2][2] = - static_cast<T>(2) / (zFar - zNear);
        matrix[3][0] = - (right + left)  / (right - left);
        matrix[3][1] = - (top + bottom)  / (top - bottom);
        matrix[3][2] = - (zFar + zNear) / (zFar - zNear);
        return matrix;
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> orthoRHNO(T left, T right, T bottom, T top, T zNear, T zFar)
    {
        LvnMat<4, 4, T> matrix(static_cast<T>(1));
        matrix[0][0] = static_cast<T>(2) / (right - left);
        matrix[1][1] = static_cast<T>(2) / (top - bottom);
        matrix[2][2] = - static_cast<T>(2) / (zFar - zNear);
        matrix[3][0] = - (right + left)  / (right - left);
        matrix[3][1] = - (top + bottom)  / (top - bottom);
        matrix[3][2] = - zNear / (zFar - zNear);
        return matrix;
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> orthoLHZO(T left, T right, T bottom, T top, T zNear, T zFar)
    {
        LvnMat<4, 4, T> matrix(static_cast<T>(1));
        matrix[0][0] = static_cast<T>(2) / (right - left);
        matrix[1][1] = static_cast<T>(2) / (top - bottom);
        matrix[2][2] = static_cast<T>(1) / (zFar - zNear);
        matrix[3][0] = - (right + left)  / (right - left);
        matrix[3][1] = - (top + bottom)  / (top - bottom);
        matrix[3][2] = - zNear / (zFar - zNear);
        return matrix;
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> orthoLHNO(T left, T right, T bottom, T top, T zNear, T zFar)
    {
        LvnMat<4, 4, T> matrix(static_cast<T>(1));
        matrix[0][0] = static_cast<T>(2) / (right - left);
        matrix[1][1] = static_cast<T>(2) / (top - bottom);
        matrix[2][2] = static_cast<T>(2) / (zFar - zNear);
        matrix[3][0] = - (right + left)  / (right - left);
        matrix[3][1] = - (top + bottom)  / (top - bottom);
        matrix[3][2] = - (zFar + zNear) / (zFar - zNear);
        return matrix;
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> ortho(T left, T right, T bottom, T top, T zNear, T zFar)
    {
        switch (lvn::getClipRegionEnum())
        {
            case Lvn_ClipRegion_RHZO: { return lvn::orthoRHZO(left, right, bottom, top, zNear, zFar); }
            case Lvn_ClipRegion_RHNO: { return lvn::orthoRHNO(left, right, bottom, top, zNear, zFar); }
            case Lvn_ClipRegion_LHZO: { return lvn::orthoLHZO(left, right, bottom, top, zNear, zFar); }
            case Lvn_ClipRegion_LHNO: { return lvn::orthoLHNO(left, right, bottom, top, zNear, zFar); }

            default: { return lvn::orthoRHNO(left, right, bottom, top, zNear, zFar); } // opengl default
        }
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> perspectiveRHZO(const T& fovy, const T& aspect, const T& zNear, const T& zFar)
    {
        T tanHalfFov = static_cast<T>(tan(fovy / 2));

        LvnMat<4, 4, T> matrix(0);
        matrix[0][0] = static_cast<T>(1) / (aspect * tanHalfFov);
        matrix[1][1] = static_cast<T>(1) / (tanHalfFov);
        matrix[2][2] = zFar / (zNear - zFar);
        matrix[2][3] = static_cast<T>(1);
        matrix[3][2] = - (zFar * zNear) / (zFar - zNear);

        return matrix;
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> perspectiveRHNO(const T& fovy, const T& aspect, const T& zNear, const T& zFar)
    {
        T tanHalfFov = static_cast<T>(tan(fovy / 2));

        LvnMat<4, 4, T> matrix(0);
        matrix[0][0] = static_cast<T>(1) / (aspect * tanHalfFov);
        matrix[1][1] = static_cast<T>(1) / (tanHalfFov);
        matrix[2][2] = - (zFar + zNear) / (zFar - zNear);
        matrix[2][3] = - static_cast<T>(1);
        matrix[3][2] = - (static_cast<T>(2) * zFar * zNear) / (zFar - zNear);

        return matrix;
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> perspectiveLHZO(const T& fovy, const T& aspect, const T& zNear, const T& zFar)
    {
        T tanHalfFov = static_cast<T>(tan(fovy / 2));

        LvnMat<4, 4, T> matrix(0);
        matrix[0][0] = static_cast<T>(1) / (aspect * tanHalfFov);
        matrix[1][1] = static_cast<T>(1) / (tanHalfFov);
        matrix[2][2] = zFar / (zFar - zNear);
        matrix[2][3] = static_cast<T>(1);
        matrix[3][2] = - (zFar * zNear) / (zFar - zNear);

        return matrix;
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> perspectiveLHNO(const T& fovy, const T& aspect, const T& zNear, const T& zFar)
    {
        T tanHalfFov = static_cast<T>(tan(fovy / 2));

        LvnMat<4, 4, T> matrix(0);
        matrix[0][0] = static_cast<T>(1) / (aspect * tanHalfFov);
        matrix[1][1] = static_cast<T>(1) / (tanHalfFov);
        matrix[2][2] = (zFar + zNear) / (zFar - zNear);
        matrix[2][3] = static_cast<T>(1);
        matrix[3][2] = - (static_cast<T>(2) * zFar * zNear) / (zFar - zNear);

        return matrix;
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> perspective(const T& fovy, const T& aspect, const T& zNear, const T& zFar)
    {
        switch (lvn::getClipRegionEnum())
        {
            case Lvn_ClipRegion_RHZO: { return lvn::perspectiveRHZO(fovy, aspect, zNear, zFar); }
            case Lvn_ClipRegion_RHNO: { return lvn::perspectiveRHNO(fovy, aspect, zNear, zFar); }
            case Lvn_ClipRegion_LHZO: { return lvn::perspectiveLHZO(fovy, aspect, zNear, zFar); }
            case Lvn_ClipRegion_LHNO: { return lvn::perspectiveLHNO(fovy, aspect, zNear, zFar); }

            default: { return lvn::perspectiveRHNO(fovy, aspect, zNear, zFar); } // opengl default
        }
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> lookAtRH(const LvnVec<3, T>& eye, const LvnVec<3, T>& center, const LvnVec<3, T>& up)
    {
        LvnVec<3, T> f(lvn::normalize(center - eye));
        LvnVec<3, T> s(lvn::normalize(lvn::cross(f, up)));
        LvnVec<3, T> u(lvn::cross(s, f));

        LvnMat<4, 4, T> matrix(static_cast<T>(1));
        matrix[0][0] =  s.x;
        matrix[1][0] =  s.y;
        matrix[2][0] =  s.z;
        matrix[0][1] =  u.x;
        matrix[1][1] =  u.y;
        matrix[2][1] =  u.z;
        matrix[0][2] = -f.x;
        matrix[1][2] = -f.y;
        matrix[2][2] = -f.z;
        matrix[3][0] = -lvn::dot(s, eye);
        matrix[3][1] = -lvn::dot(u, eye);
        matrix[3][2] =  lvn::dot(f, eye);
        return matrix;
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> lookAtLH(const LvnVec<3, T>& eye, const LvnVec<3, T>& center, const LvnVec<3, T>& up)
    {
        LvnVec<3, T> f(lvn::normalize(center - eye));
        LvnVec<3, T> s(lvn::normalize(lvn::cross(up, f)));
        LvnVec<3, T> u(lvn::cross(f, s));

        LvnMat<4, 4, T> matrix(static_cast<T>(1));
        matrix[0][0] = s.x;
        matrix[1][0] = s.y;
        matrix[2][0] = s.z;
        matrix[0][1] = u.x;
        matrix[1][1] = u.y;
        matrix[2][1] = u.z;
        matrix[0][2] = f.x;
        matrix[1][2] = f.y;
        matrix[2][2] = f.z;
        matrix[3][0] = -lvn::dot(s, eye);
        matrix[3][1] = -lvn::dot(u, eye);
        matrix[3][2] = -lvn::dot(f, eye);
        return matrix;
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> lookAt(const LvnVec<3, T>& eye, const LvnVec<3, T>& center, const LvnVec<3, T>& up)
    {
        switch (lvn::getClipRegionEnum())
        {
            case Lvn_ClipRegion_RHZO: { return lvn::lookAtRH(eye, center, up); }
            case Lvn_ClipRegion_RHNO: { return lvn::lookAtRH(eye, center, up); }
            case Lvn_ClipRegion_LHZO: { return lvn::lookAtLH(eye, center, up); }
            case Lvn_ClipRegion_LHNO: { return lvn::lookAtLH(eye, center, up); }

            default: { return lvn::lookAtRH(eye, center, up); } // opengl default
        }
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> translate(const LvnMat<4, 4, T>& mat, const LvnVec<3, T>& vec)
    {
        LvnMat<4, 4, T> translate(static_cast<T>(1));
        translate[3][0] = vec.x;
        translate[3][1] = vec.y;
        translate[3][2] = vec.z;

        return mat * translate;
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> scale(const LvnMat<4, 4, T>& mat, const LvnVec<3, T>& vec)
    {
        LvnMat<4, 4, T> scale(static_cast<T>(1));
        scale[0][0] = vec.x;
        scale[1][1] = vec.y;
        scale[2][2] = vec.z;

        return mat * scale;
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> rotate(const LvnMat<4, 4, T>& mat, T angle, const LvnVec<3, T>& axis)
    {
        const T c = cos(angle);
        const T s = sin(angle);
        const T nc = static_cast<T>(1) - cos(angle);

        LvnMat<4, 4, T> rotate(static_cast<T>(1));
        rotate[0][0] = c + axis.x * axis.x * nc;
        rotate[0][1] = axis.x * axis.y * nc + axis.z * s;
        rotate[0][2] = axis.x * axis.z * nc - axis.y * s;

        rotate[1][0] = axis.x * axis.y * nc - axis.z * s;
        rotate[1][1] = c + axis.y * axis.y * nc;
        rotate[1][2] = axis.y * axis.z * nc + axis.x * s;

        rotate[2][0] = axis.x * axis.z * nc + axis.y * s;
        rotate[2][1] = axis.y * axis.z * nc - axis.x * s;
        rotate[2][2] = c + axis.z * axis.z * nc;

        return mat * rotate;
    }

    template <typename T>
    LvnVec<2, T> rotate(const LvnVec<2, T>& v, const T& angle)
    {
        LvnVec<2, T> result;
        const T rcos(cos(angle));
        const T rsin(sin(angle));

        result.x = v.x * rcos - v.y * rsin;
        result.y = v.x * rsin + v.y * rcos;
        return result;
    }

    template <typename T>
    LvnVec<3, T> rotate(const LvnVec<3, T>& v, const T& angle, const LvnVec<3, T>& axis)
    {
        return LvnMat<3, 3, T>(lvn::rotate(LvnMat<4, 4, T>(static_cast<T>(1)), angle, axis)) * v;
    }

    template <typename T>
    LVN_API LvnQuat_t<T> angleAxis(const T& angle, const LvnVec<3, T>& axis)
    {
        const T s = sin(angle / 2);
        return LvnQuat_t<T>(cos(angle / 2), axis.x * s, axis.y * s, axis.z * s);
    }

    template <typename T>
    LVN_API LvnMat<4, 4, T> quatToMat4(const LvnQuat_t<T> quat)
    {
        const T w = quat.w;
        const T x = quat.x;
        const T y = quat.y;
        const T z = quat.z;

        LvnMat<4, 4, T> matrix(static_cast<T>(1));
        matrix[0][0] = static_cast<T>(1) - 2 * (y * y + z * z);
        matrix[0][1] = 2 * (x * y + w * z);
        matrix[0][2] = 2 * (x * z - w * y);
        matrix[1][0] = 2 * (x * y - w * z);
        matrix[1][1] = static_cast<T>(1) - 2 * (x * x + z * z);
        matrix[1][2] = 2 * (y * z + w * x);
        matrix[2][0] = 2 * (x * z + w * y);
        matrix[2][1] = 2 * (y * z - w * x);
        matrix[2][2] = static_cast<T>(1) - 2 * (x * x + y * y);

        return matrix;
    }
}


// [SECTION]: Struct Implementation
// ------------------------------------------------------------

// -- vectors
template<typename T>
struct LvnVec<2, T>
{
    union { T x, r, s, i; };
    union { T y, g, t, j; };

    static length_t length() { return 2; }

    LvnVec() = default;
    LvnVec(const LvnVec<2, T>&) = default;
    LvnVec(const T& n)
        : x(n), y(n) {}
    LvnVec(const T& nx, const T& ny)
        : x(nx), y(ny) {}
    LvnVec(const LvnVec<3, T>& v)
        : x(v.x), y(v.y) {}
    LvnVec(const LvnVec<4, T>& v)
        : x(v.x), y(v.y) {}

    T& operator[](length_t i)
    {
        LVN_ASSERT(i >= 0 && i < this->length(), "vector index out of range");

        switch (i)
        {
        default:
        case 0:
            return x;
        case 1:
            return y;
        }
    }
    const T& operator[](length_t i) const
    {
        LVN_ASSERT(i >= 0 && i < this->length(), "vector index out of range");

        switch (i)
        {
        default:
        case 0:
            return x;
        case 1:
            return y;
        }
    }

    LvnVec<2, T>& operator+=(const LvnVec<2, T>& v)
    {
        this->x += v.x;
        this->y += v.y;
        return *this;
    }
    LvnVec<2, T>& operator-=(const LvnVec<2, T>& v)
    {
        this->x -= v.x;
        this->y -= v.y;
        return *this;
    }
    LvnVec<2, T>& operator*=(const LvnVec<2, T>& v)
    {
        this->x *= v.x;
        this->y *= v.y;
        return *this;
    }
    LvnVec<2, T>& operator/=(const LvnVec<2, T>& v)
    {
        this->x /= v.x;
        this->y /= v.y;
        return *this;
    }
    LvnVec<2, T>& operator++()
    {
        this->x++;
        this->y++;
        return *this;
    }
    LvnVec<2, T>& operator--()
    {
        this->x--;
        this->y--;
        return *this;
    }
    LvnVec<2, T> operator++(int)
    {
        LvnVec<2, T> vec(*this);
        ++*this;
        return vec;
    }
    LvnVec<2, T> operator--(int)
    {
        LvnVec<2, T> vec(*this);
        --*this;
        return vec;
    }
    LvnVec<2, T> operator+() const
    {
        return LvnVec<2, T>(this->x, this->y);
    }
    LvnVec<2, T> operator-() const
    {
        return LvnVec<2, T>(-this->x, -this->y);
    }
};

template <typename T>
LvnVec<2, T> operator+(const LvnVec<2, T>& v1, const LvnVec<2, T>& v2)
{
    return LvnVec<2, T>(v1.x + v2.x, v1.y + v2.y);
}
template <typename T>
LvnVec<2, T> operator-(const LvnVec<2, T>& v1, const LvnVec<2, T>& v2)
{
    return LvnVec<2, T>(v1.x - v2.x, v1.y - v2.y);
}
template <typename T>
LvnVec<2, T> operator*(const LvnVec<2, T>& v1, const LvnVec<2, T>& v2)
{
    return LvnVec<2, T>(v1.x * v2.x, v1.y * v2.y);
}
template <typename T>
LvnVec<2, T> operator/(const LvnVec<2, T>& v1, const LvnVec<2, T>& v2)
{
    return LvnVec<2, T>(v1.x / v2.x, v1.y / v2.y);
}
template <typename T>
LvnVec<2, T> operator+(const T& s, const LvnVec<2, T>& v)
{
    return LvnVec<2, T>(s + v.x, s + v.y);
}
template <typename T>
LvnVec<2, T> operator-(const T& s, const LvnVec<2, T>& v)
{
    return LvnVec<2, T>(s - v.x, s - v.y);
}
template <typename T>
LvnVec<2, T> operator*(const T& s, const LvnVec<2, T>& v)
{
    return LvnVec<2, T>(s * v.x, s * v.y);
}
template <typename T>
LvnVec<2, T> operator/(const T& s, const LvnVec<2, T>& v)
{
    return LvnVec<2, T>(s / v.x, s / v.y);
}
template <typename T>
LvnVec<2, T> operator+(const LvnVec<2, T>& v, const T& s)
{
    return LvnVec<2, T>(v.x + s, v.y + s);
}
template <typename T>
LvnVec<2, T> operator-(const LvnVec<2, T>& v, const T& s)
{
    return LvnVec<2, T>(v.x - s, v.y - s);
}
template <typename T>
LvnVec<2, T> operator*(const LvnVec<2, T>& v, const T& s)
{
    return LvnVec<2, T>(v.x * s, v.y * s);
}
template <typename T>
LvnVec<2, T> operator/(const LvnVec<2, T>& v, const T& s)
{
    return LvnVec<2, T>(v.x / s, v.y / s);
}


template<typename T>
struct LvnVec<3, T>
{
    union { T x, r, s, i; };
    union { T y, g, t, j; };
    union { T z, b, p, k; };

    static length_t length() { return 3; }

    LvnVec() = default;
    LvnVec(const LvnVec<3, T>&) = default;
    LvnVec(const T& n)
        : x(n), y(n), z(n) {}
    LvnVec(const T& nx, const T& ny, const T& nz)
        : x(nx), y(ny), z(nz) {}
    LvnVec(const LvnVec<4, T>& v)
        : x(v.x), y(v.y), z(v.z) {}
    LvnVec(const LvnVec<2, T>& v, const T& nz)
        : x(v.x), y(v.y), z(nz) {}
    LvnVec(const T& nx, const LvnVec<2, T>& v)
        : x(nx), y(v.x), z(v.y) {}

    T& operator[](length_t i)
    {
        LVN_ASSERT(i >= 0 && i < this->length(), "vector index out of range");

        switch (i)
        {
            default:
            case 0: return x;
            case 1: return y;
            case 2: return z;
        }
    }
    const T& operator[](length_t i) const
    {
        LVN_ASSERT(i >= 0 && i < this->length(), "vector index out of range");

        switch (i)
        {
            default:
            case 0: return x;
            case 1: return y;
            case 2: return z;
        }
    }

    LvnVec<3, T>& operator+=(const LvnVec<3, T>& v)
    {
        this->x += v.x;
        this->y += v.y;
        this->z += v.z;
        return *this;
    }
    LvnVec<3, T>& operator-=(const LvnVec<3, T>& v)
    {
        this->x -= v.x;
        this->y -= v.y;
        this->z -= v.z;
        return *this;
    }
    LvnVec<3, T>& operator*=(const LvnVec<3, T>& v)
    {
        this->x *= v.x;
        this->y *= v.y;
        this->z *= v.z;
        return *this;
    }
    LvnVec<3, T>& operator/=(const LvnVec<3, T>& v)
    {
        this->x /= v.x;
        this->y /= v.y;
        this->z /= v.z;
        return *this;
    }
    LvnVec<3, T>& operator++()
    {
        this->x++;
        this->y++;
        this->z++;
        return *this;
    }
    LvnVec<3, T>& operator--()
    {
        this->x--;
        this->y--;
        this->z--;
        return *this;
    }
    LvnVec<3, T> operator++(int)
    {
        LvnVec<3, T> vec(*this);
        ++*this;
        return vec;
    }
    LvnVec<3, T> operator--(int)
    {
        LvnVec<3, T> vec(*this);
        --*this;
        return vec;
    }
    LvnVec<3, T> operator+() const
    {
        return LvnVec<3, T>(this->x, this->y, this->z);
    }
    LvnVec<3, T> operator-() const
    {
        return LvnVec<3, T>(-this->x, -this->y, -this->z);
    }
};

template <typename T>
LvnVec<3, T> operator+(const LvnVec<3, T>& v1, const LvnVec<3, T>& v2)
{
    return LvnVec<3, T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}
template <typename T>
LvnVec<3, T> operator-(const LvnVec<3, T>& v1, const LvnVec<3, T>& v2)
{
    return LvnVec<3, T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}
template <typename T>
LvnVec<3, T> operator*(const LvnVec<3, T>& v1, const LvnVec<3, T>& v2)
{
    return LvnVec<3, T>(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}
template <typename T>
LvnVec<3, T> operator/(const LvnVec<3, T>& v1, const LvnVec<3, T>& v2)
{
    return LvnVec<3, T>(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}
template <typename T>
LvnVec<3, T> operator+(const T& s, const LvnVec<3, T>& v)
{
    return LvnVec<3, T>(s + v.x, s + v.y, s + v.z);
}
template <typename T>
LvnVec<3, T> operator-(const T& s, const LvnVec<3, T>& v)
{
    return LvnVec<3, T>(s - v.x, s - v.y, s - v.z);
}
template <typename T>
LvnVec<3, T> operator*(const T& s, const LvnVec<3, T>& v)
{
    return LvnVec<3, T>(s * v.x, s * v.y, s * v.z);
}
template <typename T>
LvnVec<3, T> operator/(const T& s, const LvnVec<3, T>& v)
{
    return LvnVec<3, T>(s / v.x, s / v.y, s / v.z);
}
template <typename T>
LvnVec<3, T> operator+(const LvnVec<3, T>& v, const T& s)
{
    return LvnVec<3, T>(v.x + s, v.y + s, v.z + s);
}
template <typename T>
LvnVec<3, T> operator-(const LvnVec<3, T>& v, const T& s)
{
    return LvnVec<3, T>(v.x - s, v.y - s, v.z - s);
}
template <typename T>
LvnVec<3, T> operator*(const LvnVec<3, T>& v, const T& s)
{
    return LvnVec<3, T>(v.x * s, v.y * s, v.z * s);
}
template <typename T>
LvnVec<3, T> operator/(const LvnVec<3, T>& v, const T& s)
{
    return LvnVec<3, T>(v.x / s, v.y / s, v.z / s);
}


template<typename T>
struct LvnVec<4, T>
{
    union { T x, r, s, i; };
    union { T y, g, t, j; };
    union { T z, b, p, k; };
    union { T w, a, q, l; };

    static length_t length() { return 4; }

    LvnVec() = default;
    LvnVec(const LvnVec<4, T>&) = default;
    LvnVec(const T& n)
        : x(n), y(n), z(n) {}
    LvnVec(const T& nx, const T& ny, const T& nz, const T& nw)
        : x(nx), y(ny), z(nz), w(nw) {}
    LvnVec(const LvnVec<2, T>& v1, LvnVec<2, T>& v2)
        : x(v1.x), y(v1.y), z(v2.x), w(v2.y) {}
    LvnVec(const LvnVec<2, T>& v, const T& nz, const T& nw)
        : x(v.x), y(v.y), z(nz), w(nw) {}
    LvnVec(const T& nx, const T& ny, const LvnVec<2, T>& v)
        : x(nx), y(ny), z(v.x), w(v.y) {}
    LvnVec(const T& nx, const LvnVec<2, T>& v, const T& nw)
        : x(nx), y(v.x), z(v.y), w(nw) {}
    LvnVec(const LvnVec<3, T>& v, const T& nw)
        : x(v.x), y(v.y), z(v.z), w(nw) {}
    LvnVec(const T& nx, const LvnVec<3, T>& v)
        : x(nx), y(v.x), z(v.y), w(v.z) {}

    T& operator[](length_t i)
    {
        LVN_ASSERT(i >= 0 && i < this->length(), "vector index out of range");

        switch (i)
        {
            default:
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
        }
    }
    const T& operator[](length_t i) const
    {
        LVN_ASSERT(i >= 0 && i < this->length(), "vector index out of range");

        switch (i)
        {
            default:
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
        }
    }

    LvnVec<4, T>& operator+=(const LvnVec<4, T>& v)
    {
        this->x += v.x;
        this->y += v.y;
        this->z += v.z;
        this->w += v.w;
        return *this;
    }
    LvnVec<4, T>& operator-=(const LvnVec<4, T>& v)
    {
        this->x -= v.x;
        this->y -= v.y;
        this->z -= v.z;
        this->w -= v.w;
        return *this;
    }
    LvnVec<4, T>& operator*=(const LvnVec<4, T>& v)
    {
        this->x *= v.x;
        this->y *= v.y;
        this->z *= v.z;
        this->w *= v.w;
        return *this;
    }
    LvnVec<4, T>& operator/=(const LvnVec<4, T>& v)
    {
        this->x /= v.x;
        this->y /= v.y;
        this->z /= v.z;
        this->w *= v.w;
        return *this;
    }
    LvnVec<4, T>& operator++()
    {
        this->x++;
        this->y++;
        this->z++;
        this->w++;
        return *this;
    }
    LvnVec<4, T>& operator--()
    {
        this->x--;
        this->y--;
        this->z--;
        this->w--;
        return *this;
    }
    LvnVec<4, T> operator++(int)
    {
        LvnVec<4, T> vec(*this);
        ++*this;
        return vec;
    }
    LvnVec<4, T> operator--(int)
    {
        LvnVec<4, T> vec(*this);
        --*this;
        return vec;
    }
    LvnVec<4, T> operator+() const
    {
        return LvnVec<4, T>(this->x, this->y, this->z, this->w);
    }
    LvnVec<4, T> operator-() const
    {
        return LvnVec<4, T>(-this->x, -this->y, -this->z, -this->w);
    }
};

template <typename T>
LvnVec<4, T> operator+(const LvnVec<4, T>& v1, const LvnVec<4, T>& v2)
{
    return LvnVec<4, T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
}
template <typename T>
LvnVec<4, T> operator-(const LvnVec<4, T>& v1, const LvnVec<4, T>& v2)
{
    return LvnVec<4, T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
}
template <typename T>
LvnVec<4, T> operator*(const LvnVec<4, T>& v1, const LvnVec<4, T>& v2)
{
    return LvnVec<4, T>(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
}
template <typename T>
LvnVec<4, T> operator/(const LvnVec<4, T>& v1, const LvnVec<4, T>& v2)
{
    return LvnVec<4, T>(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
}
template <typename T>
LvnVec<4, T> operator+(const T& s, const LvnVec<4, T>& v)
{
    return LvnVec<4, T>(s + v.x, s + v.y, s + v.z, s + v.w);
}
template <typename T>
LvnVec<4, T> operator-(const T& s, const LvnVec<4, T>& v)
{
    return LvnVec<4, T>(s - v.x, s - v.y, s - v.z, s - v.w);
}
template <typename T>
LvnVec<4, T> operator*(const T& s, const LvnVec<4, T>& v)
{
    return LvnVec<4, T>(s * v.x, s * v.y, s * v.z, s * v.w);
}
template <typename T>
LvnVec<4, T> operator/(const T& s, const LvnVec<4, T>& v)
{
    return LvnVec<4, T>(s / v.x, s / v.y, s / v.z, s / v.w);
}
template <typename T>
LvnVec<4, T> operator+(const LvnVec<4, T>& v, const T& s)
{
    return LvnVec<4, T>(v.x + s, v.y + s, v.z + s, v.w + s);
}
template <typename T>
LvnVec<4, T> operator-(const LvnVec<4, T>& v, const T& s)
{
    return LvnVec<4, T>(v.x - s, v.y - s, v.z - s, v.w - s);
}
template <typename T>
LvnVec<4, T> operator*(const LvnVec<4, T>& v, const T& s)
{
    return LvnVec<4, T>(v.x * s, v.y * s, v.z * s, v.w * s);
}
template <typename T>
LvnVec<4, T> operator/(const LvnVec<4, T>& v, const T& s)
{
    return LvnVec<4, T>(v.x / s, v.y / s, v.z / s, v.w / s);
}


// -- matrices
template<typename T>
struct LvnMat<2, 2, T>
{
    LvnVec<2, T> value[2];

    static length_t length() { return 2; }

    LvnMat() = default;
    LvnMat(const LvnMat<2, 2, T>&) = default;
    LvnMat(const T& n)
    {
        this->value[0] = { n, 0 };
        this->value[1] = { 0, n };
    }
    LvnMat(const T& x0, const T& y0, const T& x1, const T& y1)
    {
        this->value[0] = { x0, y0 };
        this->value[1] = { x1, y1 };
    }
    LvnMat(const LvnVec<2, T>& v0, const LvnVec<2, T>& v1)
        : value{ v0, v1 } {}
    LvnMat(const LvnMat<3, 3, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]) } {}
    LvnMat(const LvnMat<4, 4, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]) } {}
    LvnMat(const LvnMat<2, 3, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]) } {}
    LvnMat(const LvnMat<2, 4, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]) } {}
    LvnMat(const LvnMat<3, 2, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]) } {}
    LvnMat(const LvnMat<3, 4, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]) } {}
    LvnMat(const LvnMat<4, 2, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]) } {}
    LvnMat(const LvnMat<4, 3, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]) } {}

    LvnVec<2, T>& operator[](length_t i)
    {
        return this->value[i];
    }
    const LvnVec<2, T>& operator[](length_t i) const
    {
        return this->value[i];
    }

    LvnMat<2, 2, T> operator+() const
    {
        return LvnMat<2, 2, T>(
            this->value[0],
            this->value[1]);
    }
    LvnMat<2, 2, T> operator-() const
    {
        return LvnMat<2, 2, T>(
            -this->value[0],
            -this->value[1]);
    }
    LvnMat<2, 2, T> operator*=(const T& s)
    {
        this->value[0] *= s;
        this->value[1] *= s;
        return *this;
    }
    LvnMat<2, 2, T> operator/=(const T& s)
    {
        this->value[0] /= s;
        this->value[1] /= s;
        return *this;
    }
    LvnMat<2, 2, T> operator+(const LvnMat<2, 2, T>& m)
    {
        return LvnMat<2, 2, T>(
            this->value[0] + m[0],
            this->value[1] + m[1]);
    }
    LvnMat<2, 2, T> operator-(const LvnMat<2, 2, T>& m)
    {
        return LvnMat<2, 2, T>(
            this->value[0] - m[0],
            this->value[1] - m[1]);
    }
};

template<typename T>
LvnMat<2, 2, T> operator*(const LvnMat<2, 2, T>& m, const T& s)
{
    return LvnMat<2, 2, T>(
        m[0] * s,
        m[1] * s);
}
template<typename T>
LvnMat<2, 2, T> operator/(const LvnMat<2, 2, T>& m, const T& s)
{
    return LvnMat<2, 2, T>(
        m[0] / s,
        m[1] / s);
}
template<typename T>
LvnMat<2, 2, T> operator*(const T& s, const LvnMat<2, 2, T>& m)
{
    return LvnMat<2, 2, T>(
        s * m[0],
        s * m[1]);
}
template<typename T>
LvnMat<2, 2, T> operator/(const T& s, const LvnMat<2, 2, T>& m)
{
    return LvnMat<2, 2, T>(
        s / m[0],
        s / m[1]);
}
template<typename T>
LvnMat<2, 2, T> operator*(const LvnMat<2, 2, T>& m1, const LvnMat<2, 2, T>& m2)
{
    return LvnMat<2, 2, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1]);
}
template<typename T>
LvnMat<3, 2, T> operator*(const LvnMat<2, 2, T>& m1, const LvnMat<3, 2, T>& m2)
{
    return LvnMat<3, 2, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1]);
}
template<typename T>
LvnMat<4, 2, T> operator*(const LvnMat<2, 2, T>& m1, const LvnMat<4, 2, T>& m2)
{
    return LvnMat<4, 2, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1],
        m1[0][0] * m2[3][0] + m1[1][0] * m2[3][1],
        m1[0][1] * m2[3][0] + m1[1][1] * m2[3][1]);
}
template<typename T>
LvnVec<2, T> operator*(const LvnMat<2, 2, T>& m, const LvnVec<2, T>& v)
{
    return LvnVec<2, T>(
        m[0][0] * v.x + m[1][0] * v.y,
        m[0][1] * v.x + m[1][1] * v.y);
}
template<typename T>
LvnVec<2, T> operator*(const LvnVec<2, T>& v, const LvnMat<2, 2, T>& m)
{
    return LvnVec<2, T>(
        v.x * m[0][0] + v.y * m[0][1],
        v.x * m[1][0] + v.y * m[1][1]);
}


template<typename T>
struct LvnMat<3, 3, T>
{
    LvnVec<3, T> value[3];

    static length_t length() { return 3; }

    LvnMat() = default;
    LvnMat(const LvnMat<3, 3, T>&) = default;
    LvnMat(const T& n)
    {
        this->value[0] = { n, 0, 0 };
        this->value[1] = { 0, n, 0 };
        this->value[2] = { 0, 0, n };
    }
    LvnMat(const T& x0, const T& y0, const T& z0,
           const T& x1, const T& y1, const T& z1,
           const T& x2, const T& y2, const T& z2)
    {
        this->value[0] = { x0, y0, z0 };
        this->value[1] = { x1, y1, z1 };
        this->value[2] = { x2, y2, z2 };
    }

    LvnMat(const LvnVec<3, T>& v0, const LvnVec<3, T>& v1, const LvnVec<3, T>& v2)
        : value{ v0, v1, v2 } {}
    LvnMat(const LvnMat<2, 2, T>& m)
        : value{ LvnVec<3, T>(m[0], 0), LvnVec<3, T>(m[1], 0), LvnVec<3, T>(0, 0, 1) } {}
    LvnMat(const LvnMat<4, 4, T>& m)
        : value{ LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1]), LvnVec<3, T>(m[2]) } {}
    LvnMat(const LvnMat<2, 3, T>& m)
        : value{ LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1]), LvnVec<3, T>(0, 0, 1) } {}
    LvnMat(const LvnMat<2, 4, T>& m)
        : value{ LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1]), LvnVec<3, T>(0, 0, 1) } {}
    LvnMat(const LvnMat<3, 2, T>& m)
        : value{ LvnVec<3, T>(m[0], 0), LvnVec<3, T>(m[1], 0), LvnVec<3, T>(m[2], 1) } {}
    LvnMat(const LvnMat<3, 4, T>& m)
        : value{ LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1]), LvnVec<3, T>(m[2]) } {}
    LvnMat(const LvnMat<4, 2, T>& m)
        : value{ LvnVec<3, T>(m[0], 0), LvnVec<3, T>(m[1], 0), LvnVec<3, T>(m[2], 1) } {}
    LvnMat(const LvnMat<4, 3, T>& m)
        : value{ LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1]), LvnVec<3, T>(m[2]) } {}

    LvnVec<3, T>& operator[](length_t i)
    {
        return this->value[i];
    }
    const LvnVec<3, T>& operator[](length_t i) const
    {
        return this->value[i];
    }

    LvnMat<3, 3, T> operator+() const
    {
        return LvnMat<3, 3, T>(
            this->value[0],
            this->value[1],
            this->value[2]);
    }
    LvnMat<3, 3, T> operator-() const
    {
        return LvnMat<3, 3, T>(
            -this->value[0],
            -this->value[1],
            -this->value[2]);
    }
    LvnMat<3, 3, T> operator*=(const T& s)
    {
        this->value[0] *= s;
        this->value[1] *= s;
        this->value[2] *= s;
        return *this;
    }
    LvnMat<3, 3, T> operator/=(const T& s)
    {
        this->value[0] /= s;
        this->value[1] /= s;
        this->value[2] /= s;
        return *this;
    }
    LvnMat<3, 3, T> operator+(const LvnMat<3, 3, T>& m)
    {
        return LvnMat<3, 3, T>(
            this->value[0] + m[0],
            this->value[1] + m[1],
            this->value[2] + m[2]);
    }
    LvnMat<3, 3, T> operator-(const LvnMat<3, 3, T>& m)
    {
        return LvnMat<3, 3, T>(
            this->value[0] - m[0],
            this->value[1] - m[1],
            this->value[2] - m[2]);
    }
};

template<typename T>
LvnMat<3, 3, T> operator*(LvnMat<3, 3, T>& m, const T& s)
{
    return LvnMat<3, 3, T>(
        m[0] * s,
        m[1] * s,
        m[2] * s);
}
template<typename T>
LvnMat<3, 3, T> operator/(LvnMat<3, 3, T>& m, const T& s)
{
    return LvnMat<3, 3, T>(
        m[0] / s,
        m[1] / s,
        m[2] / s);
}
template<typename T>
LvnMat<3, 3, T> operator*(const T& s, LvnMat<3, 3, T>& m)
{
    return LvnMat<3, 3, T>(
        s * m[0],
        s * m[1],
        s * m[2]);
}
template<typename T>
LvnMat<3, 3, T> operator/(const T& s, LvnMat<3, 3, T>& m)
{
    return LvnMat<3, 3, T>(
        s / m[0],
        s / m[1],
        s / m[2]);
}
template<typename T>
LvnMat<3, 3, T> operator*(const LvnMat<3, 3, T>& m1, const LvnMat<3, 3, T>& m2)
{
    return LvnMat<3, 3, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2],
        m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1] + m1[2][2] * m2[2][2]);
}
template<typename T>
LvnMat<2, 3, T> operator*(const LvnMat<3, 3, T>& m1, const LvnMat<2, 3, T>& m2)
{
    return LvnMat<2, 3, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2]);
}
template<typename T>
LvnMat<4, 3, T> operator*(const LvnMat<3, 3, T>& m1, const LvnMat<4, 3, T>& m2)
{
    return LvnMat<4, 3, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2],
        m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1] + m1[2][2] * m2[2][2],
        m1[0][0] * m2[3][0] + m1[1][0] * m2[3][1] + m1[2][0] * m2[3][2],
        m1[0][1] * m2[3][0] + m1[1][1] * m2[3][1] + m1[2][1] * m2[3][2],
        m1[0][2] * m2[3][0] + m1[1][2] * m2[3][1] + m1[2][2] * m2[3][2]);
}
template<typename T>
LvnVec<3, T> operator*(const LvnMat<3, 3, T>& m, const LvnVec<3, T>& v)
{
    return LvnVec<3, T>(
        m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z,
        m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z,
        m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z);
}
template<typename T>
LvnVec<3, T> operator*(const LvnVec<3, T>& v, const LvnMat<3, 3, T>& m)
{
    return LvnVec<3, T>(
        v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2],
        v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2],
        v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2]);
}


template<typename T>
struct LvnMat<4, 4, T>
{
    LvnVec<4, T> value[4];

    static length_t length() { return 4; }

    LvnMat() = default;
    LvnMat(const LvnMat<4, 4, T>&) = default;
    LvnMat(const T& n)
    {
        this->value[0] = { n, 0, 0, 0 };
        this->value[1] = { 0, n, 0, 0 };
        this->value[2] = { 0, 0, n, 0 };
        this->value[3] = { 0, 0, 0, n };
    }
    LvnMat(const T& x0, const T& y0, const T& z0, const T& w0,
           const T& x1, const T& y1, const T& z1, const T& w1,
           const T& x2, const T& y2, const T& z2, const T& w2,
           const T& x3, const T& y3, const T& z3, const T& w3)
    {
        this->value[0] = { x0, y0, z0, w0 };
        this->value[1] = { x1, y1, z1, w1 };
        this->value[2] = { x2, y2, z2, w2 };
        this->value[3] = { x3, y3, z3, w3 };
    }

    LvnMat(const LvnVec<4, T>& v0, const LvnVec<4, T>& v1, const LvnVec<4, T>& v2, const LvnVec<4, T>& v3)
        : value{ v0, v1, v2, v3 } {}
    LvnMat(const LvnMat<2, 2, T>& m)
        : value{ LvnVec<4, T>(m[0], 0, 0), LvnVec<4, T>(m[1], 0, 0), LvnVec<4, T>(0, 0, 1, 0), LvnVec<4, T>(0, 0, 0, 1) } {}
    LvnMat(const LvnMat<3, 3, T>& m)
        : value{ LvnVec<4, T>(m[0], 0), LvnVec<4, T>(m[1], 0), LvnVec<4, T>(m[2], 0), LvnVec<4, T>(0, 0, 0, 1) } {}
    LvnMat(const LvnMat<2, 3, T>& m)
        : value{ LvnVec<4, T>(m[0], 0), LvnVec<4, T>(m[1], 0), LvnVec<4, T>(0, 0, 1, 0), LvnVec<4, T>(0, 0, 0, 1) } {}
    LvnMat(const LvnMat<2, 4, T>& m)
        : value{ LvnVec<4, T>(m[0]), LvnVec<4, T>(m[1]), LvnVec<4, T>(0, 0, 1, 0 ), LvnVec<4, T>(0, 0, 0, 1) } {}
    LvnMat(const LvnMat<3, 2, T>& m)
        : value{ LvnVec<4, T>(m[0], 0, 0), LvnVec<4, T>(m[1], 0, 0), LvnVec<4, T>(m[2], 1, 0), LvnVec<4, T>(0, 0, 0, 1) } {}
    LvnMat(const LvnMat<3, 4, T>& m)
        : value{ LvnVec<4, T>(m[0]), LvnVec<4, T>(m[1]), LvnVec<4, T>(m[2]), LvnVec<4, T>(0, 0, 0, 1) } {}
    LvnMat(const LvnMat<4, 2, T>& m)
        : value{ LvnVec<4, T>(m[0], 0, 0), LvnVec<4, T>(m[1], 0, 0), LvnVec<4, T>(m[2], 1, 0), LvnVec<4, T>(m[3], 0, 1) } {}
    LvnMat(const LvnMat<4, 3, T>& m)
        : value{ LvnVec<4, T>(m[0], 0), LvnVec<4, T>(m[1], 0), LvnVec<4, T>(m[2], 0), LvnVec<4, T>(m[3], 1) } {}

    LvnVec<4, T>& operator[](length_t i)
    {
        return this->value[i];
    }
    const LvnVec<4, T>& operator[](length_t i) const
    {
        return this->value[i];
    }

    LvnMat<4, 4, T> operator+() const
    {
        return LvnMat<4, 4, T>(
            this->value[0],
            this->value[1],
            this->value[2],
            this->value[3]);
    }
    LvnMat<4, 4, T> operator-() const
    {
        return LvnMat<4, 4, T>(
            -this->value[0],
            -this->value[1],
            -this->value[2],
            -this->value[3]);
    }
    LvnMat<4, 4, T> operator+(const LvnMat<4, 4, T>& m)
    {
        return LvnMat<4, 4, T>(
            this->value[0] + m[0],
            this->value[1] + m[1],
            this->value[4] + m[2],
            this->value[3] + m[3]);
    }
    LvnMat<4, 4, T> operator-(const LvnMat<4, 4, T>& m)
    {
        return LvnMat<4, 4, T>(
            this->value[0] - m[0],
            this->value[1] - m[1],
            this->value[2] - m[2],
            this->value[3] - m[3]);
    }
    LvnMat<4, 4, T> operator*=(const T& s)
    {
        this->value[0] *= s;
        this->value[1] *= s;
        this->value[2] *= s;
        this->value[3] *= s;
        return *this;
    }
    LvnMat<4, 4, T> operator/=(const T& s)
    {
        this->value[0] /= s;
        this->value[1] /= s;
        this->value[2] /= s;
        this->value[3] /= s;
        return *this;
    }
};

template<typename T>
LvnMat<4, 4, T> operator*(const LvnMat<4, 4, T>& m, const T& s)
{
    return LvnMat<4, 4, T>(
        m[0] * s,
        m[1] * s,
        m[2] * s,
        m[3] * s);
}
template<typename T>
LvnMat<4, 4, T> operator/(const LvnMat<4, 4, T>& m, const T& s)
{
    return LvnMat<4, 4, T>(
        m[0] / s,
        m[1] / s,
        m[2] / s,
        m[3] / s);
}
template<typename T>
LvnMat<4, 4, T> operator*(const T& s, const LvnMat<4, 4, T>& m)
{
    return LvnMat<4, 4, T>(
        s * m[0],
        s * m[1],
        s * m[2],
        s * m[3]);
}
template<typename T>
LvnMat<4, 4, T> operator/(const T& s, const LvnMat<4, 4, T>& m)
{
    return LvnMat<4, 4, T>(
        s / m[0],
        s / m[1],
        s / m[2],
        s / m[3]);
}
template<typename T>
LvnMat<4, 4, T> operator*(const LvnMat<4, 4, T>& m1, const LvnMat<4, 4, T>& m2)
{
    return LvnMat<4, 4, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2] + m1[3][0] * m2[0][3],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2] + m1[3][1] * m2[0][3],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2] + m1[3][2] * m2[0][3],
        m1[0][3] * m2[0][0] + m1[1][3] * m2[0][1] + m1[2][3] * m2[0][2] + m1[3][3] * m2[0][3],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2] + m1[3][0] * m2[1][3],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2] + m1[3][1] * m2[1][3],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2] + m1[3][2] * m2[1][3],
        m1[0][3] * m2[1][0] + m1[1][3] * m2[1][1] + m1[2][3] * m2[1][2] + m1[3][3] * m2[1][3],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2] + m1[3][0] * m2[2][3],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2] + m1[3][1] * m2[2][3],
        m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1] + m1[2][2] * m2[2][2] + m1[3][2] * m2[2][3],
        m1[0][3] * m2[2][0] + m1[1][3] * m2[2][1] + m1[2][3] * m2[2][2] + m1[3][3] * m2[2][3],
        m1[0][0] * m2[3][0] + m1[1][0] * m2[3][1] + m1[2][0] * m2[3][2] + m1[3][0] * m2[3][3],
        m1[0][1] * m2[3][0] + m1[1][1] * m2[3][1] + m1[2][1] * m2[3][2] + m1[3][1] * m2[3][3],
        m1[0][2] * m2[3][0] + m1[1][2] * m2[3][1] + m1[2][2] * m2[3][2] + m1[3][2] * m2[3][3],
        m1[0][3] * m2[3][0] + m1[1][3] * m2[3][1] + m1[2][3] * m2[3][2] + m1[3][3] * m2[3][3]);
}
template<typename T>
LvnMat<2, 4, T> operator*(const LvnMat<4, 4, T>& m1, const LvnMat<2, 4, T>& m2)
{
    return LvnMat<2, 4, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2] + m1[3][0] * m2[0][3],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2] + m1[3][1] * m2[0][3],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2] + m1[3][2] * m2[0][3],
        m1[0][3] * m2[0][0] + m1[1][3] * m2[0][1] + m1[2][3] * m2[0][2] + m1[3][3] * m2[0][3],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2] + m1[3][0] * m2[1][3],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2] + m1[3][1] * m2[1][3],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2] + m1[3][2] * m2[1][3],
        m1[0][3] * m2[1][0] + m1[1][3] * m2[1][1] + m1[2][3] * m2[1][2] + m1[3][3] * m2[1][3]);
}
template<typename T>
LvnMat<3, 4, T> operator*(const LvnMat<4, 4, T>& m1, const LvnMat<3, 4, T>& m2)
{
    return LvnMat<3, 4, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2] + m1[3][0] * m2[0][3],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2] + m1[3][1] * m2[0][3],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2] + m1[3][2] * m2[0][3],
        m1[0][3] * m2[0][0] + m1[1][3] * m2[0][1] + m1[2][3] * m2[0][2] + m1[3][3] * m2[0][3],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2] + m1[3][0] * m2[1][3],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2] + m1[3][1] * m2[1][3],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2] + m1[3][2] * m2[1][3],
        m1[0][3] * m2[1][0] + m1[1][3] * m2[1][1] + m1[2][3] * m2[1][2] + m1[3][3] * m2[1][3],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2] + m1[3][0] * m2[2][3],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2] + m1[3][1] * m2[2][3],
        m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1] + m1[2][2] * m2[2][2] + m1[3][2] * m2[2][3],
        m1[0][3] * m2[2][0] + m1[1][3] * m2[2][1] + m1[2][3] * m2[2][2] + m1[3][3] * m2[2][3]);
}
template<typename T>
LvnVec<4, T> operator*(const LvnMat<4, 4, T>& m, const LvnVec<4, T>& v)
{
    return LvnVec<4, T>(
        m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w,
        m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w,
        m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * v.w,
        m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3] * v.w);
}
template<typename T>
LvnVec<4, T> operator*(const LvnVec<4, T>& v, const LvnMat<4, 4, T>& m)
{
    return LvnVec<4, T>(
        v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2] + v.w * m[0][3],
        v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2] + v.w * m[1][3],
        v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2] + v.w * m[2][3],
        v.x * m[3][0] + v.y * m[3][1] + v.z * m[3][2] + v.w * m[3][3]);
}


template<typename T>
struct LvnMat<2, 3, T>
{
    LvnVec<3, T> value[2];

    static length_t length() { return 2; }

    LvnMat() = default;
    LvnMat(const LvnMat<2, 3, T>&) = default;
    LvnMat(const T& n)
    {
        this->value[0] = { n, 0, 0 };
        this->value[1] = { 0, n, 0 };
    }
    LvnMat(const T& x0, const T& y0, const T& z0,
           const T& x1, const T& y1, const T& z1)
    {
        this->value[0] = { x0, y0, z0 };
        this->value[1] = { x1, y1, z1 };
    }
    LvnMat(const LvnVec<3, T>& v0, const LvnVec<3, T>& v1)
        : value{  v0, v1  } {}
    LvnMat(const LvnMat<2, 2, T>& m)
        : value{  LvnVec<3, T>(m[0], 0), LvnVec<3, T>(m[1], 0)  } {}
    LvnMat(const LvnMat<3, 3, T>& m)
        : value{  LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1])  } {}
    LvnMat(const LvnMat<4, 4, T>& m)
        : value{  LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1])  } {}
    LvnMat(const LvnMat<2, 4, T>& m)
        : value{  LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1])  } {}
    LvnMat(const LvnMat<3, 2, T>& m)
        : value{  LvnVec<3, T>(m[0], 0), LvnVec<3, T>(m[1], 0)  } {}
    LvnMat(const LvnMat<3, 4, T>& m)
        : value{  LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1])  } {}
    LvnMat(const LvnMat<4, 2, T>& m)
        : value{  LvnVec<3, T>(m[0], 0), LvnVec<3, T>(m[1], 0)  } {}
    LvnMat(const LvnMat<4, 3, T>& m)
        : value{  LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1])  } {}

    LvnVec<3, T>& operator[](length_t i)
    {
        return this->value[i];
    }
    const LvnVec<3, T>& operator[](int i) const
    {
        return this->value[i];
    }

    LvnMat<2, 3, T> operator+() const
    {
        return LvnMat<2, 3, T>(
            this->value[0],
            this->value[1]);
    }
    LvnMat<2, 3, T> operator-() const
    {
        return LvnMat<2, 3, T>(
            -this->value[0],
            -this->value[1]);
    }
    LvnMat<2, 3, T> operator+(const LvnMat<2, 3, T>& m)
    {
        return LvnMat<2, 3, T>(
            this->value[0] + m[0],
            this->value[1] + m[1]);
    }
    LvnMat<2, 3, T> operator-(const LvnMat<2, 3, T>& m)
    {
        return LvnMat<2, 3, T>(
            this->value[0] - m[0],
            this->value[1] - m[1]);
    }
};

template<typename T>
LvnMat<2, 3, T> operator*(const LvnMat<2, 3, T>& m, const T& s)
{
    return LvnMat<2, 3, T>(
        m[0] * s,
        m[1] * s);
}
template<typename T>
LvnMat<2, 3, T> operator/(const LvnMat<2, 3, T>& m, const T& s)
{
    return LvnMat<2, 3, T>(
        m[0] / s,
        m[1] / s);
}
template<typename T>
LvnMat<2, 3, T> operator*(const T& s, const LvnMat<2, 3, T>& m)
{
    return LvnMat<2, 3, T>(
        s * m[0],
        s * m[1]);
}
template<typename T>
LvnMat<2, 3, T> operator/(const T& s, const LvnMat<2, 3, T>& m)
{
    return LvnMat<2, 3, T>(
        s / m[0],
        s / m[1]);
}
template<typename T>
LvnMat<2, 3, T> operator*(const LvnMat<2, 3, T>& m1, const LvnMat<2, 2, T>& m2)
{
    return LvnMat<2, 3, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1]);
}
template<typename T>
LvnMat<3, 3, T> operator*(const LvnMat<2, 3, T>& m1, const LvnMat<3, 2, T>& m2)
{
    return LvnMat<3, 3, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1],
        m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1]);
}
template<typename T>
LvnMat<4, 3, T> operator*(const LvnMat<2, 3, T>& m1, const LvnMat<4, 2, T>& m2)
{
    return LvnMat<4, 3, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1],
        m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1],
        m1[0][0] * m2[3][0] + m1[1][0] * m2[3][1],
        m1[0][1] * m2[3][0] + m1[1][1] * m2[3][1],
        m1[0][2] * m2[3][0] + m1[1][2] * m2[3][1]);
}
template<typename T>
LvnVec<3, T> operator*(const LvnMat<2, 3, T>& m, const LvnVec<2, T>& v)
{
    return LvnVec<3, T>(
        m[0][0] * v.x + m[1][0] * v.y,
        m[0][1] * v.x + m[1][1] * v.y,
        m[0][2] * v.x + m[1][2] * v.y);
}
template<typename T>
LvnVec<2, T> operator*(const LvnVec<3, T>& v, const LvnMat<2, 3, T>& m)
{
    return LvnVec<2, T>(
        v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2],
        v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2]);
}

template<typename T>
struct LvnMat<2, 4, T>
{
    LvnVec<4, T> value[2];

    static length_t length() { return 2; }

    LvnMat() = default;
    LvnMat(const LvnMat<2, 4, T>&) = default;
    LvnMat(const T& n)
    {
        this->value[0] = { n, 0, 0, 0 };
        this->value[1] = { 0, n, 0, 0 };
    }
    LvnMat
    (
        const T& x0, const T& y0, const T& z0, const T& w0,
        const T& x1, const T& y1, const T& z1, const T& w1
    )
    {
        this->value[0] = { x0, y0, z0, w0 };
        this->value[1] = { x1, y1, z1, w1 };
    }
    LvnMat(const LvnVec<4, T>& v0, const LvnVec<4, T>& v1)
        : value{ v0, v1 } {}
    LvnMat(const LvnMat<2, 2, T>& m)
        : value{ LvnVec<4, T>(m[0], 0, 0), LvnVec<4, T>(m[1], 0, 0) } {}
    LvnMat(const LvnMat<3, 3, T>& m)
        : value{ LvnVec<4, T>(m[0], 0), LvnVec<4, T>(m[1], 0) } {}
    LvnMat(const LvnMat<4, 4, T>& m)
        : value{ LvnVec<4, T>(m[0]), LvnVec<4, T>(m[1]) } {}
    LvnMat(const LvnMat<2, 3, T>& m)
        : value{ LvnVec<4, T>(m[0], 0), LvnVec<4, T>(m[1], 0) } {}
    LvnMat(const LvnMat<3, 2, T>& m)
        : value{ LvnVec<4, T>(m[0], 0, 0), LvnVec<4, T>(m[1], 0, 0) } {}
    LvnMat(const LvnMat<3, 4, T>& m)
        : value{ LvnVec<4, T>(m[0]), LvnVec<4, T>(m[1]) } {}
    LvnMat(const LvnMat<4, 2, T>& m)
        : value{ LvnVec<4, T>(m[0], 0, 0), LvnVec<4, T>(m[1], 0, 0) } {}
    LvnMat(const LvnMat<4, 3, T>& m)
        : value{ LvnVec<4, T>(m[0], 0), LvnVec<4, T>(m[1], 0) } {}

    LvnVec<4, T>& operator[](length_t i)
    {
        return this->value[i];
    }
    const LvnVec<4, T>& operator[](length_t i) const
    {
        return this->value[i];
    }

    LvnMat<2, 4, T> operator+() const
    {
        return LvnMat<2, 4, T>(
            this->value[0],
            this->value[1]);
    }
    LvnMat<2, 4, T> operator-() const
    {
        return LvnMat<2, 4, T>(
            -this->value[0],
            -this->value[1]);
    }
    LvnMat<2, 4, T> operator+(const LvnMat<2, 4, T>& m)
    {
        return LvnMat<2, 4, T>(
            this->value[0] + m[0],
            this->value[1] + m[1]);
    }
    LvnMat<2, 4, T> operator-(const LvnMat<2, 4, T>& m)
    {
        return LvnMat<2, 4, T>(
            this->value[0] - m[0],
            this->value[1] - m[1]);
    }
};

template<typename T>
LvnMat<2, 4, T> operator*(const LvnMat<2, 4, T>& m, const T& s)
{
    return LvnMat<2, 4, T>(
        m[0] * s,
        m[1] * s);
}
template<typename T>
LvnMat<2, 4, T> operator/(const LvnMat<2, 4, T>& m, const T& s)
{
    return LvnMat<2, 4, T>(
        m[0] / s,
        m[1] / s);
}
template<typename T>
LvnMat<2, 4, T> operator*(const T& s, const LvnMat<2, 4, T>& m)
{
    return LvnMat<2, 4, T>(
        s * m[0],
        s * m[1]);
}
template<typename T>
LvnMat<2, 4, T> operator/(const T& s, const LvnMat<2, 4, T>& m)
{
    return LvnMat<2, 4, T>(
        s / m[0],
        s / m[1]);
}
template<typename T>
LvnMat<4, 4, T> operator*(const LvnMat<2, 4, T>& m1, const LvnMat<4, 2, T>& m2)
{
    return LvnMat<4, 4, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1],
        m1[0][3] * m2[0][0] + m1[1][3] * m2[0][1],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1],
        m1[0][3] * m2[1][0] + m1[1][3] * m2[1][1],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1],
        m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1],
        m1[0][3] * m2[2][0] + m1[1][3] * m2[2][1],
        m1[0][0] * m2[3][0] + m1[1][0] * m2[3][1],
        m1[0][1] * m2[3][0] + m1[1][1] * m2[3][1],
        m1[0][2] * m2[3][0] + m1[1][2] * m2[3][1],
        m1[0][3] * m2[3][0] + m1[1][3] * m2[3][1]);
}
template<typename T>
LvnMat<2, 4, T> operator*(const LvnMat<2, 4, T>& m1, const LvnMat<2, 2, T>& m2)
{
    return LvnMat<2, 4, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1],
        m1[0][3] * m2[0][0] + m1[1][3] * m2[0][1],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1],
        m1[0][3] * m2[1][0] + m1[1][3] * m2[1][1]);
}
template<typename T>
LvnMat<3, 4, T> operator*(const LvnMat<2, 4, T>& m1, const LvnMat<3, 2, T>& m2)
{
    return LvnMat<3, 4, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1],
        m1[0][3] * m2[0][0] + m1[1][3] * m2[0][1],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1],
        m1[0][3] * m2[1][0] + m1[1][3] * m2[1][1],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1],
        m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1],
        m1[0][3] * m2[2][0] + m1[1][3] * m2[2][1]);
}
template<typename T>
LvnVec<4, T> operator*(const LvnMat<2, 4, T>& m, const LvnVec<2, T>& v)
{
    return LvnVec<4, T>(
        m[0][0] * v.x + m[1][0] * v.y,
        m[0][1] * v.x + m[1][1] * v.y,
        m[0][2] * v.x + m[1][2] * v.y,
        m[0][3] * v.x + m[1][3] * v.y);
}
template<typename T>
LvnVec<2, T> operator*(const LvnVec<4, T>& v, const LvnMat<2, 4, T>& m)
{
    return LvnVec<2, T>(
        v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2] + v.w * m[0][3],
        v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2] + v.w * m[1][3]);
}


template<typename T>
struct LvnMat<3, 2, T>
{
    LvnVec<2, T> value[3];

    static length_t length() { return 3; }

    LvnMat() = default;
    LvnMat(const LvnMat<3, 2, T>&) = default;
    LvnMat(const T& n)
    {
        this->value[0] = { n, 0 };
        this->value[1] = { 0, n };
        this->value[2] = { 0, 0 };
    }
    LvnMat
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
    LvnMat(const LvnVec<2, T>& v0, const LvnVec<2, T>& v1, const LvnVec<2, T>& v2)
        : value{ v0, v1, v2 } {}
    LvnMat(const LvnMat<2, 2, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(0, 0) } {}
    LvnMat(const LvnMat<3, 3, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(m[2]) } {}
    LvnMat(const LvnMat<4, 4, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(m[2]) } {}
    LvnMat(const LvnMat<2, 3, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(0, 0) } {}
    LvnMat(const LvnMat<2, 4, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(0, 0) } {}
    LvnMat(const LvnMat<3, 4, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(m[2]) } {}
    LvnMat(const LvnMat<4, 2, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(m[2]) } {}
    LvnMat(const LvnMat<4, 3, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(m[2]) } {}

    LvnVec<2, T>& operator[](length_t i)
    {
        return this->value[i];
    }
    const LvnVec<2, T>& operator[](length_t i) const
    {
        return this->value[i];
    }

    LvnMat<3, 2, T> operator+() const
    {
        return LvnMat<3, 2, T>(
            this->value[0],
            this->value[1],
            this->value[2]);
    }
    LvnMat<3, 2, T> operator-() const
    {
        return LvnMat<3, 2, T>(
            -this->value[0],
            -this->value[1],
            -this->value[2]);
    }
    LvnMat<3, 2, T> operator+(const LvnMat<3, 2, T>& m)
    {
        return LvnMat<3, 2, T>(
            this->value[0] + m[0],
            this->value[1] + m[1],
            this->value[2] + m[2]);
    }
    LvnMat<3, 2, T> operator-(const LvnMat<3, 2, T>& m)
    {
        return LvnMat<3, 2, T>(
            this->value[0] - m[0],
            this->value[1] - m[1],
            this->value[2] - m[2]);
    }
};

template<typename T>
LvnMat<3, 2, T> operator*(const LvnMat<3, 2, T>& m, const T& s)
{
    return LvnMat<3, 2, T>(
        m[0] * s,
        m[1] * s,
        m[2] * s);
}
template<typename T>
LvnMat<3, 2, T> operator/(const LvnMat<3, 2, T>& m, const T& s)
{
    return LvnMat<3, 2, T>(
        m[0] / s,
        m[1] / s,
        m[2] / s);
}
template<typename T>
LvnMat<3, 2, T> operator*(const T& s, const LvnMat<3, 2, T>& m)
{
    return LvnMat<3, 2, T>(
        s * m[0],
        s * m[1],
        s * m[2]);
}
template<typename T>
LvnMat<3, 2, T> operator/(const T& s, const LvnMat<3, 2, T>& m)
{
    return LvnMat<3, 2, T>(
        s / m[0],
        s / m[1],
        s / m[2]);
}
template<typename T>
LvnMat<3, 2, T> operator*(const LvnMat<3, 2, T>& m1, const LvnMat<3, 3, T>& m2)
{
    return LvnMat<3, 2, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2]);
}
template<typename T>
LvnMat<4, 2, T> operator*(const LvnMat<3, 2, T>& m1, const LvnMat<4, 3, T>& m2)
{
    return LvnMat<4, 2, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2],
        m1[0][0] * m2[3][0] + m1[1][0] * m2[3][1] + m1[2][0] * m2[3][2],
        m1[0][1] * m2[3][0] + m1[1][1] * m2[3][1] + m1[2][1] * m2[3][2]);
}
template<typename T>
LvnMat<2, 2, T> operator*(const LvnMat<3, 2, T>& m1, const LvnMat<2, 3, T>& m2)
{
    return LvnMat<2, 2, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2]);
}
template<typename T>
LvnVec<2, T> operator*(const LvnMat<3, 2, T>& m, const LvnVec<3, T>& v)
{
    return LvnVec<2, T>(
        m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z,
        m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z);
}
template<typename T>
LvnVec<3, T> operator*(const LvnVec<2, T>& v, const LvnMat<3, 2, T>& m)
{
    return LvnVec<3, T>(
        v.x * m[0][0] + v.y * m[0][1],
        v.x * m[1][0] + v.y * m[1][1],
        v.x * m[2][0] + v.y * m[2][1]);
}


template<typename T>
struct LvnMat<3, 4, T>
{
    LvnVec<4, T> value[3];

    static length_t length() { return 3; }

    LvnMat() = default;
    LvnMat(const LvnMat<3, 4, T>&) = default;
    LvnMat(const T& n)
    {
        this->value[0] = { n, 0, 0, 0 };
        this->value[1] = { 0, n, 0, 0 };
        this->value[2] = { 0, 0, n, 0 };
    }
    LvnMat
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
    LvnMat(const LvnVec<4, T>& v0, const LvnVec<4, T>& v1, const LvnVec<4, T>& v2)
        : value{ v0, v1, v2 } {}
    LvnMat(const LvnMat<2, 2, T>& m)
        : value{ LvnVec<4, T>(m[0], 0, 0), LvnVec<4, T>(m[1], 0, 0), LvnVec<4, T>(0, 0, 1, 0) } {}
    LvnMat(const LvnMat<3, 3, T>& m)
        : value{ LvnVec<4, T>(m[0], 0), LvnVec<4, T>(m[1], 0), LvnVec<4, T>(m[2], 0) } {}
    LvnMat(const LvnMat<4, 4, T>& m)
        : value{ LvnVec<4, T>(m[0]), LvnVec<4, T>(m[1]), LvnVec<4, T>(m[2]) } {}
    LvnMat(const LvnMat<2, 3, T>& m)
        : value{ LvnVec<4, T>(m[0], 0), LvnVec<4, T>(m[1], 0), LvnVec<4, T>(0, 0, 1, 0) } {}
    LvnMat(const LvnMat<2, 4, T>& m)
        : value{ LvnVec<4, T>(m[0]), LvnVec<4, T>(m[1]), LvnVec<4, T>(0, 0, 1, 0) } {}
    LvnMat(const LvnMat<3, 2, T>& m)
        : value{ LvnVec<4, T>(m[0], 0, 0), LvnVec<4, T>(m[1], 0, 0), LvnVec<4, T>(m[2], 1, 0) } {}
    LvnMat(const LvnMat<4, 2, T>& m)
        : value{ LvnVec<4, T>(m[0], 0, 0), LvnVec<4, T>(m[1], 0, 0), LvnVec<4, T>(m[2], 1, 0) } {}
    LvnMat(const LvnMat<4, 3, T>& m)
        : value{ LvnVec<4, T>(m[0], 0), LvnVec<4, T>(m[1], 0), LvnVec<4, T>(m[2], 0) } {}

    LvnVec<4, T>& operator[](length_t i)
    {
        return this->value[i];
    }
    const LvnVec<4, T>& operator[](length_t i) const
    {
        return this->value[i];
    }

    LvnMat<3, 4, T> operator+() const
    {
        return LvnMat<3, 4, T>(
            this->value[0],
            this->value[1],
            this->value[2]);
    }
    LvnMat<3, 4, T> operator-() const
    {
        return LvnMat<3, 4, T>(
            -this->value[0],
            -this->value[1],
            -this->value[2]);
    }
    LvnMat<3, 4, T> operator+(const LvnMat<3, 4, T>& m)
    {
        return LvnMat<3, 4, T>(
            this->value[0] + m[0],
            this->value[1] + m[1],
            this->value[2] + m[2]);
    }
    LvnMat<3, 4, T> operator-(const LvnMat<3, 4, T>& m)
    {
        return LvnMat<3, 4, T>(
            this->value[0] - m[0],
            this->value[1] - m[1],
            this->value[2] - m[2]);
    }
};

template<typename T>
LvnMat<3, 4, T> operator*(const LvnMat<3, 4, T>& m, const T& s)
{
    return LvnMat<3, 4, T>(
        m[0] * s,
        m[1] * s,
        m[2] * s);
}
template<typename T>
LvnMat<3, 4, T> operator/(const LvnMat<3, 4, T>& m, const T& s)
{
    return LvnMat<3, 4, T>(
        m[0] / s,
        m[1] / s,
        m[2] / s);
}
template<typename T>
LvnMat<3, 4, T> operator*(const T& s, const LvnMat<3, 4, T>& m)
{
    return LvnMat<3, 4, T>(
        s * m[0],
        s * m[1],
        s * m[2]);
}
template<typename T>
LvnMat<3, 4, T> operator/(const T& s, const LvnMat<3, 4, T>& m)
{
    return LvnMat<3, 4, T>(
        s / m[0],
        s / m[1],
        s / m[2]);
}
template<typename T>
LvnMat<4, 4, T> operator*(const LvnMat<3, 4, T>& m1, const LvnMat<4, 3, T>& m2)
{
    return LvnMat<4, 4, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2],
        m1[0][3] * m2[0][0] + m1[1][3] * m2[0][1] + m1[2][3] * m2[0][2],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2],
        m1[0][3] * m2[1][0] + m1[1][3] * m2[1][1] + m1[2][3] * m2[1][2],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2],
        m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1] + m1[2][2] * m2[2][2],
        m1[0][3] * m2[2][0] + m1[1][3] * m2[2][1] + m1[2][3] * m2[2][2],
        m1[0][0] * m2[3][0] + m1[1][0] * m2[3][1] + m1[2][0] * m2[3][2],
        m1[0][1] * m2[3][0] + m1[1][1] * m2[3][1] + m1[2][1] * m2[3][2],
        m1[0][2] * m2[3][0] + m1[1][2] * m2[3][1] + m1[2][2] * m2[3][2],
        m1[0][3] * m2[3][0] + m1[1][3] * m2[3][1] + m1[2][3] * m2[3][2]);
}
template<typename T>
LvnMat<2, 4, T> operator*(const LvnMat<3, 4, T>& m1, const LvnMat<2, 3, T>& m2)
{
    return LvnMat<2, 4, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][0] + m1[2][0] * m2[0][0],
        m1[0][1] * m2[0][1] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][1],
        m1[0][2] * m2[0][2] + m1[1][2] * m2[0][2] + m1[2][2] * m2[0][2],
        m1[0][3] * m2[0][3] + m1[1][3] * m2[0][3] + m1[2][3] * m2[0][3],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][0] + m1[2][0] * m2[1][0],
        m1[0][1] * m2[1][1] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][1],
        m1[0][2] * m2[1][2] + m1[1][2] * m2[1][2] + m1[2][2] * m2[1][2],
        m1[0][3] * m2[1][3] + m1[1][3] * m2[1][3] + m1[2][3] * m2[1][3]);
}
template<typename T>
LvnMat<3, 4, T> operator*(const LvnMat<3, 4, T>& m1, const LvnMat<3, 3, T>& m2)
{
    return LvnMat<3, 4, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2],
        m1[0][3] * m2[0][0] + m1[1][3] * m2[0][1] + m1[2][3] * m2[0][2],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2],
        m1[0][3] * m2[1][0] + m1[1][3] * m2[1][1] + m1[2][3] * m2[1][2],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2],
        m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1] + m1[2][2] * m2[2][2],
        m1[0][3] * m2[2][0] + m1[1][3] * m2[2][1] + m1[2][3] * m2[2][2]);
}
template<typename T>
LvnVec<4, T> operator*(const LvnMat<3, 4, T>& m, const LvnVec<3, T>& v)
{
    return LvnVec<3, T>(
        m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z,
        m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z,
        m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z,
        m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z);
}
template<typename T>
LvnVec<3, T> operator*(const LvnVec<4, T>& v, const LvnMat<3, 4, T>& m)
{
    return LvnVec<3, T>(
        v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2] + v.w * m[0][3],
        v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2] + v.w * m[1][3],
        v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2] + v.w * m[2][3]);
}


template<typename T>
struct LvnMat<4, 2, T>
{
    LvnVec<2, T> value[4];

    static length_t length() { return 4; }

    LvnMat() = default;
    LvnMat(const LvnMat<4, 2, T>&) = default;
    LvnMat(const T& n)
    {
        this->value[0] = { n, 0 };
        this->value[1] = { 0, n };
        this->value[2] = { 0, 0 };
        this->value[3] = { 0, 0 };
    }
    LvnMat
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
    LvnMat(const LvnVec<2, T>& v0, const LvnVec<2, T>& v1, const LvnVec<2, T>& v2, const LvnVec<2, T>& v3)
        : value{ v0, v1, v2, v3 } {}
    LvnMat(const LvnMat<2, 2, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(0, 0), LvnVec<2, T>(0, 0) } {}
    LvnMat(const LvnMat<3, 3, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(m[2]), LvnVec<2, T>(0, 0) } {}
    LvnMat(const LvnMat<4, 4, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(m[2]), LvnVec<2, T>(m[3]) } {}
    LvnMat(const LvnMat<2, 3, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(0, 0), LvnVec<2, T>(0, 0) } {}
    LvnMat(const LvnMat<2, 4, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(0, 0), LvnVec<2, T>(0, 0) } {}
    LvnMat(const LvnMat<3, 2, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(m[2]), LvnVec<2, T>(0, 0) } {}
    LvnMat(const LvnMat<3, 4, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(m[2]), LvnVec<2, T>(0, 0) } {}
    LvnMat(const LvnMat<4, 3, T>& m)
        : value{ LvnVec<2, T>(m[0]), LvnVec<2, T>(m[1]), LvnVec<2, T>(m[2]), LvnVec<2, T>(m[3]) } {}

    LvnVec<2, T>& operator[](length_t i)
    {
        return this->value[i];
    }
    const LvnVec<2, T>& operator[](length_t i) const
    {
        return this->value[i];
    }

    LvnMat<4, 2, T> operator+() const
    {
        return LvnMat<4, 2, T>(
            this->value[0],
            this->value[1],
            this->value[2],
            this->value[3]);
    }
    LvnMat<4, 2, T> operator-() const
    {
        return LvnMat<4, 2, T>(
            -this->value[0],
            -this->value[1],
            -this->value[2],
            -this->value[3]);
    }
    LvnMat<4, 2, T> operator+(const LvnMat<4, 2, T>& m)
    {
        return LvnMat<4, 2, T>(
            this->value[0] + m[0],
            this->value[1] + m[1],
            this->value[2] + m[2],
            this->value[3] + m[3]);
    }
    LvnMat<4, 2, T> operator-(const LvnMat<4, 2, T>& m)
    {
        return LvnMat<4, 2, T>(
            this->value[0] - m[0],
            this->value[1] - m[1],
            this->value[2] - m[2],
            this->value[3] - m[3]);
    }
};

template<typename T>
LvnMat<4, 2, T> operator*(const LvnMat<4, 2, T>& m, const T& s)
{
    return LvnMat<4, 2, T>(
        m[0] * s,
        m[1] * s,
        m[2] * s,
        m[3] * s);
}
template<typename T>
LvnMat<4, 2, T> operator/(const LvnMat<4, 2, T>& m, const T& s)
{
    return LvnMat<4, 2, T>(
        m[0] / s,
        m[1] / s,
        m[2] / s,
        m[3] / s);
}
template<typename T>
LvnMat<4, 2, T> operator*(const T& s, const LvnMat<4, 2, T>& m)
{
    return LvnMat<4, 2, T>(
        s * m[0],
        s * m[1],
        s * m[2],
        s * m[3]);
}
template<typename T>
LvnMat<4, 2, T> operator/(const T& s, const LvnMat<4, 2, T>& m)
{
    return LvnMat<4, 2, T>(
        s / m[0],
        s / m[1],
        s / m[2],
        s / m[3]);
}
template<typename T>
LvnMat<2, 2, T> operator*(const LvnMat<4, 2, T>& m1, const LvnMat<2, 4, T>& m2)
{
    return LvnMat<2, 2, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2] + m1[3][0] * m2[0][3],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2] + m1[3][1] * m2[0][3],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2] + m1[3][0] * m2[1][3],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2] + m1[3][1] * m2[1][3]);
}
template<typename T>
LvnMat<3, 2, T> operator*(const LvnMat<4, 2, T>& m1, const LvnMat<3, 4, T>& m2)
{
    return LvnMat<3, 2, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2] + m1[3][0] * m2[0][3],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2] + m1[3][1] * m2[0][3],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2] + m1[3][0] * m2[1][3],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2] + m1[3][1] * m2[1][3],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2] + m1[3][0] * m2[2][3],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2] + m1[3][1] * m2[2][3]);
}
template<typename T>
LvnMat<4, 2, T> operator*(const LvnMat<4, 2, T>& m1, const LvnMat<2, 2, T>& m2)
{
    return LvnMat<4, 2, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2] + m1[3][0] * m2[0][3],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2] + m1[3][1] * m2[0][3],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2] + m1[3][0] * m2[1][3],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2] + m1[3][1] * m2[1][3],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2] + m1[3][0] * m2[2][3],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2] + m1[3][1] * m2[2][3],
        m1[0][0] * m2[3][0] + m1[1][0] * m2[3][1] + m1[2][0] * m2[3][2] + m1[3][0] * m2[3][3],
        m1[0][1] * m2[3][0] + m1[1][1] * m2[3][1] + m1[2][1] * m2[3][2] + m1[3][1] * m2[3][3]);
}
template<typename T>
LvnVec<2, T> operator*(const LvnMat<4, 2, T>& m, const LvnVec<4, T>& v)
{
    return LvnVec<2, T>(
        m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w,
        m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w);
}
template<typename T>
LvnVec<4, T> operator*(const LvnVec<2, T>& v, const LvnMat<4, 2, T>& m)
{
    return LvnVec<4, T>(
        v.x * m[0][0] + v.y * m[0][1],
        v.x * m[1][0] + v.y * m[1][1],
        v.x * m[2][0] + v.y * m[2][1],
        v.x * m[3][0] + v.y * m[3][1]);
}


template<typename T>
struct LvnMat<4, 3, T>
{
    LvnVec<3, T> value[4];

    static length_t length() { return 4; }

    LvnMat() = default;
    LvnMat(const LvnMat<4, 3, T>&) = default;
    LvnMat(const T& n)
    {
        this->value[0] = { n, 0, 0 };
        this->value[1] = { 0, n, 0 };
        this->value[2] = { 0, 0, n };
        this->value[3] = { 0, 0, 0 };
    }
    LvnMat
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
    LvnMat(const LvnVec<3, T>& v0, const LvnVec<3, T>& v1, const LvnVec<3, T>& v2, const LvnVec<3, T>& v3)
        : value{ v0, v1, v2, v3 } {}
    LvnMat(const LvnMat<2, 2, T>& m)
        : value{ LvnVec<3, T>(m[0], 0), LvnVec<3, T>(m[1], 0), LvnVec<3, T>(0, 0, 1), LvnVec<3, T>(0, 0, 0) } {}
    LvnMat(const LvnMat<3, 3, T>& m)
        : value{ LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1]), LvnVec<3, T>(m[2]), LvnVec<3, T>(0, 0, 0) } {}
    LvnMat(const LvnMat<4, 4, T>& m)
        : value{ LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1]), LvnVec<3, T>(m[2]), LvnVec<3, T>(m[3]) } {}
    LvnMat(const LvnMat<2, 3, T>& m)
        : value{ LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1]), LvnVec<3, T>(0, 0, 1), LvnVec<3, T>(0, 0, 0) } {}
    LvnMat(const LvnMat<2, 4, T>& m)
        : value{ LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1]), LvnVec<3, T>(0, 0, 1), LvnVec<3, T>(0, 0, 0) } {}
    LvnMat(const LvnMat<3, 2, T>& m)
        : value{ LvnVec<3, T>(m[0], 0), LvnVec<3, T>(m[1], 0), LvnVec<3, T>(m[2], 1), LvnVec<3, T>(0, 0, 0) } {}
    LvnMat(const LvnMat<3, 4, T>& m)
        : value{ LvnVec<3, T>(m[0]), LvnVec<3, T>(m[1]), LvnVec<3, T>(m[2]), LvnVec<3, T>(0, 0, 0) } {}
    LvnMat(const LvnMat<4, 2, T>& m)
        : value{ LvnVec<3, T>(m[0], 0), LvnVec<3, T>(m[1], 0), LvnVec<3, T>(m[2], 1), LvnVec<3, T>(m[3], 0) } {}

    LvnVec<3, T>& operator[](length_t i)
    {
        return this->value[i];
    }
    const LvnVec<3, T>& operator[](length_t i) const
    {
        return this->value[i];
    }

    LvnMat<4, 3, T> operator+() const
    {
        return LvnMat<4, 3, T>(
            this->value[0],
            this->value[1],
            this->value[2],
            this->value[3]);
    }
    LvnMat<4, 3, T> operator-() const
    {
        return LvnMat<4, 3, T>(
            -this->value[0],
            -this->value[1],
            -this->value[2],
            -this->value[3]);
    }
    LvnMat<4, 3, T> operator+(const LvnMat<4, 3, T>& m)
    {
        return LvnMat<4, 3, T>(
            this->value[0] + m[0],
            this->value[1] + m[1],
            this->value[2] + m[2],
            this->value[3] + m[3]);
    }
    LvnMat<4, 3, T> operator-(const LvnMat<4, 3, T>& m)
    {
        return LvnMat<4, 3, T>(
            this->value[0] - m[0],
            this->value[1] - m[1],
            this->value[2] - m[2],
            this->value[3] - m[3]);
    }
};

template<typename T>
LvnMat<4, 3, T> operator*(const LvnMat<4, 3, T>& m, const T& s)
{
    return LvnMat<4, 3, T>(
        m[0] * s,
        m[1] * s,
        m[2] * s,
        m[3] * s);
}
template<typename T>
LvnMat<4, 3, T> operator/(const LvnMat<4, 3, T>& m, const T& s)
{
    return LvnMat<4, 3, T>(
        m[0] / s,
        m[1] / s,
        m[2] / s,
        m[3] / s);
}
template<typename T>
LvnMat<4, 3, T> operator*(const T& s, const LvnMat<4, 3, T>& m)
{
    return LvnMat<4, 3, T>(
        s * m[0],
        s * m[1],
        s * m[2],
        s * m[3]);
}
template<typename T>
LvnMat<4, 3, T> operator/(const T& s, const LvnMat<4, 3, T>& m)
{
    return LvnMat<4, 3, T>(
        s / m[0],
        s / m[1],
        s / m[2],
        s / m[3]);
}
template<typename T>
LvnMat<2, 3, T> operator*(const LvnMat<4, 3, T>& m1, const LvnMat<2, 4, T>& m2)
{
    return LvnMat<2, 3, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2] + m1[3][0] * m2[0][3],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2] + m1[3][1] * m2[0][3],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2] + m1[3][2] * m2[0][3],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2] + m1[3][0] * m2[1][3],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2] + m1[3][1] * m2[1][3],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2] + m1[3][2] * m2[1][3]);
}
template<typename T>
LvnMat<3, 3, T> operator*(const LvnMat<4, 3, T>& m1, const LvnMat<3, 4, T>& m2)
{
    return LvnMat<3, 3, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2] + m1[3][0] * m2[0][3],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2] + m1[3][1] * m2[0][3],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2] + m1[3][2] * m2[0][3],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2] + m1[3][0] * m2[1][3],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2] + m1[3][1] * m2[1][3],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2] + m1[3][2] * m2[1][3],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2] + m1[3][0] * m2[2][3],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2] + m1[3][1] * m2[2][3],
        m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1] + m1[2][2] * m2[2][2] + m1[3][2] * m2[2][3]);
}
template<typename T>
LvnMat<4, 3, T> operator*(const LvnMat<4, 3, T>& m1, const LvnMat<4, 4, T>& m2)
{
    return LvnMat<4, 3, T>(
        m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2] + m1[3][0] * m2[0][3],
        m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2] + m1[3][1] * m2[0][3],
        m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2] + m1[3][2] * m2[0][3],
        m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2] + m1[3][0] * m2[1][3],
        m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2] + m1[3][1] * m2[1][3],
        m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2] + m1[3][2] * m2[1][3],
        m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2] + m1[3][0] * m2[2][3],
        m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2] + m1[3][1] * m2[2][3],
        m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1] + m1[2][2] * m2[2][2] + m1[3][2] * m2[2][3],
        m1[0][0] * m2[3][0] + m1[1][0] * m2[3][1] + m1[2][0] * m2[3][2] + m1[3][0] * m2[3][3],
        m1[0][1] * m2[3][0] + m1[1][1] * m2[3][1] + m1[2][1] * m2[3][2] + m1[3][1] * m2[3][3],
        m1[0][2] * m2[3][0] + m1[1][2] * m2[3][1] + m1[2][2] * m2[3][2] + m1[3][2] * m2[3][3]);
}
template<typename T>
LvnVec<3, T> operator*(const LvnMat<4, 3, T>& m, const LvnVec<4, T>& v)
{
    return LvnVec<3, T>(
        m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w,
        m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w,
        m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * v.w);
}
template<typename T>
LvnVec<4, T> operator*(const LvnVec<3, T>& v, const LvnMat<4, 3, T>& m)
{
    return LvnVec<4, T>(
        v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2] + v.w * m[0][3],
        v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2] + v.w * m[1][3],
        v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2] + v.w * m[2][3],
        v.x * m[3][0] + v.y * m[3][1] + v.z * m[3][2] + v.w * m[3][3]);
}


// -- quaternian
template<typename T>
struct LvnQuat_t
{
    union { T w, r; };
    union { T x, i; };
    union { T y, j; };
    union { T z, k; };

    static int length() { return 4; }

    LvnQuat_t() = default;
    LvnQuat_t(const LvnQuat_t<T>&) = default;

    LvnQuat_t(const T& nw, const T& nx, const T& ny, const T& nz)
        : w(nw), x(nx), y(ny), z(nz) {}

    T& operator[](int i)
    {
        LVN_ASSERT(i >= 0 && i < length(), "vector index out of range");

        switch (i)
        {
            default:
            case 0:
                return w;
            case 1:
                return x;
            case 2:
                return y;
            case 3:
                return z;
        }
    }
    const T& operator[](int i) const
    {
        LVN_ASSERT(i >= 0 && i < length(), "vector index out of range");

        switch (i)
        {
            default:
            case 0:
                return w;
            case 1:
                return x;
            case 2:
                return y;
            case 3:
                return z;
        }
    }

    LvnQuat_t<T>& operator++()
    {
        this->w++;
        this->x++;
        this->y++;
        this->z++;
        return *this;
    }
    LvnQuat_t<T>& operator--()
    {
        this->w--;
        this->x--;
        this->y--;
        this->z--;
        return *this;
    }
    LvnQuat_t<T> operator++(int)
    {
        LvnQuat_t<T> q(*this);
        ++*this;
        return q;
    }
    LvnQuat_t<T> operator--(int)
    {
        LvnQuat_t<T> q(*this);
        --*this;
        return q;
    }
    LvnQuat_t<T> operator+() const
    {
        return LvnQuat_t<T>(w, x, y, z);
    }
    LvnQuat_t<T> operator-() const
    {
        return LvnQuat_t<T>(-w, -x, -y, -z);
    }
    LvnQuat_t<T> operator+(const LvnQuat_t<T>& q)
    {
        return LvnQuat_t<T>(
            this->w + q.w,
            this->x + q.x,
            this->y + q.y,
            this->z + q.z);
    }
    LvnQuat_t<T> operator-(const LvnQuat_t<T>& q)
    {
        return LvnQuat_t<T>(
            this->w - q.w,
            this->x - q.x,
            this->y - q.y,
            this->z - q.z);
    }
    LvnQuat_t<T> operator*(const LvnQuat_t<T>& q)
    {
        return LvnQuat_t<T>(
            this->w * q.w - this->x * q.x - this->y * q.y - this->z * q.z,
            this->w * q.x + this->x * q.w + this->y * q.z - this->z * q.y,
            this->w * q.y + this->y * q.w + this->z * q.x - this->x * q.z,
            this->w * q.z + this->z * q.w + this->x * q.y - this->y * q.x);
    }
};

template <typename T>
LvnQuat_t<T> operator+(const LvnQuat_t<T>& q1, const LvnQuat_t<T>& q2)
{
    return LvnQuat_t<T>(q1.w + q2.w, q1.x + q2.x, q1.y + q2.y, q1.z + q2.z);
}
template <typename T>
LvnQuat_t<T> operator-(const LvnQuat_t<T>& q1, const LvnQuat_t<T>& q2)
{
    return LvnQuat_t<T>(q1.w - q2.w, q1.x - q2.x, q1.y - q2.y, q1.z - q2.z);
}
template <typename T>
LvnQuat_t<T> operator*(const LvnQuat_t<T>& q1, const LvnQuat_t<T>& q2)
{
    return LvnQuat_t<T>(q1.w * q2.w, q1.x * q2.x, q1.y * q2.y, q1.z * q2.z);
}
template <typename T>
LvnQuat_t<T> operator/(const LvnQuat_t<T>& q1, const LvnQuat_t<T>& q2)
{
    return LvnQuat_t<T>(q1.w / q2.w, q1.x / q2.x, q1.y / q2.y, q1.z / q2.z);
}
template <typename T>
LvnQuat_t<T> operator+(const T& s, const LvnQuat_t<T>& q)
{
    return LvnQuat_t<T>(s + q.w, s + q.x, s + q.y, s + q.z);
}
template <typename T>
LvnQuat_t<T> operator-(const T& s, const LvnQuat_t<T>& q)
{
    return LvnQuat_t<T>(s - q.w, s - q.x, s - q.y, s - q.z);
}
template <typename T>
LvnQuat_t<T> operator*(const T& s, const LvnQuat_t<T>& q)
{
    return LvnQuat_t<T>(s * q.w, s * q.x, s * q.y, s * q.z);
}
template <typename T>
LvnQuat_t<T> operator/(const T& s, const LvnQuat_t<T>& q)
{
    return LvnQuat_t<T>(s / q.w, s / q.x, s / q.y, s / q.z);
}
template <typename T>
LvnQuat_t<T> operator+(const LvnQuat_t<T>& q, const T& s)
{
    return LvnQuat_t<T>(q.w + s, q.x + s, q.y + s, q.z + s);
}
template <typename T>
LvnQuat_t<T> operator-(const LvnQuat_t<T>& q, const T& s)
{
    return LvnQuat_t<T>(q.w - s, q.x - s, q.y - s, q.z - s);
}
template <typename T>
LvnQuat_t<T> operator*(const LvnQuat_t<T>& q, const T& s)
{
    return LvnQuat_t<T>(q.w * s, q.x * s, q.y * s, q.z * s);
}
template <typename T>
LvnQuat_t<T> operator/(const LvnQuat_t<T>& q, const T& s)
{
    return LvnQuat_t<T>(q.w / s, q.x / s, q.y / s, q.z / s);
}

#endif /* !HG_LVN_LMATH_H */
