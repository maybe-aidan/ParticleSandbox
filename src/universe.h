#ifndef UNIVERSE_H
#define UNIVERSE_H

#include "particle.h"
#include "shader.h"
#include <vector>

class Universe{
public:
    Universe(float G);
    Universe(float G, std::vector<Particle::Particle>& particles);

    void update(float deltaTime);

    void render(Shader& shader);

    void addParticle(Particle::Particle& p);

private:
    bool checkCollision(int i, int j);

    float G;
    std::vector<Particle::Particle> particles;
};

#endif