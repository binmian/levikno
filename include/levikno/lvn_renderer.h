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

struct LvnCircle;
struct LvnCollisionCircle;
struct LvnCollisionPoint;
struct LvnCollisionRect;
struct LvnRect;
struct LvnRenderer;
struct LvnSprite;
struct LvnText;
struct LvnTriangle;
struct LvnUVBox;


// ------------------------------------------------------------
// [SECTION]: Renderer Struct Implementation
// ------------------------------------------------------------

struct LvnUVBox
{
    float x0, y0, x1, y1;
};

struct LvnTriangle
{
    LvnVec2 v1;
    LvnVec2 v2;
    LvnVec2 v3;
    LvnColor color;
    LvnTexture* texture;
};

struct LvnRect
{
    LvnVec2 size;
    LvnColor color;
    LvnTexture* texture;
};

struct LvnCircle
{
    float radius;
    float startAngle;
    float endAngle;
    uint32_t nSides;
    LvnColor color;
    LvnTexture* texture;
};

struct LvnSprite
{
    LvnVec2 size;
    LvnUVBox uv;
    LvnTexture* texture;
};

struct LvnText
{
    LvnString text;
    LvnColor color;
    float scale;
    float lineHeight;
    float textBoxWidth;
};

struct LvnCollisionPoint
{
    float x, y;
};

struct LvnCollisionRect
{
    LvnVec2 pos;
    LvnVec2 size;
};

struct LvnCollisionCircle
{
    LvnVec2 pos;
    float radius;
};


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

    LVN_API LvnTriangle                 configTriangleInit(const LvnVec2& v1, const LvnVec2& v2, const LvnVec2& v3, const LvnColor& color, LvnTexture* texture = nullptr);
    LVN_API LvnRect                     configRectInit(const LvnVec2& size, const LvnColor& color, LvnTexture* texture = nullptr);
    LVN_API LvnCircle                   configCircleInit(float radius, const LvnColor& color, LvnTexture* texture = nullptr);
    LVN_API LvnSprite                   configSpriteInit(const LvnVec2& size, const LvnUVBox& uv, LvnTexture* texture);
    LVN_API LvnText                     configTextInit(const char* text, const LvnColor& color, float scale, float lineHeight = 2.0f, float textBoxWidth = 0.0f);

    LVN_API LvnResult                   rendererUploadTexture(LvnTexture* texture);
    LVN_API LvnResult                   rendererUnloadTexture(LvnTexture* texture);

    LVN_API void                        drawBegin();
    LVN_API void                        drawEnd();
    LVN_API void                        drawClearColor(float r, float g, float b, float a);
    LVN_API void                        drawClearColor(const LvnColor& color);
    LVN_API void                        drawBindTexture(LvnTexture* texture);
    LVN_API void                        drawTriangle(const LvnVec2& pos, const LvnVec2& v1, const LvnVec2& v2, const LvnVec2& v3, const LvnColor& color);
    LVN_API void                        drawTriangleEx(const LvnTriangle& triangle, const LvnVec2& pos);
    LVN_API void                        drawRect(const LvnVec2& pos, const LvnVec2& size, const LvnColor& color);
    LVN_API void                        drawRectEx(const LvnRect& rect, const LvnVec2& pos);
    LVN_API void                        drawCircle(const LvnVec2& pos, float radius, const LvnColor& color);
    LVN_API void                        drawCircleSector(const LvnVec2& pos, float radius, float startAngle, float endAngle, const LvnColor& color);
    LVN_API void                        drawPolyNgon(const LvnVec2& pos, float radius, uint32_t nSides, const LvnColor& color);
    LVN_API void                        drawPolyNgonSector(const LvnVec2& pos, float radius, float startAngle, float endAngle, uint32_t nSides, const LvnColor& color);
    LVN_API void                        drawCircleEx(const LvnCircle& circle, const LvnVec2& pos);
    LVN_API void                        drawSprite(const LvnSprite& sprite, const LvnVec2& pos, const LvnColor& tint = {255,255,255,255});
    LVN_API void                        drawText(const char* text, const LvnVec2& pos, const LvnColor& color, float scale);
    LVN_API void                        drawTextEx(const LvnText& text, const LvnVec2& pos);
    LVN_API void                        drawTextBox(const char* text, const LvnVec2& pos, const LvnColor& color, float scale, float lineHeight, float textBoxWidth);

    LVN_API bool                        collisionPointToPoint(const LvnCollisionPoint& p1, const LvnCollisionPoint& p2, float epsilon = std::numeric_limits<float>::epsilon());
    LVN_API bool                        collisionPointToRect(const LvnCollisionPoint& point, const LvnCollisionRect& rect);
    LVN_API bool                        collisionPointToCircle(const LvnCollisionPoint& point, const LvnCollisionCircle& circle);
    LVN_API bool                        collisionRectToRect(const LvnCollisionRect& rect1, const LvnCollisionRect& rect2);
    LVN_API bool                        collisionRectToCircle(const LvnCollisionRect& rect, const LvnCollisionCircle& circle);
    LVN_API bool                        collisionCircleToCircle(const LvnCollisionCircle& circle1, const LvnCollisionCircle& circle2);
} /* namespace lvn */



#endif
