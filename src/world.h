#pragma once
#include <vector>
#include <stdlib.h>
#include <memory>
#include <rigidBody.h>

class World{
    public:
    std::vector<std::unique_ptr<RigidBody>> bodies;
    glm::vec2 gravity = glm::vec2 (0.0f, -1.0f);
    float borderX = 1.0f, borderY = 0.85f;
    float friction = 0.7f;
    void addCircle(float radius, float x, float y, int res,float width, float height, glm::vec2 velocity, glm::vec4 color = glm::vec4(0.96f, 0.19f, 0.29f, 1.0f));
    void clearCircles();
    void update(float dt);
    void render();
    void setProjection(float width, float height);
    void applyBorderConstraints();
    void applyGravity();
    void solveCollision();
    void applyVerletIntegration(float dt);
};