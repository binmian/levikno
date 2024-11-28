#include "lvn_loadModel.h"

#include "json.h"
#include "levikno.h"
#include "levikno_internal.h"

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
	struct LvnTextureIndexData
	{
		LvnTexture* texture;
		uint32_t index;
	};

	struct LvnSamplerIndexData
	{
		LvnSampler* sampler;
		int index;
	};

	struct gltfLoadData
	{
		nlm::json JSON;
		std::string filepath;
		LvnFileType filetype;
		LvnData<uint8_t> binData;
		std::vector<LvnMesh> meshes;
		std::vector<LvnTextureIndexData> textureData;
		std::vector<LvnSamplerIndexData> samplerData;
		std::vector<LvnTexture*> textures;
		std::vector<LvnSampler*> samplers;
		LvnSampler* defaultSampler;
	};

	struct TextureSampler
	{
		LvnTexture* texture;
		LvnSampler* sampler;
	};

	static void                    traverseNode(gltfLoadData* gltfData, uint32_t nextNode, LvnMat4 matrix);
	static void                    loadMesh(gltfLoadData* gltfData, uint32_t meshIndex, lvn::mat4 matrix);
	static LvnData<uint8_t>        getData(nlm::json JSON, const char* filepath);
	static void                    getFloats(gltfLoadData* gltfData, nlm::json accessor, uint32_t* beginningOfData, uint32_t* count, uint32_t* numType);
	static LvnVec4                 getColors(nlm::json material);
	static std::vector<uint32_t>   getIndices(gltfLoadData* gltfData, nlm::json accessor);
	static TextureSampler          getTexture(gltfLoadData* gltfData, nlm::json texInd);
	static LvnMaterial             getMaterial(gltfLoadData* gltfData, nlm::json accessor);
	static LvnTextureFilter        getTexFilter(uint32_t filter);
	static std::vector<LvnVec3>    calculateBitangents(const std::vector<LvnVec3>& normals, const std::vector<LvnVec4>& tangents);

	static void traverseNode(gltfLoadData* gltfData, uint32_t nextNode, LvnMat4 matrix)
	{
		nlm::json JSON = gltfData->JSON;
		nlm::json node = JSON["nodes"][nextNode];

		LvnVec3 nodeTranslation = LvnVec3(0.0f, 0.0f, 0.0f);
		if (node.find("translation") != node.end())
		{
			for (uint32_t i = 0; i < node["translation"].size(); i++)
				nodeTranslation[i] = node["translation"][i];
		}

		lvn::quat nodeRotation = lvn::quat(1.0f, 0.0f, 0.0f, 0.0f); // lvn::quat has order (w, x, y, z)
		if (node.find("rotation") != node.end())
		{
			nodeRotation[0] = node["rotation"][3]; // w
			nodeRotation[1] = node["rotation"][0]; // x
			nodeRotation[2] = node["rotation"][1]; // y
			nodeRotation[3] = node["rotation"][2]; // z
		}

		LvnVec3 nodeScale = LvnVec3(1.0f, 1.0f, 1.0f);
		if (node.find("scale") != node.end())
		{
			for (uint32_t i = 0; i < node["scale"].size(); i++)
				nodeScale[i] = node["scale"][i];
		}

		lvn::mat4 nodeMatrix = lvn::mat4(1.0f);
		if (node.find("matrix") != node.end())
		{
			float matValues[16];
			for (uint32_t i = 0; i < node["matrix"].size(); i++)
				matValues[i] = (node["matrix"][i]);
			memcpy(&nodeMatrix[0][0], matValues, 16 * sizeof(float));
		}

		lvn::mat4 translation = lvn::mat4(1.0f);
		lvn::mat4 rotate = lvn::mat4(1.0f);
		lvn::mat4 scale = lvn::mat4(1.0f);

		translation = lvn::translate(translation, nodeTranslation);
		rotate = lvn::quatToMat4(nodeRotation);
		scale = lvn::scale(scale, nodeScale);

		lvn::mat4 nextNodeMatrix = matrix * nodeMatrix * translation * rotate * scale;

		if (node.find("mesh") != node.end())
		{
			loadMesh(gltfData, node["mesh"], nextNodeMatrix);
		}

		// Check if the node has children
		if (node.find("children") != node.end())
		{
			for (uint32_t i = 0; i < node["children"].size(); i++)
				traverseNode(gltfData, node["children"][i], nextNodeMatrix);
		}
	}

	static void loadMesh(gltfLoadData* gltfData, uint32_t meshIndex, lvn::mat4 matrix)
	{
		nlm::json JSON = gltfData->JSON;

		for (uint32_t i = 0; i < JSON["meshes"][meshIndex]["primitives"].size(); i++)
		{
			int meshPosIndex = JSON["meshes"][meshIndex]["primitives"][i]["attributes"]["POSITION"];
			int meshTexIndex = JSON["meshes"][meshIndex]["primitives"][i]["attributes"].value("TEXCOORD_0", -1);
			int meshNormalIndex = JSON["meshes"][meshIndex]["primitives"][i]["attributes"].value("NORMAL", -1);
			int meshTangentIndex = JSON["meshes"][meshIndex]["primitives"][i]["attributes"].value("TANGENT", -1);
			int meshIndicesIndex = JSON["meshes"][meshIndex]["primitives"][i]["indices"];
			int meshMaterialIndex = JSON["meshes"][meshIndex]["primitives"][i]["material"];


			// get Pos mesh data
			uint32_t beginningOfData, count, numType;
			gltfs::getFloats(gltfData, JSON["accessors"][meshPosIndex], &beginningOfData, &count, &numType);

			std::vector<LvnVec3> position(count);

			switch (numType)
			{
				case 1:
				{
					for (uint32_t i = 0; i < count; i++)
						position[i] = LvnVec3(*(float*)(&gltfData->binData[beginningOfData] + i * numType * sizeof(float)), 0.0f, 0.0f);
					break;
				}
				case 2:
				{
					for (uint32_t i = 0; i < count; i++)
						position[i] = LvnVec3(*(LvnVec2*)(&gltfData->binData[beginningOfData] + i * numType * sizeof(float)), 0.0f);
					break;
				}
				case 3:
				{
					for (uint32_t i = 0; i < count; i++)
						position[i] = *(LvnVec3*)(&gltfData->binData[beginningOfData] + i * numType * sizeof(float));
					break;
				}
				case 4:
				{
					for (uint32_t i = 0; i < count; i++)
						position[i] = LvnVec3(*(LvnVec4*)(&gltfData->binData[beginningOfData] + i * numType * sizeof(float)));
					break;
				}
			}

			// get color data, assuming each vertex of the mesh primitive has the same color
			LvnVec4 colors = getColors(JSON["materials"][meshMaterialIndex]);

			// get texUV mesh data
			std::vector<LvnVec2> texCoord;
			if (meshTexIndex >= 0)
			{
				gltfs::getFloats(gltfData, JSON["accessors"][meshTexIndex], &beginningOfData, &count, &numType);
				texCoord.resize(count);
				memcpy(texCoord.data(), &gltfData->binData[beginningOfData], count * numType * sizeof(float));
			}
			else
			{
				texCoord.resize(position.size(), 0);
			}

			// get normal mesh data
			std::vector<LvnVec3> normals;
			if (meshNormalIndex >= 0)
			{
				gltfs::getFloats(gltfData, JSON["accessors"][meshNormalIndex], &beginningOfData, &count, &numType);
				normals.resize(count);
				memcpy(normals.data(), &gltfData->binData[beginningOfData], count * numType * sizeof(float));
			}
			else // mesh has no normals
			{
				normals.resize(position.size(), 0);
			}

			std::vector<LvnVec4> tangents;
			if (meshTangentIndex >= 0)
			{
				gltfs::getFloats(gltfData, JSON["accessors"][meshTangentIndex], &beginningOfData, &count, &numType);
				tangents.resize(count);
				memcpy(tangents.data(), &gltfData->binData[beginningOfData], count * numType * sizeof(float));
			}
			else // mesh has no tangents
			{
				tangents.resize(position.size(), 0);
			}

			// bitangents
			std::vector<LvnVec3> bitangents;
			if (meshNormalIndex >= 0 && meshTangentIndex >= 0)
			{
				bitangents = gltfs::calculateBitangents(normals, tangents);
			}
			else
			{
				bitangents.resize(position.size(), 0);
			}

			// get Mesh Indices
			std::vector<uint32_t> indices = gltfs::getIndices(gltfData, JSON["accessors"][meshIndicesIndex]);

			// combine all mesh data and Get Model data
			std::vector<LvnVertex> vertices;
			vertices.reserve(position.size());

			for (uint32_t j = 0; j < position.size(); j++)
			{
				vertices.push_back(
					LvnVertex
					{
						position[j],    // pos
						colors,         // color
						texCoord[j],    // texUV
						normals[j],     // normal

						LvnVec3(tangents[j]),  // tangent
						bitangents[j],         // bitangent
					}
				);
			}

			// Get Textures
			LvnMaterial material = gltfs::getMaterial(gltfData, JSON["materials"][meshMaterialIndex]);


			LvnMeshCreateInfo meshCreateInfo{};

			LvnBufferCreateInfo meshBufferInfo = lvn::meshGetVertexBufferCreateInfoConfig(vertices.data(), vertices.size(), indices.data(), indices.size());
			meshCreateInfo.bufferInfo = &meshBufferInfo;
			meshCreateInfo.material = material;

			LvnMesh mesh = lvn::createMesh(&meshCreateInfo);
			lvn::meshSetMatrix(&mesh, matrix);
			mesh.vertexCount = vertices.size();
			mesh.indexCount = indices.size();

			gltfData->meshes.push_back(mesh);
		}
	}

	static LvnData<uint8_t> getData(nlm::json JSON, const char* filepath)
	{
		std::string uri = JSON["buffers"][0]["uri"];

		std::string path = std::string(filepath);
		std::string fileDirectory = path.substr(0, path.find_last_of("/\\") + 1);
		std::string pathbin = fileDirectory + uri;
		
		return lvn::loadFileSrcBin(pathbin.c_str());
	}

	static void getFloats(gltfLoadData* gltfData, nlm::json accessor, uint32_t* beginningOfData, uint32_t* count, uint32_t* numType)
	{
		LVN_CORE_ASSERT(gltfData != nullptr && beginningOfData != nullptr && count != nullptr && numType != nullptr, "getFloats function has nullptr parameters");

		nlm::json JSON = gltfData->JSON;

		uint32_t bufferViewIndex = accessor.value("bufferView", 0);
		uint32_t accByteOffset = accessor.value("byteOffset", 0);
		std::string type = accessor["type"];

		nlm::json bufferView = JSON["bufferViews"][bufferViewIndex];
		uint32_t BVbyteOffset = bufferView.value("byteOffset", 0);

		if (type == "SCALAR") { *numType = 1; }
		else if (type == "VEC2") { *numType = 2; }
		else if (type == "VEC3") { *numType = 3; }
		else if (type == "VEC4") { *numType = 4; }
		else { LVN_CORE_ERROR("unkown float type, type (%s) does not match with one of the following: VEC2, VEC3, VEC4, SCALER", type.c_str()); return; }

		*beginningOfData = accByteOffset + BVbyteOffset;
		*count = accessor["count"];
	}

	static LvnVec4 getColors(nlm::json material)
	{
		if (material["pbrMetallicRoughness"].find("baseColorFactor") != material["pbrMetallicRoughness"].end())
		{
			return LvnVec4(
				material["pbrMetallicRoughness"]["baseColorFactor"][0],    // r
				material["pbrMetallicRoughness"]["baseColorFactor"][1],    // g
				material["pbrMetallicRoughness"]["baseColorFactor"][2],    // b
				material["pbrMetallicRoughness"]["baseColorFactor"][3]);   // a
		}

		return LvnVec4(1.0f, 1.0f, 1.0f, 1.0f); // if no color factor found, return white color
	}

	static std::vector<uint32_t> getIndices(gltfLoadData* gltfData, nlm::json accessor)
	{
		nlm::json JSON = gltfData->JSON;

		uint32_t bufferViewIndex = accessor.value("bufferView", 0);
		uint32_t count = accessor["count"];
		uint32_t accByteOffset = accessor.value("byteOffset", 0);
		uint32_t componentType = accessor["componentType"];

		nlm::json bufferView = JSON["bufferViews"][bufferViewIndex];
		uint32_t bufferViewByteOffset = bufferView.value("byteOffset", 0);

		uint32_t beginningOfData = bufferViewByteOffset + accByteOffset;

		std::vector<uint32_t> indices;
		indices.reserve(count);

		if (componentType == 5125) // UNSIGNED_INT = 5125
		{
			const uint8_t* data = gltfData->binData.data();

			for (uint32_t i = beginningOfData; i < bufferViewByteOffset + accByteOffset + count * sizeof(unsigned int); i += 4)
			{
				uint8_t bytes[] = { data[i], data[i + 1], data[i + 2], data[i + 3] };
				uint32_t value;
				memcpy(&value, bytes, sizeof(uint32_t));
				indices.push_back(value);
			}
		}
		else if (componentType == 5123) // UNSIGNED_SHORT = 5123
		{
			const uint8_t* data = gltfData->binData.data();

			for (unsigned int i = beginningOfData; i < bufferViewByteOffset + accByteOffset + count * sizeof(unsigned short); i += 2)
			{
				uint8_t bytes[] = { data[i], data[i + 1] };
				uint16_t value;
				memcpy(&value, bytes, sizeof(uint16_t));
				indices.push_back((uint32_t)value);
			}
		}
		else if (componentType == 5122) // SHORT = 5122
		{
			const uint8_t* data = gltfData->binData.data();

			for (unsigned int i = beginningOfData; i < bufferViewByteOffset + accByteOffset + count * sizeof(short); i += 2)
			{
				uint8_t bytes[] = { data[i], data[i + 1] };
				int16_t value;
				memcpy(&value, bytes, sizeof(int16_t));
				indices.push_back((uint32_t)value);
			}
		}
		else // no type found
		{
			LVN_CORE_ERROR("indices component type not supported (%u), only unsigned int types are supported: '5125' (UNSIGNED_INT)", componentType);
			return {};
		}

		return indices;
	}

	static TextureSampler getTexture(gltfLoadData* gltfData, nlm::json texInd)
	{
		nlm::json JSON = gltfData->JSON;

		TextureSampler texSamp{};
		bool skipTex = false, skipSamp = false;


		// sampler
		int texSamplerIndex = texInd.value("sampler", -1);

		// model has samplers
		if (texSamplerIndex >= 0)
		{
			// find if sampler is already used before it isnt created again
			for (uint32_t i = 0; i < gltfData->samplerData.size(); i++)
			{
				if (gltfData->samplerData[i].index = texSamplerIndex)
				{
					texSamp.sampler = gltfData->samplerData[i].sampler;
					skipSamp = true;
					break;
				}
			}

			if (!skipSamp) // if model has different sampler not loaded yet
			{
				LvnSamplerCreateInfo samplerCreateInfo{};
				samplerCreateInfo.wrapMode = Lvn_TextureMode_Repeat;

				uint32_t magFilter = JSON["samplers"][texSamplerIndex]["magFilter"];
				uint32_t minFilter = JSON["samplers"][texSamplerIndex]["minFilter"];
				samplerCreateInfo.minFilter = gltfs::getTexFilter(minFilter);
				samplerCreateInfo.magFilter = gltfs::getTexFilter(magFilter);

				LvnSampler* sampler;
				lvn::createSampler(&sampler, &samplerCreateInfo);

				texSamp.sampler = sampler;

				LvnSamplerIndexData samplerIndex = { sampler, texSamplerIndex };
				gltfData->samplerData.push_back(samplerIndex);
				gltfData->samplers.push_back(sampler);
			}
		}
		else // model does not have sampler, use default sampler
		{
			texSamp.sampler = gltfData->defaultSampler;
		}


		// texture
		std::string fileDirectory;
		if (gltfData->filetype == Lvn_FileType_Gltf) { fileDirectory = gltfData->filepath.substr(0, gltfData->filepath.find_last_of("/\\") + 1); }

		uint32_t texSource;
		std::string texPath;

		texSource = texInd["source"];
		if (gltfData->filetype == Lvn_FileType_Gltf) { texPath = JSON["images"][texSource]["uri"]; }

		for (uint32_t i = 0; i < gltfData->textureData.size(); i++)
		{
			if (gltfData->textureData[i].index == texSource)
			{
				texSamp.texture = gltfData->textureData[i].texture;
				skipTex = true;
				break;
			}
		}

		if (!skipTex)
		{
			LvnTextureCreateInfo textureCreateInfo{};
			textureCreateInfo.format = Lvn_TextureFormat_Srgb;
			textureCreateInfo.sampler = texSamp.sampler;

			if (gltfData->filetype == Lvn_FileType_Gltf)
			{
				textureCreateInfo.imageData = lvn::loadImageData((fileDirectory + texPath).c_str(), 4);
			}
			else if (gltfData->filetype == Lvn_FileType_Glb)
			{
				uint32_t bufferViewIndex = JSON["images"][texSource]["bufferView"];
				nlm::json bufferview = JSON["bufferViews"][bufferViewIndex];
				uint32_t bvByteOffset = bufferview.value("byteOffset", 0);
				uint32_t bvByteLength = bufferview.value("byteLength", 0);

				textureCreateInfo.imageData = lvn::loadImageDataMemory(&gltfData->binData[bvByteOffset], bvByteLength, 4);
			}

			LvnTexture* texture;
			lvn::createTexture(&texture, &textureCreateInfo);

			LvnTextureIndexData textureIndexData = { texture, texSource };
			gltfData->textureData.push_back(textureIndexData);
			gltfData->textures.push_back(texture);

			texSamp.texture = texture;
		}

		return texSamp;
	}

	static LvnMaterial getMaterial(gltfLoadData* gltfData, nlm::json accessor)
	{
		nlm::json JSON = gltfData->JSON;


		LvnSamplerCreateInfo samplerCreateInfo{};
		samplerCreateInfo.wrapMode = Lvn_TextureMode_Repeat;
		samplerCreateInfo.minFilter = Lvn_TextureFilter_Nearest;
		samplerCreateInfo.magFilter = Lvn_TextureFilter_Nearest;

		lvn::createSampler(&gltfData->defaultSampler, &samplerCreateInfo);
		gltfData->samplers.push_back(gltfData->defaultSampler);

		LvnMaterial material{};

		std::string fileDirectory;
		if (gltfData->filetype == Lvn_FileType_Gltf) { fileDirectory = gltfData->filepath.substr(0, gltfData->filepath.find_last_of("/\\") + 1); }

		uint32_t texSource;
		std::string texPath;

		// base color
		if (accessor["pbrMetallicRoughness"].find("baseColorTexture") != accessor["pbrMetallicRoughness"].end())
		{
			nlm::json texInd = JSON["textures"][(uint32_t)accessor["pbrMetallicRoughness"]["baseColorTexture"]["index"]];
			TextureSampler texSamp = gltfs::getTexture(gltfData, texInd);
			material.albedo = texSamp.texture;
			material.albedoSampler = texSamp.sampler;
		}
		else // white texture if no texture was found
		{
			uint8_t whiteTextureData[] = { 0xff, 0xff, 0xff, 0xff };
			LvnImageData imageData;
			imageData.pixels = LvnData<uint8_t>(whiteTextureData, sizeof(whiteTextureData) / sizeof(uint8_t));
			imageData.width = 1;
			imageData.height = 1;
			imageData.channels = 4;
			imageData.size = 4;

			LvnTextureCreateInfo textureCreateInfo{};
			textureCreateInfo.imageData = imageData;
			textureCreateInfo.format = Lvn_TextureFormat_Srgb;
			textureCreateInfo.sampler = gltfData->defaultSampler;

			LvnTexture* texture;
			lvn::createTexture(&texture, &textureCreateInfo);

			material.albedo = texture;
			material.albedoSampler = gltfData->defaultSampler;
			gltfData->textures.push_back(texture);
		}

		// metalic roughness
		if (accessor["pbrMetallicRoughness"].find("metallicRoughnessTexture") != accessor["pbrMetallicRoughness"].end())
		{
			nlm::json texInd = JSON["textures"][(uint32_t)accessor["pbrMetallicRoughness"]["metallicRoughnessTexture"]["index"]];
			TextureSampler texSamp = gltfs::getTexture(gltfData, texInd);
			material.metallicRoughnessOcclusion = texSamp.texture;
			material.mroSampler = texSamp.sampler;
			
		}
		else // create default texture if no texture was found; creates a 1x1 texture, 2 channels, metalic stored in red channel, roughness stored in green channel
		{
			uint8_t whiteTextureData[4] = { 0x00, 0xcc, 0x00, 0xff };
			LvnImageData imageData;
			imageData.pixels = LvnData<uint8_t>(whiteTextureData, sizeof(whiteTextureData) / sizeof(uint8_t));
			imageData.width = 1;
			imageData.height = 1;
			imageData.channels = 4;
			imageData.size = 4;

			LvnTextureCreateInfo textureCreateInfo{};
			textureCreateInfo.imageData = imageData;
			textureCreateInfo.format = Lvn_TextureFormat_Srgb;
			textureCreateInfo.sampler = gltfData->defaultSampler;

			LvnTexture* texture;
			lvn::createTexture(&texture, &textureCreateInfo);

			material.metallicRoughnessOcclusion = texture;
			material.mroSampler = gltfData->defaultSampler;
			gltfData->textures.push_back(texture);
		}

		// normal
		if (accessor.find("normalTexture") != accessor.end())
		{
			nlm::json texInd = JSON["textures"][(uint32_t)accessor["normalTexture"]["index"]];
			TextureSampler texSamp = gltfs::getTexture(gltfData, texInd);
			material.normal = texSamp.texture;
			material.normalSampler = texSamp.sampler;
		}
		else // default normal texture if no texture was found; creates a 1x1 texture, 4 channels (0x8080ffff)
		{
			uint8_t normalTextureData[4] = { 0x80, 0x80, 0xff, 0xff };
			LvnImageData imageData;
			imageData.pixels = LvnData<uint8_t>(normalTextureData, sizeof(normalTextureData) / sizeof(uint8_t));
			imageData.width = 1;
			imageData.height = 1;
			imageData.channels = 4;
			imageData.size = 4;

			LvnTextureCreateInfo textureCreateInfo{};
			textureCreateInfo.imageData = imageData;
			textureCreateInfo.format = Lvn_TextureFormat_Unorm;
			textureCreateInfo.sampler = gltfData->defaultSampler;

			LvnTexture* texture;
			lvn::createTexture(&texture, &textureCreateInfo);

			material.normal = texture;
			material.normalSampler = gltfData->defaultSampler;
			gltfData->textures.push_back(texture);
		}

		// emissive
		if (accessor.find("emissiveTexture") != accessor.end())
		{
			nlm::json texInd = JSON["textures"][(uint32_t)accessor["emissiveTexture"]["index"]];
			TextureSampler texSamp = gltfs::getTexture(gltfData, texInd);
			material.emissive = texSamp.texture;
			material.emissiveSampler = texSamp.sampler;
		}
		else // default emissive texture if no texture was found
		{
			uint8_t emissiveTextureData[4] = { 0x00, 0x00, 0x00, 0x00 };
			LvnImageData imageData;
			imageData.pixels = LvnData<uint8_t>(emissiveTextureData, sizeof(emissiveTextureData) / sizeof(uint8_t));
			imageData.width = 1;
			imageData.height = 1;
			imageData.channels = 4;
			imageData.size = 4;

			LvnTextureCreateInfo textureCreateInfo{};
			textureCreateInfo.imageData = imageData;
			textureCreateInfo.format = Lvn_TextureFormat_Unorm;
			textureCreateInfo.sampler = gltfData->defaultSampler;

			LvnTexture* texture;
			lvn::createTexture(&texture, &textureCreateInfo);

			material.emissive = texture;
			material.emissiveSampler = gltfData->defaultSampler;
			gltfData->textures.push_back(texture);
		}

		return material;
	}

	static LvnTextureFilter getTexFilter(uint32_t filter)
	{
		switch (filter)
		{
			case 9728: { return Lvn_TextureFilter_Nearest; }
			case 9729: { return Lvn_TextureFilter_Linear; }

			default:
			{
				return Lvn_TextureFilter_Nearest;
			}
		}
	}

	static std::vector<LvnVec3> calculateBitangents(const std::vector<LvnVec3>& normals, const std::vector<LvnVec4>& tangents)
	{
		std::vector<LvnVec3> bitangents(normals.size());
		for (uint32_t i = 0; i < normals.size(); i++)
		{
			bitangents[i] = lvn::normalize(lvn::cross(normals[i], tangents[i]) * tangents[i].w);
		}

		return bitangents;
	}
}

LvnModel loadGltfModel(const char* filepath)
{
	gltfs::gltfLoadData gltfData{};
	std::string jsonText = lvn::loadFileSrc(filepath);
	gltfData.JSON = nlm::json::parse(jsonText);
	gltfData.binData = gltfs::getData(gltfData.JSON, filepath);
	gltfData.filepath = filepath;
	gltfData.filetype = Lvn_FileType_Gltf;

	nlm::json JSON = gltfData.JSON;

	uint32_t scene = JSON.value("scene", 0);

	for (uint32_t i = 0; i < JSON["scenes"][scene]["nodes"].size(); i++) // curent impl only supports loading first scene
		gltfs::traverseNode(&gltfData, JSON["scenes"][0]["nodes"][i], LvnMat4(1.0f));

	LvnModel model{};
	model.meshes = LvnData(gltfData.meshes.data(), gltfData.meshes.size());
	model.samplers = LvnData(gltfData.samplers.data(), gltfData.samplers.size());
	model.textures = LvnData(gltfData.textures.data(), gltfData.textures.size());
	model.modelMatrix = LvnMat4(1.0f);

	return model;
}

LvnModel loadGlbModel(const char* filepath)
{
	LvnBin binData = lvn::loadFileSrcBin(filepath);

	// chunk 0 (JSON)
	uint32_t chunkLengthJson = 0;
	memcpy(&chunkLengthJson, &binData[12], sizeof(uint32_t));
	std::vector<uint8_t> chunkDataJson(&binData[20], &binData[20] + chunkLengthJson);

	// chunk 1 (Buffer)
	uint32_t chunkLengthBuffer = 0;
	memcpy(&chunkLengthBuffer, &binData[20 + chunkLengthJson], sizeof(uint32_t));
	std::vector<uint8_t> chunkDataBuffer(&binData[28 + chunkLengthJson], &binData[28 + chunkLengthJson] + chunkLengthBuffer);

	gltfs::gltfLoadData gltfData{};
	std::string jsonText = std::string(chunkDataJson.begin(), chunkDataJson.end());
	gltfData.JSON = nlm::json::parse(jsonText);
	gltfData.binData = LvnBin(chunkDataBuffer.data(), chunkDataBuffer.size());
	gltfData.filepath = filepath;
	gltfData.filetype = Lvn_FileType_Glb;

	nlm::json JSON = gltfData.JSON;

	for (uint32_t i = 0; i < JSON["scenes"][0]["nodes"].size(); i++) // curent impl only supports loading first scene
		gltfs::traverseNode(&gltfData, JSON["scenes"][0]["nodes"][i], LvnMat4(1.0f));

	LvnModel model{};
	model.meshes = LvnData(gltfData.meshes.data(), gltfData.meshes.size());
	model.textures = LvnData(gltfData.textures.data(), gltfData.textures.size());
	model.samplers = LvnData(gltfData.samplers.data(), gltfData.samplers.size());
	model.modelMatrix = LvnMat4(1.0f);

	return model;
}

} /* namespace lvn */
