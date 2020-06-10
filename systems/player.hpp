#pragma once

#include "shaders.hpp"

#include <glm/vec2.hpp>

namespace Systems
{
	class Player
	{
	public:
		Player();

		void step() const;
		void render() const;

	private:
		void initPhysics() const;
		void initGraphics();

		void turn(glm::vec2 mouseDelta) const;
		void throttle(bool rmb) const;

		shaders::ProgramId shadersProgram;
		GLuint vertexArray;
		GLuint vertexBuffer;
	};
}
