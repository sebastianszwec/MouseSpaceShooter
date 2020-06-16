#pragma once

#include <vector>

namespace Components
{
	struct MouseState;
	struct ScreenInfo;
	struct MVP;
	struct Physics;
	struct Player;
	struct Wall;
}

namespace Systems
{
	class Physics;
	class Player;
	class Level;
}

namespace Globals
{
	namespace Components
	{
		extern ::Components::MouseState& mouseState;
		extern ::Components::ScreenInfo& screenInfo;
		extern ::Components::MVP& mvp;
		extern ::Components::Physics& physics;
		extern ::Components::Player& player;

		extern std::vector<::Components::Wall>& walls;
	}

	namespace Systems
	{
		void Initialize();

		::Systems::Physics& AccessPhysics();
		::Systems::Player& AccessPlayer();
		::Systems::Level& AccessLevel();
	}

	namespace Defaults
	{
		constexpr float mouseSensitivity = 0.01f;
		constexpr float hProjectionSize = 25.0f;
		constexpr float thrust = 15.0f;
	}
}
