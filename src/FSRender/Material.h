#pragma once

#define SHADER_COLOR_NAME "Color"
#define SHADER_TEXTURE_NAME "Texture"
#define SHADER_COLOR_A_NAME "ColorTransparent"
#define SHADER_TEXTURE_A_NAME "TextureTransparent"
#define SHADER_COLOR_TEXTURE_NAME "ColorTexture"
#define SHADER_COLOR_TEXTURE_A_NAME "ColorTextureTransparent"

#define SHADER_SPRITE_NAME "Sprite"

#define MATERIAL_DEFAULT_COLOR_NAME "DefaultColorMat"
#define MATERIAL_DEFAULT_COLOR_A_NAME "DefaultColorTransparentMat"
#define MATERIAL_DEFAULT_TEXTURE_NAME "DefaultTextureTransparentMat"

namespace FSR
{
    struct Material
    {
        ShaderPSO* Shader = nullptr;
        UINT TextureIndex = 0;

        XMFLOAT4 m_DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
        XMFLOAT3 m_Metallic = { 0.01f, 0.01f, 0.01f };
        float m_Roughness = 0.25f;
    };

    class MaterialManager
    {
        static inline MaterialManager* instance;
        std::vector<Material*> m_MaterialLibrary = std::vector<Material*>();
        std::vector<std::string> m_MaterialNames = std::vector<std::string>();

    public:

        MaterialManager() { instance = this; };
        static MaterialManager* Get() { return instance; };

        //Create a new material in the renderer (if no texture specified, it apply only color)
        static Material* CreateMaterial(std::string shaderName, std::string materialName, std::string textureName = "", float roughness = 0.25f, XMFLOAT3 metallic = { 0.01f, 0.01f, 0.01f }, XMFLOAT4 diffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f });
        static int GetMaterialIndex(std::string materialName);
        static int GetMaterialIndexFromTextureID(int textureID);
        static Material* GetMaterial(std::string materialName);
        static Material* GetMaterial(int materialIndex);

        friend class Device;
        friend class RendererManager;
    };

}
