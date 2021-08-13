// TODO License

#include "box2d/b2_geometry.h"
#include "box2d/b2_edge_shape.h"

void CreateLinks(b2Body* b, b2FixtureDef* fd, const b2Vec2* vertices, int32 count, bool closed, const b2Vec2 prevVertex, const b2Vec2 nextVertex) {
	b2Assert(closed? count >= 3 : count >= 2);

	const b2Shape* oldShape = fd->shape;

	for (int32 i = 1; i < count; ++i) {
		// If the code crashes here, it means your vertices are too close together.
		b2Assert(b2DistanceSquared(vertices[i - 1], vertices[i]) > b2_linearSlop * b2_linearSlop);
		
		b2Vec2 v0 = (i > 1)? vertices[i - 2] : prevVertex;
		b2Vec2 v3 = (i < count - 1)? vertices[i + 1] : nextVertex;

		b2EdgeShape shape;
		shape.SetOneSided(v0, vertices[i - 1], vertices[i], v3);
		
		fd->shape = &shape;
		b->CreateFixture(fd);
	}
	
	if (closed) {
		b2EdgeShape shape;
		shape.SetOneSided(vertices[count - 2], vertices[count - 1], vertices[0], vertices[1]);

		fd->shape = &shape;
		b->CreateFixture(fd);
	}

	fd->shape = oldShape;
}

void b2CreateLoop(b2Body* b, b2FixtureDef* fd, const b2Vec2* vertices, int32 count) {
	CreateLinks(b, fd, vertices, count, true, vertices[count - 1], vertices[0]);
}

void b2CreateChain(b2Body* b, b2FixtureDef* fd, const b2Vec2* vertices, int32 count, const b2Vec2& prevVertex, const b2Vec2& nextVertex) {
	CreateLinks(b, fd, vertices, count, false, prevVertex, nextVertex);
}

void b2CreateLoop(b2Body* b, const b2Vec2* vertices, int32 count) {
	b2FixtureDef fd;
	CreateLinks(b, &fd, vertices, count, true, vertices[count - 1], vertices[0]);
}

void b2CreateChain(b2Body* b, const b2Vec2* vertices, int32 count, const b2Vec2& prevVertex, const b2Vec2& nextVertex) {
	b2FixtureDef fd;
	CreateLinks(b, &fd, vertices, count, false, prevVertex, nextVertex);
}
