#pragma once
class FS_Renderer
{
public:

    FS_Renderer();
    ~FS_Renderer();

	ID3D12RootSignature* GetRootSignature() { return m_RootSignature.Get(); }
	FS_ShaderPSO* GetShader(std::string name) { return m_ShaderMap[name]; }
	void SetSpriteRender(bool isEnabled) { m_spriteRenderEnabled = isEnabled; }

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
    void AddObject(FS_3DObject* object);
    bool RemoveObject(FS_3DObject* object);
    void RemoveObject(int index);

	// UI
    FS_Sprite* AddSprite(std::string texName, bool isMat = false);
    void RemoveSprite(FS_Sprite* sprite);
    void RemoveSprite(int index);

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

//public:
protected:

    std::unordered_map<std::string, FS_ShaderPSO*> m_ShaderMap;

    std::vector<FS_3DObject*> m_3DObjects;
    std::vector<FS_Sprite*> m_Sprites;

    void UpdateObjects();
    void UpdateCameras();
    void Update3DObjects();
    void UpdateSprites();
    void UpdateMainPassCB();

	void Draw3DObjects();
	void DrawSprites();

    // Lights
	XMFLOAT4 m_AmbientLight = LIGHT_AMBIANT_STRENGTH;
    std::vector<PointLight*> m_pointLights;
	std::vector<SpotLight*> m_spotLights;
	std::vector<DirectionalLight*> m_dirLights;

    std::vector<UploadBuffer<ObjectConstants>*> m_CBs;
    std::vector<UploadBuffer<MaterialConstants>*> m_MBs;

    std::vector<UploadBuffer<ObjectConstants2D>*> m_CB2Ds;
    std::vector<UploadBuffer<MaterialConstants>*> m_MB2Ds;

	UploadBuffer<PassConstants>* m_PassCB;
    UploadBuffer<CameraOC3D>* m_CameraCB3D;
    UploadBuffer<CameraOC2D>* m_CameraCB2D;

    ComPtr<ID3D12DescriptorHeap> m_SrvDescriptorHeap = nullptr;
    ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;

    UINT m_CbvSrvDescriptorSize = 0;
    UINT m_CurrentSrvIndex = 0;

    bool m_spriteRenderEnabled = true;

	friend class SpriteGenerator;
};