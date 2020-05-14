#pragma once

namespace Components
{
	struct MouseState;
	struct ScreenInfo;
	struct MVP;
	struct Physics;
}

namespace Globals
{
	extern Components::MouseState& mouseState;
	extern Components::ScreenInfo& screenInfo;
	extern Components::MVP& mvp;
	extern Components::Physics& physics;
}
