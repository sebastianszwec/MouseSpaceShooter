#include "physics.hpp"

#include <globals.hpp>

#include <components/physics.hpp>

namespace Systems
{
	Physics::Physics()
	{
		using namespace Globals::Components;
	}

	void Physics::step() const
	{
		using namespace Globals::Components;

		physics.world.Step(1.0f / 60, 3, 8);
	}
}
