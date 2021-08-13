// TODO License

#ifndef B2_GEOMETRY_H
#define B2_GEOMETRY_H

#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"

void b2CreateLoop(b2Body* b, b2FixtureDef* fd, const b2Vec2* vertices, int32 count);
void b2CreateLoop(b2Body* b, const b2Vec2* vertices, int32 count);
void b2CreateChain(b2Body* b, b2FixtureDef* fd, const b2Vec2* vertices, int32 count, const b2Vec2 prevVertex, const b2Vec2 nextVertex);
void b2CreateChain(b2Body* b, const b2Vec2* vertices, int32 count, const b2Vec2 prevVertex, const b2Vec2 nextVertex);

#endif
