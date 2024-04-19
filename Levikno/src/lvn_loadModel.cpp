#include "lvn_loadModel.h"

#include "json.h"
namespace nlm = nlohmann;

namespace lvn
{

namespace gltfs
{
	struct gltfLoadData
	{
		nlm::json JSON;
		std::string filepath;
		LvnData<uint8_t> binData;
		LvnVector<LvnMesh> meshes;
	};

	static void                  traverseNode(gltfLoadData* gltfData, uint32_t nextNode, LvnMat4 matrix);
	static void                  loadMesh(gltfLoadData* gltfData, uint32_t meshIndex, lvn::mat4 matrix);
	static LvnData<uint8_t>      getData(nlm::json JSON, const char* filepath);
	static LvnVector<float>      getFloats(gltfLoadData* gltfData, nlm::json accessor);
	static LvnVec4               getColors(nlm::json material);
	static LvnVector<uint32_t>   getIndices(gltfLoadData* gltfData, nlm::json accessor);

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
			int meshNormalIndex = JSON["meshes"][meshIndex]["primitives"][i]["attributes"].value("NORMAL", -1);
			int meshTexIndex = JSON["meshes"][meshIndex]["primitives"][i]["attributes"].value("TEXCOORD_0", -1);
			int meshIndicesIndex = JSON["meshes"][meshIndex]["primitives"][i]["indices"];
			int meshMaterialIndex = JSON["meshes"][meshIndex]["primitives"][i]["material"];

			// get Pos mesh data
			LvnVector<LvnVec3> position;
			LvnVector<float> posRaw = gltfs::getFloats(gltfData, JSON["accessors"][meshPosIndex]);
			for (uint32_t j = 0; j < posRaw.size() / 3; j++)
				position.push_back(LvnVec3(posRaw[(j * 3)], posRaw[(j * 3) + 1], posRaw[(j * 3) + 2]));
			
			// get color data, assuming each vertex of the mesh primitive has the same color
			LvnVec4 colors = getColors(JSON["materials"][meshMaterialIndex]);

			// get normal mesh data
			LvnVector<LvnVec3> normals;
			if (meshNormalIndex >= 0)
			{
				LvnVector<float> normalsRaw = gltfs::getFloats(gltfData, JSON["accessors"][meshNormalIndex]);
				for (uint32_t j = 0; j < normalsRaw.size() / 3; j++)
					normals.push_back(LvnVec3(normalsRaw[(j * 3)], normalsRaw[(j * 3) + 1], normalsRaw[(j * 3) + 2]));
			}
			else // mesh has no normals
			{
				for (uint32_t j = 0; j < position.size(); j++)
					normals.push_back(LvnVec3(0.0f));
			}

			// get texUV mesh data
			LvnVector<LvnVec2> texCoord;
			if (meshTexIndex >= 0)
			{
				LvnVector<float> texCoordRaw = gltfs::getFloats(gltfData, JSON["accessors"][meshTexIndex]);
				for (int j = 0; j < texCoordRaw.size() / 2; j++)
					texCoord.push_back(LvnVec2(texCoordRaw[(j * 2)], texCoordRaw[(j * 2) + 1]));
			}
			else
			{
				for (uint32_t j = 0; j < position.size(); j++)
					texCoord.push_back(LvnVec2(0.0f, 0.0f));
			}

			// get Mesh Indices
			LvnVector<uint32_t> indices = gltfs::getIndices(gltfData, JSON["accessors"][meshIndicesIndex]);

			// combine all mesh data and Get Model data
			LvnVector<LvnVertex> vertices;
			for (uint32_t j = 0; j < position.size(); j++)
			{
				vertices.push_back(
					LvnVertex
					{
						position[j],    // pos
						colors,         // color
						texCoord[j],    // texUV
						normals[j],     // normal

						LvnVec3(0.0f),  // tangent
						LvnVec3(0.0f),  // bitangent
					}
				);
			}

			// Get Textures
			// LvnVector<Texture*> textures = getTextures(JSON["materials"][meshMaterialIndex]);

			// Create Mesh
			LvnMeshCreateInfo meshCreateInfo{};
			meshCreateInfo.bufferInfo = lvn::createMeshDefaultVertexBufferCreateInfo(vertices.data(), vertices.size(), indices.data(), indices.size());

			LvnMesh mesh = lvn::createMesh(&meshCreateInfo);
			lvn::setMeshMatrix(&mesh, matrix);
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
		
		return lvn::getFileSrcBin(pathbin.c_str());
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

		if (componentType == 5125) // UNSIGNED_INT = 5125
		{
			for (uint32_t i = beginningOfData; i < bufferViewByteOffset + accByteOffset + count * sizeof(unsigned int); i += 4)
			{
				const uint8_t* data = gltfData->binData.data();
				uint8_t bytes[] = { data[i], data[i + 1], data[i + 2], data[i + 3] };
				uint32_t value;
				std::memcpy(&value, bytes, sizeof(uint8_t));
				indices.push_back(value);
			}
		}
		else if (componentType == 5123) // UNSIGNED_SHORT = 5123
		{
			for (unsigned int i = beginningOfData; i < bufferViewByteOffset + accByteOffset + count * sizeof(unsigned short); i += 2)
			{
				const uint8_t* data = gltfData->binData.data();
				uint8_t bytes[] = { data[i], data[i + 1] };
				uint16_t value;
				std::memcpy(&value, bytes, sizeof(uint16_t));
				indices.push_back((uint32_t)value);
			}
		}
		else if (componentType == 5122) // SHORT = 5122
		{
			for (unsigned int i = beginningOfData; i < bufferViewByteOffset + accByteOffset + count * sizeof(short); i += 2)
			{
				const uint8_t* data = gltfData->binData.data();
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
}

LvnModel loadGltfModel(const char* filepath)
{
	gltfs::gltfLoadData gltfData{};
	std::string jsonText = lvn::getFileSrc(filepath);
	gltfData.JSON = nlm::json::parse(jsonText);
	gltfData.binData = gltfs::getData(gltfData.JSON, filepath);
	gltfData.filepath = filepath;

	nlm::json JSON = gltfData.JSON;

	for (uint32_t i = 0; i < JSON["scenes"][0]["nodes"].size(); i++) // curent impl only supports loading first scene
		gltfs::traverseNode(&gltfData, JSON["scenes"][0]["nodes"][i], LvnMat4(1.0f));

	LvnModel model{};
	model.meshes = LvnData(gltfData.meshes.data(), gltfData.meshes.size());
	model.modelMatrix = LvnMat4(1.0f);

	return model;
}

} /* namespace lvn */
