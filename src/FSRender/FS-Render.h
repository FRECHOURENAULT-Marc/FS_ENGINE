#pragma once

//Framework
#include "../FSCore/FS-Core.h"

//DirectX
#include <dxgi1_4.h>
#include <d3d12.h>
#include <d3d11_1.h>
#include <D3Dcompiler.h>

//Tools
#include <comdef.h>
#include <stdint.h>
#include <assert.h>

//Forward decl
namespace FSR
{
	class Device;
	class MaterialManager;
	class Window;
	class Camera3D;
	class CommandList;
	class ShaderPSO;
}

//Render Files
#include "Common/d3dx12.h"
#include "Common/DDSTextureLoader.h"
#include "Common/d3dUtil.h"
#include "Common/UploadBuffer.h"
#include "GeometryManager.h"

#include "CommandList.h"
#include "SwapChain.h"

#include "Texture.h"
#include "Material.h"
#include "SpriteGenerator.h"
#include "UIText.h"

#include "ShaderPSO.h"
#include "3DCamera.h"
#include "Window.h"

#include "RendererManager.h"

#include "Device.h"

using namespace FSR;