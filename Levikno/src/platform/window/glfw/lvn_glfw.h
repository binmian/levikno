#ifndef HG_LEVIKNO_GLFW_H
#define HG_LEVIKNO_GLFW_H

#include "levikno_internal.h"

namespace lvn
{
	LvnResult glfwImplInitWindowContext(LvnWindowContext* windowContext);
	void glfwImplTerminateWindowContext();

	LvnResult glfwImplCreateWindow(LvnWindow* window, LvnWindowCreateInfo* createInfo);
	void glfwImplDestroyWindow(LvnWindow* window);

	void glfwImplUpdateWindow(LvnWindow* window);
	bool glfwImplWindowOpen(LvnWindow* window);
	LvnPair<int> glfwImplGetDimensions(LvnWindow* window);
	unsigned int glfwImplGetWindowWidth(LvnWindow* window);
	unsigned int glfwImplGetWindowHeight(LvnWindow* window);
	void glfwImplSetWindowVSync(LvnWindow* window, bool enable);
	bool glfwImplGetWindowVSync(LvnWindow* window);
	void glfwImplSetWindowContextCurrent(LvnWindow* window);
	void glfwImplEventCallBackFn(LvnEvent* e);

	bool glfwImplKeyPressed(LvnWindow* window, int keycode);
	bool glfwImplKeyReleased(LvnWindow* window, int keycode);
	bool glfwImplMouseButtonPressed(LvnWindow* window, int button);
	bool glfwImplMouseButtonReleased(LvnWindow* window, int button);

	void glfwImplSetMousePos(LvnWindow* window, float x, float y);

	LvnPair<float> glfwImplGetMousePos(LvnWindow* window);
	void glfwImplGetMousePosPtr(LvnWindow* window, float* xpos, float* ypos);
	float glfwImplGetMouseX(LvnWindow* window);
	float glfwImplGetMouseY(LvnWindow* window);

	LvnPair<int> glfwImplGetWindowPos(LvnWindow* window);
	void glfwImplGetWindowPosPtr(LvnWindow* window, int* xpos, int* ypos);
	LvnPair<int> glfwImplGetWindowSize(LvnWindow* window);
	void glfwImplGetWindowSizePtr(LvnWindow* window, int* width, int* height);
}

#endif // !HG_CHONPS_GLFW_H
