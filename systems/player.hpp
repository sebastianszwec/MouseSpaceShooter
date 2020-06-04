#pragma once

#include "shaders.hpp"

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

		shaders::ProgramId shadersProgram;
		GLuint vertexArray;
		GLuint vertexBuffer;
	};
}
