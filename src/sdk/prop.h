#pragma once
#include "math.h"

#include <string>
#include <iostream>

class DagorPropContainer
{
	struct DagorProp
	{
		std::uint16_t index;
		std::uint16_t unknown;
		std::uint32_t valueSize;

		const char* name;
		void* function;
		DagorProp* next;
		// value
	};

protected:
	constexpr const std::uint32_t Count() const { return end->index - start->index; }

	constexpr DagorProp* Find(const char* const name) const noexcept
	{
		for (auto current = start; current != end; current = current->next)
		{
			if (!std::strcmp(current->name, name))
				return current;
		}

		return nullptr;
	}

	constexpr DagorProp* Start() const { return start; }
	constexpr DagorProp* End() const { return end; }

	template <typename T>
	constexpr T& GetValue(const char* const name) const noexcept
	{
		const auto prop = Find(name);
	}

private:
	void* vtable;
	uint64_t unknown;
	DagorProp* start;
	DagorProp* end;
};
