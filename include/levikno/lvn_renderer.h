#ifndef HG_LVN_RENDERER_H
#define HG_LVN_RENDERER_H

// ------------------------------------------------------------
// Layout: lvn_renderer.h
// ------------------------------------------------------------
// - higher level renderer api
//
// [SECTION]: Renderer Enums
// [SECTION]: Renderer Struct Defines
// [SECTION]: Renderer Functions
// [SECTION]: Renderer Struct Implementation


#include "levikno.h"


// ------------------------------------------------------------
// [SECTION]: Renderer Enums
// ------------------------------------------------------------

enum LvnAttributeLocation
{
    Lvn_AttributeLocation_Position = 0,
    Lvn_AttributeLocation_Color,
    Lvn_AttributeLocation_TexCoords,
    Lvn_AttributeLocation_TexId,
    // Lvn_AttributeLocation_Normal,
    // Lvn_AttributeLocation_Tangent,
    // Lvn_AttributeLocation_BoneIds,
    // Lvn_AttributeLocation_Weights,

    Lvn_AttributeLocation_Max_Value,
};

enum LvnRenderModeEnum
{
    Lvn_RenderMode_2d,
    Lvn_RenderMode_2dText,

    Lvn_RenderMode_Max_Value,
};


// ------------------------------------------------------------
// [SECTION]: Renderer Struct Defines
// ------------------------------------------------------------

struct LvnColor;
struct LvnCircle;
struct LvnPoint;
struct LvnRect;
struct LvnRenderer;
struct LvnSprite;
struct LvnTriangle;
struct LvnUVBox;


namespace lvn
{
    // ------------------------------------------------------------
    // [SECTION]: Renderer Functions
    // ------------------------------------------------------------
    // - high end api functions

    LVN_API LvnResult                   renderInit(const char* title, int width, int height);
    LVN_API LvnResult                   renderInit(const LvnWindowCreateInfo* createInfo);
    LVN_API void                        renderTerminate();
    LVN_API bool                        rendererIsInitialized();
    LVN_API LvnWindow*                  getRendererWindow();
    LVN_API bool                        renderWindowOpen();

    LVN_API LvnSprite                   createSprite(const LvnTextureCreateInfo& texCreateInfo, const LvnUVBox& uv);
    LVN_API void                        destroySprite(LvnSprite& sprite);

    LVN_API void                        drawBegin();
    LVN_API void                        drawEnd();
    LVN_API void                        drawClearColor(float r, float g, float b, float a);
    LVN_API void                        drawClearColor(const LvnColor& color);
    LVN_API void                        drawTriangle(const LvnVec2& v1, const LvnVec2& v2, const LvnVec2& v3, const LvnColor& color);
    LVN_API void                        drawRect(const LvnVec2& pos, const LvnVec2& size, const LvnColor& color);
    LVN_API void                        drawRectEx(const LvnRect& rect);
    LVN_API void                        drawCircle(const LvnVec2& pos, float radius, const LvnColor& color);
    LVN_API void                        drawCircleSector(const LvnVec2& pos, float radius, float startAngle, float endAngle, const LvnColor& color);
    LVN_API void                        drawPolyNgon(const LvnVec2& pos, float radius, uint32_t nSides, const LvnColor& color);
    LVN_API void                        drawPolyNgonSector(const LvnVec2& pos, float radius, float startAngle, float endAngle, uint32_t nSides, const LvnColor& color);
    LVN_API void                        drawText(const char* text, const LvnVec2& pos, const LvnColor& color, float scale);
    LVN_API void                        drawTextEx(const char* text, const LvnVec2& pos, const LvnColor& color, float scale, float lineHeight, float textBoxWidth);
} /* namespace lvn */


// ------------------------------------------------------------
// [SECTION]: Renderer Struct Implementation
// ------------------------------------------------------------

struct LvnColor
{
    uint8_t r, g, b, a;
};

struct LvnUVBox
{
    float x0, y0, x1, y1;
};

struct LvnPoint
{
    float x, y;
};

struct LvnTriangle
{
    LvnVec2 v1;
    LvnVec2 v2;
    LvnVec2 v3;
};

struct LvnRect
{
    LvnVec2 pos;
    LvnVec2 size;
    LvnColor color;
    LvnTexture* texture;
};

struct LvnCircle
{
    LvnVec2 pos;
    float radius;
};

struct LvnSprite
{
    LvnTexture* texture;
    LvnUVBox uv;
};

#endif
