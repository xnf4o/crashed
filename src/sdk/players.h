#pragma once
#include "prop.h"

#include <array>

class UnitInfo
{
public:
	char* typeName;
	char* modelPath;
	char* blkPath;
	char* fullName;
	char* shortName;
	char* vehicleTypeShort;
	char* vehicleType;
	char* nationName;
};

class Unit : public DagorPropContainer
{
	enum UnitState : std::uint8_t
	{
		ALIVE = 0,
		DEAD = 1,
		SPECTATING = 3,
	};

	// right-left
	// 4th bit = spotted
	enum UnitFlags : std::uint32_t
	{
		SPOTTED = 0x8,
	};

	enum LowRateUnitFlags : std::uint8_t
	{
		FIGHTER = 8,
		BOMBER = 16,
		ATTACKER = 64
	};

public:
	bool IsSpotted() const noexcept
	{
		return GetUnitFlags() & UnitFlags::SPOTTED;
	}

	bool IsAlive() const noexcept
	{
		return GetUnitState() == UnitState::ALIVE;
	}

public:
	Vector3& GetBBMin() const noexcept
	{
		return *reinterpret_cast<Vector3*>(std::uintptr_t(this) + 0x200);
	}

	Vector3& GetBBMax() const noexcept
	{
		return *reinterpret_cast<Vector3*>(std::uintptr_t(this) + 0x20C);
	}

	Matrix3x3& GetRotationMatrix() const noexcept
	{
		return *reinterpret_cast<Matrix3x3*>(std::uintptr_t(this) + 0x89C);
	}

	Vector3& GetPosition() const noexcept
	{
		return *reinterpret_cast<Vector3*>(std::uintptr_t(this) + 0x8C0);
	}

	class Player* GetPlayer() const noexcept
	{
		return *reinterpret_cast<class Player**>(std::uintptr_t(this) + 0xFE0);
	}

	UnitInfo* GetUnitInfo() const noexcept
	{
		return *reinterpret_cast<UnitInfo**>(std::uintptr_t(this) + 0x1038);
	}

	std::uint8_t& GetUnitState() const noexcept
	{
		static const auto offset = std::uintptr_t(Find("unitState")) - std::uintptr_t(this) + 0x20;
		return *reinterpret_cast<std::uint8_t*>(std::uintptr_t(this) + offset);
	}

	std::uint32_t& GetUnitFlags() const noexcept
	{
		static const auto offset = std::uintptr_t(Find("unitFlags")) - std::uintptr_t(this) + 0x20;
		return *reinterpret_cast<std::uint32_t*>(std::uintptr_t(this) + offset);
	}

	std::uint8_t& GetLowRateUnitFlags() const noexcept
	{
		static const auto offset = std::uintptr_t(Find("lowRateUnitFlags")) - std::uintptr_t(this) + 0x20;
		return *reinterpret_cast<std::uint8_t*>(std::uintptr_t(this) + offset);
	}

	std::uint8_t& GetUnitArmyNumber() const noexcept
	{
		static const auto offset = std::uintptr_t(Find("unitArmyNo")) - std::uintptr_t(this) + 0x20;
		return *reinterpret_cast<std::uint8_t*>(std::uintptr_t(this) + offset);
	}

	Vector3& GetAirVelocity() const noexcept
	{
		return *reinterpret_cast<Vector3*>(std::uintptr_t(this) + 0x2A548);
	}
};

class Player : public DagorPropContainer
{
	enum GuiState : std::uint8_t
	{
		NONE = 0,
		MENU = 1,
		ALIVE = 2,
		DEAD = 3,
		SPEC = 6,
		LIMBO = 8,
		LOADING = 10
	};

	enum State : std::uint8_t
	{
		STATE_MENU = 0,
		STATE_LIMBO = 6,
		STATE_ALIVE = 7
	};

public:
	bool IsAlive() const noexcept
	{
		return GetGuiState() == GuiState::ALIVE;
	}

public:
	const char* GetFullName() const noexcept
	{
		return *reinterpret_cast<const char**>(std::uintptr_t(this) + 0x60);
	}

	std::string GetUserId() const noexcept
	{
		static const auto offset = std::uintptr_t(Find("uid")) - std::uintptr_t(this) + 0x28;
		return std::string{ reinterpret_cast<const char*>(std::uintptr_t(this) + offset), 64 };
	}

	const char* GetClanTag() const noexcept
	{
		static const auto offset = std::uintptr_t(Find("clanTag")) - std::uintptr_t(this) + 0x20;
		return *reinterpret_cast<const char**>(std::uintptr_t(this) + offset);
	}

	const char* GetTitle() const noexcept
	{
		static const auto offset = std::uintptr_t(Find("title")) - std::uintptr_t(this) + 0x20;
		return *reinterpret_cast<const char**>(std::uintptr_t(this) + offset);
	}

	Unit* GetOwnedUnit() const noexcept
	{
		static const auto offset = std::uintptr_t(Find("ownedUnitOID")) - std::uintptr_t(this) + 0x28;
		return *reinterpret_cast<Unit**>(std::uintptr_t(this) + offset);
	}

	std::uint8_t& GetTeam() const noexcept
	{
		static const auto offset = std::uintptr_t(Find("team")) - std::uintptr_t(this) + 0x20;
		return *reinterpret_cast<std::uint8_t*>(std::uintptr_t(this) + offset);
	}

	float& GetTimeToRespawn() const noexcept
	{
		static const auto offset = std::uintptr_t(Find("timeToRespawn")) - std::uintptr_t(this) + 0x28;
		return *reinterpret_cast<float*>(std::uintptr_t(this) + offset);
	}

	std::uint8_t& GetCoutryId() const noexcept
	{
		static const auto offset = std::uintptr_t(Find("countryId")) - std::uintptr_t(this) + 0x20;
		return *reinterpret_cast<std::uint8_t*>(std::uintptr_t(this) + offset);
	}

	std::uint8_t& GetGuiState() const noexcept
	{
		static const auto offset = std::uintptr_t(Find("guiState")) - std::uintptr_t(this) + 0x20;
		return *reinterpret_cast<std::uint8_t*>(std::uintptr_t(this) + offset);
	}

	std::uint8_t& GetState() const noexcept
	{
		static const auto offset = std::uintptr_t(Find("state")) - std::uintptr_t(this) + 0x20;
		return *reinterpret_cast<std::uint8_t*>(std::uintptr_t(this) + offset);
	}
};

class LocalPlayer
{
public:
	class Player* player;
};

class Entity
{
public:
	std::uint64_t unknown;
	class Player* player;
};

class EntityList
{
public:
	class Entity list[64];
};

class PlayerList
{
public:
	class EntityList* entityList; //0x0000
	std::uint64_t playerCount; //0x0008
	char pad_0010[208]; //0x0010
	class Player* localPlayer; //0x00E0
};

