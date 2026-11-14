#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <iostream>
#include <chrono>
#include <vector>
#include <stdlib.h>
#include <cmath>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <world.h>
using namespace std;

static void error_callback(int error, const char* description){
    fprintf(stderr, "Error: %s\n", description);
}

void processInput(GLFWwindow* window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
    // update projection
    auto* world = static_cast<World*>(glfwGetWindowUserPointer(window));

    if(world){
        world->setProjection((float)width, (float)height);
    }
}

//                                 R     B     G
glm::vec4 circleColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
int state = 0;
int increment = 50;
float incrementValue = 1.0f / increment;

// increments the color in a rainbow fashion
void colorWheel(){
    switch (state)
    {
    case 0: // If RED is the only one max = add blue
        circleColor[1] += incrementValue;
        if (circleColor[1] >= 1.0f) {circleColor[1] = 1.0f; state = 1;}
        break;
    
    case 1: // if RED is max and BLUE is max = lower red
        circleColor[0] -= incrementValue;
        if (circleColor[0] <= 0.0f) {circleColor[0] = 0.0f; state = 2;}
        break;
    
    case 2: // if BLUE is the only one max = add green
        circleColor[2] += incrementValue;
        if (circleColor[2] >= 1.0f) {circleColor[2] = 1.0f; state = 3;}
        break;
    
    case 3: //if BLUE and GREEN is max = lower blue
        circleColor[1] -= incrementValue;
        if (circleColor[1] <= 0.0f) {circleColor[1] = 0.0f; state = 4;}
        break;
    
    case 4: // if GREEN is the only one max = add red
        circleColor[0] += incrementValue;
        if (circleColor[0] >= 1.0f) {circleColor[0] = 1.0f; state = 5;}
        break;

    case 5: // if GREEN and RED is max = lower green
        circleColor[2] -= incrementValue;
        if (circleColor[2] <= 0.0f) {circleColor[2] = 0.0f; state = 0;}
        break;
    default:
        break;
    }
}

int main(){
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLfloat windowWidth = 1300.0f;
    GLfloat windowHeight = 1000.0f;

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Physics Engine", NULL, NULL);
    if (!window){
        glfwTerminate();
        cerr << "creating window failed" << endl;
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        cerr << "failed to initialize glad" << endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSwapInterval(1);

    glViewport(0, 0, windowWidth, windowHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);  // true = ImGui installs GLFW callbacks
    ImGui_ImplOpenGL3_Init("#version 330");      // match the GLSL version your shaders use

    // the circles parameters
    int const res = 50;
    GLfloat radius = 10.0f;    // value in pixels
    radius = radius / (windowHeight / 2);   //convert to absolute value (0.0 - 1.0)

    World world;
    glfwSetWindowUserPointer(window, &world);

    double prevTime = glfwGetTime();
    const double frameTime = 1.0 / 60;  // 60hz
    int ballsCount = 0;
    int frameCount = 0;
    int angleCount = 0;
    // MAIN LOOP
    while (!glfwWindowShouldClose(window)){
        // input
        processInput(window);

        double currentTime = glfwGetTime();
        // updates eevery frameTime
        if ((currentTime - prevTime) >= frameTime){
            // render commands here
            glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // spamming circles in a circular pattern
            if(frameCount == 2){
                glm::vec2 velocity;
                float angle = 2.0f * 3.14159265359 * (static_cast<float>(angleCount) / 50);
                velocity.x = cos(angle) * 100.0;
                velocity.y = sin(angle) * 100.0;
                world.addCircle(radius, 0.0f, 0.0f, res, windowWidth, windowHeight, velocity, circleColor);
                colorWheel(); // increments the color in a wheel fashion
                angleCount++;
                frameCount = 0;
                
            }
            
            world.update(frameTime);
            world.render();
            prevTime = glfwGetTime();

            // ImGui stuff
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Stats");
            ImGui::Text("total circles: %zu", world.bodies.size());
            ImGui::Text("fps: %.1f", ImGui::GetIO().Framerate);
            if (ImGui::Button("clear circles")) {world.clearCircles();}
            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // check and call events and swap the buffers
            glfwSwapBuffers(window);
            glfwPollEvents();
            frameCount++;
        }
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

// TODO: 
// -create a physics module that will contain all phyics