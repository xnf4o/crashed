#pragma once
#include "../menu.h"

#include "../../external/squirrel/squirrel.h"
#include "../../external/squirrel/sqstdio.h"
#include "../../external/squirrel/sqstdaux.h"

struct BufState
{
	const char* buffer;
	uintptr_t pointer;
	uintptr_t size;
};

namespace hooks
{
	void Setup();
	void Shutdown() noexcept;

	using PresentFn = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT) noexcept;
	inline PresentFn presentOriginal = nullptr;
	HRESULT __stdcall Present(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags) noexcept;

	using CompileFn = SQRESULT(__stdcall*)(HSQUIRRELVM, SQLEXREADFUNC, BufState*, const SQChar*, SQBool, const HSQOBJECT*);
	inline CompileFn compileOriginal = nullptr;
	SQRESULT __stdcall Compile(HSQUIRRELVM v, SQLEXREADFUNC read, BufState* state, const SQChar* sourcename, SQBool raiseerror, const HSQOBJECT* bindings);
}
