#include "levikno.h"
#define MINIAUDIO_IMPLEMENTATION
#define MA_MALLOC(sz)      LVN_MALLOC((sz))
#define MA_REALLOC(p, sz)  LVN_REALLOC((p), (sz))
#define MA_FREE(p)         LVN_FREE((p))
#include "miniaudio.h"
