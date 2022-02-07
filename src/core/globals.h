#pragma once
#include <array>
#include <string_view>

class LocalPlayer;
class PlayerList;
class Game;

namespace g
{
	inline LocalPlayer* local = nullptr;
	inline PlayerList* list = nullptr;
	inline Game* game = nullptr;
}

struct Variables
{
	struct Visuals
	{
		bool enabled = true;
		float max_dist = 30.f;

		const std::array<std::string_view, 2U> visuals_type
		{
			"box",
			"circle"
		};
		int type = 0;

		std::array <float, 4U> color_fighter = { 1.f, 1.f, 0.f, 1.f };
		std::array <float, 4U> color_attacker = { 0.f, 1.f, 0.f, 1.f };
		std::array <float, 4U> color_bomber = { 0.f, 0.f, 1.f, 1.f };
	}visuals;

	struct Misc
	{
		bool never_spotted = true;

	}misc;
};

inline Variables vars;
