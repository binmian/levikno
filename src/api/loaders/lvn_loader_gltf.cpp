#include "levikno.h"
#include "lvn_loaders.h"

#include <future>

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
        uint32_t     byteOffset;
        int          componentType;
        bool         normalized;
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

    struct GLTFSkin
    {
        std::string name;
        int inverseBindMatrices;
        std::vector<int> joints;
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
        std::vector<GLTFAnimation> animations;
        std::vector<GLTFSkin> skins;
        std::vector<LvnImageData> images;
        std::vector<LvnSampler*> samplers;
        std::vector<LvnTexture*> textures;
        std::vector<LvnBuffer*> meshBuffers;
        std::vector<LvnMesh> meshes;

        LvnSampler* defaultSampler;
        LvnTexture* defaultBaseColorTexture;
        LvnTexture* defaultMetalicRoughnessTexture;
        LvnTexture* defaultNormalTexture;
        LvnTexture* defaultOcclusionTexture;
        LvnTexture* defaultEmissiveTexture;
    };

    static std::vector<LvnBin>         loadBuffers(const nlm::json& JSON, std::string_view filepath);
    static std::vector<GLTFAccessor>   loadAccessors(const nlm::json& JSON);
    static std::vector<GLTFBufferView> loadBufferViews(const nlm::json& JSON);
    static std::vector<GLTFMatrial>    loadMaterials(const nlm::json& JSON);
    static std::vector<GLTFAnimation>  loadAnimations(const nlm::json& JSON);
    static std::vector<GLTFSkin>       loadSkins(const nlm::json& JSON);
    static std::vector<LvnImageData>   loadImages(const GLTFLoadData& gltfData);
    static std::vector<LvnSampler*>    loadSamplers(const nlm::json& JSON, LvnSampler** defaultSampler);
    static std::vector<LvnAnimation>   bindAnimationsToNodes(const GLTFLoadData& gltfData);
    static std::vector<LvnSkin>        bindSkinsToNodes(const GLTFLoadData& gltfData);
    static size_t                      getCompType(int compType);
    static bool                        isNormalizedType(int compType);
    static std::vector<float>          getAttributeData(const GLTFLoadData* gltfData, const GLTFAccessor& accessor);
    static LvnTextureFilter            getSamplerFilterEnum(int filter);
    static LvnTextureMode              getSamplerWrapModeEnum(int mode);
    static LvnTopologyType             getTopologyEnum(int mode);
    static LvnInterpolationMode        getInterpolationMode(std::string interpolation);
    static std::vector<LvnVec3>        calculateBitangents(const std::vector<LvnVec3>& normals, const std::vector<LvnVec4>& tangents);
    static std::vector<LvnVec4>        calculateTangents(GLTFTangentCalcInfo* calcInfo);
    static void                        traverseNode(GLTFLoadData* gltfData, LvnNode* nextNode, int nextNodeIndex);
    static LvnMaterial                 getMaterial(GLTFLoadData* gltfData, int meshMaterialIndex);
    static void                        loadDefaultTextures(GLTFLoadData* gltfData);
    static std::vector<LvnMesh>        loadMeshes(GLTFLoadData* gltfData);
    static void                        bindMeshToNodes(GLTFLoadData* gltfData);


    static std::vector<LvnBin> loadBuffers(const nlm::json& JSON, std::string_view filepath)
    {
        std::vector<LvnBin> buffers(JSON["buffers"].size());

        for (int i = 0; i < JSON["buffers"].size(); i++)
        {
            std::string uri = JSON["buffers"][i]["uri"];
            std::string_view fileDirectory = filepath.substr(0, filepath.find_last_of("/\\") + 1);
            std::string pathbin = std::string(fileDirectory) + uri;
            
            buffers[i] = lvn::loadFileSrcBin(pathbin.c_str());
        }

        return buffers;
    }
    static std::vector<GLTFAccessor> loadAccessors(const nlm::json& JSON)
    {
        std::vector<GLTFAccessor> accessors(JSON["accessors"].size());

        for (int i = 0; i < JSON["accessors"].size(); i++)
        {
            accessors[i].bufferView = JSON["accessors"][i].value("bufferView", 0);
            accessors[i].byteOffset = JSON["accessors"][i].value("byteOffset", 0);
            accessors[i].componentType = JSON["accessors"][i]["componentType"];
            accessors[i].normalized = JSON["accessors"][i].value("normalized", false);
            accessors[i].count = JSON["accessors"][i]["count"];
            accessors[i].type = JSON["accessors"][i]["type"];

            std::vector<float> min = JSON["accessors"][i].value("min", std::vector<float>(3));
            accessors[i].min.x = min[0];
            accessors[i].min.y = min[1];
            accessors[i].min.z = min[2];
            std::vector<float> max = JSON["accessors"][i].value("max", std::vector<float>(3));
            accessors[i].max.x = max[0];
            accessors[i].max.y = max[1];
            accessors[i].max.z = max[2];
        }

        return accessors;
    }
    static std::vector<GLTFBufferView> loadBufferViews(const nlm::json& JSON)
    {
        std::vector<GLTFBufferView> bufferViews(JSON["bufferViews"].size());

        for (int i = 0; i < JSON["bufferViews"].size(); i++)
        {
            bufferViews[i].buffer = JSON["bufferViews"][i]["buffer"];
            bufferViews[i].byteLength = JSON["bufferViews"][i]["byteLength"];
            bufferViews[i].byteOffset = JSON["bufferViews"][i].value("byteOffset", 0);
        }

        return bufferViews;
    }
    static std::vector<GLTFMatrial> loadMaterials(const nlm::json& JSON)
    {
        if (!JSON.contains("materials"))
            return {};

        std::vector<GLTFMatrial> materials(JSON["materials"].size());

        for (uint32_t i = 0; i < JSON["materials"].size(); i++)
        {
            nlm::json materialNode = JSON["materials"][i];

            if (materialNode.find("pbrMetallicRoughness") != materialNode.end())
            {
                // color factors
                if (materialNode["pbrMetallicRoughness"].find("baseColorFactor") != materialNode["pbrMetallicRoughness"].end())
                {
                    nlm::json colorNode = materialNode["pbrMetallicRoughness"]["baseColorFactor"];
                    materials[i].pbrMetallicRoughness.baseColorFactor = LvnVec4(colorNode[0], colorNode[1], colorNode[2], colorNode[3]);
                }
                else
                {
                    materials[i].pbrMetallicRoughness.baseColorFactor = LvnVec4(1, 1, 1, 1);
                }

                // color texture
                if (materialNode["pbrMetallicRoughness"].find("baseColorTexture") != materialNode["pbrMetallicRoughness"].end())
                    materials[i].pbrMetallicRoughness.baseColorTexture.index = materialNode["pbrMetallicRoughness"]["baseColorTexture"]["index"];
                else
                    materials[i].pbrMetallicRoughness.baseColorTexture.index = -1;

                // metallic & roughness factors
                materials[i].pbrMetallicRoughness.metallicFactor = materialNode["pbrMetallicRoughness"].value("metallicFactor", 1);
                materials[i].pbrMetallicRoughness.roughnessFactor = materialNode["pbrMetallicRoughness"].value("roughnessFactor", 1);

                // metallic roughness texture
                if (materialNode["pbrMetallicRoughness"].find("metallicRoughnessTexture") != materialNode["pbrMetallicRoughness"].end())
                    materials[i].pbrMetallicRoughness.metallicRoughnessTexture.index = materialNode["pbrMetallicRoughness"]["metallicRoughnessTexture"]["index"];
                else
                    materials[i].pbrMetallicRoughness.metallicRoughnessTexture.index = -1;
            }

            // normal texture
            if (materialNode.find("normalTexture") != materialNode.end())
                materials[i].normalTexture.index = materialNode["normalTexture"]["index"];
            else
                materials[i].normalTexture.index = -1;

            // occlusion texture
            if (materialNode.find("occlusionTexture") != materialNode.end())
                materials[i].occlusionTexture.index = materialNode["occlusionTexture"]["index"];
            else
                materials[i].occlusionTexture.index = -1;

            // emissive texture
            if (materialNode.find("emissiveTexture") != materialNode.end())
                materials[i].emissiveTexture.index = materialNode["emissiveTexture"]["index"];
            else
                materials[i].emissiveTexture.index = -1;

            // emissive factor
            if (materialNode.find("emissiveFactor") != materialNode.end())
            {
                nlm::json emissiveNode = materialNode["emissiveFactor"];
                materials[i].emissiveFactor = LvnVec3(emissiveNode[0], emissiveNode[1], emissiveNode[2]);
            }
            else
            {
                materials[i].emissiveFactor = LvnVec3(0, 0, 0);
            }

            materials[i].alphaMode = materialNode.value("alphaMode", "OPAQUE");
            materials[i].alphaCutoff = materialNode.value("alphaCutoff", 0.5);
            materials[i].doubleSided = materialNode.value("doubleSided", false);
        }

        return materials;
    }
    static std::vector<GLTFAnimation>  loadAnimations(const nlm::json& JSON)
    {
        if (!JSON.contains("animations"))
            return {};

        std::vector<GLTFAnimation> animations(JSON["animations"].size());

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

            animations[i] = animation;
        }

        return animations;
    }
    static std::vector<GLTFSkin> loadSkins(const nlm::json& JSON)
    {
        if (!JSON.contains("skins"))
            return {};

        std::vector<GLTFSkin> skins(JSON["skins"].size());

        for (uint32_t i = 0; i < JSON["skins"].size(); i++)
        {
            const nlm::json& skinNode = JSON["skins"][i];

            skins[i].name = skinNode.value("name", "");

            skins[i].joints.resize(skinNode["joints"].size());
            for (uint32_t j = 0; j < skinNode["joints"].size(); j++)
                skins[i].joints[j] = skinNode["joints"][j];

            skins[i].inverseBindMatrices = skinNode.value("inverseBindMatrices", -1);
        }

        return skins;
    }
    static std::vector<LvnImageData> loadImages(const GLTFLoadData& gltfData)
    {
        LvnContext* lvnctx = lvn::getContext();

        const nlm::json& JSON = gltfData.JSON;

        if (!JSON.contains("images"))
            return {};

        std::vector<LvnImageData> images(JSON["images"].size());

        if (lvnctx->multithreading) // multithreading enabled
        {
            std::vector<std::future<LvnImageData>> futureImages(images.size());

            if (gltfData.filetype == Lvn_FileType_Gltf)
            {
                for (uint32_t i = 0; i < JSON["images"].size(); i++)
                {
                    std::string uri = JSON["images"][i]["uri"];
                    std::string fileDirectory = gltfData.filepath.substr(0, gltfData.filepath.find_last_of("/\\") + 1);

                    futureImages[i] = std::async(std::launch::async, lvn::loadImageDataThread, fileDirectory + uri, 4, false);
                }
            }
            else if (gltfData.filetype == Lvn_FileType_Glb)
            {
                for (uint32_t i = 0; i < JSON["images"].size(); i++)
                {
                    uint32_t bufferViewIndex = JSON["images"][i]["bufferView"];
                    GLTFBufferView bufferView = gltfData.bufferViews[bufferViewIndex];
                    LvnBin buffer = gltfData.buffers[bufferView.buffer];

                    futureImages[i] = std::async(std::launch::async, lvn::loadImageDataMemoryThread, &buffer[bufferView.byteOffset], bufferView.byteLength, 4, false);
                }
            }

            for (uint32_t i = 0; i < JSON["images"].size(); i++)
            {
                images[i] = std::move(futureImages[i].get());
            }
        }
        else // no multithreading
        {
            if (gltfData.filetype == Lvn_FileType_Gltf)
            {
                for (uint32_t i = 0; i < JSON["images"].size(); i++)
                {
                    std::string uri = JSON["images"][i]["uri"];
                    std::string fileDirectory = gltfData.filepath.substr(0, gltfData.filepath.find_last_of("/\\") + 1);

                    images[i] = lvn::loadImageData((fileDirectory + uri).c_str(), 4);
                }
            }
            else if (gltfData.filetype == Lvn_FileType_Glb)
            {
                for (uint32_t i = 0; i < JSON["images"].size(); i++)
                {
                    uint32_t bufferViewIndex = JSON["images"][i]["bufferView"];
                    GLTFBufferView bufferView = gltfData.bufferViews[bufferViewIndex];
                    LvnBin buffer = gltfData.buffers[bufferView.buffer];

                    images[i] = lvn::loadImageDataMemory(&buffer[bufferView.byteOffset], bufferView.byteLength, 4);
                }
            }
        }

        return images;
    }
    static std::vector<LvnSampler*> loadSamplers(const nlm::json& JSON, LvnSampler** defaultSampler)
    {
        if (!JSON.contains("samplers"))
        {
            LvnSamplerCreateInfo samplerCreateInfo{};
            samplerCreateInfo.wrapS = Lvn_TextureMode_Repeat;
            samplerCreateInfo.wrapT = Lvn_TextureMode_Repeat;
            samplerCreateInfo.minFilter = Lvn_TextureFilter_Nearest;
            samplerCreateInfo.magFilter = Lvn_TextureFilter_Nearest;

            lvn::createSampler(defaultSampler, &samplerCreateInfo);
            return std::vector<LvnSampler*>{*defaultSampler};
        }

        std::vector<LvnSampler*> samplers(JSON["samplers"].size());

        for (uint32_t i = 0; i < JSON["samplers"].size(); i++)
        {
            LvnSamplerCreateInfo samplerCreateInfo{};
            samplerCreateInfo.magFilter = gltfs::getSamplerFilterEnum(JSON["samplers"][i]["magFilter"]);
            samplerCreateInfo.minFilter = gltfs::getSamplerFilterEnum(JSON["samplers"][i]["minFilter"]);
            samplerCreateInfo.wrapS = gltfs::getSamplerWrapModeEnum(JSON["samplers"][i]["wrapS"]);
            samplerCreateInfo.wrapT = gltfs::getSamplerWrapModeEnum(JSON["samplers"][i]["wrapT"]);

            lvn::createSampler(&samplers[i], &samplerCreateInfo);
        }

        LvnSamplerCreateInfo samplerCreateInfo{};
        samplerCreateInfo.wrapS = Lvn_TextureMode_Repeat;
        samplerCreateInfo.wrapT = Lvn_TextureMode_Repeat;
        samplerCreateInfo.minFilter = Lvn_TextureFilter_Nearest;
        samplerCreateInfo.magFilter = Lvn_TextureFilter_Nearest;

        lvn::createSampler(defaultSampler, &samplerCreateInfo);
        samplers.push_back(*defaultSampler);

        return samplers;
    }
    static std::vector<LvnAnimation> bindAnimationsToNodes(const GLTFLoadData& gltfData)
    {
        std::vector<LvnAnimation> animations(gltfData.animations.size());

        for (uint32_t i = 0; i < gltfData.animations.size(); i++)
        {
            animations[i].currentTime = 0.0f;
            animations[i].start = 0.0f;
            animations[i].end = 0.0f;
            animations[i].channels.resize(gltfData.animations[i].channels.size());

            // set the animation start and end points to the first values in the buffer
            if (!gltfData.animations[i].channels.empty())
            {
                const GLTFAnimationChannel channel = gltfData.animations[i].channels[0];
                const GLTFAnimationSampler sampler = gltfData.animations[i].samplers[channel.sampler];

                GLTFAccessor accessor = gltfData.accessors[sampler.input];
                GLTFBufferView bufferView = gltfData.bufferViews[accessor.bufferView];
                LvnBin buffer = gltfData.buffers[bufferView.buffer];

                uint32_t beginningOfData = accessor.byteOffset + bufferView.byteOffset;

                animations[i].start = *reinterpret_cast<float*>(&buffer[beginningOfData]);
                animations[i].end = *reinterpret_cast<float*>(&buffer[beginningOfData] + (accessor.count - 1) * sizeof(float));
            }

            // bind the channels, samplers, input, output
            for (uint32_t j = 0; j < gltfData.animations[i].channels.size(); j++)
            {
                const GLTFAnimationChannel channel = gltfData.animations[i].channels[j];
                const GLTFAnimationSampler sampler = gltfData.animations[i].samplers[channel.sampler];

                if (channel.target.path == "translation")
                    animations[i].channels[j].path = Lvn_AnimationPath_Translation;
                if (channel.target.path == "rotation")
                    animations[i].channels[j].path = Lvn_AnimationPath_Rotation;
                if (channel.target.path == "scale")
                    animations[i].channels[j].path = Lvn_AnimationPath_Scale;

                animations[i].channels[j].node = gltfData.nodeArray[channel.target.node];
                animations[i].channels[j].interpolation = gltfs::getInterpolationMode(sampler.interpolation);

                // sampler input (keyframes)
                GLTFAccessor accessor = gltfData.accessors[sampler.input];
                GLTFBufferView bufferView = gltfData.bufferViews[accessor.bufferView];
                LvnBin buffer = gltfData.buffers[bufferView.buffer];

                uint32_t beginningOfData = accessor.byteOffset + bufferView.byteOffset;

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
                accessor = gltfData.accessors[sampler.output];
                bufferView = gltfData.bufferViews[accessor.bufferView];
                buffer = gltfData.buffers[bufferView.buffer];
                beginningOfData = accessor.byteOffset + bufferView.byteOffset;

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
    static std::vector<LvnSkin> bindSkinsToNodes(const GLTFLoadData& gltfData)
    {
        const nlm::json& JSON = gltfData.JSON;

        std::vector<LvnSkin> skins(gltfData.skins.size());

        for (uint32_t i = 0; i < gltfData.skins.size(); i++)
        {
            const GLTFSkin& skinData = gltfData.skins[i];
            skins[i].name = skinData.name;

            skins[i].joints.resize(skinData.joints.size());
            for (uint32_t j = 0; j < skinData.joints.size(); j++)
            {
                LvnNode* node = gltfData.nodeArray[skinData.joints[j]];
                skins[i].joints[j] = node;
                node->skin = i;
            }

            if (skinData.inverseBindMatrices >= 0)
            {
                const GLTFAccessor& accessor = gltfData.accessors[skinData.inverseBindMatrices];
                const GLTFBufferView& bufferView = gltfData.bufferViews[accessor.bufferView];
                const LvnBin& buffer = gltfData.buffers[bufferView.buffer];

                skins[i].inverseBindMatrices.resize(accessor.count);
                memcpy(skins[i].inverseBindMatrices.data(), &buffer[accessor.byteOffset + bufferView.byteOffset], accessor.count * 16 * sizeof(float));
            }

            skins[i].ssbo;
            LvnBufferCreateInfo ssboCreateInfo{};
            ssboCreateInfo.type = Lvn_BufferType_Storage;
            ssboCreateInfo.usage = Lvn_BufferUsage_Dynamic;
            ssboCreateInfo.size = 16 * sizeof(float) * skins[i].inverseBindMatrices.size();
            ssboCreateInfo.data = nullptr;

            lvn::createBuffer(&skins[i].ssbo, &ssboCreateInfo);
            lvn::bufferUpdateData(skins[i].ssbo, skins[i].inverseBindMatrices.data(), skins[i].inverseBindMatrices.size() * 16 * sizeof(float), 0);
        }

        return skins;
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
    static bool isNormalizedType(int compType)
    {
        switch (compType)
        {
            case 5120: { return true; }
            case 5121: { return true; }
            case 5122: { return true; }
            case 5123: { return true; }
            case 5125: { return false; }
            case 5126: { return false; }

            default: { LVN_CORE_ERROR("unknown component type: %d", compType); return false; }
        }
    }
    static std::vector<float> getAttributeData(const GLTFLoadData* gltfData, const GLTFAccessor& accessor)
    {
        GLTFBufferView bufferView = gltfData->bufferViews[accessor.bufferView];
        LvnBin buffer = gltfData->buffers[bufferView.buffer];

        uint32_t beginningOfData = accessor.byteOffset + bufferView.byteOffset;

        size_t compSize = gltfs::getCompType(accessor.componentType);

        uint32_t type = 2;
        if (accessor.type == "VEC2")
            type = 2;
        else if (accessor.type == "VEC3")
            type = 3;
        else if (accessor.type == "VEC4")
            type = 4;

        std::vector<float> att(accessor.count * type);

        if (accessor.componentType == 5126) // float
        {
            for (uint32_t i = 0; i < accessor.count; i++)
            {
                for (uint32_t j = 0; j < type; j++)
                {
                    att[i * type + j] = *reinterpret_cast<float*>(&buffer[beginningOfData] + i * type * sizeof(float) + j * sizeof(float));
                }
            }
        }
        else if (accessor.componentType == 5125) // unsigned int (uint32_t)
        {
            for (uint32_t i = 0; i < accessor.count; i++)
            {
                for (uint32_t j = 0; j < type; j++)
                {
                    uint32_t at = *reinterpret_cast<uint32_t*>(&buffer[beginningOfData] + i * type * sizeof(uint32_t) + j * sizeof(uint32_t));
                    att[i * type + j] = static_cast<float>(at);
                }
            }
        }
        else if (accessor.componentType == 5120) // signed byte (int8_t)
        {
            for (uint32_t i = 0; i < accessor.count; i++)
            {
                for (uint32_t j = 0; j < type; j++)
                {
                    int8_t at = *reinterpret_cast<int8_t*>(&buffer[beginningOfData] + i * type * sizeof(int8_t) + j * sizeof(int8_t));
                    att[i * type + j] = accessor.normalized ? static_cast<float>(at) / INT8_MAX : static_cast<float>(at);
                }
            }
        }
        else if (accessor.componentType == 5121) // unsigned byte (uint8_t)
        {
            for (uint32_t i = 0; i < accessor.count; i++)
            {
                for (uint32_t j = 0; j < type; j++)
                {
                    uint8_t at = *reinterpret_cast<uint8_t*>(&buffer[beginningOfData] + i * type * sizeof(uint8_t) + j * sizeof(uint8_t));
                    att[i * type + j] = accessor.normalized ? static_cast<float>(at) / UINT8_MAX : static_cast<float>(at);
                }
            }
        }
        else if (accessor.componentType == 5122) // signed short (int16_t)
        {
            for (uint32_t i = 0; i < accessor.count; i++)
            {
                for (uint32_t j = 0; j < type; j++)
                {
                    int16_t at = *reinterpret_cast<int16_t*>(&buffer[beginningOfData] + i * type * sizeof(int16_t) + j * sizeof(int16_t));
                    att[i * type + j] = accessor.normalized ? static_cast<float>(at) / INT16_MAX : static_cast<float>(at);
                }
            }
        }
        else if (accessor.componentType == 5123) // unsigned short (uint16_t)
        {
            for (uint32_t i = 0; i < accessor.count; i++)
            {
                for (uint32_t j = 0; j < type; j++)
                {
                    uint16_t at = *reinterpret_cast<uint16_t*>(&buffer[beginningOfData] + i * type * sizeof(uint16_t) + j * sizeof(uint16_t));
                    att[i * type + j] = accessor.normalized ? static_cast<float>(at) / UINT16_MAX : static_cast<float>(at);
                }
            }
        }

        return att;
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

        if (node.contains("skin"))
            nextNode->skin = node["skin"];
        else
            nextNode->skin = -1;

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
        nextNode->matrix = matrix;

        // Check if the node has children
        if (node.contains("children"))
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
    static LvnMaterial getMaterial(GLTFLoadData* gltfData, int meshMaterialIndex)
    {
        const nlm::json& JSON = gltfData->JSON;
        GLTFMatrial gltfMaterial = gltfData->materials[meshMaterialIndex];

        LvnMaterial material{};

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
    static void loadDefaultTextures(GLTFLoadData* gltfData)
    {
        // default base color texture
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

        // default metalic roughness texture
        uint8_t metalicRoughnessData[4] = { 0x00, 0xff, 0x00, 0xff };
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

        // default normal texture
        uint8_t normalTextureData[4] = { 0x80, 0x80, 0xff, 0xff };
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

        // default emissive texture
        uint8_t emissiveTextureData[4] = { 0x00, 0x00, 0x00, 0x00 };
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
    }
    static std::vector<LvnMesh> loadMeshes(GLTFLoadData* gltfData)
    {
        const nlm::json JSON = gltfData->JSON;

        if (!JSON.contains("meshes"))
            return {};

        std::vector<LvnMesh> meshes(JSON["meshes"].size());

        for (uint32_t meshIndex = 0; meshIndex < JSON["meshes"].size(); meshIndex++)
        {
            std::vector<LvnPrimitive> meshPrimitives(JSON["meshes"][meshIndex]["primitives"].size());
            for (uint32_t i = 0; i < JSON["meshes"][meshIndex]["primitives"].size(); i++)
            {
                nlm::json primitiveNode = JSON["meshes"][meshIndex]["primitives"][i];

                int posIndex      = primitiveNode["attributes"]["POSITION"];
                int colorIndex    = primitiveNode["attributes"].value("COLOR_0", -1);
                int texIndex      = primitiveNode["attributes"].value("TEXCOORD_0", -1);
                int normalIndex   = primitiveNode["attributes"].value("NORMAL", -1);
                int tangentIndex  = primitiveNode["attributes"].value("TANGENT", -1);
                int jointsIndex   = primitiveNode["attributes"].value("JOINTS_0", -1);
                int weightsIndex  = primitiveNode["attributes"].value("WEIGHTS_0", -1);
                int indicesIndex  = primitiveNode.value("indices", -1);
                int materialIndex = primitiveNode.value("material", -1);

                // position
                GLTFAccessor accessor = gltfData->accessors[posIndex];
                GLTFBufferView bufferView = gltfData->bufferViews[accessor.bufferView];
                LvnBin buffer = gltfData->buffers[bufferView.buffer];

                uint32_t beginningOfData = accessor.byteOffset + bufferView.byteOffset;

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

                    beginningOfData = accessor.byteOffset + bufferView.byteOffset;
                    size_t compType = gltfs::getCompType(accessor.componentType);

                    indices.resize(accessor.count);
                    for (uint32_t j = 0; j < accessor.count; j++)
                    {
                        memcpy(&indices[j], &buffer[beginningOfData] + j * compType, compType);
                    }
                }

                // color
                std::vector<LvnVec4> colors;
                if (colorIndex >= 0)
                {
                    accessor = gltfData->accessors[colorIndex];
                    colors.resize(accessor.count);
                    std::vector<float> data = gltfs::getAttributeData(gltfData, accessor);
                    memcpy(colors.data(), data.data(), data.size() * sizeof(float));
                }
                else if (materialIndex >= 0) // check material for base color if no color attribute exists
                {
                    colors.resize(positions.size());
                    for (uint32_t j = 0; j < positions.size(); j++)
                        colors[j] = gltfData->materials[materialIndex].pbrMetallicRoughness.baseColorFactor;
                }
                else // default vertex color if no material exists
                {
                    colors.resize(positions.size());
                    for (uint32_t j = 0; j < positions.size(); j++)
                        colors[j] = LvnVec4(1, 1, 1, 1);
                }

                // texcoords
                std::vector<LvnVec2> texcoords;
                if (texIndex >= 0)
                {
                    accessor = gltfData->accessors[texIndex];
                    texcoords.resize(accessor.count);
                    std::vector<float> data = gltfs::getAttributeData(gltfData, accessor);
                    memcpy(texcoords.data(), data.data(), data.size() * sizeof(float));
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

                    beginningOfData = accessor.byteOffset + bufferView.byteOffset;

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

                    beginningOfData = accessor.byteOffset + bufferView.byteOffset;

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

                // joints
                std::vector<LvnVec4> joints;
                if (jointsIndex >= 0)
                {
                    accessor = gltfData->accessors[jointsIndex];
                    joints.resize(accessor.count);
                    std::vector<float> data = gltfs::getAttributeData(gltfData, accessor);
                    memcpy(joints.data(), data.data(), data.size() * sizeof(float));
                }
                else
                {
                    joints.resize(positions.size(), 0);
                }

                // weights
                std::vector<LvnVec4> weights;
                if (weightsIndex >= 0)
                {
                    accessor = gltfData->accessors[weightsIndex];
                    weights.resize(accessor.count);
                    std::vector<float> data = gltfs::getAttributeData(gltfData, accessor);
                    memcpy(weights.data(), data.data(), data.size() * sizeof(float));
                }
                else
                {
                    weights.resize(positions.size(), 0);
                }

                // combine vertex data
                std::vector<LvnVertex> vertices;
                vertices.resize(positions.size());

                for (uint32_t j = 0; j < positions.size(); j++)
                {
                    vertices[j] = LvnVertex {
                        positions[j],
                        colors[j],
                        texcoords[j],
                        normals[j],
                        LvnVec3(tangents[j]),
                        bitangents[j],
                        joints[j],
                        weights[j],
                    };
                }

                // create buffer

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

                    // load all default textures if no material found
                    gltfs::loadDefaultTextures(gltfData);
                    meshPrimitives[i].material.albedo = gltfData->defaultBaseColorTexture;
                    meshPrimitives[i].material.metallicRoughnessOcclusion = gltfData->defaultMetalicRoughnessTexture;
                    meshPrimitives[i].material.normal = gltfData->defaultNormalTexture;
                    meshPrimitives[i].material.emissive = gltfData->defaultEmissiveTexture;
                }

                meshPrimitives[i].topology = gltfs::getTopologyEnum(primitiveNode.value("mode", 4));
            }

            LvnMesh mesh{};
            mesh.primitives = std::move(meshPrimitives);

            meshes[meshIndex] = std::move(mesh);
        }

        return meshes;
    }
    static void bindMeshToNodes(GLTFLoadData* gltfData)
    {
        const nlm::json& JSON = gltfData->JSON;

        for (uint32_t i = 0; i < JSON["nodes"].size(); i++)
        {
            if (JSON["nodes"][i].contains("mesh"))
                gltfData->nodeArray[i]->mesh = gltfData->meshes[JSON["nodes"][i]["mesh"]];
        }
    }

} /* namespace gltf */

LvnModel loadGltfModel(const char* filepath)
{
    LvnContext* lvnctx = lvn::getContext();

    gltfs::GLTFLoadData gltfData{};
    std::string jsonText = lvn::loadFileSrc(filepath);
    gltfData.JSON = nlm::json::parse(jsonText);
    gltfData.filepath = filepath;
    gltfData.filetype = Lvn_FileType_Gltf;

    nlm::json JSON = gltfData.JSON;

    if (JSON["scenes"].size() > 1)
        LVN_CORE_WARN("gltf model has more than one scene, loading mesh data from the first scene; Filepath: %s", filepath);

    if (JSON.contains("textures"))
    {
        gltfData.textures.reserve(JSON["textures"].size() + 4); // reserve extra spaces for default textures
        gltfData.textures.resize(JSON["textures"].size());
    }

    gltfData.buffers = std::move(gltfs::loadBuffers(gltfData.JSON, gltfData.filepath));
    gltfData.accessors = std::move(gltfs::loadAccessors(gltfData.JSON));
    gltfData.bufferViews = std::move(gltfs::loadBufferViews(gltfData.JSON));
    gltfData.materials = std::move(gltfs::loadMaterials(gltfData.JSON));
    gltfData.animations = std::move(gltfs::loadAnimations(gltfData.JSON));
    gltfData.skins = std::move(gltfs::loadSkins(gltfData.JSON));
    gltfData.images = std::move(gltfs::loadImages(gltfData));
    gltfData.samplers = std::move(gltfs::loadSamplers(gltfData.JSON, &gltfData.defaultSampler));
    
    gltfData.nodes.resize(JSON["scenes"][0]["nodes"].size());
    gltfData.nodeArray.resize(JSON["nodes"].size());

    for (uint32_t i = 0; i < JSON["scenes"][0]["nodes"].size(); i++)
    {
        gltfData.nodes[i] = std::make_shared<LvnNode>();
        int nodeIndex = JSON["scenes"][0]["nodes"][i];
        gltfData.nodeArray[nodeIndex] = gltfData.nodes[i].get();
        gltfs::traverseNode(&gltfData, gltfData.nodes[i].get(), nodeIndex);
    }

    std::vector<LvnAnimation> modelAnimations;
    std::future<std::vector<LvnAnimation>> animationFuture;

    if (lvnctx->multithreading)
        animationFuture = std::async(std::launch::async, gltfs::bindAnimationsToNodes, gltfData);
    else
        modelAnimations = std::move(gltfs::bindAnimationsToNodes(gltfData));

    gltfData.meshes = std::move(gltfs::loadMeshes(&gltfData));
    gltfs::bindMeshToNodes(&gltfData);

    if (lvnctx->multithreading)
        modelAnimations = std::move(animationFuture.get());

    LvnModel model{};
    model.matrix = LvnMat4(1.0f);
    model.buffers = std::move(gltfData.meshBuffers);
    model.textures = std::move(gltfData.textures);
    model.samplers = std::move(gltfData.samplers);
    model.nodes = std::move(gltfData.nodes);
    model.animations = std::move(modelAnimations);
    model.skins = std::move(gltfs::bindSkinsToNodes(gltfData));


    return model;
}
LvnModel loadGlbModel(const char* filepath)
{
    LvnContext* lvnctx = lvn::getContext();

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

    if (JSON.contains("textures"))
    {
        gltfData.textures.reserve(JSON["textures"].size() + 4); // reserve extra spaces for default textures
        gltfData.textures.resize(JSON["textures"].size());
    }

    gltfData.accessors = std::move(gltfs::loadAccessors(gltfData.JSON));
    gltfData.bufferViews = std::move(gltfs::loadBufferViews(gltfData.JSON));
    gltfData.materials = std::move(gltfs::loadMaterials(gltfData.JSON));
    gltfData.animations = std::move(gltfs::loadAnimations(gltfData.JSON));
    gltfData.skins = std::move(gltfs::loadSkins(gltfData.JSON));
    gltfData.images = std::move(gltfs::loadImages(gltfData));
    gltfData.samplers = std::move(gltfs::loadSamplers(gltfData.JSON, &gltfData.defaultSampler));

    gltfData.nodes.resize(JSON["scenes"][0]["nodes"].size());
    gltfData.nodeArray.resize(JSON["nodes"].size());

    for (uint32_t i = 0; i < JSON["scenes"][0]["nodes"].size(); i++)
    {
        gltfData.nodes[i] = std::make_shared<LvnNode>();
        int nodeIndex = JSON["scenes"][0]["nodes"][i];
        gltfData.nodeArray[nodeIndex] = gltfData.nodes[i].get();
        gltfs::traverseNode(&gltfData, gltfData.nodes[i].get(), nodeIndex);
    }

    std::vector<LvnAnimation> modelAnimations;
    std::future<std::vector<LvnAnimation>> animationFuture;

    if (lvnctx->multithreading)
        animationFuture = std::async(std::launch::async, gltfs::bindAnimationsToNodes, gltfData);
    else
        modelAnimations = std::move(gltfs::bindAnimationsToNodes(gltfData));

    gltfData.meshes = std::move(gltfs::loadMeshes(&gltfData));
    gltfs::bindMeshToNodes(&gltfData);

    if (lvnctx->multithreading)
        modelAnimations = std::move(animationFuture.get());

    LvnModel model{};
    model.matrix = LvnMat4(1.0f);
    model.buffers = std::move(gltfData.meshBuffers);
    model.textures = std::move(gltfData.textures);
    model.samplers = std::move(gltfData.samplers);
    model.nodes = std::move(gltfData.nodes);
    model.animations = std::move(modelAnimations);
    model.skins = std::move(gltfs::bindSkinsToNodes(gltfData));


    return model;
}

} /* namespace lvn */
