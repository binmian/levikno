#include "levikno.h"
#include "lvn_loaders.h"

#include <sstream>
#include <string>


namespace lvn
{

struct OBJMatrial
{
    LvnVec3 ambient;
    LvnVec3 diffuse;
    LvnVec3 specular;
    float shininess;

    std::string diffuseMap;
    std::string normalMap;
    std::string specularMap;
};

LvnModel loadObjModel(const char* filepath)
{
    std::vector<LvnVec3> positions;
    std::vector<LvnVec2> texCoords;
    std::vector<LvnVec3> normals;
    std::vector<uint32_t> indices;
    std::vector<LvnVertex> vertices;

    std::unordered_map<std::string, uint32_t> indicesMap;

    std::string filesrc = lvn::loadFileSrc(filepath).c_str();
    std::istringstream filess(filesrc);

    std::string line;
    while (std::getline(filess, line))
    {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v")
        {
            LvnVec3 pos;
            ss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        }
        else if (prefix == "vt")
        {
            LvnVec2 uv;
            ss >> uv.x >> uv.y;
            texCoords.push_back(uv);
        }
        else if (prefix == "vn")
        {
            LvnVec3 norm;
            ss >> norm.x >> norm.y >> norm.z;
            normals.push_back(norm);
        }
        else if (prefix == "f")
        {
            std::vector<uint32_t> faceIndices;
            std::string vertexStr;
            while (ss >> vertexStr)
            {
                // if vertex with same data exists, use same indices
                if (indicesMap.count(vertexStr))
                {
                    indices.push_back(indicesMap[vertexStr]);
                }
                else
                {
                    std::istringstream vs(vertexStr);
                    std::string posIdxStr, uvIdxStr, normIdxStr;
                    int posIdx = 0, uvIdx = 0, normIdx = 0;

                    std::getline(vs, posIdxStr, '/');
                    std::getline(vs, uvIdxStr, '/');
                    std::getline(vs, normIdxStr, '/');

                    posIdx = std::stoi(posIdxStr) - 1;
                    if (!uvIdxStr.empty()) uvIdx = std::stoi(uvIdxStr) - 1;
                    if (!normIdxStr.empty()) normIdx = std::stoi(normIdxStr) - 1;

                    LvnVertex vert{};
                    vert.pos = positions[posIdx];
                    vert.texUV = uvIdxStr.empty() ? LvnVec2(0, 0) : texCoords[uvIdx];
                    vert.normal = normIdxStr.empty() ? LvnVec3(0, 0, 0) : normals[normIdx];

                    vertices.push_back(vert);
                    uint32_t index = static_cast<uint32_t>(vertices.size()) - 1;
                    indicesMap[vertexStr] = index;
                    faceIndices.push_back(index);
                }
            }

            // triangulate the face indices if faces are quads/n-gons (triangle fan method)
            for (uint32_t i = 1; i < faceIndices.size() - 1; i++)
            {
                indices.push_back(faceIndices[0]);
                indices.push_back(faceIndices[i]);
                indices.push_back(faceIndices[i + 1]);
            }
        }
    }

    LvnVertexAttribute meshVertexAttributes[] =
    {
        { 0, 0, Lvn_AttributeFormat_Vec3_f32, 0 },                   // pos
        { 0, 1, Lvn_AttributeFormat_Vec4_f32, 3 * sizeof(float) },   // color
        { 0, 2, Lvn_AttributeFormat_Vec2_f32, 7 * sizeof(float) },   // texUV
        { 0, 3, Lvn_AttributeFormat_Vec3_f32, 9 * sizeof(float) },   // normal
        { 0, 4, Lvn_AttributeFormat_Vec3_f32, 12 * sizeof(float) },  // tangent
        { 0, 5, Lvn_AttributeFormat_Vec3_f32, 15 * sizeof(float) },  // bitangent
        { 0, 6, Lvn_AttributeFormat_Vec4_f32, 18 * sizeof(float) },  // joints
        { 0, 7, Lvn_AttributeFormat_Vec4_f32, 22 * sizeof(float) },  // weights
    };

    LvnVertexBindingDescription meshVertexBindingDescription;
    meshVertexBindingDescription.binding = 0;
    meshVertexBindingDescription.stride = sizeof(LvnVertex);

    std::vector<uint8_t> bufferData(vertices.size() * sizeof(LvnVertex) + indices.size() * sizeof(uint32_t));
    memcpy(bufferData.data(), vertices.data(), vertices.size() * sizeof(LvnVertex));
    memcpy(bufferData.data() + vertices.size() * sizeof(LvnVertex), indices.data(), indices.size() * sizeof(uint32_t));

    LvnBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.type = Lvn_BufferType_Vertex;
    if (!indices.empty()) bufferCreateInfo.type |= Lvn_BufferType_Index;
    bufferCreateInfo.usage = Lvn_BufferUsage_Static;
    bufferCreateInfo.size = vertices.size() * sizeof(LvnVertex) + indices.size() * sizeof(uint32_t);
    bufferCreateInfo.data = bufferData.data();

    LvnBuffer* buffer;
    lvn::createBuffer(&buffer, &bufferCreateInfo);

    LvnPrimitive primitive{};
    primitive.buffer = buffer;
    primitive.vertexCount = vertices.size();
    primitive.indexCount = indices.size();
    primitive.topology = Lvn_TopologyType_Triangle;

    LvnMesh mesh{};
    mesh.primitives.push_back(primitive);

    std::shared_ptr<LvnNode> node = std::make_shared<LvnNode>();
    node->transform.translation = LvnVec3(0, 0, 0);
    node->transform.rotation = LvnQuat(1, 0, 0, 0);
    node->transform.scale = LvnVec3(1, 1, 1);
    node->matrix = LvnMat4(1.0f);
    node->skin = -1;
    node->mesh = mesh;

    LvnModel model{};
    model.matrix = LvnMat4(1.0f);
    model.buffers.push_back(buffer);
    model.nodes.push_back(node);

    return model;
}

} /* namespace lvn */
