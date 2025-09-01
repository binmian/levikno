#include "levikno.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_SIMD
#define STBI_MALLOC(sz)         lvn::memAlloc(sz)
#define STBI_REALLOC(p,newsz)   lvn::memRealloc(p,newsz)
#define STBI_FREE(p)            lvn::memFree(p)
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBIW_MALLOC(sz)        lvn::memAlloc(sz)
#define STBIW_REALLOC(p,newsz)  lvn::memRealloc(p,newsz)
#define STBIW_FREE(p)           lvn::memFree(p)
#include "stb_image_write.h"
