#include "components/mouseState.hpp"
#include "components/screenInfo.hpp"
#include "components/mvp.hpp"
#include "components/physics.hpp"

namespace
{
	Components::MouseState mouseState;
	Components::ScreenInfo screenInfo;
	Components::MVP mvp;
	Components::Physics physics;
}

namespace Globals
{
	Components::MouseState& mouseState = ::mouseState;
	Components::ScreenInfo& screenInfo = ::screenInfo;
	Components::MVP& mvp = ::mvp;
	Components::Physics& physics = ::physics;
}
