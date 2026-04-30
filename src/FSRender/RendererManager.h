#pragma once

#define REORDER_3D_ALPHA_TIME 0.1f

namespace FSR
{
    class RendererManager
    {
    public:

        RendererManager();
        ~RendererManager();

        void Init();
		void ClearAllObjects();
        void ForceUpdate();

        ID3D12RootSignature* GetRootSignature() { return m_RootSignature.Get(); }
        ShaderPSO* GetShader(std::string name) { return m_ShaderMap[name]; }
        void SetSpriteRender(bool isEnabled) { m_SpriteRenderEnabled = isEnabled; }

        void Draw();
        void UpdateRender(float dt);

        void BuildShaderAndPSO(std::string name, std::wstring path, std::vector<D3D12_INPUT_ELEMENT_DESC> _Layout, bool isTransparent, bool isUI = false);
        void BuildRootSignature();
        void BuildSrvHeap();
        void BuildTextureSRV(ID3D12Resource* texture);

        void CreateCBObject();
        void CreateCBSprite();
        void CreateCBCameraAndPass();

        // 3D Objects
        void AddObject(Object3D* object);
        void MoveToAlphaRender(Object3D* object);
        void MoveToRender(Object3D* object);
        bool RemoveObject(Object3D* object);
        void RemoveObject(int index, std::vector<Object3D*>& vec, std::vector<UploadBuffer<ObjectConstants>*>& CB, std::vector<UploadBuffer<MaterialConstants>*>& MB);

        // UI
        Object2D* AddSprite(std::string texName, bool isMat = false);
        void RemoveSprite(Object2D* sprite);
        void RemoveSprite(int index);
		void SetReorderSprites(bool reorder) { m_ReorderSprites = reorder; }
		void UpdateWinSizeForSprites(int width, int height);

        // Lights
        void SetAmbientLight(XMFLOAT4 ambient) { m_AmbientLight = ambient; }
        void AddPointLight(PointLight* light);
        void AddSpotLight(SpotLight* light);
        void AddDirectionalLight(DirectionalLight* light);
        void RemoveLight(Light* light);
        void RemovePointLight(PointLight* light);
        void RemoveSpotLight(SpotLight* light);
        void RemoveDirectionalLight(DirectionalLight* light);
        void ClearLights() { ClearPointLights(); ClearSpotLights(); ClearDirectionalLights(); }
        void ClearPointLights();
        void ClearSpotLights();
        void ClearDirectionalLights();

    protected:

        void Reorder3DAlphaObjects();
        void ReorderSprites();

        void UpdateObjects();
        void UpdateCameras();
        void Update3DObjects();
        void Update3DAlphaObjects();
        void UpdateSprites();
        void UpdateMainPassCB();

        void AfterUpdateObjects();
        void AfterUpdateCameras();

        void Draw3DObjects();
        void Draw3DAlphaObjects();
        void DrawSprites();

        std::unordered_map<std::string, ShaderPSO*> m_ShaderMap;

        std::vector<Object3D*> m_3DObjects;
        std::vector<Object3D*> m_3DAlphaObjects;
        std::vector<Object2D*> m_Sprites;

        // Lights
        XMFLOAT4 m_AmbientLight = LIGHT_AMBIANT_STRENGTH;
        std::vector<PointLight*> m_PointLights;
        std::vector<SpotLight*> m_SpotLights;
        std::vector<DirectionalLight*> m_DirLights;

        std::vector<UploadBuffer<ObjectConstants>*> m_CBs;
        std::vector<UploadBuffer<MaterialConstants>*> m_MBs;
        std::vector<UploadBuffer<ObjectConstants>*> m_CBAlphas;
        std::vector<UploadBuffer<MaterialConstants>*> m_MBAlphas;

        std::vector<UploadBuffer<ObjectConstants2D>*> m_CB2Ds;
        std::vector<UploadBuffer<MaterialConstants>*> m_MB2Ds;

        UploadBuffer<PassConstants>* m_PassCB;
        UploadBuffer<CameraOC3D>* m_CameraCB3D;
        UploadBuffer<CameraOC2D>* m_CameraCB2D;

        ComPtr<ID3D12DescriptorHeap> m_SrvDescriptorHeap = nullptr;
        ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;

        Device* m_Device = nullptr;
        MaterialManager* m_MatManager = nullptr;
        CommandList* m_Cmd = nullptr;
		Camera3D* m_Camera = nullptr;

        float m_DeltaTime = 0.0f;
        float m_ReorderTimer = 0.0f;

        UINT m_CbvSrvDescriptorSize = 0;
        UINT m_CurrentSrvIndex = 0;

        bool m_SpriteRenderEnabled = true;
		bool m_ReorderSprites = false;
        bool m_UpdateAllSprites = false;

		bool m_UpdateAllObjects3D = false;

        friend class SpriteGenerator;
    };
}