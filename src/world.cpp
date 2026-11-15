#include <world.h>
#include <rigidBody.h>
#include <circle.h>
#include <shape.h>

// Sets the grid for the collision checks.
// radius: the radius of each square cell in absolute value (1.0 - 0.0)
void World::setGrid(float radius){
    gridRadius = radius;
    gridRows = ((borderX * 2) / radius) + 1;
    gridCols   = ((borderY * 2) / radius) + 1;
    grid.resize(gridRows, std::vector<Cell>(gridCols));    
}

// Clears the grid of any assigned bodies.
void World::clearGrid(){
    for(auto& row : grid){
        for(auto& col : row){
            col.bodies.clear();
        }
    }
}

void World::setBodiesToGrid(){
    // resets all cell to free it
    clearGrid();

    for(auto&body : bodies){
        // calculate which cell does the circle lay in
        int rows = static_cast<int>((body->position.x + borderX) / (gridRadius * 2));
        int cols = static_cast<int>((body->position.y + borderY) / (gridRadius * 2));

        grid[rows][cols].bodies.push_back(body.get());
    }
}

void World::addCircle(float radius, float x, float y, int res,float width, float height, glm::vec2 velocity, glm::vec4 color){
    auto body = std::make_unique<RigidBody>();
    body->position = {x, y};
    body->positionOld = {x, y}; // position old is used in verlet integration. For init, make it the same as current position
    body->mass = 1.0f;
    body->restitution = 0.3f;
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
    // this is where things like gravity, collision, world borders and stuff happens
    if (gravityMode){applyDownGravity();}
    else {applyGravityAtCenter();}

    applyVerletIntegration(dt);

    // collision detection and resolve ran in multiple substeps
    for (int i = 0; i < 8; i++)
    {
        applyBorderConstraints();
        setBodiesToGrid();
        checkCollisionSpatialGrid();
        //checkCollisionBruteForce();
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

void World::applyDownGravity(){
    for(auto&body : bodies){
        body->accelerate(this->gravity); // all of this body-velocity needs to be changed to accelerate twin
    }
}

// applying a gravity like pull at the center of the screen to all objects
void World::applyGravityAtCenter(){
    for(auto&body : bodies){
        glm::vec2 toCenter = glm::vec2 (0.0f, 0.0f) - body->position;
        float distance = glm::length(toCenter);
        distance = std::max(distance, 0.01f);
        body->accelerate((toCenter/distance) * (gravityStrength / distance*distance));
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

void World::solveCollision(RigidBody* bodyA, RigidBody* bodyB){
    float ra = bodyA->shape->getBoundingRadius();
    float rb = bodyB->shape->getBoundingRadius();
    glm::vec2 collisionAxis = bodyA->position - bodyB->position;
    float distance = glm::length(collisionAxis);
    if(distance < (ra + rb)){
        // calculate the distance of overlap
        glm::vec2 n = collisionAxis / distance;
        float delta = ((ra + rb) - distance) * bodyA->restitution;    // TODO: implement what happens when 2 circles has different restitutions

        // move the 2 objects in the axis of collision at equal distance
        glm::vec2 body1 = bodyA->position + (0.5f * delta * n);
        glm::vec2 body2 = bodyB->position - (0.5f * delta * n);
        bodyA->position = body1;
        bodyB->position = body2;
    }
}

// Check and resolve collisions with a brute force O^n algorithm.
void World::checkCollisionBruteForce(){
    for (int i = 0; i < bodies.size(); i++)
    {
        for (int j = i+1; j < bodies.size(); j++)
        {
            solveCollision(bodies[i].get(), bodies[j].get());
        }
    }
}

// Check and resolve collisions with a spatial grid algorithm.
void World::checkCollisionSpatialGrid(){
    for (int x=0; x < gridRows; x++){
        for(int y=0; y < gridCols; y++){
            auto* currentCell = &grid[x][y];
            // iterate on all surrounding cell, including itself
            for(int dx = -1; dx <= 1; dx++){
                for(int dy = -1; dy <= 1; dy++){
                    int otherX = x+dx;
                    int otherY = y+dy;
                    if((otherX >= 0 && otherX < gridRows) && (otherY >= 0 && otherY < gridCols)){
                        auto* otherCell = &grid[x+dx][y+dy];
                        checkCellCollisions(currentCell, otherCell);
                    }
                }
            }
        }
    }
}

void World::checkCellCollisions(Cell* cellA, Cell* cellB){
    for(auto* bodyA : cellA->bodies){
        for(auto* bodyB : cellB->bodies){
            if(bodyA != bodyB){
                solveCollision(bodyA, bodyB);
            }
        }
    }
}