#pragma once

#include <glm/vec2.hpp>

namespace Components
{
	struct MouseState
	{
		bool lmb{ false };
		bool rmb{ false };
		glm::ivec2 position{ 0, 0 };
		glm::ivec2 prevPosition{ 0, 0 };
		
		glm::ivec2 getMouseDelta() const
		{
			return position - prevPosition;
		}
	};
}
