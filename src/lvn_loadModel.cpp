#include "lvn_loadModel.h"

#include "json.h"
#include "levikno.h"
#include "levikno_internal.h"
#include <cstdint>
namespace nlm = nlohmann;

namespace lvn
{

namespace gltfs
{
	struct LvnTextureIndexData
	{
		LvnTexture* texture;
		uint32_t index;
	};

	struct gltfLoadData
	{
		nlm::json JSON;
		std::string filepath;
		LvnData<uint8_t> binData;
		LvnVector<LvnMesh> meshes;
		LvnVector<LvnTextureIndexData> textureData;
		LvnVector<LvnTexture*> textures;
	};

	static void                  traverseNode(gltfLoadData* gltfData, uint32_t nextNode, LvnMat4 matrix);
	static void                  loadMesh(gltfLoadData* gltfData, uint32_t meshIndex, lvn::mat4 matrix);
	static LvnData<uint8_t>      getData(nlm::json JSON, const char* filepath);
	static LvnVector<float>      getFloats(gltfLoadData* gltfData, nlm::json accessor);
	static LvnVec4               getColors(nlm::json material);
	static LvnVector<uint32_t>   getIndices(gltfLoadData* gltfData, nlm::json accessor);
	static LvnMaterial           getMaterial(gltfLoadData* gltfData, nlm::json accessor);
	static LvnTextureFilter      getTexFilter(uint32_t filter);
	static LvnVector<LvnVec3>    calculateBitangents(const LvnVector<LvnVec3>& normals, const LvnVector<LvnVec4>& tangents);

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
			LvnVector<float> posRaw = gltfs::getFloats(gltfData, JSON["accessors"][meshPosIndex]);
			LvnVector<LvnVec3> position(posRaw.size() / 3);
			memcpy(&position[0], &posRaw[0], position.memsize());
			
			// get color data, assuming each vertex of the mesh primitive has the same color
			LvnVec4 colors = getColors(JSON["materials"][meshMaterialIndex]);

			// get texUV mesh data
			LvnVector<LvnVec2> texCoord;
			if (meshTexIndex >= 0)
			{
				LvnVector<float> texCoordRaw = gltfs::getFloats(gltfData, JSON["accessors"][meshTexIndex]);
				texCoord.resize(texCoordRaw.size() / 2);
				memcpy(&texCoord[0], &texCoordRaw[0], texCoord.memsize());
			}
			else
			{
				texCoord.resize(position.size());
				memset(&texCoord[0], 0, texCoord.memsize());
			}

			// get normal mesh data
			LvnVector<LvnVec3> normals;
			if (meshNormalIndex >= 0)
			{
				LvnVector<float> normalsRaw = gltfs::getFloats(gltfData, JSON["accessors"][meshNormalIndex]);
				normals.resize(normalsRaw.size() / 3);
				memcpy(&normals[0], &normalsRaw[0], normals.memsize());
			}
			else // mesh has no normals
			{
				normals.resize(position.size());
				memset(&normals[0], 0, normals.memsize());
			}

			LvnVector<LvnVec4> tangents;
			if (meshTangentIndex >= 0)
			{
				LvnVector<float> tangentsRaw = gltfs::getFloats(gltfData, JSON["accessors"][meshTangentIndex]);
				tangents.resize(tangentsRaw.size() / 4);
				memcpy(&tangents[0], &tangentsRaw[0], tangents.memsize());
			}
			else // mesh has no tangents
			{
				tangents.resize(position.size());
				memset(&tangents[0], 0, tangents.memsize());
			}

			// get Mesh Indices
			LvnVector<uint32_t> indices = gltfs::getIndices(gltfData, JSON["accessors"][meshIndicesIndex]);

			// bitangents
			LvnVector<LvnVec3> bitangents;
			if (meshNormalIndex >= 0 && meshTangentIndex >= 0)
			{
				bitangents = gltfs::calculateBitangents(normals, tangents);
			}
			else
			{
				bitangents.resize(position.size());
				memset(&bitangents[0], 0, bitangents.size());
			}

			// combine all mesh data and Get Model data
			LvnVector<LvnVertex> vertices;
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

			// Create Mesh
			// lvn::computeTangents(vertices.data(), vertices.size(), indices.data(), indices.size());

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

	static LvnVector<float> getFloats(gltfLoadData* gltfData, nlm::json accessor)
	{
		nlm::json JSON = gltfData->JSON;

		uint32_t bufferViewIndex = accessor.value("bufferView", 0);
		uint32_t count = accessor["count"];
		uint32_t accByteOffset = accessor.value("byteOffset", 0);
		std::string type = accessor["type"];

		nlm::json bufferView = JSON["bufferViews"][bufferViewIndex];
		uint32_t BVbyteOffset = bufferView.value("byteOffset", 0);

		uint32_t typeNum = 0;
		if (type == "SCALAR") { typeNum = 1; }
		else if (type == "VEC2") { typeNum = 2; }
		else if (type == "VEC3") { typeNum = 3; }
		else if (type == "VEC4") { typeNum = 4; }
		else { LVN_CORE_ERROR("unkown float type, type (%s) does not match with one of the following: VEC2, VEC3, VEC4, SCALER", type.c_str()); return {}; }

		uint32_t beginningOfData = accByteOffset + BVbyteOffset;
		uint32_t lengthOfData = count * typeNum; // number of floats, count * number of components in vec type

		LvnVector<float> floatValues(lengthOfData);
		memcpy(floatValues.data(), &gltfData->binData[beginningOfData], lengthOfData * sizeof(float)); // HACK: convert bin data to float values by copy and implicit casting, Note: 1 float = 4 bytes
		return floatValues;
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

	static LvnVector<uint32_t> getIndices(gltfLoadData* gltfData, nlm::json accessor)
	{
		nlm::json JSON = gltfData->JSON;

		uint32_t bufferViewIndex = accessor.value("bufferView", 0);
		uint32_t count = accessor["count"];
		uint32_t accByteOffset = accessor.value("byteOffset", 0);
		uint32_t componentType = accessor["componentType"];

		nlm::json bufferView = JSON["bufferViews"][bufferViewIndex];
		uint32_t bufferViewByteOffset = bufferView.value("byteOffset", 0);

		uint32_t beginningOfData = bufferViewByteOffset + accByteOffset;

		LvnVector<uint32_t> indices;
		indices.reserve(count);

		if (componentType == 5125) // UNSIGNED_INT = 5125
		{
			const uint8_t* data = gltfData->binData.data();

			for (uint32_t i = beginningOfData; i < bufferViewByteOffset + accByteOffset + count * sizeof(unsigned int); i += 4)
			{
				uint8_t bytes[] = { data[i], data[i + 1], data[i + 2], data[i + 3] };
				uint32_t value;
				std::memcpy(&value, bytes, sizeof(uint32_t));
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
				std::memcpy(&value, bytes, sizeof(uint16_t));
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
				std::memcpy(&value, bytes, sizeof(int16_t));
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

	static LvnMaterial getMaterial(gltfLoadData* gltfData, nlm::json accessor)
	{
		nlm::json JSON = gltfData->JSON;

		LvnContext* lvnctx = lvn::getContext();

		LvnMaterial material{};

		std::string fileDirectory = gltfData->filepath.substr(0, gltfData->filepath.find_last_of("/\\") + 1);

		uint32_t texSource;
		std::string texPath;

		// base color
		if (accessor["pbrMetallicRoughness"].find("baseColorTexture") != accessor["pbrMetallicRoughness"].end())
		{
			nlm::json texInd = JSON["textures"][(uint32_t)accessor["pbrMetallicRoughness"]["baseColorTexture"]["index"]];
			texSource = texInd["source"];
			texPath = JSON["images"][texSource]["uri"];

			bool skip = false;
			for (uint32_t i = 0; i < gltfData->textureData.size(); i++)
			{
				if (gltfData->textureData[i].index == texSource)
				{
					material.albedo = gltfData->textureData[i].texture;
					skip = true;
					break;
				}
			}
			if (!skip)
			{
				int texSamplerIndex = texInd.value("sampler", -1);

				LvnTextureCreateInfo textureCreateInfo{};
				textureCreateInfo.imageData = lvn::loadImageData((fileDirectory + texPath).c_str(), 4);

				// model has samplers
				if (texSamplerIndex >= 0)
				{
					uint32_t magFilter = JSON["samplers"][texSamplerIndex]["magFilter"];
					uint32_t minFilter = JSON["samplers"][texSamplerIndex]["minFilter"];
					textureCreateInfo.minFilter = getTexFilter(minFilter);
					textureCreateInfo.magFilter = getTexFilter(magFilter);
				}
				else // default to nearest if no sampler found
				{
					textureCreateInfo.minFilter = Lvn_TextureFilter_Nearest;
					textureCreateInfo.magFilter = Lvn_TextureFilter_Nearest;
				}

				textureCreateInfo.wrapMode = Lvn_TextureMode_Repeat;
				textureCreateInfo.format = Lvn_TextureFormat_Srgb;

				LvnTexture* texture;
				lvn::createTexture(&texture, &textureCreateInfo);

				LvnTextureIndexData textureIndexData = { texture, texSource };
				gltfData->textureData.push_back(textureIndexData);
				gltfData->textures.push_back(texture);
				material.albedo = texture;
			}
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
			textureCreateInfo.minFilter = Lvn_TextureFilter_Nearest;
			textureCreateInfo.magFilter = Lvn_TextureFilter_Nearest;
			textureCreateInfo.wrapMode = Lvn_TextureMode_Repeat;
			textureCreateInfo.format = Lvn_TextureFormat_Srgb;

			LvnTexture* texture;
			lvn::createTexture(&texture, &textureCreateInfo);

			material.albedo = texture;
			gltfData->textures.push_back(texture);
		}

		// metalic roughness
		if (accessor["pbrMetallicRoughness"].find("metallicRoughnessTexture") != accessor["pbrMetallicRoughness"].end())
		{
			nlm::json texInd = JSON["textures"][(uint32_t)accessor["pbrMetallicRoughness"]["metallicRoughnessTexture"]["index"]];
			texSource = texInd["source"];
			texPath = JSON["images"][texSource]["uri"];

			bool skip = false;
			for (uint32_t i = 0; i < gltfData->textureData.size(); i++)
			{
				if (gltfData->textureData[i].index == texSource)
				{
					material.metallicRoughnessOcclusion = gltfData->textureData[i].texture;
					skip = true;
					break;
				}
			}
			if (!skip)
			{
				int texSamplerIndex = texInd.value("sampler", -1);

				LvnTextureCreateInfo textureCreateInfo{};
				textureCreateInfo.imageData = lvn::loadImageData((fileDirectory + texPath).c_str(), 4);

				// model has samplers
				if (texSamplerIndex >= 0)
				{
					uint32_t magFilter = JSON["samplers"][texSamplerIndex]["magFilter"];
					uint32_t minFilter = JSON["samplers"][texSamplerIndex]["minFilter"];
					textureCreateInfo.minFilter = getTexFilter(minFilter);
					textureCreateInfo.magFilter = getTexFilter(magFilter);
				}
				else // default to nearest if no sampler found
				{
					textureCreateInfo.minFilter = Lvn_TextureFilter_Nearest;
					textureCreateInfo.magFilter = Lvn_TextureFilter_Nearest;
				}

				textureCreateInfo.wrapMode = Lvn_TextureMode_Repeat;
				textureCreateInfo.format = Lvn_TextureFormat_Srgb;

				LvnTexture* texture;
				lvn::createTexture(&texture, &textureCreateInfo);

				LvnTextureIndexData textureIndexData = { texture, texSource };
				gltfData->textureData.push_back(textureIndexData);
				gltfData->textures.push_back(texture);
				material.metallicRoughnessOcclusion = texture;
			}
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
			textureCreateInfo.minFilter = Lvn_TextureFilter_Nearest;
			textureCreateInfo.magFilter = Lvn_TextureFilter_Nearest;
			textureCreateInfo.wrapMode = Lvn_TextureMode_Repeat;
			textureCreateInfo.format = Lvn_TextureFormat_Srgb;

			LvnTexture* texture;
			lvn::createTexture(&texture, &textureCreateInfo);

			material.metallicRoughnessOcclusion = texture;
			gltfData->textures.push_back(texture);
		}

		// normal
		if (accessor.find("normalTexture") != accessor.end())
		{
			nlm::json texInd = JSON["textures"][(uint32_t)accessor["normalTexture"]["index"]];
			texSource = texInd["source"];
			texPath = JSON["images"][texSource]["uri"];

			bool skip = false;
			for (uint32_t i = 0; i < gltfData->textureData.size(); i++)
			{
				if (gltfData->textureData[i].index == texSource)
				{
					material.normal = gltfData->textureData[i].texture;
					skip = true;
					break;
				}
			}
			if (!skip)
			{
				int texSamplerIndex = texInd.value("sampler", -1);

				LvnTextureCreateInfo textureCreateInfo{};
				textureCreateInfo.imageData = lvn::loadImageData((fileDirectory + texPath).c_str(), 4);

				// model has samplers
				if (texSamplerIndex >= 0)
				{
					uint32_t magFilter = JSON["samplers"][texSamplerIndex]["magFilter"];
					uint32_t minFilter = JSON["samplers"][texSamplerIndex]["minFilter"];
					textureCreateInfo.minFilter = getTexFilter(minFilter);
					textureCreateInfo.magFilter = getTexFilter(magFilter);
				}
				else // default to nearest if no sampler found
				{
					textureCreateInfo.minFilter = Lvn_TextureFilter_Nearest;
					textureCreateInfo.magFilter = Lvn_TextureFilter_Nearest;
				}

				textureCreateInfo.wrapMode = Lvn_TextureMode_Repeat;
				textureCreateInfo.format = Lvn_TextureFormat_Unorm;

				LvnTexture* texture;
				lvn::createTexture(&texture, &textureCreateInfo);

				LvnTextureIndexData textureIndexData = { texture, texSource };
				gltfData->textureData.push_back(textureIndexData);
				gltfData->textures.push_back(texture);
				material.normal = texture;
			}
		}
		else // default normal texture if no texture was found; creates a 1x1 texture, 4 channels (0x7f7fffff)
		{
			uint8_t normalTextureData[4] = { 0x7f, 0x7f, 0xff, 0xff };
			LvnImageData imageData;
			imageData.pixels = LvnData<uint8_t>(normalTextureData, sizeof(normalTextureData) / sizeof(uint8_t));
			imageData.width = 1;
			imageData.height = 1;
			imageData.channels = 4;
			imageData.size = 4;

			LvnTextureCreateInfo textureCreateInfo{};
			textureCreateInfo.imageData = imageData;
			textureCreateInfo.minFilter = Lvn_TextureFilter_Nearest;
			textureCreateInfo.magFilter = Lvn_TextureFilter_Nearest;
			textureCreateInfo.wrapMode = Lvn_TextureMode_Repeat;
			textureCreateInfo.format = Lvn_TextureFormat_Unorm;

			LvnTexture* texture;
			lvn::createTexture(&texture, &textureCreateInfo);

			material.normal = texture;
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

	static LvnVector<LvnVec3> calculateBitangents(const LvnVector<LvnVec3>& normals, const LvnVector<LvnVec4>& tangents)
	{
		LvnVector<LvnVec3> bitangents(normals.size());
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

	nlm::json JSON = gltfData.JSON;

	for (uint32_t i = 0; i < JSON["scenes"][0]["nodes"].size(); i++) // curent impl only supports loading first scene
		gltfs::traverseNode(&gltfData, JSON["scenes"][0]["nodes"][i], LvnMat4(1.0f));

	LvnModel model{};
	model.meshes = LvnData(gltfData.meshes.data(), gltfData.meshes.size());
	model.textures = LvnData(gltfData.textures.data(), gltfData.textures.size());
	model.modelMatrix = LvnMat4(1.0f);

	return model;
}

} /* namespace lvn */
