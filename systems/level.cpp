#include "level.hpp"

#include <Box2D/Box2D.h>

#include <glm/gtc/type_ptr.hpp>

#include <globals.hpp>

#include <components/wall.hpp>
#include <components/physics.hpp>
#include <components/mvp.hpp>

namespace Systems
{
	Level::Level()
	{
		initPhysics();
		initGraphics();
	}

	void Level::initPhysics() const
	{
		using namespace Globals::Components;
	}

	void Level::initGraphics()
	{
		using namespace Globals::Components;

		shadersProgram = shaders::LinkProgram(shaders::CompileShaders("shaders/basic.vs", "shaders/basic.fs"),
			{ {0, "bPos"} });

		glCreateVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);
	}

	void Level::step()
	{
		using namespace Globals::Components;

		staticWallsVerticesCache.clear();
		for (auto& staticWall : staticWalls)
		{
			staticWall.body->SetTransform(staticWall.body->GetPosition(), staticWall.body->GetAngle() + 0.01f);
			staticWall.updateVerticesCache();
			staticWallsVerticesCache.insert(staticWallsVerticesCache.end(), staticWall.verticesCache.begin(), staticWall.verticesCache.end());
		}

		glBindVertexArray(vertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

		glBufferData(GL_ARRAY_BUFFER, staticWallsVerticesCache.size() * sizeof(staticWallsVerticesCache.front()),
			staticWallsVerticesCache.data(), GL_STATIC_DRAW);
	}

	void Level::render() const
	{
		glUseProgram(shadersProgram);
		glUniformMatrix4fv(glGetUniformLocation(shadersProgram, "mvp"), 1, GL_FALSE,
			glm::value_ptr(Globals::Components::mvp.getVP()));

		glBindVertexArray(vertexArray);
		glDrawArrays(GL_TRIANGLES, 0, staticWallsVerticesCache.size());
	}
}
