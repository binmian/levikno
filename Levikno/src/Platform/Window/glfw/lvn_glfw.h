#ifndef HG_LEVIKNO_GLFW_H
#define HG_LEVIKNO_GLFW_H

#include "Core/core_internal.h"

namespace lvn
{
	LVN_API void				glfwImplInitWindowContext(LvnWindowContext* windowContext);
	LVN_API void				glfwImplTerminateWindowContext();

	LVN_API LvnWindow*			glfwImplCreateWindow(int width, int height, const char* title, bool fullscreen, bool resizable, int minWidth, int minHeight);
	LVN_API LvnWindow*			glfwImplCreateWindowInfo(LvnWindowCreateInfo* winCreateInfo);
		
	LVN_API void				glfwImplInitWindow(LvnWindow* window);
		
	LVN_API void				glfwImplUpdateWindow(LvnWindow* window);
	LVN_API bool				glfwImplWindowOpen(LvnWindow* window);
	LVN_API LvnWindowDimension	glfwImplGetDimensions(LvnWindow* window);
	LVN_API unsigned int		glfwImplGetWindowWidth(LvnWindow* window);
	LVN_API unsigned int		glfwImplGetWindowHeight(LvnWindow* window);
	LVN_API void				glfwImplSetWindowVSync(LvnWindow* window, bool enable);
	LVN_API bool				glfwImplGetWindowVSync(LvnWindow* window);
	LVN_API void				glfwImplSetWindowContextCurrent(LvnWindow* window);
	LVN_API void				glfwImplDestroyWindow(LvnWindow* window);
	LVN_API void				glfwImplEventCallBackFn(LvnEvent* e);

	LVN_API const char**		glfwImplGetInstanceExtensions(uint32_t* extensionCount);
}

#endif // !HG_CHONPS_GLFW_H
