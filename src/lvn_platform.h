#ifndef HG_LVN_PLATFORM_H
#define HG_LVN_PLATFORM_H


#include "lvn_config.h"

struct LvnGraphicsContext;

namespace lvn
{
    LvnResult initWindowApiFuncs(LvnGraphicsContext* ctx);
    void      terminateWindowApiFuncs(LvnGraphicsContext* ctx);

    LvnResult initGraphicsApiFuncs(LvnGraphicsContext* ctx);
    void      terminateGraphicsApiFuncs(LvnGraphicsContext* ctx);
}

#endif
