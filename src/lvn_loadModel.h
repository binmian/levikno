#ifndef HG_LEVIKNO_LOAD_MODEL_H
#define HG_LEVIKNO_LOAD_MODEL_H

#include "levikno_internal.h"

namespace lvn
{
	LvnModel loadGltfModel(const char* filepath);
	LvnModel loadGlbModel(const char* filepath);
}

#endif
