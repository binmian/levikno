#ifndef HG_LEVIKNO_CORE_INTERNAL_H
#define HG_LEVIKNO_CORE_INTERNAL_H

#include "levikno/Core.h"
#include <string.h>

namespace lvn
{
	/* [API] */
	struct LvnString
	{
		char* str;
		uint32_t size;

		LvnString()
			: str(0), size(0) {}

		LvnString(const char* strsrc)
		{
			size = strlen(strsrc) + 1;
			str = (char*)malloc(size);
			if (!str) { LVN_MALLOC_FAILURE(str); return; }
			memcpy(str, strsrc, size);
		}
		LvnString(char* strsrc, uint32_t strsize)
		{
			size = strsize;
			str = (char*)malloc(strsize);
			if (!str) { LVN_MALLOC_FAILURE(str); return; }
			memcpy(str, strsrc, strsize);
		}
		LvnString(const LvnString& lvnstr)
		{
			size = lvnstr.size;
			str = (char*)malloc(size);
			if (!str) { LVN_MALLOC_FAILURE(str); return; }
			memcpy(str, lvnstr.str, size);
		}

		~LvnString() { free(str); }

		LvnString& operator=(const LvnString& lvnstr)
		{
			size = lvnstr.size;
			str = (char*)malloc(size);
			if (!str) { LVN_MALLOC_FAILURE(str); return *this; }
			memcpy(str, lvnstr.str, size);
			return *this;
		}
	};

	template <typename T>
	struct LvnVector
	{
		T* arr;
		uint32_t size;

		LvnVector()
			: arr(0), size(0) {}

		LvnVector(T* arr, uint32_t size)
		{
			this->size = size;
			this->arr = arr;
		}
		LvnVector(const LvnVector& lvnvec)
		{
			size = lvnvec.size;
			if (!size)
				memcpy(arr, lvnvec.arr, size);
		}
	};

	struct LvnDateTimeStringContainer
	{
		char time24HHMMSS[9];
		char time12HHMMSS[9];
		char yearName[5];
		char year02dName[3];
		char monthNumberName[3];
		char dayNumberName[3];
		char hourNumberName[3];
		char hour12NumberName[3];
		char minuteNumberName[3];
		char secondNumberName[3];
	};


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
			struct
			{
				union
				{
					int x, y;
					double xd, yd;
				};
			};
			struct
			{
				int code;
				unsigned int ucode;
				bool repeat;
			};
		} data;
	};

	struct AppRenderEvent
	{
		Event info;
		char* eventString;

		AppRenderEvent()
		{
			info.type = EventType::AppRender;
			info.category = LvnEventCategory_Application;
			info.name = "AppRenderEvent";
			info.handled = false;
		}
	};

	struct AppTickEvent
	{
		Event info;
		char* eventString;

		AppTickEvent()
		{
			info.type = EventType::AppTick;
			info.category = LvnEventCategory_Application;
			info.name = "AppTickEvent";
			info.handled = false;
		}
	};

	struct KeyHoldEvent
	{
		Event info;
		char* eventString;

		char* key;
		int keyCode;
		bool repeat;

		KeyHoldEvent()
		{
			info.type = EventType::KeyHold;
			info.category = LvnEventCategory_Input | LvnEventCategory_Keyboard;
			info.name = "KeyHoldEvent";
			info.handled = false;
		}
	};

	struct KeyPressedEvent
	{
		Event info;
		char* eventString;

		char* key;
		int keyCode;

		KeyPressedEvent()
		{
			info.type = EventType::KeyPressed;
			info.category = LvnEventCategory_Input | LvnEventCategory_Keyboard;
			info.name = "KeyPressedEvent";
			info.handled = false;
		}
	};

	struct KeyReleasedEvent
	{
		Event info;
		char* eventString;

		char* key;
		int keyCode;

		KeyReleasedEvent()
		{
			info.type = EventType::KeyReleased;
			info.category = LvnEventCategory_Input | LvnEventCategory_Keyboard;
			info.name = "KeyReleasedEvent";
			info.handled = false;
		}
	};

	struct MouseButtonPressedEvent
	{
		Event info;
		char* eventString;

		int buttonCode;

		MouseButtonPressedEvent()
		{
			info.type = EventType::MouseButtonPressed;
			info.category = LvnEventCategory_Input | LvnEventCategory_MouseButton | LvnEventCategory_Mouse;
			info.name = "MouseButtonPressedEvent";
			info.handled = false;
		}
	};

	struct MouseButtonReleasedEvent
	{
		Event info;
		char* eventString;

		int buttonCode;

		MouseButtonReleasedEvent()
		{
			info.type = EventType::MouseButtonReleased;
			info.category = LvnEventCategory_Input | LvnEventCategory_MouseButton | LvnEventCategory_Mouse;
			info.name = "MouseButtonReleasedEvent";
			info.handled = false;
		}
	};

	struct MouseMovedEvent
	{
		Event info;
		char* eventString;

		int x, y;

		MouseMovedEvent()
		{
			info.type = EventType::MouseMoved;
			info.category = LvnEventCategory_Input | LvnEventCategory_Mouse;
			info.name = "MouseMovedEvent";
			info.handled = false;
		}
	};

	struct MouseScrolledEvent
	{
		Event info;
		char* eventString;

		int x, y;

		MouseScrolledEvent()
		{
			info.type = EventType::MouseScrolled;
			info.category = LvnEventCategory_Input | LvnEventCategory_MouseButton | LvnEventCategory_Mouse;
			info.name = "MouseScrolledEvent";
			info.handled = false;
		}
	};

	struct WindowCloseEvent
	{
		Event info;
		char* eventString;

		WindowCloseEvent()
		{
			info.type = EventType::WindowClose;
			info.category = LvnEventCategory_Window;
			info.name = "WindowCloseEvent";
			info.handled = false;
		}
	};

	struct WindowFocusEvent
	{
		Event info;
		char* eventString;

		WindowFocusEvent()
		{
			info.type = EventType::WindowFocus;
			info.category = LvnEventCategory_Window;
			info.name = "WindowFocusEvent";
			info.handled = false;
		}
	};

	struct WindowFramebufferResizeEvent
	{
		Event info;
		char* eventString;

		unsigned int width, height;

		WindowFramebufferResizeEvent()
		{
			info.type = EventType::WindowFramebufferResize;
			info.category = LvnEventCategory_Window;
			info.name = "WindowFramebufferResizeEvent";
			info.handled = false;
		}
	};

	struct WindowLostFocusEvent
	{
		Event info;
		char* eventString;

		WindowLostFocusEvent()
		{
			info.type = EventType::WindowLostFocus;
			info.category = LvnEventCategory_Window;
			info.name = "WindowLostFocusEvent";
			info.handled = false;
		}
	};

	struct WindowMovedEvent
	{
		Event info;
		char* eventString;

		int x, y;

		WindowMovedEvent()
		{
			info.type = EventType::WindowMoved;
			info.category = LvnEventCategory_Window;
			info.name = "WindowMovedEvent";
			info.handled = false;
		}
	};

	struct WindowResizeEvent
	{
		Event info;
		char* eventString;

		int width, height;

		WindowResizeEvent()
		{
			info.type = EventType::WindowResize;
			info.category = LvnEventCategory_Window;
			info.name = "WindowResizeEvent";
			info.handled = false;
		}
	};

	/* [Window] */
	struct WindowInfo
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
		WindowInfo	info;
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
