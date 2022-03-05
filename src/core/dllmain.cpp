#include <Windows.h>
#include <thread>
#include <iostream>
#include <format>

#include "hooks/hooks.h"

unsigned long __stdcall setup(void* instance)
{
	AllocConsole();
	FILE* fDummy;
	freopen_s(&fDummy, "CONOUT$", "w", stdout);

	const auto get = [](const char* signature) noexcept -> std::uintptr_t
	{
		return reinterpret_cast<std::uintptr_t>(memory::Scan(signature));
	};

	const auto local = get("48 8B 05 ? ? ? ? B1 FF 48 85 C0 74 03 8A 48 08");
	g::local = reinterpret_cast<LocalPlayer*>(memory::GetOffset<std::uintptr_t*>(local, 0x3));

	//const auto unit = get("48 8B 0D ? ? ? ? 48 85 C9 74 4D 4C 8B 81 ? ? ? ? 4D 85 C0 74 53");
	//g::unit = reinterpret_cast<Unit*>(*memory::GetOffset<std::uintptr_t*>(unit, 0x3));

	const auto list = get("48 8B 1D ? ? ? ? 48 85 DB 0F 84 ? ? ? ? 4C 8B 0D ? ? ? ? 49 83 F9 04");
	g::list = reinterpret_cast<PlayerList*>(memory::GetOffset<std::uintptr_t*>(list, 0x3));

	const auto game = get("48 8B 05 ? ? ? ? F2 0F 10 4F ? F2 0F 5A C9 F2 0F 10 17 F2 0F 5A D2 F3 0F 11 54 24 ?");
	g::game = reinterpret_cast<Game*>(*memory::GetOffset<std::uintptr_t*>(game, 0x3));

	try
	{
		menu::Setup();
		hooks::Setup();
	}
	catch (const std::exception& error)
	{
		MessageBoxA(NULL, error.what(), "crashed error", MB_OK);
		hooks::Shutdown();
		menu::Shutdown();
		::FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 0);
	}

	std::cout << "Welcome to Crashed - Do not close this window!" << std::endl;
	std::cout << std::endl;

	std::cout << "END to un-load the cheat..." << std::endl;
	std::cout << std::endl;

	std::cout << "[Initialization]:" << std::endl;
	std::cout << std::hex
		<< "- g::local -> 0x" << g::local << std::endl
		//<< "- g::unit -> 0x" << g::unit << std::endl
		<< "- g::list -> 0x" << g::list << std::endl
		<< "- g::game -> 0x" << g::game
		<< std::dec << std::endl;
	std::cout << std::endl;

	// Get base address to calculate offsets
	const auto base = reinterpret_cast<std::uintptr_t>(GetModuleHandleA("aces.exe"));

	std::cout << "[Base]:\n- \"aces.exe\" -> 0x" << std::hex << base << std::dec << std::endl;
	std::cout << std::endl;

	const auto offset = [base](std::uintptr_t address, int offset) noexcept -> std::uintptr_t
	{
		return reinterpret_cast<std::uintptr_t>(memory::GetOffset<std::uintptr_t*>(address, offset)) - base;
	};

	std::cout << "[Offsets]:" << std::endl;
	std::cout << std::hex <<
		"- local player -> 0x" << offset(local, 3) << std::endl <<
		//"- local unit -> 0x" << offset(unit, 3) << std::endl <<
		"- player list -> 0x" << offset(list, 3) << std::endl <<
		"- game -> 0x" << offset(game, 3) << std::endl <<
		std::dec << std::endl << std::endl;

	while (!::GetAsyncKeyState(VK_END))
		std::this_thread::sleep_for(std::chrono::milliseconds(200));

	hooks::Shutdown();
	menu::Shutdown();

	std::cout << "[Shutdown] Cheat has been unloaded" << std::endl;

	::FreeConsole();
	::FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 0);

	return 0;
}

int __stdcall DllMain(void* instance, unsigned long reason, void* previous)
{
	if (reason == 1)
	{
		::DisableThreadLibraryCalls(static_cast<HMODULE>(instance));

		const auto thread = ::CreateThread(NULL,
			NULL,
			setup,
			instance,
			NULL,
			nullptr);

		if (thread)
			::CloseHandle(thread);
	}

	return 1;
}

