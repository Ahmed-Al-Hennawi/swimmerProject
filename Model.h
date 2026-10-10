#ifndef MODEL_H
#define MODEL_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

class Model
{
public:
    unsigned int VAO, VBO;
    int vertexCount;

    Model(const std::string &path)
    {
        loadOBJ(path);
    }

    void draw()
    {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }

private:
    void loadOBJ(const std::string &path)
    {
        std::vector<glm::vec3> temp_vertices;
        std::vector<glm::vec2> temp_uvs;
        std::vector<float> vertexData;

        glm::vec3 minBounds(1e9f);
        glm::vec3 maxBounds(-1e9f);

        std::ifstream file(path);
        if (!file.is_open())
        {
            std::cerr << "Failed to open OBJ file: " << path << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::string type;
            iss >> type;

            if (type == "v")
            {
                glm::vec3 vertex;
                iss >> vertex.x >> vertex.y >> vertex.z;
                temp_vertices.push_back(vertex);

                minBounds.x = std::min(minBounds.x, vertex.x);
                minBounds.y = std::min(minBounds.y, vertex.y);
                minBounds.z = std::min(minBounds.z, vertex.z);
                maxBounds.x = std::max(maxBounds.x, vertex.x);
                maxBounds.y = std::max(maxBounds.y, vertex.y);
                maxBounds.z = std::max(maxBounds.z, vertex.z);
            }
            else if (type == "vt")
            {
                glm::vec2 uv;
                iss >> uv.x >> uv.y;
                temp_uvs.push_back(uv);
            }
            else if (type == "f")
            {
                std::vector<std::string> faceTokens;
                std::string faceData;
                while (iss >> faceData)
                {
                    faceTokens.push_back(faceData);
                }

                if (faceTokens.size() >= 3)
                {
                    for (size_t i = 1; i < faceTokens.size() - 1; ++i)
                    {
                        std::string tokens[3] = {faceTokens[0], faceTokens[i], faceTokens[i + 1]};

                        for (int j = 0; j < 3; ++j)
                        {
                            std::istringstream fiss(tokens[j]);
                            std::string vStr, vtStr, vnStr;
                            std::getline(fiss, vStr, '/');
                            std::getline(fiss, vtStr, '/');
                            std::getline(fiss, vnStr, '/');

                            int vIdx = std::stoi(vStr) - 1;
                            int vtIdx = vtStr.empty() ? 0 : std::stoi(vtStr) - 1;

                            glm::vec3 vertex = temp_vertices[vIdx];
                            glm::vec2 uv = temp_uvs.empty() ? glm::vec2(0.0f) : temp_uvs[vtIdx];

                            vertexData.push_back(vertex.x);
                            vertexData.push_back(vertex.y);
                            vertexData.push_back(vertex.z);
                            vertexData.push_back(uv.x);
                            vertexData.push_back(uv.y);
                        }
                    }
                }
            }
        }

        glm::vec3 center = (minBounds + maxBounds) / 2.0f;
        glm::vec3 size = maxBounds - minBounds;
        float maxDim = std::max({size.x, size.y, size.z});
        if (maxDim == 0.0f)
            maxDim = 1.0f;

        for (size_t i = 0; i < vertexData.size(); i += 5)
        {
            vertexData[i] = (vertexData[i] - center.x) / maxDim;
            vertexData[i + 1] = (vertexData[i + 1] - center.y) / maxDim;
            vertexData[i + 2] = (vertexData[i + 2] - center.z) / maxDim;
        }

        vertexCount = vertexData.size() / 5;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }
};

#endif