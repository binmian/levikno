#include <levikno/levikno.h>

// NOTE: this example explains and demonstrates how the internal memory pool works


// levikno has the option to allocate a memory pool or resource manager when creating and destroying objects.
// In the internal library, the resource manger is divided into two parts, the base memory manager and the block memory managers.
//
// The base memory manager consists of all sTypes (structure types eg. LvnBuffer, LvnTexture) all allocated into one large memory chunk,
// each sType has a size in bytes, and a count parameter that specifies how many objects are allocated, by default levikno assumes a number
// of each sType will be used depending on the functionality of the object (for example, less LvnWindow objects will be allocates than LvnBuffers).
// The number of objects to be allocated on the base memory block and further memory blocks can be changed in the context config.
//
// The base memory block is further divided again into smaller sections for each sType, a LvnMemoryBinding manages the number of objects
// allocated for this sType with the chunk is it given within the memory blocks
//
// When a memory binding is full (all allocations within the binding have been used), a second binding and memory block will be created that is linked
// at the end of the first binding
//
// Note that further block memories created after the base memory block will be seperate for each sType
//
// Later on when an object is destroyed, the memory index of that object will be returned back to the memory binding to be used later again when
// another object of the same type is created.
//
//
//  Base Memory Block:
//
//  Bindings:                  Buffers                         Textures         Loggers      - LvnBindings keep track of the memory space they are given
//                                |                               |                |           and the number of objects currently allocated
//            /-------------------------------------\ /----------------------\ /-------\
//                  v                                            v
//           +---------------------------------------+------------------------+---------+
//  memory:  | X  |    | X  | X  | X  |    | X  | X  | X  | X  |    |    |    | X  | X  |   <---- Logger LvnBinding is full, levikno allocates a second memory block/binding
//           +---------------------------------------+------------------------+---------+         and links the second binding to the end of the first
//  count:   | 1    2    3    4    5    6    7    8  | 1    2    3    4    5  | 1    2
//                                                |                        |         |
//                     +--------------------------+                        |         |
//                     |                             +---------------------+ +-------+
//  Block Memories:    |                             |                       |              - Note that block memories for each sType are seperate from each other unlike
//                     |                             |                       |                the base memory block where evert sType binding is contained within on block
//  Bindings:       Buffers                       Textures                Loggers
//                     |                             |                       |
//            /-----------------\        /----------------------\        /-------\
//                                                                             v  <---- next memory index to be used when creating new object
//           +-------------------+      +------------------------+      +---------+
//  memory:  | X  | X  |    |    |      |    |    |    |    |    |      | X  |    |
//           +-------------------+      +-------------------+----+      +---------+
//  count:     1    2    3    4           1    2    3    4    5           1    2
//                            |                               |                |
//                            |                               |                |
//                           ...                             ...              ...
//

int main(int argc, char** argv)
{
	LvnContextCreateInfo lvnCreateInfo{};
	lvnCreateInfo.logging.enableLogging = true;
	lvnCreateInfo.logging.enableVulkanValidationLayers = true;
	lvnCreateInfo.windowapi = Lvn_WindowApi_glfw;
	lvnCreateInfo.graphicsapi = Lvn_GraphicsApi_vulkan;

	// base memory bindings for the first memory block
	std::vector<LvnMemoryBindingInfo> memoryBindings =
	{
		// { sType, count }
		{ Lvn_Stype_Buffer, 8 },
		{ Lvn_Stype_Texture, 5 },
		{ Lvn_Stype_Logger, 1 },
	};

	// block memory bindings for memory blocks created afterwards.
	// note that each sType will allocate its own memory block from then on
	std::vector<LvnMemoryBindingInfo> blockMemoryBindings =
	{
		// { sType, count }
		{ Lvn_Stype_Buffer, 4 },
		{ Lvn_Stype_Texture, 5 },
		{ Lvn_Stype_Logger, 2 },
	};

	// attach the memory bind infos here
	lvnCreateInfo.memoryInfo.memAllocMode = Lvn_MemAllocMode_MemPool;
	lvnCreateInfo.memoryInfo.pMemoryBindings = memoryBindings.data();
	lvnCreateInfo.memoryInfo.memoryBindingCount = memoryBindings.size();
	lvnCreateInfo.memoryInfo.pBlockMemoryBindings = blockMemoryBindings.data();
	lvnCreateInfo.memoryInfo.blockMemoryBindingCount = blockMemoryBindings.size();

	lvn::createContext(&lvnCreateInfo);


	LvnLoggerCreateInfo loggerCreateInfo{};
	loggerCreateInfo.loggerName = "logger";
	loggerCreateInfo.level = Lvn_LogLevel_None;
	loggerCreateInfo.format = "[%T] [%l]: %v%$";


	// we create two loggers which fills the base memory block to its capacity
	LvnLogger *l1, *l2;
	lvn::createLogger(&l1, &loggerCreateInfo);
	lvn::createLogger(&l2, &loggerCreateInfo);


	// when we create more loggers, levikno will allocate another memory block designated only for logger objects
	// the memory block size is based on the count we passed in the block memory bindings during initial config
	LvnLogger *l3, *l4;
	lvn::createLogger(&l3, &loggerCreateInfo);
	lvn::createLogger(&l4, &loggerCreateInfo);


	// destroying an object returns its allocated memory back to the memory pool again for further use
	lvn::destroyLogger(l1);


	// we specified 2 loggers for the count of each new memory block so leviknow
	// will have to allocate another memory block again for 2 new loggers
	LvnLogger *l5, *l6;
	lvn::createLogger(&l5, &loggerCreateInfo);
	lvn::createLogger(&l6, &loggerCreateInfo);


	// by the end, the memory layout of Logger object should look like this:
	// +----+----+          +----+----+          +----+----+
	// | l5 | l2 | -------> | l3 | l4 | -------> | l6 |    |
	// +----+----+          +----+----+          +----+----+

	// NOTE: we destroyed logger l1 which was at the first memory index, then
	//       logger l5 was created filling in the empty space at the first index


	// it is still adviced to call the destroy functions for each object before
	// terminating the context to ensure all resources are cleaned up that were allocated by other APIs
	lvn::destroyLogger(l2);
	lvn::destroyLogger(l3);
	lvn::destroyLogger(l4);
	lvn::destroyLogger(l5);
	lvn::destroyLogger(l6);

	lvn::terminateContext();
}
