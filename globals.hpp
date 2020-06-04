#pragma once

namespace Components
{
	struct MouseState;
	struct ScreenInfo;
	struct MVP;
	struct Physics;
	struct Player;
}

namespace Systems
{
	class Player;
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
	}

	namespace Systems
	{
		void Initialize();

		::Systems::Player& AccessPlayer();
	}

	namespace Defaults
	{
		constexpr float mouseSensitivity = 0.01f;
		constexpr float hProjectionSize = 25.0f;
	}
}
