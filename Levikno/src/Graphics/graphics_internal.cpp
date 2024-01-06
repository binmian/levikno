#include "graphics_internal.h"
#include "levikno/Core.h"

#include "Platform/API/Vulkan/lvn_vulkan.h"

namespace lvn
{
	static GraphicsContext* s_GraphicsContext = nullptr;
	
	bool createGraphicsContext(GraphicsAPI graphicsapi, RendererBackends* renderBackends)
	{
		if (graphicsapi != GraphicsAPI::None)
		{
			s_GraphicsContext = new GraphicsContext();
			s_GraphicsContext->graphicsapi = graphicsapi;
		
			switch (graphicsapi)
			{
				case GraphicsAPI::vulkan:
				{
					vksImplCreateContext(s_GraphicsContext, renderBackends);
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
		else
		{
			LVN_CORE_WARN("setting Graphics API to None, no Graphics API selected!");
			return false;
		}
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
				vksImplTerminateContext();
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

	void getPhysicalDevices(PhysicalDevice* pPhysicalDevices, uint32_t* deviceCount)
	{
		vksImplGetPhysicalDevices(pPhysicalDevices, deviceCount);
	}
}