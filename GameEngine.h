#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <fstream>
#include <algorithm>
#include <glm/glm.hpp>

#include "NetworkManager.h"
#include "AudioManager.h"
#include "ParticleSystem.h"
#include "ObstacleManager.h"

enum GameState
{
    WAITING,
    PLAYING,
    GAMEOVER
};

class GameEngine
{
public:
    AudioManager audio;
    NetworkManager network;
    ParticleSystem particles;
    ObstacleManager obstacles;

    GameState state = WAITING;
    float distanceSwum = 0.0f;
    float timeElapsed = 0.0f;
    float highScore = 0.0f;
    float currentSpeed = 12.0f;
    float worldScrollOffset = 0.0f;

    int targetLane = 0;
    float playerX = 0.0f;
    float playerY = -0.35f;
    bool isKeyboardDiving = false;
    float networkDiveTimer = 0.0f;

    void init()
    {
        audio.init();
        network.init(5005);
        loadHighScore();
    }

    void loadHighScore()
    {
        std::ifstream file("highscore.txt");
        if (file.is_open())
        {
            file >> highScore;
            file.close();
        }
    }

    void saveHighScore()
    {
        std::ofstream file("highscore.txt");
        if (file.is_open())
        {
            file << highScore;
            file.close();
        }
    }

    void resetGame()
    {
        state = PLAYING;
        distanceSwum = 0.0f;
        timeElapsed = 0.0f;
        currentSpeed = 12.0f;
        worldScrollOffset = 0.0f;
        targetLane = 0;
        playerX = 0.0f;
        obstacles.clear();
        particles.clear();
    }

    void processNetwork()
    {
        std::string cmd = network.pollCommand();
        if (cmd.empty())
            return;

        if (state == WAITING || state == GAMEOVER)
        {
            resetGame();
        }
        else if (state == PLAYING)
        {
            if (cmd == "LEFT" && targetLane > -1)
            {
                targetLane--;
                particles.spawn(glm::vec3(playerX, playerY, 0.0f), 35, glm::vec3(3.0f, 3.0f, 4.0f), 2.5f, 0.8f);
            }
            else if (cmd == "RIGHT" && targetLane < 1)
            {
                targetLane++;
                particles.spawn(glm::vec3(playerX, playerY, 0.0f), 35, glm::vec3(-3.0f, 3.0f, 4.0f), 2.5f, 0.8f);
            }
            else if (cmd == "CENTER")
            {
                targetLane = 0;
            }
            else if (cmd == "DIVE")
            {
                // ONLY trigger the big splash if we weren't already diving
                if (networkDiveTimer <= 0.0f && !isKeyboardDiving)
                {
                    particles.spawn(glm::vec3(playerX, playerY + 0.2f, 0.0f), 150, glm::vec3(0.0f, 6.0f, 4.0f), 5.0f, 1.5f);
                }
                networkDiveTimer = 0.5f;
            }
            else if (cmd == "UP")
            {
                networkDiveTimer = 0.0f;
            }
        }
    }

    void update(float deltaTime)
    {
        if (state != PLAYING)
        {
            audio.update(false, false);
            return;
        }

        timeElapsed += deltaTime;
        currentSpeed = 12.0f + (timeElapsed * 0.3f);
        distanceSwum += ((currentSpeed / 3.0f) * deltaTime);
        worldScrollOffset += (currentSpeed / 5.0f) * deltaTime;

        float targetX = targetLane * obstacles.LANE_WIDTH;
        playerX += (targetX - playerX) * 0.4f;

        bool isDiving = isKeyboardDiving || (networkDiveTimer > 0.0f);
        if (networkDiveTimer > 0.0f)
            networkDiveTimer -= deltaTime;

        float targetY = isDiving ? -2.2f : -0.35f;
        playerY += (targetY - playerY) * 0.5f;

        audio.update(isDiving, true);

        if (!isDiving)
        {
            particles.spawn(glm::vec3(playerX, playerY + 0.1f, 0.6f), 20, glm::vec3(0.0f, 1.5f, 4.0f), 2.5f, 0.7f);
        }

        particles.update(deltaTime);
        obstacles.update(deltaTime, currentSpeed, timeElapsed);

        if (obstacles.checkCollision(playerX, playerY))
        {
            audio.playCrash();
            particles.spawn(glm::vec3(playerX, playerY, 0.0f), 100, glm::vec3(0.0f, 6.0f, 8.0f), 8.0f, 1.5f);

            if (distanceSwum > highScore)
            {
                highScore = distanceSwum;
                saveHighScore();
            }
            state = GAMEOVER;
        }
    }

    void cleanup()
    {
        audio.cleanup();
        network.cleanup();
    }
};

#endif