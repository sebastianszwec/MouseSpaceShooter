#pragma once

#include <memory>

#include <b2Helpers.hpp>

namespace Components
{
	struct Player
	{
		std::unique_ptr<b2Body, b2BodyDeleter> body;
	};
}
