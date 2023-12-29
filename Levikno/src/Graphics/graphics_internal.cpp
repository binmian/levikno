#include "graphics_internal.h"
#include "levikno/Core.h"

namespace lvn
{
	static GraphicsContext* s_GraphicsContext = nullptr;
	
	bool createGraphicsContext(GraphicsAPI graphicsapi)
	{
		switch (graphicsapi)
		{
			case GraphicsAPI::None:
			{
				LVN_CORE_WARN("setting Graphics API to None, no Graphics API selected!");
				return false;
			}
			case GraphicsAPI::vulkan:
			{
				s_GraphicsContext = new GraphicsContext();
				s_GraphicsContext->graphicsapi = graphicsapi;
				break;
			}
			case GraphicsAPI::opengl:
			{
				break;
			}
		}

		LVN_CORE_INFO("graphics context set: %s", getGraphicsAPIName());
		return true;
	}

	bool terminateGraphicsContext()
	{
		switch (s_GraphicsContext->graphicsapi)
		{
			case GraphicsAPI::None:
			{
				LVN_CORE_WARN("no Graphics API Initialized! Cannot terminate Graphics API!");
				return false;
			}
			case GraphicsAPI::vulkan:
			{
				delete s_GraphicsContext;
				break;
			}
			case GraphicsAPI::opengl:
			{

				break;
			}
			default:
			{
				LVN_CORE_ERROR("unknown Graphics API selected! Cannot terminate Graphics API!");
				return false;
			}
		}

		LVN_CORE_INFO("graphics context terminated");
		return true;
	}

	GraphicsAPI getGraphicsAPI()
	{
		return s_GraphicsContext->graphicsapi;
	}

	const char* getGraphicsAPIName()
	{
		switch (s_GraphicsContext->graphicsapi)
		{
			case GraphicsAPI::None:   { return "None";   }
			case GraphicsAPI::vulkan: { return "vulkan"; }
			case GraphicsAPI::opengl: { return "opengl"; }
		}

		LVN_CORE_ERROR("Unknown Graphics API selected!");
		return nullptr;
	}
}