#include "enemy.hpp"

#include <iostream>


// Constructeur générique : initialise les stats
Enemy::Enemy(int hp, float spd) : dead(false), eat(false), health(hp), speed(spd) {}

// ----------------------------------------------------------
// Gestion des points de vie
    void Enemy::hit(int amount)
    {
        health -= amount;
        if (health < 0) health = 0; // La vie ne peut pas être négative
        if (health == 0) dead = true; // L'ennemi est mort
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
