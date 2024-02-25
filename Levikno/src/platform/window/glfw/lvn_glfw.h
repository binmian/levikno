#ifndef HG_LEVIKNO_GLFW_H
#define HG_LEVIKNO_GLFW_H

#include "levikno_internal.h"

namespace lvn
{
	LVN_API LvnResult glfwImplInitWindowContext(LvnWindowContext* windowContext);
	LVN_API void glfwImplTerminateWindowContext();

	LVN_API LvnResult glfwImplCreateWindow(LvnWindow* window, LvnWindowCreateInfo* createInfo);
	LVN_API void glfwImplDestroyWindow(LvnWindow* window);

	LVN_API void glfwImplUpdateWindow(LvnWindow* window);
	LVN_API bool glfwImplWindowOpen(LvnWindow* window);
	LVN_API LvnWindowDimensions glfwImplGetDimensions(LvnWindow* window);
	LVN_API unsigned int glfwImplGetWindowWidth(LvnWindow* window);
	LVN_API unsigned int glfwImplGetWindowHeight(LvnWindow* window);
	LVN_API void glfwImplSetWindowVSync(LvnWindow* window, bool enable);
	LVN_API bool glfwImplGetWindowVSync(LvnWindow* window);
	LVN_API void glfwImplSetWindowContextCurrent(LvnWindow* window);
	LVN_API void glfwImplEventCallBackFn(LvnEvent* e);
}

#endif // !HG_CHONPS_GLFW_H
