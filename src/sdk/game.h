#pragma once
#include "players.h"

namespace fnv
{
	inline constexpr uint32_t base = 0x811C9DC5;
	inline constexpr uint32_t prime = 0x1000193;

	constexpr uint32_t HashConst(const char* data, const std::uint32_t value = base) noexcept
	{
		return (data[0] == '\0') ? value : HashConst(&data[1], (value ^ std::uint32_t(data[0])) * prime);
	}

	inline uint32_t Hash(const char* data) noexcept
	{
		std::uint32_t hashed = base;

		for (size_t i = 0U; i < strlen(data); ++i)
		{
			hashed ^= data[i];
			hashed *= prime;
		}

		return hashed;
	}
}

class Units
{
public:
	std::array<Unit*, 500U> units;
};

class UnitList
{
public:
	Units* unitList;
	std::uint16_t unitCount;
};

class Game
{
public:
	constexpr bool InHangar() const noexcept
	{
		return this->inHanger;
	}

	ViewMatrix& GetViewMatrix() const noexcept
	{
		return *reinterpret_cast<ViewMatrix*>(std::uintptr_t(this) + 0x740);
	}

	UnitList& GetUnitList() const noexcept
	{
		return *reinterpret_cast<UnitList*>(std::uintptr_t(this) + 0x390);
	}

private:
	char pad_0000[88]; //0x0000
	class N0000047D* N00000275; //0x0058
	char pad_0060[120]; //0x0060
	int8_t N00000285; //0x00D8
	char pad_00D9[4]; //0x00D9
	int8_t inHanger; //0x00DD
	char pad_00DE[242]; //0x00DE
	char* map; //0x01D0
	char* time; //0x01D8
	char* weather; //0x01E0
	char pad_01E8[24]; //0x01E8
	char* mission; //0x0200
};
