#pragma once

#define SHADER_COLOR_NAME "Color"
#define SHADER_COLOR_A_NAME "ColorTransparent"
#define SHADER_TEXTURE_NAME "Texture"
#define SHADER_TEXTURE_A_NAME "TextureTransparent"
#define SHADER_COLOREDTEXTURE_NAME "ColoredTexture"

#define SHADER_SPRITE_NAME "Sprite"

#define MATERIAL_DEFAULT_COLOR_NAME "DefaultColorMat"
#define MATERIAL_DEFAULT_COLOR_A_NAME "DefaultColorTransparentMat"

struct FS_Material
{
    FS_ShaderPSO* Shader;
    UINT TextureIndex = 0;

    XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
    XMFLOAT3 Metallic = { 0.01f, 0.01f, 0.01f };
    float Roughness = 0.25f;
};

class MaterialManager
{
    static inline MaterialManager* instance;
    std::vector<FS_Material*> m_MaterialLibrary = std::vector<FS_Material*>();
    std::vector<std::string> m_MaterialNames = std::vector<std::string>();

public:

    MaterialManager() { instance = this; };
    static MaterialManager* Get() { return instance; };

    //Create a new material in the renderer (if no tetxure specified, it apply only color)
    static void CreateMaterial(std::string shaderName, std::string materialName, std::string textureName = "", float roughness = 0.25f, XMFLOAT3 metallic = { 0.01f, 0.01f, 0.01f }, XMFLOAT4 diffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f });
    static int GetMaterialIndex(std::string materialName);
    static FS_Material* GetMaterial(std::string materialName);
    static FS_Material* GetMaterial(int materialIndex);

    friend class FS_Device;
    friend class FS_Renderer;
};

