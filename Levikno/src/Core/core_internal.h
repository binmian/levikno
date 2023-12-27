#ifndef HG_LEVIKNO_CORE_INTERNAL_H
#define HG_LEVIKNO_CORE_INTERNAL_H

#include "levikno/Core.h"

namespace lvn
{
	/* [API] */
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
		const LogPattern* pLogPatterns;
		uint32_t logPatternCount;
	};


	/* [Events] */
	struct AppRenderEvent
	{
		Event info;
		char* eventString;

		AppRenderEvent()
		{
			info.type = EventType::AppRender;
			info.category = LvnEventCategory_Application;
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
			info.handled = false;
		}
	};

	struct WindowResizeEvent
	{
		Event info;
		char* eventString;

		unsigned int width, height;

		WindowResizeEvent()
		{
			info.type = EventType::WindowResize;
			info.category = LvnEventCategory_Window;
			info.handled = false;
		}
	};

	/* [Window] */
	struct Window
	{
		WindowInfo info;
		void* nativeWindow;
		void (*eventCallBackFn)(Event*);

		void (*updateWindow)(Window*);
		bool (*windowOpen)(Window*);
	};

}

#endif // !HG_CHONPS_CORE_INTERNAL_H
