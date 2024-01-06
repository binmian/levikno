#ifndef HG_LEVIKNO_GLFW_H
#define HG_LEVIKNO_GLFW_H

#include "Core/core_internal.h"

namespace lvn
{
	void				glfwImplInitWindowContext(WindowContext* windowContext);
	void				glfwImplTerminateWindowContext();

	Window*				glfwImplCreateWindow(int width, int height, const char* title, bool fullscreen, bool resizable, int minWidth, int minHeight);
	Window*				glfwImplCreateWindowInfo(WindowCreateInfo* winCreateInfo);
		
	void				glfwImplInitWindow(Window* window);
		
	void				glfwImplUpdateWindow(Window* window);
	bool				glfwImplWindowOpen(Window* window);
	WindowDimension		glfwImplGetDimensions(Window* window);
	unsigned int		glfwImplGetWindowWidth(Window* window);
	unsigned int		glfwImplGetWindowHeight(Window* window);
	void				glfwImplSetWindowVSync(Window* window, bool enable);
	bool				glfwImplGetWindowVSync(Window* window);
	void				glfwImplSetWindowContextCurrent(Window* window);
	void				glfwImplDestroyWindow(Window* window);
	void				glfwImplEventCallBackFn(Event* e);

	const char**		glfwImplGetInstanceExtensions(uint32_t* extensionCount);
}

#endif // !HG_CHONPS_GLFW_H
