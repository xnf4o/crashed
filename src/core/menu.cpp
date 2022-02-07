#include "menu.h"

#include <stdexcept>
#include <fstream>

LRESULT CALLBACK WindowProcess(HWND window, UINT message, WPARAM wideParam, LPARAM longParam);

#include "../external/squirrel/squirrel.h"
#include "../external/squirrel/sqstdio.h"
#include "../external/squirrel/sqstdaux.h"

void menu::Render() noexcept
{
	ImGui::SetNextWindowSize({ 500, 400 });
	if (ImGui::Begin("script executor", &open,
		ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoCollapse))
	{
		static char script_buffer[500];

		ImGui::InputTextMultiline("##script",
			script_buffer,
			IM_ARRAYSIZE(script_buffer),
			{ ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - 50 });

		//if (ImGui::Button("instant leave"))
		//{
		//	static const auto vm = *reinterpret_cast<HSQUIRRELVM*>(
		//		memory::GetOffset<std::uintptr_t>(reinterpret_cast<std::uintptr_t>(
		//			memory::Scan("48 8B 35 ? ? ? ? 48 8B 7E 30 48 8B 46 50 48 8D 48 01 48 89 4E 50 48 C1 E0 04")), 0x3));

		//	using CallFn = SQRESULT(__fastcall*)(HSQUIRRELVM, SQInteger, SQBool, SQBool);
		//	using PushStringFn = void(__fastcall*)(HSQUIRRELVM, const SQChar*, SQInteger);

		//	static const auto game_sq_call = reinterpret_cast<CallFn>(
		//		memory::Scan("56 57 53 48 83 EC 40 4C 89 C3 48 89 D7 48 89 CE C7 44 24 ? ? ? ? ?")
		//		);

		//	static const auto game_sq_pushstring = reinterpret_cast<PushStringFn>(
		//		memory::Scan("56 57 48 83 EC 28 48 89 CF 48 85 D2 75 48 4D 85 C0 74 43 48 8B 47 30")
		//		);

		//	if (vm)
		//	{
		//		sq_pushroottable(vm);
		//		game_sq_pushstring(vm, "show_not_available_msg_box", -1);

		//		if (SQ_SUCCEEDED(sq_get(vm, -2)))
		//		{
		//			sq_pushroottable(vm);

		//			if (SQ_SUCCEEDED(game_sq_call(vm, 1, SQFalse, SQFalse)))
		//			{

		//			}

		//			sq_pop(vm, 2);
		//		}
		//	}
		//}

		if (ImGui::Button("execute"))
		{
			using RunScriptFn = std::int64_t(__fastcall*)(const char* script, std::int64_t a2, std::int64_t a3);
			static const auto run_script = reinterpret_cast<RunScriptFn>(
				memory::Scan("56 57 53 48 81 EC ? ? ? ? 4C 89 C7 48 89 D6 48 8D 1D ? ? ? ? 48 89 5C 24 ? 48 C7 44 24 ? ? ? ? ?")
				);

			//run_script("foreach(unit in ::all_units) {\r\nunit.costGold = 1\r\n}", 0, 0);
			//run_script("::showInfoMsgBox(::format(\"Welcome to Crashed!\"));", 0, 0);
			//run_script("foreach(unit in ::all_units) {\r\nunit.costGold = 1\r\n}", 0, 0);
			run_script(script_buffer, 0, 0);
		}

		ImGui::End();
	}
}

void menu::SetupMenu(IDXGISwapChain* swap_chain) noexcept
{
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ID3D11RenderTargetView* mainRenderTargetView;

	if (SUCCEEDED(swap_chain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&device))))
	{
		device->GetImmediateContext(&context);

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		swap_chain->GetDesc(&swapChainDesc);
		window = swapChainDesc.OutputWindow;

		ID3D11Texture2D* backBuffer;
		swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&backBuffer));
		device->CreateRenderTargetView(backBuffer, NULL, &mainRenderTargetView);
		backBuffer->Release();

		windowProcessOriginal = reinterpret_cast<WNDPROC>(SetWindowLongPtr(window,
			GWLP_WNDPROC,
			reinterpret_cast<LONG_PTR>(WindowProcess)));

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

		ImGuiStyle* style = &ImGui::GetStyle();

		style->WindowPadding = { 15, 15 };
		style->WindowRounding = 1.f;
		style->WindowTitleAlign = { 0.5f, 0.5f };
		style->WindowBorderSize = 0;

		style->FrameRounding = 0.f;
		style->FramePadding = { 20, 7 };
		style->FrameBorderSize = 1;

		ImGui_ImplWin32_Init(window);
		ImGui_ImplDX11_Init(device, context);
	}
}

void menu::Shutdown() noexcept
{
	free(methodsTable);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	SetWindowLongPtrW(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(windowProcessOriginal));
}

void menu::Setup()
{
	WNDCLASSEX windowClass;
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = DefWindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.hIcon = NULL;
	windowClass.hCursor = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = "Crashed";
	windowClass.hIconSm = NULL;

	RegisterClassEx(&windowClass);

	const auto tempWindow = CreateWindow(windowClass.lpszClassName,
		"Crashed DirectX Window",
		WS_OVERLAPPEDWINDOW,
		0, 0,
		100, 100,
		NULL,
		NULL,
		windowClass.hInstance,
		NULL);

	const auto d3dlib = GetModuleHandleA("d3d11.dll");

	if (!d3dlib)
	{
		DestroyWindow(tempWindow);
		UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
		throw std::runtime_error("Unable to get d3d11.dll handle");
	}

	using Fn = HRESULT(__stdcall*)(
		IDXGIAdapter*,
		D3D_DRIVER_TYPE,
		HMODULE,
		UINT,
		const D3D_FEATURE_LEVEL*,
		UINT,
		UINT,
		const DXGI_SWAP_CHAIN_DESC*,
		IDXGISwapChain**,
		ID3D11Device**,
		D3D_FEATURE_LEVEL*,
		ID3D11DeviceContext**);

	const auto function = reinterpret_cast<Fn>(GetProcAddress(d3dlib, "D3D11CreateDeviceAndSwapChain"));

	if (!function)
	{
		DestroyWindow(tempWindow);
		UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
		throw std::runtime_error("Unable to get D3D11CreateDeviceAndSwapChain");
	}

	D3D_FEATURE_LEVEL featureLevel = { };
	const D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_11_0 };

	DXGI_RATIONAL refreshRate;
	refreshRate.Numerator = 60;
	refreshRate.Denominator = 1;

	DXGI_MODE_DESC bufferDesc;
	bufferDesc.Width = 100;
	bufferDesc.Height = 100;
	bufferDesc.RefreshRate = refreshRate;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	DXGI_SAMPLE_DESC sampleDesc;
	sampleDesc.Count = 1;
	sampleDesc.Quality = 0;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc = sampleDesc;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = tempWindow;
	swapChainDesc.Windowed = 1;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	IDXGISwapChain* swapChain;
	ID3D11Device* device;
	ID3D11DeviceContext* context;

	if (function(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		featureLevels,
		2,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&swapChain, &device,
		&featureLevel,
		&context) < 0)
	{
		DestroyWindow(tempWindow);
		UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
		throw std::runtime_error("Fuck knows what this problem is");
	}

	methodsTable = reinterpret_cast<uint64_t*>(calloc(205, sizeof(uint64_t)));
	memcpy(methodsTable, *reinterpret_cast<uint64_t**>(swapChain), 18 * sizeof(uint64_t));
	memcpy(methodsTable + 18, *reinterpret_cast<uint64_t**>(device), 43 * sizeof(uint64_t));
	memcpy(methodsTable + 18 + 43, *reinterpret_cast<uint64_t**>(context), 144 * sizeof(uint64_t));

	swapChain->Release();
	swapChain = NULL;

	device->Release();
	device = NULL;

	context->Release();
	context = NULL;

	DestroyWindow(tempWindow);
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window, UINT message, WPARAM wideParam, LPARAM longParam);
LRESULT CALLBACK WindowProcess(HWND window, UINT message, WPARAM wideParam, LPARAM longParam)
{
	if (GetAsyncKeyState(VK_INSERT) & 1)
		menu::open = !menu::open;

	if (menu::open && ImGui_ImplWin32_WndProcHandler(window, message, wideParam, longParam))
		return 1L;

	return CallWindowProcW(menu::windowProcessOriginal, window, message, wideParam, longParam);
}
