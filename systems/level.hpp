#pragma once

#include <vector>

#include <glm/vec3.hpp>

#include "shaders.hpp"

namespace Systems
{
	class Level
	{
	public:
		Level();

		void step();
		void render() const;

	private:
		void initPhysics() const;
		void initGraphics();

		shaders::ProgramId shadersProgram;
		GLuint vertexArray;
		GLuint vertexBuffer;

		std::vector<glm::vec3> wallsVerticesCache;
	};
}
