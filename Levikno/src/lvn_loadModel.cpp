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
	static lvn::vec3             getColors(nlm::json material);
	static LvnVector<uint32_t>   getIndices(gltfLoadData* gltfData, nlm::json accessor);

	static void traverseNode(gltfLoadData* gltfData, uint32_t nextNode, LvnMat4 matrix)
	{
		nlm::json JSON = gltfData->JSON;
		nlm::json node = JSON["nodes"][nextNode];

		lvn::vec3 nodeTranslation = lvn::vec3(0.0f, 0.0f, 0.0f);
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

		lvn::vec3 nodeScale = lvn::vec3(1.0f, 1.0f, 1.0f);
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

			// Get Pos mesh data
			LvnVector<lvn::vec3> position;
			LvnVector<float> posRaw = gltfs::getFloats(gltfData, JSON["accessors"][meshPosIndex]);
			for (uint32_t j = 0; j < posRaw.size() / 3; j++)
				position.push_back(lvn::vec3(posRaw[(j * 3)], posRaw[(j * 3) + 1], posRaw[(j * 3) + 2]));
			
			// Get Color mesh data
			LvnVector<lvn::vec3> colors;
			lvn::vec3 colorRaw = getColors(JSON["materials"][meshMaterialIndex]);
			for (uint32_t j = 0; j < position.size(); j++)
				colors.push_back(colorRaw);

			// Get Normal mesh data
			LvnVector<lvn::vec3> normals;
			if (meshNormalIndex >= 0)
			{
				LvnVector<float> normalsRaw = gltfs::getFloats(gltfData, JSON["accessors"][meshNormalIndex]);
				for (uint32_t j = 0; j < normalsRaw.size() / 3; j++)
					normals.push_back(lvn::vec3(normalsRaw[(j * 3)], normalsRaw[(j * 3) + 1], normalsRaw[(j * 3) + 2]));
			}
			else // Mesh has no normals
			{
				for (uint32_t j = 0; j < position.size(); j++)
					normals.push_back(lvn::vec3(0.0f));
			}

			// Get texUV mesh data
			LvnVector<lvn::vec2> texCoord;
			if (meshTexIndex >= 0)
			{
				LvnVector<float> texCoordRaw = gltfs::getFloats(gltfData, JSON["accessors"][meshTexIndex]);
				for (int j = 0; j < texCoordRaw.size() / 2; j++)
					texCoord.push_back(lvn::vec2(texCoordRaw[(j * 2)], texCoordRaw[(j * 2) + 1]));
			}
			else
			{
				for (uint32_t j = 0; j < position.size(); j++)
					texCoord.push_back(lvn::vec2(0.0f, 0.0f));
			}

			// Get Mesh Indices
			LvnVector<uint32_t> indices = gltfs::getIndices(gltfData, JSON["accessors"][meshIndicesIndex]);

			// Combine all mesh data and Get Model data
			LvnVector<LvnVertex> vertices;
			for (int j = 0; j < position.size(); j++)
			{
				vertices.push_back(
					vertex
					{
						position[j],
						LvnVec4(colors[j], 1.0f),
						texCoord[j],
						normals[j],
					}
				);
			}

			// Get Textures
			// LvnVector<Texture*> textures = getTextures(JSON["materials"][meshMaterialIndex]);

			// Create Mesh
			LvnMeshCreateInfo meshCreateInfo{};
			meshCreateInfo.vertices = vertices.data();
			meshCreateInfo.vertexCount = vertices.size();
			meshCreateInfo.indices = indices.data();
			meshCreateInfo.indexCount = indices.size();

			LvnMesh* mesh;
			lvn::createMesh(&mesh, &meshCreateInfo);
			lvn::setMeshMatrix(mesh, matrix);

			gltfData->meshes.push_back(*mesh);
			lvn::destroyMesh(mesh);
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

	static lvn::vec3 getColors(nlm::json material)
	{
		lvn::vec3 colors = lvn::vec3(1.0f);

		if (material["pbrMetallicRoughness"].find("baseColorFactor") != material["pbrMetallicRoughness"].end())
		{
			colors = lvn::vec3(material["pbrMetallicRoughness"]["baseColorFactor"][0], material["pbrMetallicRoughness"]["baseColorFactor"][1], material["pbrMetallicRoughness"]["baseColorFactor"][2]);
			return colors;
		}

		return colors;
	}

	static LvnVector<uint32_t> getIndices(gltfLoadData* gltfData, nlm::json accessor)
	{
		nlm::json JSON = gltfData->JSON;

		uint32_t bufferViewIndex = accessor.value("bufferView", 0);
		uint32_t count = accessor["count"];
		uint32_t accByteOffset = accessor.value("byteOffset", 0);
		uint32_t componentType = accessor["componentType"];
		if (componentType == 5125) // UNSIGNED_INT = 5125
		{
			LVN_CORE_ERROR("indices component type not supported (%u), only unsigned int types are supported: '5125' (UNSIGNED_INT)", componentType);
			return {};
		}

		nlm::json bufferView = JSON["bufferViews"][bufferViewIndex];
		uint32_t bufferViewByteOffset = bufferView.value("byteOffset", 0);

		uint32_t beginningOfData = bufferViewByteOffset + accByteOffset;

		LvnVector<uint32_t> indicesValues(count);
		memcpy(indicesValues.data(), &gltfData->binData[beginningOfData], count * sizeof(uint32_t)); // HACK: convert bin data to unsigned int values by copy and implicit casteing
		return indicesValues;
	}
}

LvnModel loadGltfModel(const char* filepath)
{
	gltfs::gltfLoadData gltfData{};
	// TODO: add function to add meshes

	LvnModel model{};
	model.meshes = LvnData(gltfData.meshes.data(), gltfData.meshes.size());
	model.modelMatrix = LvnMat4(1.0f);

	return model;
}

} /* namespace lvn */
