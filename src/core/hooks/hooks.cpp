#include "hooks.h"

#include <stdexcept>
#include <string>
#include <iostream>
#include <fstream>
#include <format>
#include <filesystem>

#include "../../external/minhook/minhook.h"

#include "../../external/imgui/imgui.h"
#include "../../external/imgui/imgui_impl_dx11.h"
#include "../../external/imgui/imgui_impl_win32.h"

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

BOOL KeVirtualProtect(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect)
{
	return NT_SUCCESS(_syscall<NTSTATUS>(0x50,
		((HANDLE)-1),
		&lpAddress,
		&dwSize,
		PAGE_EXECUTE_READWRITE,
		lpflOldProtect)) ? TRUE : FALSE;
}

//static std::uint8_t* compileAddress;

void hooks::Setup()
{
	//if (!(compileAddress = memory::Scan(
	//	"56 57 53 48 83 EC 50 48 89 CE 48 8B 84 24 ? ? ? ? C7 44 24 ? ? ? ? ? C6 44 24 ? ?")))
	//	throw std::runtime_error("Outdated Compile signature");


	if (MH_Initialize())
		throw std::runtime_error("Unable to initialize minhook");


	if (MH_CreateHook(reinterpret_cast<void*>(menu::methodsTable[8]), &Present, reinterpret_cast<void**>(&presentOriginal)))
		throw std::runtime_error("Unable to hook present");

	//if (MH_CreateHook(compileAddress, &Compile, reinterpret_cast<void**>(&compileOriginal)))
	//	throw std::runtime_error("Unable to hook setLocalPlayer");


	if (MH_EnableHook(reinterpret_cast<void*>(menu::methodsTable[8])))
		throw std::runtime_error("Unable to enable present");

	//if (MH_EnableHookEx(compileAddress, KeVirtualProtect))
	//	throw std::runtime_error("Unable to enable compile");
}

void hooks::Shutdown() noexcept
{
	MH_DisableHook(reinterpret_cast<void*>(menu::methodsTable[8]));

	MH_RemoveHook(reinterpret_cast<void*>(menu::methodsTable[8]));
	//MH_RemoveHookEx(compileAddress, KeVirtualProtect);

	MH_Uninitialize();
}

HRESULT __stdcall hooks::Present(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags) noexcept
{
	static bool setup = false;

	if (!setup)
	{
		menu::SetupMenu(swap_chain);
		setup = !setup;
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//if (menu::open)
		//menu::Render();

	hacks::Visuals();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return presentOriginal(swap_chain, sync_interval, flags);
}

SQRESULT __stdcall hooks::Compile(HSQUIRRELVM v, SQLEXREADFUNC read, BufState* state, const SQChar* sourcename, SQBool raiseerror, const HSQOBJECT* bindings)
{
	const auto source = std::string(sourcename);

	if (!std::filesystem::exists("D:/Dumps/Quirrel/" + source.substr(0, source.find_last_of('/'))))
		std::filesystem::create_directories("D:/Dumps/Quirrel/" + source.substr(0, source.find_last_of('/')));

	std::ofstream file("D:/Dumps/Quirrel/" + source /*source.substr(source.find_last_of('/'), source.size())*/, std::ofstream::out);
	file << std::string(state->buffer, state->buffer + state->size);
	file.close();

	std::cout << source << std::endl;

	return compileOriginal(v, read, state, sourcename, raiseerror, bindings);
}
