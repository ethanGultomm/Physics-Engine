#include <world.h>
#include <rigidBody.h>
#include <circle.h>
#include <shape.h>

void World::addCircle(float radius, float x, float y, int res,float width, float height, glm::vec2 velocity, glm::vec4 color){
    auto body = std::make_unique<RigidBody>();
    body->position = {x, y};
    body->positionOld = {x, y}; // position old is used in verlet integration. For init, make it the same as current position
    body->mass = 1.0f;
    body->restitution = 0.7f;
    body->accelerate(velocity);
    body->shape = std::make_unique<Circle>(radius, res, color);
    body->shape->setProjection(width, height);
    bodies.push_back(std::move(body));
}

void World::clearCircles(){
    bodies.clear();
    bodies.shrink_to_fit();
}

void World::update(float dt){
    // this is where things like gravity, collision, world borders and things happens
    applyGravity();
    applyVerletIntegration(dt); // verlet integratio

    // collision detection and resolve ran in multiple substeps
    for (int i = 0; i < 8; i++)
    {
        applyBorderConstraints();
        solveCollision();
    }
}

void World::render(){
    // draw the ball
    for (auto&body : bodies){
        body->shape->draw(body->position);
    }
}

void World::setProjection(float width, float height){
    for(auto& body : bodies){
        body->shape->setProjection(width, height);
    }
}

// Move the objects based on Verlet Integration
void World::applyVerletIntegration(float dt){
    for(auto&body : bodies){
        body->velocity = body->position - body->positionOld;
        body->positionOld = body->position;                                                 // saves position for next time
        body->position = body->position + body->velocity + body->acceleration * (dt * dt);  // performs verlet integration
        body->acceleration = glm::vec2 (0.0f, 0.0f);                                        // resets acceleration
    }
}

void World::applyGravity(){
    for(auto&body : bodies){
        body->accelerate(this->gravity); // all of this body-velocity needs to be changed to accelerate twin
    }
}

void World::applyBorderConstraints(){
    for(auto&body : bodies){
        float r = body->shape->getBoundingRadius();
        glm::vec2 v = body->position - body->positionOld;   // current velocity

        // Left Wall
        if (body->position.x - r < -borderX){
            body->position.x = r - borderX;
            body->positionOld.x = body->position.x + (v.x * body->restitution); // change positionOld so verlet integration flips the velocity
        }

        // Right Wall
        if (body->position.x + r > borderX){
            body->position.x = borderX - r;
            body->positionOld.x = body->position.x + (v.x * body->restitution);
        }

        // Lower Wall
        if (body->position.y - r < -borderY){
            body->position.y = r - borderY;
            body->positionOld.y = body->position.y + (v.y * body->restitution);
            body->velocity.x = body->position.x - (v.x * friction);
        }

        // Upper Wall
        if (body->position.y + r > borderY){
            body->position.y = borderY - r;
            body->velocity.y = body->position.y + (v.y * body->restitution);
        }
    }
}

void World::solveCollision(){
    // checking collision with a brute force 0^n algorithm
    // TODO: switch to a better zone style algorithm
    for (int i = 0; i < bodies.size(); i++)
    {
        for (int j = i+1; j < bodies.size(); j++)
        {
            float ri = bodies[i]->shape->getBoundingRadius();
            float rj = bodies[j]->shape->getBoundingRadius();
            glm::vec2 collisionAxis = bodies[i]->position - bodies[j]->position;
            float distance = glm::length(collisionAxis);
            if(distance < (ri + rj)){
                // calculate the distance of overlap
                glm::vec2 n = collisionAxis / distance;
                float delta = ((ri + rj) - distance) * bodies[i]->restitution;    // TODO: implement what happens when 2 circles has different restitutions

                // move the 2 objects in the axis of collision at equal distance
                glm::vec2 body1 = bodies[i]->position + (0.5f * delta * n);
                glm::vec2 body2 = bodies[j]->position - (0.5f * delta * n);
                bodies[i]->position = body1;
                bodies[j]->position = body2;
            }
        }
    }
}