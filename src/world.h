#pragma once
#include <vector>
#include <memory>
#include <rigidBody.h>

struct Cell
{
    // size and location is lowkirkenuenly not needed like ever but who knows so im leaving this in lol
    float size;
    glm::vec2 location;
    std::vector<RigidBody*> bodies;    // list of bodies within the cell
};

class World{
    private:
    std::vector<std::unique_ptr<RigidBody>> bodies;
    std::vector<std::vector<Cell>> grid;    // 2d vector for the spatial grid for collision detection thingy
    int gridRows, gridCols;
    glm::vec2 gravity = glm::vec2 (0.0f, -1.0f);
    float gridRadius;
    float borderX = 1.0f, borderY = 0.85f;
    float friction = 0.7f;
    float gravityStrength = 2.0f;
    bool gravityMode = true;


    public:
    void setGrid(float radius);
    void clearGrid();
    void setBodiesToGrid();
    void addCircle(float radius, float x, float y, int res,float width, float height, glm::vec2 velocity, glm::vec4 color = glm::vec4(0.96f, 0.19f, 0.29f, 1.0f));
    void clearCircles();
    void update(float dt);
    void render();
    void setProjection(float width, float height);
    void switchGravity() {gravityMode = !gravityMode;}
    int getBodiesCount() {return bodies.size();}
    
    // all of these can maybe be put in a different file maybe idk
    void applyBorderConstraints();
    void applyDownGravity();
    void applyGravityAtCenter();
    void applyRealGravity();
    void solveCollision(RigidBody* bodyA, RigidBody* bodyB);
    void checkCollisionBruteForce();
    void checkCollisionSpatialGrid();
    void checkCellCollisions(Cell* cellA, Cell* cellB);
    void applyVerletIntegration(float dt);
};