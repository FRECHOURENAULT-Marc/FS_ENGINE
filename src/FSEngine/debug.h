#pragma once

#ifdef _DEBUG	

//Start record of memory usage (creation of object)
#define DEBUG_MEMORY_START_RECORD	\
_CrtMemState memStateInit;			\
_CrtMemCheckpoint(&memStateInit);	\
//End record of memory usage (detect memorys leaks and dump them)
#define DEBUG_MEMORY_END_RECORD										\
_CrtMemState memState, memStateDiff;								\
_CrtMemCheckpoint(&memState);										\
if (_CrtMemDifference(&memStateDiff, &memStateInit, &memState))		\
	/*MessageBoxA(nullptr, "Memory leaks", "ALERT", 0);	*/			\
_CrtDumpMemoryLeaks();

#define DEBUG_ALLOC_CONSOLE										\
	AllocConsole();												\
	FILE* fp;													\
	freopen_s(&fp, "CONOUT$", "w", stdout);						\
	freopen_s(&fp, "CONOUT$", "w", stderr);						\
	freopen_s(&fp, "CONIN$", "r", stdin);						\
_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);	\

#define ENALBLE_D3D12_DEBUG_LAYER															\
ComPtr<ID3D12Debug> debugController;														\
if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))						\
{																							\
	debugController->EnableDebugLayer();													\
	ComPtr<ID3D12Debug1> debug1;															\
	if (SUCCEEDED(debugController.As(&debug1))) debug1->SetEnableGPUBasedValidation(TRUE);	\
}																							\


class debug
{
	std::vector<FS_3DObject*> objects = std::vector<FS_3DObject*>();
	inline static debug* instance = nullptr;
	static debug* Get();

	void Draw(float scale, XMFLOAT3 pos, GEO_TYPE geo);
	void ClearObjects();

public:
	static void DrawSphere(float radius, XMFLOAT3 pos) { Get()->Draw(radius, pos, GEO_TYPE::SPHERE); };
	static void DrawBox(float scale, XMFLOAT3 pos) { Get()->Draw(scale, pos, GEO_TYPE::BOX); };
	static void Update(float dt) { Get()->ClearObjects(); };
};

#else

class debug
{
public:
	static void DrawSphere(float radius, XMFLOAT3 pos) {};
	static void DrawBox(float scale, XMFLOAT3 pos) {};

	static void Update(float dt) {};
};

#define DEBUG_MEMORY_START_RECORD
#define DEBUG_MEMORY_END_RECORD
#define DEBUG_ALLOC_CONSOLE
#define ENALBLE_D3D12_DEBUG_LAYER

#endif

