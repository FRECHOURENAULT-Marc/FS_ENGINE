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
#include <functional>
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
namespace FSC
{
	struct MeshGeometry;
	template<typename T>
	class Condition;
}

//Fichiers Core
#include "utils.h"
#include "Transition.h"
#include "Condition.h"
#include "Action.h"
#include "StateMachine.h"
#include "InputsStruct.h"
#include "InputsManager.h"
#include "Vertex.h"
#include "Maths.h"
#include "Clock.h"
#include "Geometry.h"
#include "Collider.h"
#include "Light.h"
#include "ConstantBuffer.h"

#include "transform.h"
#include "RenderObject.h"
#include "Object3D.h"
#include "Object2D.h"
#include "Particule.h"

#include "Thread.h"

using namespace FSC;
