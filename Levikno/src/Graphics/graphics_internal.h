#ifndef HG_LEVIKNO_GRAPHICS_INTERNAL_H
#define HG_LEVIKNO_GRAPHICS_INTERNAL_H

#include "levikno/Graphics.h"

/* [Graphics API] */
struct LvnGraphicsContext
{
	LvnGraphicsApi graphicsapi;

	void (*renderClearColor)(const float, const float, const float, const float);
	void (*renderClear)();
	void (*renderDraw)(uint32_t);
	void (*renderDrawIndexed)(uint32_t);
	void (*renderDrawInstanced)(uint32_t, uint32_t, uint32_t);
	void (*renderDrawIndexedInstanced)(uint32_t, uint32_t, uint32_t);
	void (*renderSetStencilReference)(uint32_t);
	void (*renderSetStencilMask)(uint32_t, uint32_t);
	void (*renderBeginNextFrame)();
	void (*renderDrawSubmit)();
	void (*renderBeginRenderPass)();
	void (*renderEndRenderPass)();
};

namespace lvn
{

}

#endif // !HG_LEVIKNO_GRAPHICS_INTERNAL_H
