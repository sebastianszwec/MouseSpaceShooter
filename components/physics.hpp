#pragma once

#include <Box2D/Box2D.h>

namespace Components
{
	struct Physics
	{
		b2World world{ {0.0f, 0.0f} };
	};
}
