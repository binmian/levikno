#ifndef HG_LEVIKNO_MODEL_LOADERS_H
#define HG_LEVIKNO_MODEL_LOADERS_H

#include "levikno_internal.h"

namespace lvn
{
	// gltf/glb
	LvnModel loadGltfModel(const char* filepath);
	LvnModel loadGlbModel(const char* filepath);

	// wavefront obj
	LvnModel loadObjModel(const char* filepath);
}

#endif
