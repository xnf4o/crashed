#pragma once
#include <d3d11.h>

#include "hacks/hacks.h"

#include "../external/imgui/imgui.h"
#include "../external/imgui/imgui_impl_dx11.h"
#include "../external/imgui/imgui_impl_win32.h"

namespace menu
{
	inline bool open = false;
	void Render() noexcept;

	void SetupMenu(IDXGISwapChain* swap_chain) noexcept;
	void Shutdown() noexcept;

	inline HWND window = nullptr;
	inline WNDPROC windowProcessOriginal = nullptr;

	void Setup();
	inline std::uint64_t* methodsTable = nullptr;
}
