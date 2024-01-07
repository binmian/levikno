#include "graphics_internal.h"
#include "levikno/Core.h"

#include "Platform/API/Vulkan/lvn_vulkan.h"

namespace lvn
{
	static LvnGraphicsContext* s_GraphicsContext = nullptr;
	
	bool createGraphicsContext(LvnGraphicsApi graphicsapi)
	{
		if (graphicsapi != Lvn_GraphicsApi_None)
		{
			s_GraphicsContext = new LvnGraphicsContext();
			s_GraphicsContext->graphicsapi = graphicsapi;
		
			switch (graphicsapi)
			{
				case Lvn_GraphicsApi_vulkan:
				{
					vksImplCreateContext(s_GraphicsContext);
					break;
				}
				case Lvn_GraphicsApi_opengl:
				{
					
					break;
				}
			}
		
			LVN_CORE_INFO("graphics context set: %s", getGraphicsApiName());
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
			case Lvn_GraphicsApi_None:
			{
				LVN_CORE_WARN("no Graphics API Initialized! Cannot terminate Graphics API!");
				return false;
			}
			case Lvn_GraphicsApi_vulkan:
			{
				vksImplTerminateContext();
				delete s_GraphicsContext;
				break;
			}
			case Lvn_GraphicsApi_opengl:
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

	LvnGraphicsApi getGraphicsApi()
	{
		return s_GraphicsContext->graphicsapi;
	}

	const char* getGraphicsApiName()
	{
		switch (s_GraphicsContext->graphicsapi)
		{
			case Lvn_GraphicsApi_None:   { return "None";   }
			case Lvn_GraphicsApi_vulkan: { return "vulkan"; }
			case Lvn_GraphicsApi_opengl: { return "opengl"; }
		}

		LVN_CORE_ERROR("Unknown Graphics API selected!");
		return nullptr;
	}

	void getPhysicalDevices(LvnPhysicalDevice* pPhysicalDevices, uint32_t* deviceCount)
	{
		vksImplGetPhysicalDevices(pPhysicalDevices, deviceCount);
	}

	bool renderInit(LvnRendererBackends* renderBackends)
	{
		return vksImplRenderInit(renderBackends);
	}
}