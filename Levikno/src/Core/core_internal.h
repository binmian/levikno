#ifndef HG_LEVIKNO_CORE_INTERNAL_H
#define HG_LEVIKNO_CORE_INTERNAL_H

#include "levikno/Core.h"

namespace lvn
{
	/* [Logging] */
	struct Logger
	{
		const char* loggerName;
		LvnLogLevel logLevel;
		const char* logPatternFormat;
		LogPattern* pLogPatterns;
		uint32_t logPatternCount;
	};

	void logParseFormat(const char* fmt, LogPattern** pLogPatterns, uint32_t* logPatternCount);


	/* [Events] */
	struct Event
	{
		EventType type;
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
	struct WindowData
	{
		int width, height;
		const char* title;
		int minWidth, minHeight;
		int maxWidth, maxHeight;
		bool fullscreen, resizable, vSync;
		WindowIconData* pIcons;
		uint32_t iconCount;
		void (*eventCallBackFn)(Event*);
	};

	struct Window
	{
		WindowData	data;
		void*		nativeWindow;
	};

	struct WindowContext
	{
		WindowAPI			windowapi;

		Window*				(*createWindow)(int, int, const char*, bool, bool, int, int);
		Window*				(*createWindowInfo)(WindowCreateInfo*);
		void				(*updateWindow)(Window*);
		bool				(*windowOpen)(Window*);
		WindowDimension		(*getDimensions)(Window*);
		unsigned int		(*getWindowWidth)(Window*);
		unsigned int		(*getWindowHeight)(Window*);
		void				(*setWindowVSync)(Window*, bool);
		bool				(*getWindowVSync)(Window*);
		void				(*setWindowContextCurrent)(Window*);
		void				(*destroyWindow)(Window*);
	};

}

#endif // !HG_LEVINKO_CORE_INTERNAL_H
