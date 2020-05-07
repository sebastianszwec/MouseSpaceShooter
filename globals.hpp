#pragma once

namespace Components
{
	struct MouseState;
	struct ScreenInfo;
	struct MVP;
}

namespace Globals
{
	extern Components::MouseState& mouseState;
	extern Components::ScreenInfo& screenInfo;
	extern Components::MVP& mvp;
}
