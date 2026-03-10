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
struct MeshGeometry;
class FS_Window;
class FS_3DCamera;
class FS_Command;
class FS_ShaderPSO;

//Render Files
#include "Common/d3dx12.h"
#include "Common/DDSTextureLoader.h"
#include "Common/d3dUtil.h"
#include "Common/UploadBuffer.h"
#include "GeometryManager.h"

#include "FS_Command.h"
#include "FS_SwapChain.h"

#include "FS_Texture.h"
#include "FS_Material.h"
#include "SpriteGenerator.h"
#include "FS_Text.h"

#include "FS_ShaderPSO.h"
#include "FS_3DCamera.h"
#include "FS_Window.h"

#include "FS_Renderer.h"

#include "FS_Device.h"

