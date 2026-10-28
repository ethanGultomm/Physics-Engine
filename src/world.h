#pragma once
#include <vector>
#include <stdlib.h>
#include <memory>
#include <rigidBody.h>

class World{
    public:
    std::vector<std::unique_ptr<RigidBody>> bodies;
    float gravity = -2.7f;
    float borderX = 1.0f, borderY = 0.85f;
    float friction = 0.7f;
    void addCircle(float radius, float x, float y, int res,float width, float height, glm::vec2 velocity);
    void update(float dt);
    void render();
    void setProjection(float width, float height);
    void applyBorderConstraints();
};