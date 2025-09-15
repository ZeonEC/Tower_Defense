#include "enemy.hpp"

#include <iostream>


// Constructeur générique : initialise les stats
Enemy::Enemy(int hp, float spd, float radius, sf::Color color) : dead(false), eat(false), health(hp), speed(spd), shape(radius) 
{
    shape.setFillColor(color);
    shape.setOrigin(radius, radius); // origine au centre pour faciliter la position
}

// ----------------------------------------------------------
// Gestion des points de vie
    void Enemy::hit(int amount)
    {
        health -= amount;
        if (health < 0) health = 0; // La vie ne peut pas être négative
        if (health == 0) dead = true; // L'ennemi est mort

        // Option visuelle : griser le cercle pour montrer la mort
        auto c = shape.getFillColor();
        shape.setFillColor(sf::Color(c.r/2, c.g/2, c.b/2, 120));
    }

    void Enemy::update(float dt)
    {
    if (dead) return;
    // Déplacement simple vers la droite
    shape.move(speed * dt, 0.f);
    }

    bool Enemy::isDead() const 
    {
        return dead;
    }

    int Enemy::getHp() const 
    {
        return health;
    }

    float Enemy::getSpeed() const 
    {
        return speed;
    }
