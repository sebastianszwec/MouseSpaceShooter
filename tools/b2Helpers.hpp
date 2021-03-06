#pragma once

#include <memory>

#include <Box2D/Box2D.h>

#include <glm/vec2.hpp>

struct b2BodyDeleter {
	void operator()(b2Body* body) const;
};

inline b2Vec2 toB2Vec(const glm::vec2 v)
{
	return { v.x, v.y };
}

namespace tools
{
	std::unique_ptr<b2Body, b2BodyDeleter> CreateBoxBody(glm::vec2 position, glm::vec2 hSize, float angle = 0.0f,
		b2BodyType bodyType = b2_staticBody, float density = 1.0f);
	void PinBodies(b2Body& body1, b2Body& body2, glm::vec2 pinPoint, bool collideConnected = false);
}
