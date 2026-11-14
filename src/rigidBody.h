#pragma once
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <shape.h>

using namespace std;

class RigidBody{
  public:
    glm::vec2 position;
    glm::vec2 positionOld;
    glm::vec2 acceleration;
    glm::vec2 velocity;
    float mass;
    float restitution;    // how bouncy it is
    void accelerate(glm::vec2 acc){ acceleration += acc;}
    glm::vec2 getVelocity(){return velocity;}
    std::unique_ptr<Shape> shape;
};