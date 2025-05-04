#include "levikno.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_SIMD
#define STBI_MALLOC(sz)         LVN_MALLOC(sz)
#define STBI_REALLOC(p,newsz)   LVN_REALLOC(p,newsz)
#define STBI_FREE(p)            LVN_FREE(p)
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBIW_MALLOC(sz)        LVN_MALLOC(sz)
#define STBIW_REALLOC(p,newsz)  LVN_REALLOC(p,newsz)
#define STBIW_FREE(p)           LVN_FREE(p)
#include "stb_image_write.h"

// #define STB_TRUETYPE_IMPLEMENTATION
// #define STBTT_malloc(x,u)  ((void)(u),LVN_MALLOC(x))
// #define STBTT_free(x,u)    ((void)(u),LVN_FREE(x))
// #include "stb_truetype.h"
