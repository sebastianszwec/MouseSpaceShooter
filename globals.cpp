#include "components/mouseState.hpp"
#include "components/screenInfo.hpp"
#include "components/mvp.hpp"

namespace
{
	Components::MouseState mouseState;
	Components::ScreenInfo screenInfo;
	Components::MVP mvp;
}

namespace Globals
{
	Components::MouseState& mouseState = ::mouseState;
	Components::ScreenInfo& screenInfo = ::screenInfo;
	Components::MVP& mvp = ::mvp;
}
