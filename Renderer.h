#ifndef RENDERER_H
#define RENDERER_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "GameEngine.h"
#include "Model.h"
#include "Shader.h"
#include "UIManager.h"
#include "stb_image.h"

const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "layout (location = 1) in vec2 aTexCoord;\n"
                                 "out vec2 TexCoord;\n"
                                 "out vec3 WorldPos;\n"
                                 "uniform mat4 model;\n"
                                 "uniform mat4 view;\n"
                                 "uniform mat4 projection;\n"
                                 "uniform float textureScroll;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
                                 "   TexCoord = vec2(aTexCoord.x, aTexCoord.y + textureScroll);\n"
                                 "   WorldPos = vec3(model * vec4(aPos, 1.0));\n"
                                 "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "in vec2 TexCoord;\n"
                                   "in vec3 WorldPos;\n"
                                   "uniform sampler2D texture1;\n"
                                   "uniform vec4 objectColor;\n"
                                   "uniform bool useTexture;\n"
                                   "uniform bool isFloor;\n"
                                   "uniform bool useLighting;\n"
                                   "uniform vec3 cameraPos;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   vec4 finalColor;\n"
                                   "   if(useTexture) {\n"
                                   "       vec4 texColor = texture(texture1, TexCoord) * objectColor;\n"
                                   "       if(isFloor) {\n"
                                   "           if(abs(WorldPos.x - (-1.0)) < 0.05 || abs(WorldPos.x - (1.0)) < 0.05) {\n"
                                   "               texColor = mix(texColor, vec4(1.0, 1.0, 1.0, texColor.a), 0.8);\n"
                                   "           }\n"
                                   "           if(abs(WorldPos.x - (-3.0)) < 0.1 || abs(WorldPos.x - (3.0)) < 0.1) {\n"
                                   "               texColor = mix(texColor, vec4(0.8, 0.1, 0.1, texColor.a), 0.9);\n"
                                   "           }\n"
                                   "       }\n"
                                   "       finalColor = texColor;\n"
                                   "   } else {\n"
                                   "       finalColor = objectColor;\n"
                                   "   }\n"
                                   "   if(useLighting) {\n"
                                   "       vec3 dx = dFdx(WorldPos);\n"
                                   "       vec3 dy = dFdy(WorldPos);\n"
                                   "       vec3 normal = normalize(cross(dx, dy));\n"
                                   "       vec3 lightDir = normalize(vec3(0.2, 1.0, 0.5));\n"
                                   "       float diff = max(dot(normal, lightDir), 0.6);\n"
                                   "       finalColor.rgb *= diff;\n"
                                   "   }\n"
                                   "   float distance = length(cameraPos - WorldPos);\n"
                                   "   float fogFactor = clamp((distance - 15.0) / 35.0, 0.0, 1.0);\n"
                                   "   vec4 fogColor = vec4(0.5, 0.8, 0.9, 1.0);\n"
                                   "   FragColor = mix(finalColor, fogColor, fogFactor);\n"
                                   "}\n\0";

class Renderer
{
public:
    Shader *shader;
    UIManager ui;
    unsigned int waterTexture, boxTexture, tileTexture;
    unsigned int cubeVAO, cubeVBO, floorVAO, floorVBO;
    Model *dolphinModel;

    unsigned int loadTexture(char const *path)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);

        if (data)
        {
            GLenum format = (nrComponents == 4) ? GL_RGBA : GL_RGB;
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Failed to load texture: " << path << std::endl;
            stbi_image_free(data);
        }
        return textureID;
    }

    void init(GLFWwindow *window)
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        waterTexture = loadTexture("../assets/water.png");
        boxTexture = loadTexture("../assets/box.jpg");
        tileTexture = loadTexture("../assets/tiles.jpg");

        dolphinModel = new Model("../assets/dolphin.obj");
        shader = new Shader(vertexShaderSource, fragmentShaderSource);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        float cubeVertices[] = {
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f, -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f, -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};

        float floorVertices[] = {
            20.0f, 0.0f, 20.0f, 20.0f, 0.0f,
            -20.0f, 0.0f, 20.0f, 0.0f, 0.0f,
            -20.0f, 0.0f, -60.0f, 0.0f, 40.0f,
            20.0f, 0.0f, 20.0f, 20.0f, 0.0f,
            -20.0f, 0.0f, -60.0f, 0.0f, 40.0f,
            20.0f, 0.0f, -60.0f, 20.0f, 40.0f};

        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glGenVertexArrays(1, &floorVAO);
        glGenBuffers(1, &floorVBO);
        glBindVertexArray(floorVAO);
        glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    void renderFrame(GLFWwindow *window, GameEngine &engine)
    {
        glClearColor(0.5f, 0.8f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ui.renderHUD(engine);

        shader->use();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, -7.0f));

        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
        shader->setVec3("cameraPos", 0.0f, 1.0f, 7.0f);

        // 1: OPAQUE TILE FLOOR
        shader->setBool("useTexture", true);
        shader->setBool("isFloor", true);
        shader->setBool("useLighting", false);
        shader->setFloat("textureScroll", engine.worldScrollOffset);
        shader->setVec4("objectColor", 1.0f, 1.0f, 1.0f, 1.0f);
        glBindTexture(GL_TEXTURE_2D, tileTexture);
        glBindVertexArray(floorVAO);
        glm::mat4 tileModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.5f, 0.0f));
        shader->setMat4("model", tileModel);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // 2: OPAQUE PLAYER (DOLPHIN MODEL)
        float time = glfwGetTime();
        shader->setFloat("textureScroll", 0.0f);
        shader->setBool("useTexture", false);
        shader->setBool("isFloor", false);
        shader->setBool("useLighting", true);

        shader->setVec4("objectColor", 0.55f, 0.6f, 0.65f, 1.0f);

        glm::mat4 playerModel = glm::translate(glm::mat4(1.0f), glm::vec3(engine.playerX, engine.playerY + sin(time * 8.0f) * 0.05f, 0.0f));
        playerModel = glm::scale(playerModel, glm::vec3(1.2f));

        playerModel = glm::rotate(playerModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        playerModel = glm::rotate(playerModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        playerModel = glm::rotate(playerModel, sin(time * 12.0f) * 0.15f, glm::vec3(1.0f, 0.0f, 0.0f));

        shader->setMat4("model", playerModel);
        dolphinModel->draw();

        // 3: OPAQUE OBSTACLES
        if (engine.state == PLAYING)
        {
            shader->setBool("useTexture", true);
            shader->setBool("useLighting", true);
            glBindTexture(GL_TEXTURE_2D, boxTexture);
            shader->setVec4("objectColor", 1.0f, 1.0f, 1.0f, 1.0f);
            glBindVertexArray(cubeVAO);
            for (const auto &obs : engine.obstacles.obstacles)
            {
                glm::mat4 obsModel = glm::translate(glm::mat4(1.0f), glm::vec3(obs.lane * engine.obstacles.LANE_WIDTH, 0.0f, obs.zPos));
                shader->setMat4("model", obsModel);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        // 4: TRANSPARENT WATER SURFACE
        shader->setBool("useTexture", true);
        shader->setBool("isFloor", false);
        shader->setBool("useLighting", false);
        shader->setFloat("textureScroll", engine.worldScrollOffset * 1.2f);
        shader->setVec4("objectColor", 1.0f, 1.0f, 1.0f, 0.6f);
        glBindTexture(GL_TEXTURE_2D, waterTexture);
        glBindVertexArray(floorVAO);
        glm::mat4 waterModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.2f, 0.0f));
        shader->setMat4("model", waterModel);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // 5: TRANSPARENT WATER SPRAY DROPLETS
        shader->setBool("useTexture", false);
        shader->setBool("useLighting", true);

        shader->setVec4("objectColor", 1.0f, 1.0f, 1.0f, 0.85f);

        glBindVertexArray(cubeVAO);
        for (const auto &p : engine.particles.particles)
        {
            glm::mat4 pModel = glm::translate(glm::mat4(1.0f), p.position);
            pModel = glm::scale(pModel, glm::vec3(0.01f, 0.01f, 0.01f));
            shader->setMat4("model", pModel);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    void cleanup()
    {
        delete dolphinModel;
        delete shader;
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glDeleteVertexArrays(1, &cubeVAO);
        glDeleteVertexArrays(1, &floorVAO);
        glDeleteBuffers(1, &cubeVBO);
        glDeleteBuffers(1, &floorVBO);
    }
};

#endif