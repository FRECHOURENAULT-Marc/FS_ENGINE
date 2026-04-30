#pragma once

#ifdef _DEBUG	

//Start record of memory usage
#define DEBUG_MEMORY_START_RECORD	\
_CrtMemState memStateInit;			\
_CrtMemCheckpoint(&memStateInit);	\
//End record of memory usage (detect memories leaks and dump them)
#define DEBUG_MEMORY_END_RECORD										\
_CrtMemState memState, memStateDiff;								\
_CrtMemCheckpoint(&memState);										\
_CrtMemDifference(&memStateDiff, &memStateInit, &memState);			\
_CrtDumpMemoryLeaks();												\

#define DEBUG_ALLOC_CONSOLE										\
AllocConsole();													\
FILE* fp;														\
freopen_s(&fp, "CONOUT$", "w", stdout);							\
freopen_s(&fp, "CONOUT$", "w", stderr);							\
freopen_s(&fp, "CONIN$", "r", stdin);							\

//Record all memory usage (detect memories leaks and dump them)
#define DEBUG_MEMORY_RECORD_ALL									\
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
	struct UIDebug
	{
		UIString* m_Str = nullptr;
		bool m_IsActive = false;
	};

	std::vector<Object3D*> objects = std::vector<Object3D*>();
	std::vector<UIDebug*> uis = std::vector<UIDebug*>();
	inline static debug* instance = nullptr;

	debug();
	static debug* Get();

	void Draw(XMFLOAT3 scale, XMFLOAT3 pos, FSR::GEO_TYPE geo);
	void DrawUI(std::string text, XMINT2 pos);
	void ClearObjects();

public:
	static void DrawSphere(float radius, XMFLOAT3 pos) { Get()->Draw({radius, radius, radius}, pos, FSR::GEO_TYPE::SPHERE); };
	static void DrawBox(float scale, XMFLOAT3 pos) { Get()->Draw({scale, scale , scale}, pos, FSR::GEO_TYPE::BOX); };
	static void DrawBox(XMFLOAT3 scale, XMFLOAT3 pos) { Get()->Draw(scale, pos, FSR::GEO_TYPE::BOX); };
	static void DrawUIText(std::string text, XMINT2 pos) { Get()->DrawUI(text, pos); };

	static void Update(float dt);

	static void Cout(std::string text) { std::cout << text << std::endl; }
	static void Cout(std::wstring text) { std::wcout << text << std::endl; }
};

#else

class debug
{
public:
	static void DrawSphere(float radius, XMFLOAT3 pos) {};
	static void DrawBox(float scale, XMFLOAT3 pos) {};
	static void DrawBox(XMFLOAT3 scale, XMFLOAT3 pos) {};
	static void DrawUIText(std::string text, XMINT2 pos) {};

	static void Update(float dt) {};

	static void Cout(std::string text) {};
	static void Cout(std::wstring text) {};
};

#define DEBUG_MEMORY_START_RECORD
#define DEBUG_MEMORY_END_RECORD
#define DEBUG_MEMORY_RECORD_ALL	
#define DEBUG_ALLOC_CONSOLE
#define ENALBLE_D3D12_DEBUG_LAYER

#endif

using D = debug;