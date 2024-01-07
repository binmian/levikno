#ifndef HG_LEVIKNO_CORE_INTERNAL_H
#define HG_LEVIKNO_CORE_INTERNAL_H

#include "levikno/Core.h"


/* [Logging] */
struct LvnLogger
{
	const char* loggerName;
	LvnLogLevel logLevel;
	const char* logPatternFormat;
	LvnLogPattern* pLogPatterns;
	uint32_t logPatternCount;
};

/* [Events] */
struct LvnEvent
{
	LvnEventType type;
	int category;
	const char* name;
	bool handled;

	union data
	{
		struct /* Mouse/Window pos & size */
		{
			union
			{
				struct
				{
					int x, y;
				};
				struct
				{
					double xd, yd;
				};
			};
		};
		struct /* key/mouse button codes */
		{
			int code;
			unsigned int ucode;
			bool repeat;
		};
	} data;
};


/* [Window] */
struct LvnWindowData
{
	int width, height;
	const char* title;
	int minWidth, minHeight;
	int maxWidth, maxHeight;
	bool fullscreen, resizable, vSync;
	LvnWindowIconData* pIcons;
	uint32_t iconCount;
	void (*eventCallBackFn)(LvnEvent*);
};

struct LvnWindow
{
	LvnWindowData	data;
	void* nativeWindow;
};

struct LvnWindowContext
{
	LvnWindowApi		windowapi;

	LvnWindow*			(*createWindow)(int, int, const char*, bool, bool, int, int);
	LvnWindow*			(*createWindowInfo)(LvnWindowCreateInfo*);
	void				(*updateWindow)(LvnWindow*);
	bool				(*windowOpen)(LvnWindow*);
	LvnWindowDimension	(*getDimensions)(LvnWindow*);
	unsigned int		(*getWindowWidth)(LvnWindow*);
	unsigned int		(*getWindowHeight)(LvnWindow*);
	void				(*setWindowVSync)(LvnWindow*, bool);
	bool				(*getWindowVSync)(LvnWindow*);
	void				(*setWindowContextCurrent)(LvnWindow*);
	void				(*destroyWindow)(LvnWindow*);
};


#endif // !HG_LEVINKO_CORE_INTERNAL_H