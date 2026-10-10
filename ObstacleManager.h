#ifndef OBSTACLE_MANAGER_H
#define OBSTACLE_MANAGER_H

#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cmath>

struct Obstacle
{
    int lane;
    float zPos;
};

class ObstacleManager
{
public:
    std::vector<Obstacle> obstacles;
    float spawnTimer = 0.0f;
    const float LANE_WIDTH = 2.0f;

    void update(float deltaTime, float currentSpeed, float timeElapsed)
    {
        spawnTimer += deltaTime;
        float spawnTarget = std::max(0.4f, 1.2f - (timeElapsed * 0.02f));
        if (spawnTimer > spawnTarget)
        {
            obstacles.push_back({(rand() % 3) - 1, -30.0f});
            spawnTimer = 0.0f;
        }

        for (auto it = obstacles.begin(); it != obstacles.end();)
        {
            it->zPos += currentSpeed * deltaTime;
            if (it->zPos > 5.0f)
            {
                it = obstacles.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    bool checkCollision(float playerX, float playerY)
    {
        for (const auto &obs : obstacles)
        {
            if (obs.zPos > -0.5f && obs.zPos < 0.5f &&
                std::abs(playerX - (obs.lane * LANE_WIDTH)) < 0.8f &&
                playerY > -1.0f)
            {
                return true;
            }
        }
        return false;
    }

    void clear()
    {
        obstacles.clear();
        spawnTimer = 0.0f;
    }
};

#endif