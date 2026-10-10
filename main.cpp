#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include <iostream>

#define MINIAUDIO_IMPLEMENTATION
#include "GameEngine.h"

#define STB_IMAGE_IMPLEMENTATION
#include "Renderer.h"

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    GameEngine *engine = static_cast<GameEngine *>(glfwGetWindowUserPointer(window));

    if (action == GLFW_PRESS && (engine->state == WAITING || engine->state == GAMEOVER))
        engine->resetGame();

    if (engine->state == PLAYING)
    {
        if (key == GLFW_KEY_A && action == GLFW_PRESS)
        {
            if (engine->targetLane > -1)
            {
                engine->targetLane--;
                engine->particles.spawn(glm::vec3(engine->playerX, engine->playerY, 0.0f), 35, glm::vec3(-3.0f, 3.0f, 4.0f), 2.5f, 0.8f);
            }
        }
        if (key == GLFW_KEY_D && action == GLFW_PRESS)
        {
            if (engine->targetLane < 1)
            {
                engine->targetLane++;
                engine->particles.spawn(glm::vec3(engine->playerX, engine->playerY, 0.0f), 35, glm::vec3(3.0f, 3.0f, 4.0f), 2.5f, 0.8f);
            }
        }
        if (key == GLFW_KEY_S && action == GLFW_PRESS)
        {
            // ONLY trigger the big splash if we weren't already diving
            if (!engine->isKeyboardDiving && engine->networkDiveTimer <= 0.0f)
            {
                engine->particles.spawn(glm::vec3(engine->playerX, engine->playerY + 0.2f, 0.0f), 150, glm::vec3(0.0f, 6.0f, 4.0f), 5.0f, 1.5f);
            }
            engine->isKeyboardDiving = true;
        }
        if (key == GLFW_KEY_S && action == GLFW_RELEASE)
        {
            engine->isKeyboardDiving = false;
        }
    }
}

int main()
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "Infinite Swimmer", NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    GameEngine engine;
    glfwSetWindowUserPointer(window, &engine);
    glfwSetKeyCallback(window, key_callback);

    engine.init();

    Renderer renderer;
    renderer.init(window);

    float lastFrameTime = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrameTime = glfwGetTime();
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        engine.processNetwork();
        engine.update(deltaTime);
        renderer.renderFrame(window, engine);
    }

    engine.cleanup();
    renderer.cleanup();
    glfwTerminate();

    return 0;
}