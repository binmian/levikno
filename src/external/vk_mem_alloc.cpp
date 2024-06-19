#include "levikno.h"

#define VMA_IMPLEMENTATION

#ifdef LVN_CONFIG_DEBUG
	#define VMA_ASSERT(expr) (static_cast<bool>(expr) ? void(0) : LVN_CORE_ERROR("[VMA]: " #expr))
#endif

#include "vk_mem_alloc.h"
