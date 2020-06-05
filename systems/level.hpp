#pragma once

#include "shaders.hpp"

namespace Systems
{
	class Level
	{
	public:
		Level();

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
