#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#endif

//Windows
#include <WindowsX.h>
#include <Windows.h>

//DirectX
#include <wrl.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

//Lib
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

//Tools
#include <iostream>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cassert>

//Namespaces
using Microsoft::WRL::ComPtr;
using namespace std;
using namespace DirectX;

//Forward declarations
struct MeshGeometry;

template<typename T>
class Condition;

//Fichiers Core
#include "Transition.h"
#include "Condition.h"
#include "Action.h"
#include "FS_StateMachine.h"
#include "InputsStruct.h"
#include "FS_InputsManager.h"
#include "Vertex.h"
#include "Maths.h"
#include "FS_Clock.h"
#include "Geometry.h"
#include "FS_Collider.h"
#include "FS_Light.h"
#include "ConstantBuffer.h"

#include "transform.h"
#include "FS_3DObject.h"
#include "FS_Sprite.h"
#include "FS_Particule.h"
