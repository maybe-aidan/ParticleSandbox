#include "universe.h"

Universe::Universe(float G) : G{G}{ }

Universe::Universe(float G, std::vector<Particle::Particle>& particles) : G{G}, particles{particles} {}

void Universe::update(float deltaTime){
    for(Particle::Particle& p : particles){
        p.force = {0, 0, 0};
    }

    for(int i = 0; i < particles.size(); i++){
        for(int j = i + 1; j < particles.size(); j++){
            if(checkCollision(i, j)){
                continue;
            }
            auto& p1 = particles[i];
            auto& p2 = particles[j];
            float distSq = Particle::distance_squared(p1.position, p2.position);
            float minDistance = 0.01f;
            distSq = std::max(distSq, minDistance*minDistance);
            Particle::Vector3 direction = (p1.position - p2.position).normalize();
            float magnitude = (G * p1.mass * p2.mass) / distSq;
            p1.force -= (direction * magnitude);
            p2.force += (direction * magnitude); // Newton's Third Law
        }
    }

    for(auto& p : particles){
        p.radius = p.radius + (p.targetRadius - p.radius) * deltaTime;
        Particle::Vector3 acceleration = p.force / p.mass; // Newton's Second Law
        p.velocity += acceleration * deltaTime;
        p.position += p.velocity * deltaTime;
    }

}

void Universe::addParticle(Particle::Particle& p){
    particles.push_back(p);
}

void Universe::render(Shader& shader){
    std::vector<float> positionsFlat;
    std::vector<float> radii;
    for(const auto& particle : particles){
        positionsFlat.push_back(particle.position.x);
        positionsFlat.push_back(particle.position.y);
        positionsFlat.push_back(particle.position.z);
    }
    for(const auto& particle : particles){
        radii.push_back(particle.radius);
    }

    shader.use();
    shader.setFloat3Array("u_positions", particles.size(), positionsFlat.data());
    shader.setFloatArray("u_radii", particles.size(), radii.data());
    shader.setInt("numParticles", particles.size());
}

bool Universe::checkCollision(int i, int j){
    // Check to see if the smaller of the two is inside the larger one (|position1 - position2| < max_radius )
    // if so, delete the smaller one, add its mass (and half the radius) to the larger.
    auto& p1 = particles[i];
    auto& p2 = particles[j];

    float r = std::max(p1.radius, p2.radius);

    if(Particle::distance_squared(p1.position, p2.position) < r*r){
        if(p1.radius == r){
            p1.mass += p2.mass;
            p1.targetRadius = p1.radius + p2.radius;
            particles.erase(particles.begin() + j);
            return true;
        }else{
            p2.mass += p1.mass;
            p2.targetRadius = p2.radius + p1.radius;
            particles.erase(particles.begin() + i);
            return true;
        }
    }

    return false;
}