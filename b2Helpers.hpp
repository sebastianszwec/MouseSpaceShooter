#pragma once

#include <Box2D/Box2D.h>

struct b2BodyDeleter {
	void operator()(b2Body* body) const {
		body->GetWorld()->DestroyBody(body);
	}
};
