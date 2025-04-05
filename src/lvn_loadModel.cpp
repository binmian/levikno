#include "lvn_loadModel.h"

#include "json.h"
#include "mikktspace.h"

namespace nlm = nlohmann;

enum LvnFileType
{
	Lvn_FileType_Gltf,
	Lvn_FileType_Glb,
};

namespace lvn
{

namespace gltfs
{
	struct GLTFAccessor
	{
		int          bufferView;
		uint32_t     byteOffest;
		int          componentType;
		int          count;
		std::string  type;
		LvnVec3      min;
		LvnVec3      max;
	};

	struct GLTFBufferView
	{
		int buffer;
		uint32_t byteLength;
		uint32_t byteOffset;
	};

	struct GLTFTextureInfo
	{
		int index;
	};

	struct GLTFPbrMetalicRoughness
	{
		LvnVec4 baseColorFactor;
		float metallicFactor;
		float roughnessFactor;
		GLTFTextureInfo baseColorTexture;
		GLTFTextureInfo metallicRoughnessTexture;
	};

	struct GLTFMatrial
	{
		GLTFPbrMetalicRoughness pbrMetallicRoughness;
		GLTFTextureInfo normalTexture;
		GLTFTextureInfo occlusionTexture;
		GLTFTextureInfo emissiveTexture;
		LvnVec3 emissiveFactor;
		std::string alphaMode;
		float alphaCutoff;
		bool doubleSided;
	};

	struct GLTFAnimationSampler
	{
		int input, output;
		std::string interpolation;
	};

	struct GLTFAnimationTarget
	{
		int node;
		std::string path;
	};

	struct GLTFAnimationChannel
	{
		int sampler;
		GLTFAnimationTarget target;
	};

	struct GLTFAnimation
	{
		std::vector<GLTFAnimationSampler> samplers;
		std::vector<GLTFAnimationChannel> channels;
	};

	struct GLTFTangentCalcInfo
	{
		std::vector<LvnVec4> outTangents;
		std::vector<LvnVec3> positions;
		std::vector<LvnVec3> normals;
		std::vector<LvnVec2> texUVs;
		std::vector<uint32_t> indices;
		int32_t  numFaces;
		int32_t  vertexPerFace;
	};


	struct GLTFLoadData
	{
		nlm::json JSON;
		std::string filepath;
		LvnFileType filetype;

		std::vector<std::shared_ptr<LvnNode>> nodes;
		std::vector<LvnNode*> nodeArray;
		std::vector<LvnBin> buffers;
		std::vector<GLTFAccessor> accessors;
		std::vector<GLTFBufferView> bufferViews;
		std::vector<GLTFMatrial> materials;
		std::vector<LvnImageData> images;
		std::vector<LvnSampler*> samplers;
		std::vector<LvnTexture*> textures;
		std::vector<LvnBuffer*> meshBuffers;
		std::vector<GLTFAnimation> animations;

		LvnSampler* defaultSampler;
		LvnTexture* defaultBaseColorTexture;
		LvnTexture* defaultMetalicRoughnessTexture;
		LvnTexture* defaultNormalTexture;
		LvnTexture* defaultOcclusionTexture;
		LvnTexture* defaultEmissiveTexture;
	};

	static void                      loadBuffers(GLTFLoadData* gltfData);
	static void                      loadAccessors(GLTFLoadData* gltfData);
	static void                      loadBufferViews(GLTFLoadData* gltfData);
	static void                      loadMaterials(GLTFLoadData* gltfData);
	static void                      loadImages(GLTFLoadData* gltfData);
	static void                      loadSamplers(GLTFLoadData* gltfData);
	static void                      prepareTextures(GLTFLoadData* gltfData);
	static void                      loadAnimations(GLTFLoadData* gltfData);
	static std::vector<LvnAnimation> bindAnimationsToNodes(GLTFLoadData* gltfData);
	static size_t                    getCompType(int compType);
	static LvnTextureFilter          getSamplerFilterEnum(int filter);
	static LvnTextureMode            getSamplerWrapModeEnum(int mode);
	static LvnTopologyType           getTopologyEnum(int mode);
	static LvnInterpolationMode      getInterpolationMode(std::string interpolation);
	static std::vector<LvnVec3>      calculateBitangents(const std::vector<LvnVec3>& normals, const std::vector<LvnVec4>& tangents);
	static std::vector<LvnVec4>      calculateTangents(GLTFTangentCalcInfo* calcInfo);
	static void                      traverseNode(GLTFLoadData* gltfData, LvnNode* nextNode, int nextNodeIndex);
	static LvnMesh                   loadMesh(GLTFLoadData* gltfData, int meshIndex);
	static LvnMaterial               getMaterial(GLTFLoadData* gltfData, int meshMaterialIndex);

	static void loadBuffers(GLTFLoadData* gltfData)
	{
		nlm::json JSON = gltfData->JSON;

		gltfData->buffers.resize(JSON["buffers"].size());

		for (int i = 0; i < JSON["buffers"].size(); i++)
		{
			std::string uri = JSON["buffers"][i]["uri"];
			std::string fileDirectory = gltfData->filepath.substr(0, gltfData->filepath.find_last_of("/\\") + 1);
			std::string pathbin = fileDirectory + uri;
			
			gltfData->buffers[i] = lvn::loadFileSrcBin(pathbin.c_str());
		}
	}
	static void loadAccessors(GLTFLoadData* gltfData)
	{
		nlm::json JSON = gltfData->JSON;

		gltfData->accessors.resize(JSON["accessors"].size());

		for (int i = 0; i < JSON["accessors"].size(); i++)
		{
			gltfData->accessors[i].bufferView = JSON["accessors"][i].value("bufferView", 0);
			gltfData->accessors[i].byteOffest = JSON["accessors"][i].value("byteOffset", 0);
			gltfData->accessors[i].componentType = JSON["accessors"][i]["componentType"];
			gltfData->accessors[i].count = JSON["accessors"][i]["count"];
			gltfData->accessors[i].type = JSON["accessors"][i]["type"];

			std::vector<float> min = JSON["accessors"][i].value("min", std::vector<float>(3));
			gltfData->accessors[i].min.x = min[0];
			gltfData->accessors[i].min.y = min[1];
			gltfData->accessors[i].min.z = min[2];
			std::vector<float> max = JSON["accessors"][i].value("max", std::vector<float>(3));
			gltfData->accessors[i].max.x = max[0];
			gltfData->accessors[i].max.y = max[1];
			gltfData->accessors[i].max.z = max[2];
		}
	}
	static void loadBufferViews(GLTFLoadData* gltfData)
	{
		nlm::json JSON = gltfData->JSON;

		gltfData->bufferViews.resize(JSON["bufferViews"].size());

		for (int i = 0; i < JSON["bufferViews"].size(); i++)
		{
			gltfData->bufferViews[i].buffer = JSON["bufferViews"][i]["buffer"];
			gltfData->bufferViews[i].byteLength = JSON["bufferViews"][i]["byteLength"];
			gltfData->bufferViews[i].byteOffset = JSON["bufferViews"][i].value("byteOffset", 0);
		}
	}
	static void loadMaterials(GLTFLoadData* gltfData)
	{
		nlm::json JSON = gltfData->JSON;

		gltfData->materials.resize(JSON["materials"].size());

		for (uint32_t i = 0; i < JSON["materials"].size(); i++)
		{
			nlm::json materialNode = JSON["materials"][i];

			if (materialNode.find("pbrMetallicRoughness") != materialNode.end())
			{
				// color factors
				if (materialNode["pbrMetallicRoughness"].find("baseColorFactor") != materialNode["pbrMetallicRoughness"].end())
				{
					nlm::json colorNode = materialNode["pbrMetallicRoughness"]["baseColorFactor"];
					gltfData->materials[i].pbrMetallicRoughness.baseColorFactor = LvnVec4(colorNode[0], colorNode[1], colorNode[2], colorNode[3]);
				}
				else
				{
					gltfData->materials[i].pbrMetallicRoughness.baseColorFactor = LvnVec4(1, 1, 1, 1);
				}

				// color texture
				if (materialNode["pbrMetallicRoughness"].find("baseColorTexture") != materialNode["pbrMetallicRoughness"].end())
					gltfData->materials[i].pbrMetallicRoughness.baseColorTexture.index = materialNode["pbrMetallicRoughness"]["baseColorTexture"]["index"];
				else
					gltfData->materials[i].pbrMetallicRoughness.baseColorTexture.index = -1;

				// metallic & roughness factors
				gltfData->materials[i].pbrMetallicRoughness.metallicFactor = materialNode["pbrMetallicRoughness"].value("metallicFactor", 1);
				gltfData->materials[i].pbrMetallicRoughness.roughnessFactor = materialNode["pbrMetallicRoughness"].value("roughnessFactor", 1);

				// metallic roughness texture
				if (materialNode["pbrMetallicRoughness"].find("metallicRoughnessTexture") != materialNode["pbrMetallicRoughness"].end())
					gltfData->materials[i].pbrMetallicRoughness.metallicRoughnessTexture.index = materialNode["pbrMetallicRoughness"]["metallicRoughnessTexture"]["index"];
				else
					gltfData->materials[i].pbrMetallicRoughness.metallicRoughnessTexture.index = -1;
			}

			// normal texture
			if (materialNode.find("normalTexture") != materialNode.end())
				gltfData->materials[i].normalTexture.index = materialNode["normalTexture"]["index"];
			else
				gltfData->materials[i].normalTexture.index = -1;

			// occlusion texture
			if (materialNode.find("occlusionTexture") != materialNode.end())
				gltfData->materials[i].occlusionTexture.index = materialNode["occlusionTexture"]["index"];
			else
				gltfData->materials[i].occlusionTexture.index = -1;

			// emissive texture
			if (materialNode.find("emissiveTexture") != materialNode.end())
				gltfData->materials[i].emissiveTexture.index = materialNode["emissiveTexture"]["index"];
			else
				gltfData->materials[i].emissiveTexture.index = -1;

			// emissive factor
			if (materialNode.find("emissiveFactor") != materialNode.end())
			{
				nlm::json emissiveNode = materialNode["emissiveFactor"];
				gltfData->materials[i].emissiveFactor = LvnVec3(emissiveNode[0], emissiveNode[1], emissiveNode[2]);
			}
			else
			{
				gltfData->materials[i].emissiveFactor = LvnVec3(0, 0, 0);
			}

			gltfData->materials[i].alphaMode = materialNode.value("alphaMode", "OPAQUE");
			gltfData->materials[i].alphaCutoff = materialNode.value("alphaCutoff", 0.5);
			gltfData->materials[i].doubleSided = materialNode.value("doubleSided", false);
		}
	}
	static void loadImages(GLTFLoadData* gltfData)
	{
		nlm::json JSON = gltfData->JSON;

		gltfData->images.resize(JSON["images"].size());

		if (gltfData->filetype == Lvn_FileType_Gltf)
		{
			for (uint32_t i = 0; i < JSON["images"].size(); i++)
			{
				std::string uri = JSON["images"][i]["uri"];
				std::string fileDirectory;
				if (gltfData->filetype == Lvn_FileType_Gltf) { fileDirectory = gltfData->filepath.substr(0, gltfData->filepath.find_last_of("/\\") + 1); }

				gltfData->images[i] = lvn::loadImageData((fileDirectory + uri).c_str(), 4);
			}
		}
		else if (gltfData->filetype == Lvn_FileType_Glb)
		{
			for (uint32_t i = 0; i < JSON["images"].size(); i++)
			{
				uint32_t bufferViewIndex = JSON["images"][i]["bufferView"];
				GLTFBufferView bufferView = gltfData->bufferViews[bufferViewIndex];
				LvnBin buffer = gltfData->buffers[bufferView.buffer];

				gltfData->images[i] = lvn::loadImageDataMemory(&buffer[bufferView.byteOffset], bufferView.byteLength, 4);
			}
		}
	}
	static void loadSamplers(GLTFLoadData* gltfData)
	{
		nlm::json JSON = gltfData->JSON;

		gltfData->samplers.resize(JSON["samplers"].size());

		for (uint32_t i = 0; i < JSON["samplers"].size(); i++)
		{
			LvnSamplerCreateInfo samplerCreateInfo{};
			samplerCreateInfo.magFilter = gltfs::getSamplerFilterEnum(JSON["samplers"][i]["magFilter"]);
			samplerCreateInfo.minFilter = gltfs::getSamplerFilterEnum(JSON["samplers"][i]["minFilter"]);
			samplerCreateInfo.wrapS = gltfs::getSamplerWrapModeEnum(JSON["samplers"][i]["wrapS"]);
			samplerCreateInfo.wrapT = gltfs::getSamplerWrapModeEnum(JSON["samplers"][i]["wrapT"]);

			lvn::createSampler(&gltfData->samplers[i], &samplerCreateInfo);
		}

		LvnSamplerCreateInfo samplerCreateInfo{};
		samplerCreateInfo.wrapS = Lvn_TextureMode_Repeat;
		samplerCreateInfo.wrapT = Lvn_TextureMode_Repeat;
		samplerCreateInfo.minFilter = Lvn_TextureFilter_Nearest;
		samplerCreateInfo.magFilter = Lvn_TextureFilter_Nearest;

		lvn::createSampler(&gltfData->defaultSampler, &samplerCreateInfo);
		gltfData->samplers.push_back(gltfData->defaultSampler);
	}
	static void prepareTextures(GLTFLoadData* gltfData)
	{
		nlm::json JSON = gltfData->JSON;

		gltfData->textures.reserve(JSON["textures"].size() + 4); // add in case of default textures
		gltfData->textures.resize(JSON["textures"].size());
	}
	static size_t getCompType(int compType)
	{
		switch (compType)
		{
			case 5120: { return sizeof(int8_t); }
			case 5121: { return sizeof(uint8_t); }
			case 5122: { return sizeof(int16_t); }
			case 5123: { return sizeof(uint16_t); }
			case 5125: { return sizeof(uint32_t); }
			case 5126: { return sizeof(float); }

			default: { LVN_CORE_ERROR("unknown component type: %d", compType); return 0; }
		}
	}
	static void loadAnimations(GLTFLoadData* gltfData)
	{
		nlm::json JSON = gltfData->JSON;

		gltfData->animations.resize(JSON["animations"].size());

		for (uint32_t i = 0; i < JSON["animations"].size(); i++)
		{
			GLTFAnimation animation{};

			nlm::json samplerNode = JSON["animations"][i]["samplers"];
			nlm::json channelNode = JSON["animations"][i]["channels"];

			animation.samplers.resize(samplerNode.size());
			for (uint32_t j = 0; j < samplerNode.size(); j++)
			{
				animation.samplers[j].input = samplerNode[j]["input"];
				animation.samplers[j].output = samplerNode[j]["output"];
				animation.samplers[j].interpolation = samplerNode[j].value("interpolation", "LINEAR");
			}

			animation.channels.resize(channelNode.size());
			for (uint32_t j = 0; j < channelNode.size(); j++)
			{
				animation.channels[j].sampler = channelNode[j]["sampler"];
				animation.channels[j].target.node = channelNode[j]["target"]["node"];
				animation.channels[j].target.path = channelNode[j]["target"]["path"];
			}

			gltfData->animations[i] = animation;
		}
	}
	static std::vector<LvnAnimation> bindAnimationsToNodes(GLTFLoadData* gltfData)
	{
		std::vector<LvnAnimation> animations(gltfData->animations.size());

		for (uint32_t i = 0; i < gltfData->animations.size(); i++)
		{
			animations[i].currentTime = 0.0f;
			animations[i].start = 0.0f;
			animations[i].end = 0.0f;
			animations[i].channels.resize(gltfData->animations[i].channels.size());

			// set the animation start and end points to the first values in the buffer
			if (!gltfData->animations[i].channels.empty())
			{
				const GLTFAnimationChannel channel = gltfData->animations[i].channels[0];
				const GLTFAnimationSampler sampler = gltfData->animations[i].samplers[channel.sampler];

				GLTFAccessor accessor = gltfData->accessors[sampler.input];
				GLTFBufferView bufferView = gltfData->bufferViews[accessor.bufferView];
				LvnBin buffer = gltfData->buffers[bufferView.buffer];

				uint32_t beginningOfData = accessor.byteOffest + bufferView.byteOffset;

				animations[i].start = *reinterpret_cast<float*>(&buffer[beginningOfData]);
				animations[i].end = *reinterpret_cast<float*>(&buffer[beginningOfData] + (accessor.count - 1) * sizeof(float));
			}

			// bind the channels, samplers, input, output
			for (uint32_t j = 0; j < gltfData->animations[i].channels.size(); j++)
			{
				const GLTFAnimationChannel channel = gltfData->animations[i].channels[j];
				const GLTFAnimationSampler sampler = gltfData->animations[i].samplers[channel.sampler];

				if (channel.target.path == "translation")
					animations[i].channels[j].path = Lvn_AnimationPath_Translation;
				if (channel.target.path == "rotation")
					animations[i].channels[j].path = Lvn_AnimationPath_Rotation;
				if (channel.target.path == "scale")
					animations[i].channels[j].path = Lvn_AnimationPath_Scale;

				animations[i].channels[j].node = gltfData->nodeArray[channel.target.node];
				animations[i].channels[j].interpolation = gltfs::getInterpolationMode(sampler.interpolation);

				// sampler input (keyframes)
				GLTFAccessor accessor = gltfData->accessors[sampler.input];
				GLTFBufferView bufferView = gltfData->bufferViews[accessor.bufferView];
				LvnBin buffer = gltfData->buffers[bufferView.buffer];

				uint32_t beginningOfData = accessor.byteOffest + bufferView.byteOffset;

				animations[i].channels[j].keyFrames.resize(accessor.count);
				for (uint32_t k = 0; k < accessor.count; k++)
				{
					// adjust animation start and end times
					animations[i].channels[j].keyFrames[k] = *reinterpret_cast<float*>(&buffer[beginningOfData] + k * sizeof(float));
					if (animations[i].channels[j].keyFrames[k] < animations[i].start)
						animations[i].start = animations[i].channels[j].keyFrames[k];
					if (animations[i].channels[j].keyFrames[k] > animations[i].end)
						animations[i].end = animations[i].channels[j].keyFrames[k];
				}

				// sampler outputs (translations/rotations/scale)
				accessor = gltfData->accessors[sampler.output];
				bufferView = gltfData->bufferViews[accessor.bufferView];
				buffer = gltfData->buffers[bufferView.buffer];
				beginningOfData = accessor.byteOffest + bufferView.byteOffset;

				animations[i].channels[j].outputs.resize(accessor.count);
				if (accessor.type == "VEC3")
					for (uint32_t k = 0; k < accessor.count; k++)
						animations[i].channels[j].outputs[k] = LvnVec4(*reinterpret_cast<LvnVec3*>(&buffer[beginningOfData] + k * 3 * sizeof(float)), 0.0f);
				else if (accessor.type == "VEC4")
					for (uint32_t k = 0; k < accessor.count; k++)
						animations[i].channels[j].outputs[k] = *reinterpret_cast<LvnVec4*>(&buffer[beginningOfData] + k * 4 * sizeof(float));
			}
		}

		return animations;
	}
	static LvnTextureFilter getSamplerFilterEnum(int filter)
	{
		switch (filter)
		{
			case 9728: { return Lvn_TextureFilter_Nearest; }
			case 9729: { return Lvn_TextureFilter_Linear; }
			case 9984: { return Lvn_TextureFilter_Nearest; }
			case 9985: { return Lvn_TextureFilter_Nearest; }
			case 9986: { return Lvn_TextureFilter_Linear; }
			case 9987: { return Lvn_TextureFilter_Linear; }

			default: { LVN_CORE_ERROR("unknown sampler filter type: %d", filter); return Lvn_TextureFilter_Nearest; }
		}
	}
	static LvnTextureMode getSamplerWrapModeEnum(int mode)
	{
		switch (mode)
		{
			case 10497: { return Lvn_TextureMode_Repeat; }
			case 33071: { return Lvn_TextureMode_ClampToEdge; }
			case 33648: { return Lvn_TextureMode_MirrorRepeat; }

			default: { LVN_CORE_ERROR("unknown sampler wrap type: %d", mode); return Lvn_TextureMode_Repeat; }
		}
	}
	static LvnTopologyType getTopologyEnum(int mode)
	{
		switch (mode)
		{
			case 0: { return Lvn_TopologyType_Point; }
			case 1: { return Lvn_TopologyType_Line; }
			case 3: { return Lvn_TopologyType_LineStrip; }
			case 4: { return Lvn_TopologyType_Triangle; }
			case 5: { return Lvn_TopologyType_TriangleStrip; }

			default: { LVN_CORE_ERROR("unknown topology mode type: %d", mode); return Lvn_TopologyType_Triangle; }
		}
	}
	static LvnInterpolationMode getInterpolationMode(std::string interpolation)
	{
		if (interpolation == "STEP")
			return Lvn_InterpolationMode_Step;
		else if (interpolation == "LINEAR")
			return Lvn_InterpolationMode_Linear;

		LVN_CORE_ERROR("unknown interpolation type: %s", interpolation.c_str());
		return Lvn_InterpolationMode_Step;
	}
	static std::vector<LvnVec3> calculateBitangents(const std::vector<LvnVec3>& normals, const std::vector<LvnVec4>& tangents)
	{
		std::vector<LvnVec3> bitangents(normals.size());
		for (uint32_t i = 0; i < normals.size(); i++)
		{
			bitangents[i] = lvn::normalize(lvn::cross(normals[i], LvnVec3(tangents[i])) * tangents[i].w);
		}

		return bitangents;
	}
	static std::vector<LvnVec4> calculateTangents(GLTFTangentCalcInfo* calcInfo)
	{
		SMikkTSpaceInterface iface{};
		SMikkTSpaceContext context{};

		iface.m_getNumFaces = [](const SMikkTSpaceContext* context) -> int
		{
			GLTFTangentCalcInfo* calcInfo = static_cast<GLTFTangentCalcInfo*>(context->m_pUserData);
			return calcInfo->numFaces;
		};
		iface.m_getNumVerticesOfFace = [](const SMikkTSpaceContext* context, const int iFace) -> int
		{
			GLTFTangentCalcInfo* calcInfo = static_cast<GLTFTangentCalcInfo*>(context->m_pUserData);
			return calcInfo->vertexPerFace;
		};
		iface.m_getPosition = [](const SMikkTSpaceContext* context, float* outpos, const int iFace, const int iVert)
		{
			GLTFTangentCalcInfo* calcInfo = static_cast<GLTFTangentCalcInfo*>(context->m_pUserData);

			uint32_t indicesIndex = iFace * calcInfo->vertexPerFace + iVert;
			uint32_t index = calcInfo->indices[indicesIndex];
			LvnVec3 position = calcInfo->positions[index];

			outpos[0] = position.x;
			outpos[1] = position.y;
			outpos[2] = position.z;
		};
		iface.m_getNormal = [](const SMikkTSpaceContext* context, float* outnormal, const int iFace, const int iVert)
		{
			GLTFTangentCalcInfo* calcInfo = static_cast<GLTFTangentCalcInfo*>(context->m_pUserData);

			uint32_t indicesIndex = iFace * calcInfo->vertexPerFace + iVert;
			uint32_t index = calcInfo->indices[indicesIndex];
			LvnVec3 normal = calcInfo->normals[index];

			outnormal[0] = normal.x;
			outnormal[1] = normal.y;
			outnormal[2] = normal.z;
		};
		iface.m_getTexCoord = [](const SMikkTSpaceContext* context, float* outuv, const int iFace, const int iVert)
		{
			GLTFTangentCalcInfo* calcInfo = static_cast<GLTFTangentCalcInfo*>(context->m_pUserData);

			uint32_t indicesIndex = iFace * calcInfo->vertexPerFace + iVert;
			uint32_t index = calcInfo->indices[indicesIndex];
			LvnVec2 texUV = calcInfo->texUVs[index];

			outuv[0] = texUV.x;
			outuv[1] = texUV.y;
		};
		iface.m_setTSpaceBasic = [](const SMikkTSpaceContext *context, const float *tangentu, const float fSign, const int iFace, const int iVert)
		{
			GLTFTangentCalcInfo* calcInfo = static_cast<GLTFTangentCalcInfo*>(context->m_pUserData);

			uint32_t indicesIndex = iFace * calcInfo->vertexPerFace + iVert;
			uint32_t index = calcInfo->indices[indicesIndex];
			LvnVec4* tangent = &calcInfo->outTangents[index];

			tangent->x = tangentu[0];
			tangent->y = tangentu[1];
			tangent->z = tangentu[2];
			tangent->w = fSign;
		};

		context.m_pInterface = &iface;
		context.m_pUserData = calcInfo;

		calcInfo->outTangents.resize(calcInfo->positions.size());

		genTangSpaceDefault(&context);

		return calcInfo->outTangents;
	}
	static void traverseNode(GLTFLoadData* gltfData, LvnNode* nextNode, int nextNodeIndex)
	{
		nlm::json JSON = gltfData->JSON;
		nlm::json node = JSON["nodes"][nextNodeIndex];

		LvnVec3 translationVec = LvnVec3(0.0f, 0.0f, 0.0f);
		if (node.find("translation") != node.end())
		{
			for (uint32_t i = 0; i < node["translation"].size(); i++)
				translationVec[i] = node["translation"][i];
		}

		LvnQuat rotationQuat = LvnQuat(1.0f, 0.0f, 0.0f, 0.0f); // lvn::quat has order (w, x, y, z)
		if (node.find("rotation") != node.end())
		{
			rotationQuat[0] = node["rotation"][3]; // w
			rotationQuat[1] = node["rotation"][0]; // x
			rotationQuat[2] = node["rotation"][1]; // y
			rotationQuat[3] = node["rotation"][2]; // z
		}

		LvnVec3 scaleVec = LvnVec3(1.0f, 1.0f, 1.0f);
		if (node.find("scale") != node.end())
		{
			for (uint32_t i = 0; i < node["scale"].size(); i++)
				scaleVec[i] = node["scale"][i];
		}

		LvnMat4 matrix = LvnMat4(1.0f);
		if (node.find("matrix") != node.end())
		{
			float matValues[16];
			for (uint32_t i = 0; i < node["matrix"].size(); i++)
				matValues[i] = (node["matrix"][i]);
			memcpy(&matrix[0][0], matValues, 16 * sizeof(float));
		}

		nextNode->transform.translation = translationVec;
		nextNode->transform.rotation = rotationQuat;
		nextNode->transform.scale = scaleVec;

		LvnMat4 translation = lvn::translate(LvnMat4(1.0f), translationVec);
		LvnMat4 rotate = lvn::quatToMat4(rotationQuat);
		LvnMat4 scale = lvn::scale(LvnMat4(1.0f), scaleVec);
		nextNode->matrix = matrix * translation * rotate * scale;

		if (node.find("mesh") != node.end())
		{
			nextNode->mesh = gltfs::loadMesh(gltfData, node["mesh"]);
		}

		// Check if the node has children
		if (node.find("children") != node.end())
		{
			nextNode->children.resize(node["children"].size());

			for (uint32_t i = 0; i < node["children"].size(); i++)
			{
				nextNode->children[i] = std::make_shared<LvnNode>();
				nextNode->children[i]->parent = nextNode;

				int nodeIndex = node["children"][i];
				gltfData->nodeArray[nodeIndex] = nextNode->children[i].get();
				gltfs::traverseNode(gltfData, nextNode->children[i].get(), nodeIndex);
			}
		}
	}
	static LvnMesh loadMesh(GLTFLoadData* gltfData, int meshIndex)
	{
		nlm::json JSON = gltfData->JSON;

		std::vector<LvnPrimitive> meshPrimitives(JSON["meshes"][meshIndex]["primitives"].size());
		for (uint32_t i = 0; i < JSON["meshes"][meshIndex]["primitives"].size(); i++)
		{
			nlm::json primitiveNode = JSON["meshes"][meshIndex]["primitives"][i];

			int posIndex      = primitiveNode["attributes"]["POSITION"];
			int texIndex      = primitiveNode["attributes"].value("TEXCOORD_0", -1);
			int normalIndex   = primitiveNode["attributes"].value("NORMAL", -1);
			int tangentIndex  = primitiveNode["attributes"].value("TANGENT", -1);
			int indicesIndex  = primitiveNode.value("indices", -1);
			int materialIndex = primitiveNode.value("material", -1);

			// position
			GLTFAccessor accessor = gltfData->accessors[posIndex];
			GLTFBufferView bufferView = gltfData->bufferViews[accessor.bufferView];
			LvnBin buffer = gltfData->buffers[bufferView.buffer];

			uint32_t beginningOfData = accessor.byteOffest + bufferView.byteOffset;

			std::vector<LvnVec3> positions(accessor.count);
			for (uint32_t j = 0; j < accessor.count; j++)
				positions[j] = *reinterpret_cast<LvnVec3*>(&buffer[beginningOfData] + j * 3 * sizeof(float));

			// indices
			std::vector<uint32_t> indices;
			if (indicesIndex >= 0)
			{
				accessor = gltfData->accessors[indicesIndex];
				bufferView = gltfData->bufferViews[accessor.bufferView];
				buffer = gltfData->buffers[bufferView.buffer];

				beginningOfData = accessor.byteOffest + bufferView.byteOffset;
				size_t compType = gltfs::getCompType(accessor.componentType);

				indices.resize(accessor.count);
				for (uint32_t j = 0; j < accessor.count; j++)
				{
					memcpy(&indices[j], &buffer[beginningOfData] + j * compType, compType);
				}
			}

			// color
			LvnVec4 color = gltfData->materials[materialIndex].pbrMetallicRoughness.baseColorFactor;

			// texcoords
			std::vector<LvnVec2> texcoords;
			if (texIndex >= 0)
			{
				accessor = gltfData->accessors[texIndex];
				bufferView = gltfData->bufferViews[accessor.bufferView];
				buffer = gltfData->buffers[bufferView.buffer];

				beginningOfData = accessor.byteOffest + bufferView.byteOffset;
				size_t compType = gltfs::getCompType(accessor.componentType);

				texcoords.resize(accessor.count);
				for (uint32_t j = 0; j < accessor.count; j++)
				{
					memcpy(&texcoords[j].x, &buffer[beginningOfData] + j * 2 * compType + 0 * compType, compType);
					memcpy(&texcoords[j].y, &buffer[beginningOfData] + j * 2 * compType + 1 * compType, compType);
				}
			}
			else
			{
				texcoords.resize(positions.size(), 0);
			}

			// normals
			std::vector<LvnVec3> normals;
			if (normalIndex >= 0)
			{
				accessor = gltfData->accessors[normalIndex];
				bufferView = gltfData->bufferViews[accessor.bufferView];
				buffer = gltfData->buffers[bufferView.buffer];

				beginningOfData = accessor.byteOffest + bufferView.byteOffset;

				normals.resize(accessor.count);
				for (uint32_t j = 0; j < accessor.count; j++)
					normals[j] = *reinterpret_cast<LvnVec3*>(&buffer[beginningOfData] + j * 3 * sizeof(float));
			}
			else
			{
				normals.resize(positions.size(), 0);
			}

			// tangents
			std::vector<LvnVec4> tangents;
			if (tangentIndex >= 0)
			{
				accessor = gltfData->accessors[tangentIndex];
				bufferView = gltfData->bufferViews[accessor.bufferView];
				buffer = gltfData->buffers[bufferView.buffer];

				beginningOfData = accessor.byteOffest + bufferView.byteOffset;

				tangents.resize(accessor.count);
				for (uint32_t j = 0; j < accessor.count; j++)
					tangents[j] = *reinterpret_cast<LvnVec4*>(&buffer[beginningOfData] + j * 4 * sizeof(float));
			}
			else if (primitiveNode.value("mode", 4) >= 4 && posIndex >= 0 && normalIndex >= 0 && texIndex >= 0) // calculate tangents
			{
				GLTFTangentCalcInfo calcInfo{};
				calcInfo.positions = positions;
				calcInfo.normals = normals;
				calcInfo.texUVs = texcoords;
				calcInfo.indices = indices;
				calcInfo.vertexPerFace = 3;
				calcInfo.numFaces = indices.size() / 3;

				tangents = gltfs::calculateTangents(&calcInfo);
			}
			else // mesh has no tangents
			{
				tangents.resize(positions.size(), 0);
			}

			// bitangents
			std::vector<LvnVec3> bitangents;
			if (normalIndex >= 0 && tangentIndex >= 0)
			{
				bitangents = gltfs::calculateBitangents(normals, tangents);
			}
			else
			{
				bitangents.resize(positions.size(), 0);
			}

			// combine vertex data
			std::vector<LvnVertex> vertices;
			vertices.resize(positions.size());

			for (uint32_t j = 0; j < positions.size(); j++)
			{
				vertices[j] = LvnVertex {
					positions[j],
					color,
					texcoords[j],
					normals[j],
					LvnVec3(tangents[j]),
					bitangents[j],
				};
			}

			// create buffer

			LvnVertexAttribute meshVertexAttributes[] =
			{
				{ 0, 0, Lvn_VertexDataType_Vec3f, 0 },                   // pos
				{ 0, 1, Lvn_VertexDataType_Vec4f, 3 * sizeof(float) },   // color
				{ 0, 2, Lvn_VertexDataType_Vec2f, 7 * sizeof(float) },   // texUV
				{ 0, 3, Lvn_VertexDataType_Vec3f, 9 * sizeof(float) },   // normal
				{ 0, 4, Lvn_VertexDataType_Vec3f, 12 * sizeof(float) },  // tangent
				{ 0, 5, Lvn_VertexDataType_Vec3f, 15 * sizeof(float) },  // bitangent
			};

			LvnVertexBindingDescription meshVertexBindingDescription;
			meshVertexBindingDescription.binding = 0;
			meshVertexBindingDescription.stride = sizeof(LvnVertex);

			LvnBufferCreateInfo bufferCreateInfo{};
			bufferCreateInfo.type = Lvn_BufferType_Vertex;
			if (!indices.empty()) bufferCreateInfo.type |= Lvn_BufferType_Index;
			bufferCreateInfo.vertexAttributeCount = sizeof(meshVertexAttributes) / sizeof(LvnVertexAttribute);
			bufferCreateInfo.pVertexAttributes = meshVertexAttributes;
			bufferCreateInfo.vertexBindingDescriptionCount = 1;
			bufferCreateInfo.pVertexBindingDescriptions = &meshVertexBindingDescription;
			bufferCreateInfo.vertexBufferSize = vertices.size() * sizeof(LvnVertex);
			bufferCreateInfo.pVertices = vertices.data();
			bufferCreateInfo.indexBufferSize = indices.size() * sizeof(uint32_t);
			bufferCreateInfo.pIndices = indices.data();

			LvnBuffer* meshBuffer;
			lvn::createBuffer(&meshBuffer, &bufferCreateInfo);
			meshPrimitives[i].buffer = meshBuffer;
			gltfData->meshBuffers.push_back(meshBuffer);

			meshPrimitives[i].vertexCount = vertices.size();
			meshPrimitives[i].indexCount = indices.size();

			// material textures
			if (materialIndex >= 0)
			{
				meshPrimitives[i].material = gltfs::getMaterial(gltfData, materialIndex);
			}
			else
			{
				meshPrimitives[i].material.albedo = gltfData->defaultBaseColorTexture;
				meshPrimitives[i].material.metallicRoughnessOcclusion = gltfData->defaultMetalicRoughnessTexture;
				meshPrimitives[i].material.normal = gltfData->defaultNormalTexture;
				meshPrimitives[i].material.emissive = gltfData->defaultEmissiveTexture;
				meshPrimitives[i].material.baseColorFactor = LvnVec4(1, 1, 1, 1);
				meshPrimitives[i].material.metallicFactor = 1.0f;
				meshPrimitives[i].material.roughnessFactor = 1.0f;
				meshPrimitives[i].material.emissiveFactor = LvnVec3(0, 0, 0);
				meshPrimitives[i].material.doubleSided = false;
			}

			meshPrimitives[i].topology = gltfs::getTopologyEnum(primitiveNode.value("mode", 4));
		}

		LvnMesh mesh{};
		mesh.primitives = std::move(meshPrimitives);

		return mesh;
	}
	static LvnMaterial getMaterial(GLTFLoadData* gltfData, int meshMaterialIndex)
	{
		nlm::json JSON = gltfData->JSON;
		GLTFMatrial gltfMaterial = gltfData->materials[meshMaterialIndex];

		LvnMaterial material{};

		std::string fileDirectory;
		if (gltfData->filetype == Lvn_FileType_Gltf) { fileDirectory = gltfData->filepath.substr(0, gltfData->filepath.find_last_of("/\\") + 1); }

		uint32_t texSource;
		std::string texPath;

		// base color
		if (gltfMaterial.pbrMetallicRoughness.baseColorTexture.index >= 0)
		{
			LvnSampler* sampler;
			LvnImageData image{};
			uint32_t texIndex = gltfMaterial.pbrMetallicRoughness.baseColorTexture.index;

			if (gltfData->textures[texIndex] == nullptr)
			{
				// sampler
				if (JSON["textures"][texIndex].find("sampler") != JSON["textures"][texIndex].end())
					sampler = gltfData->samplers[JSON["textures"][texIndex]["sampler"]];
				else
					sampler = gltfData->defaultSampler;

				// image
				image = gltfData->images[JSON["textures"][texIndex]["source"]];

				LvnTextureSamplerCreateInfo textureCreateInfo{};
				textureCreateInfo.format = Lvn_TextureFormat_Srgb;
				textureCreateInfo.imageData = image;
				textureCreateInfo.sampler = sampler;

				lvn::createTexture(&gltfData->textures[texIndex], &textureCreateInfo);
			}

			material.albedo = gltfData->textures[texIndex];
		}
		else // no base color found, load default white color
		{
			uint8_t whiteTextureData[] = { 0xff, 0xff, 0xff, 0xff };
			LvnImageData imageData;
			imageData.pixels = LvnData<uint8_t>(whiteTextureData, sizeof(whiteTextureData) / sizeof(uint8_t));
			imageData.width = 1;
			imageData.height = 1;
			imageData.channels = 4;
			imageData.size = 4;

			LvnTextureSamplerCreateInfo baseColorCreateInfo{};
			baseColorCreateInfo.imageData = imageData;
			baseColorCreateInfo.format = Lvn_TextureFormat_Srgb;
			baseColorCreateInfo.sampler = gltfData->defaultSampler;

			if (gltfData->defaultBaseColorTexture == nullptr)
			{
				lvn::createTexture(&gltfData->defaultBaseColorTexture, &baseColorCreateInfo);
				gltfData->textures.push_back(gltfData->defaultBaseColorTexture);
			}

			material.albedo = gltfData->defaultBaseColorTexture;
		}

		// metalic roughness
		if (gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0)
		{
			LvnSampler* sampler;
			LvnImageData image{};
			uint32_t texIndex = gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index;

			if (gltfData->textures[texIndex] == nullptr)
			{
				// sampler
				if (JSON["textures"][texIndex].find("sampler") != JSON["textures"][texIndex].end())
					sampler = gltfData->samplers[JSON["textures"][texIndex]["sampler"]];
				else
					sampler = gltfData->defaultSampler;

				// image
				image = gltfData->images[JSON["textures"][texIndex]["source"]];

				LvnTextureSamplerCreateInfo textureCreateInfo{};
				textureCreateInfo.format = Lvn_TextureFormat_Srgb;
				textureCreateInfo.imageData = image;
				textureCreateInfo.sampler = sampler;

				lvn::createTexture(&gltfData->textures[texIndex], &textureCreateInfo);
			}

			material.metallicRoughnessOcclusion = gltfData->textures[texIndex];
		}
		else // no metalic roughness found, load default texture (0,1,0,1)
		{
			// default metalic roughness texture
			uint8_t metalicRoughnessData[4] = { 0x00, 0xff, 0x00, 0xff };
			LvnImageData imageData;
			imageData.pixels = LvnData<uint8_t>(metalicRoughnessData, sizeof(metalicRoughnessData) / sizeof(uint8_t));
			imageData.width = 1;
			imageData.height = 1;
			imageData.channels = 4;
			imageData.size = 4;

			LvnTextureSamplerCreateInfo metalicRoughnessCreateInfo{};
			metalicRoughnessCreateInfo.imageData = imageData;
			metalicRoughnessCreateInfo.format = Lvn_TextureFormat_Srgb;
			metalicRoughnessCreateInfo.sampler = gltfData->defaultSampler;

			if (gltfData->defaultMetalicRoughnessTexture == nullptr)
			{
				lvn::createTexture(&gltfData->defaultMetalicRoughnessTexture, &metalicRoughnessCreateInfo);
				gltfData->textures.push_back(gltfData->defaultMetalicRoughnessTexture);
			}

			material.metallicRoughnessOcclusion = gltfData->defaultMetalicRoughnessTexture;
		}

		// normal
		if (gltfMaterial.normalTexture.index >= 0)
		{
			LvnSampler* sampler;
			LvnImageData image{};
			uint32_t texIndex = gltfMaterial.normalTexture.index;

			if (gltfData->textures[texIndex] == nullptr)
			{
				// sampler
				if (JSON["textures"][texIndex].find("sampler") != JSON["textures"][texIndex].end())
					sampler = gltfData->samplers[JSON["textures"][texIndex]["sampler"]];
				else
					sampler = gltfData->defaultSampler;

				// image
				image = gltfData->images[JSON["textures"][texIndex]["source"]];

				LvnTextureSamplerCreateInfo textureCreateInfo{};
				textureCreateInfo.format = Lvn_TextureFormat_Unorm;
				textureCreateInfo.imageData = image;
				textureCreateInfo.sampler = sampler;

				lvn::createTexture(&gltfData->textures[texIndex], &textureCreateInfo);
			}

			material.normal = gltfData->textures[texIndex];
		}
		else // no normal found, load default normal texture
		{
			// default normal texture
			uint8_t normalTextureData[4] = { 0x80, 0x80, 0xff, 0xff };
			LvnImageData imageData;
			imageData.pixels = LvnData<uint8_t>(normalTextureData, sizeof(normalTextureData) / sizeof(uint8_t));
			imageData.width = 1;
			imageData.height = 1;
			imageData.channels = 4;
			imageData.size = 4;

			LvnTextureSamplerCreateInfo normalCreateInfo{};
			normalCreateInfo.imageData = imageData;
			normalCreateInfo.format = Lvn_TextureFormat_Unorm;
			normalCreateInfo.sampler = gltfData->defaultSampler;

			if (gltfData->defaultNormalTexture == nullptr)
			{
				lvn::createTexture(&gltfData->defaultNormalTexture, &normalCreateInfo);
				gltfData->textures.push_back(gltfData->defaultNormalTexture);
			}

			material.normal = gltfData->defaultNormalTexture;
		}

		// emissive
		if (gltfMaterial.emissiveTexture.index >= 0)
		{
			LvnSampler* sampler;
			LvnImageData image{};
			uint32_t texIndex = gltfMaterial.emissiveTexture.index;

			if (gltfData->textures[texIndex] == nullptr)
			{
				// sampler
				if (JSON["textures"][texIndex].find("sampler") != JSON["textures"][texIndex].end())
					sampler = gltfData->samplers[JSON["textures"][texIndex]["sampler"]];
				else
					sampler = gltfData->defaultSampler;

				// image
				image = gltfData->images[JSON["textures"][texIndex]["source"]];

				LvnTextureSamplerCreateInfo textureCreateInfo{};
				textureCreateInfo.format = Lvn_TextureFormat_Unorm;
				textureCreateInfo.imageData = image;
				textureCreateInfo.sampler = sampler;

				lvn::createTexture(&gltfData->textures[texIndex], &textureCreateInfo);
			}

			material.emissive = gltfData->textures[texIndex];
		}
		else // no emissive found, load default emissive texture
		{
			// default emissive texture
			uint8_t emissiveTextureData[4] = { 0x00, 0x00, 0x00, 0x00 };
			LvnImageData imageData;
			imageData.pixels = LvnData<uint8_t>(emissiveTextureData, sizeof(emissiveTextureData) / sizeof(uint8_t));
			imageData.width = 1;
			imageData.height = 1;
			imageData.channels = 4;
			imageData.size = 4;

			LvnTextureSamplerCreateInfo emissiveCreateInfo{};
			emissiveCreateInfo.imageData = imageData;
			emissiveCreateInfo.format = Lvn_TextureFormat_Unorm;
			emissiveCreateInfo.sampler = gltfData->defaultSampler;

			if (gltfData->defaultEmissiveTexture == nullptr)
			{
				lvn::createTexture(&gltfData->defaultEmissiveTexture, &emissiveCreateInfo);
				gltfData->textures.push_back(gltfData->defaultEmissiveTexture);
			}

			material.emissive = gltfData->defaultEmissiveTexture;
		}

		// factors
		material.baseColorFactor = gltfMaterial.pbrMetallicRoughness.baseColorFactor;
		material.metallicFactor = gltfMaterial.pbrMetallicRoughness.metallicFactor;
		material.roughnessFactor = gltfMaterial.pbrMetallicRoughness.roughnessFactor;
		material.emissiveFactor = gltfMaterial.emissiveFactor;
		material.doubleSided = gltfMaterial.doubleSided;

		return material;
	}

} /* namespace gltf */

LvnModel loadGltfModel(const char* filepath)
{
	gltfs::GLTFLoadData gltfData{};
	std::string jsonText = lvn::loadFileSrc(filepath);
	gltfData.JSON = nlm::json::parse(jsonText);
	gltfData.filepath = filepath;
	gltfData.filetype = Lvn_FileType_Gltf;

	nlm::json JSON = gltfData.JSON;

	if (JSON["scenes"].size() > 1)
		LVN_CORE_WARN("gltf model has more than one scene, loading mesh data from the first scene; Filepath: %s", filepath);

	gltfs::loadBuffers(&gltfData);
	gltfs::loadAccessors(&gltfData);
	gltfs::loadBufferViews(&gltfData);
	gltfs::loadMaterials(&gltfData);
	gltfs::loadImages(&gltfData);
	gltfs::loadSamplers(&gltfData);
	gltfs::prepareTextures(&gltfData);
	gltfs::loadAnimations(&gltfData);

	gltfData.nodes.resize(JSON["scenes"][0]["nodes"].size());
	gltfData.nodeArray.resize(JSON["nodes"].size());

	for (uint32_t i = 0; i < JSON["scenes"][0]["nodes"].size(); i++)
	{
		gltfData.nodes[i] = std::make_shared<LvnNode>();
		int nodeIndex = JSON["scenes"][0]["nodes"][i];
		gltfData.nodeArray[nodeIndex] = gltfData.nodes[i].get();
		gltfs::traverseNode(&gltfData, gltfData.nodes[i].get(), nodeIndex);
	}

	LvnModel model{};
	model.matrix = LvnMat4(1.0f);
	model.buffers = std::move(gltfData.meshBuffers);
	model.textures = std::move(gltfData.textures);
	model.samplers = std::move(gltfData.samplers);
	model.nodes = std::move(gltfData.nodes);
	model.animations = gltfs::bindAnimationsToNodes(&gltfData);

	return model;
}
LvnModel loadGlbModel(const char* filepath)
{
	LvnBin binData = lvn::loadFileSrcBin(filepath);

	// chunk 0 (JSON)
	uint32_t chunkLengthJson = 0;
	memcpy(&chunkLengthJson, &binData[12], sizeof(uint32_t));
	std::vector<uint8_t> chunkDataJson(&binData[20], &binData[20] + chunkLengthJson);

	gltfs::GLTFLoadData gltfData{};
	std::string jsonText = std::string(chunkDataJson.begin(), chunkDataJson.end());
	gltfData.JSON = nlm::json::parse(jsonText);
	gltfData.filepath = filepath;
	gltfData.filetype = Lvn_FileType_Glb;

	nlm::json JSON = gltfData.JSON;

	if (JSON["scenes"].size() > 1)
		LVN_CORE_WARN("gltf model has more than one scene, loading mesh data from the first scene; Filepath: %s", filepath);


	// load buffers
	uint64_t chunkOffset = 0;
	gltfData.buffers.resize(JSON["buffers"].size());
	for (int i = 0; i < JSON["buffers"].size(); i++)
	{
		// chunk 1... (Buffer)
		uint32_t chunkLengthBuffer = 0;
		memcpy(&chunkLengthBuffer, &binData[20 + chunkLengthJson + chunkOffset], sizeof(uint32_t));
		std::vector<uint8_t> chunkDataBuffer(&binData[28 + chunkLengthJson + chunkOffset], &binData[28 + chunkLengthJson + chunkOffset] + chunkLengthBuffer);

		gltfData.buffers[i] = LvnBin(chunkDataBuffer.data(), chunkDataBuffer.size());
		chunkOffset += chunkLengthBuffer + 8;
	}

	gltfs::loadAccessors(&gltfData);
	gltfs::loadBufferViews(&gltfData);
	gltfs::loadMaterials(&gltfData);
	gltfs::loadImages(&gltfData);
	gltfs::loadSamplers(&gltfData);
	gltfs::prepareTextures(&gltfData);
	gltfs::loadAnimations(&gltfData);

	gltfData.nodes.resize(JSON["scenes"][0]["nodes"].size());
	gltfData.nodeArray.resize(JSON["nodes"].size());

	for (uint32_t i = 0; i < JSON["scenes"][0]["nodes"].size(); i++)
	{
		gltfData.nodes[i] = std::make_shared<LvnNode>();
		int nodeIndex = JSON["scenes"][0]["nodes"][i];
		gltfData.nodeArray[nodeIndex] = gltfData.nodes[i].get();
		gltfs::traverseNode(&gltfData, gltfData.nodes[i].get(), nodeIndex);
	}

	LvnModel model{};
	model.matrix = LvnMat4(1.0f);
	model.buffers = std::move(gltfData.meshBuffers);
	model.textures = std::move(gltfData.textures);
	model.samplers = std::move(gltfData.samplers);
	model.nodes = std::move(gltfData.nodes);
	model.animations = gltfs::bindAnimationsToNodes(&gltfData);

	return model;
}

} /* namespace lvn */
