#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <vector>
#include <glm/glm.hpp>
#include <cstdlib>

struct Particle
{
    glm::vec3 position;
    glm::vec3 velocity;
    float life;
};

class ParticleSystem
{
public:
    std::vector<Particle> particles;

    void spawn(glm::vec3 startPos, int count, glm::vec3 baseVel, float spread, float lifeTime)
    {
        for (int i = 0; i < count; i++)
        {
            Particle p;
            p.position = startPos;
            float vx = baseVel.x + (((rand() % 100) / 100.0f) * spread - (spread / 2.0f));
            float vy = baseVel.y + (((rand() % 100) / 100.0f) * spread);
            float vz = baseVel.z + (((rand() % 100) / 100.0f) * spread - (spread / 2.0f));
            p.velocity = glm::vec3(vx, vy, vz);
            p.life = lifeTime + (((rand() % 100) / 100.0f) * 0.5f);
            particles.push_back(p);
        }
    }

    void update(float deltaTime)
    {
        for (auto it = particles.begin(); it != particles.end();)
        {
            it->life -= deltaTime * 1.5f;
            if (it->life > 0.0f)
            {
                it->velocity.y -= 9.8f * deltaTime;
                it->position += it->velocity * deltaTime;
                it->position.z += 4.0f * deltaTime;
                ++it;
            }
            else
            {
                it = particles.erase(it);
            }
        }
    }

    void clear()
    {
        particles.clear();
    }
};

#endif