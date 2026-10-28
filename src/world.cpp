#include <world.h>
#include <rigidBody.h>
#include <circle.h>
#include <shape.h>

void World::addCircle(float radius, float x, float y, int res,float width, float height, glm::vec2 velocity){
    auto body = std::make_unique<RigidBody>();
    body->position = {x, y};
    body->mass = 1.0f;
    body->restitution = 0.7f;
    body->velocity = velocity;
    body->shape = std::make_unique<Circle>(radius, res);
    body->shape->setProjection(width, height);
    bodies.push_back(std::move(body));
}

void World::update(float dt){
    // this is where things like gravity, collision, world borders and things happens
    // world constraints
    applyBorderConstraints();
    // add gravity
    for(auto&body : bodies){
        body->velocity.y += this->gravity * dt;
        body->position += body->velocity * dt;
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

void World::applyBorderConstraints(){
    for(auto&body : bodies){
        float r = body->shape->getBoundingRadius();

        // Left Wall
        if (body->position.x - r < -borderX){
            body->position.x = r - borderX;
            body->velocity.x *= -body->restitution;
        }

        // Right Wall
        if (body->position.x + r > borderX){
            body->position.x = borderX - r;
            body->velocity.x *= -body->restitution;
        }

        // Lower Wall
        if (body->position.y - r < -borderY){
            body->position.y = r - borderY;
            body->velocity.y *= -body->restitution;
            body->velocity.x *= friction;
        }

        // Upper Wall
        if (body->position.y + r > borderY){
            body->position.y = borderY - r;
            body->velocity.y *= -body->restitution;
        }
    }
}