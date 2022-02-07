#pragma once
#include "syscall.h"

namespace memory
{
	std::uint8_t* Scan(const char* signature) noexcept;

	template<typename T = std::uintptr_t>
	constexpr T GetOffset(std::uintptr_t address, int offset)
	{
		return (T)(address + (int)((*(int*)(address + offset) + offset) + sizeof(int)));
	}
}
