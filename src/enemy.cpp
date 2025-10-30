#include "enemy.hpp"
#include "render.hpp"

#include <iostream>


// ==== Définition des membres statiques  ====
int Enemy::counter      = 0;
int BasicEnemy::counter = 0;
int FastEnemy::counter  = 0;
int TankEnemy::counter  = 0;
int TargetEnemy::counter= 0;
int FlyEnemy::counter   = 0;
// --------------------------------------------


// Constructeur générique : initialise les stats
Enemy::Enemy(int hp, float spd, float radius, sf::Color color) : dead(false), eat(false), health(hp), speed(spd), shape(radius) 
{
    shape.setFillColor(color);
    shape.setOrigin(radius, radius); // origine au centre pour faciliter la position
    counter++; // Incrémente le compteur d'ennemis à chaque création
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

    void Enemy::draw(sf::RenderWindow& win) const {
    if (!dead)
        win.draw(shape);
}


    void Enemy::update(float dt)
    {
        // ALGO A*
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

    float Enemy::getRadius() const 
    {
        const float r = Render::Cell::cellSize * 0.5f;
        return r;
    }

    void Enemy::takeDamage(int amount) {
    if (dead) return;
    health -= amount;
    if (health <= 0) {
        health = 0;
        dead = true;
        shape.setFillColor(sf::Color(60, 60, 60, 100)); // effet visuel
    }
}

//----------------------------------------------------------
//Ennemi de base 

BasicEnemy::BasicEnemy()
    : Enemy(
        /*hp*/     100,
        /*speed*/  3,
        /*radius*/ Render::Cell::cellSize * 0.5f, // taille = cellule
        /*color*/  sf::Color(255, 0, 0)
    ) { ++counter; }
    
    BasicEnemy::~BasicEnemy() { --BasicEnemy::counter; }

//----------------------------------------------------------
//Ennemi rapide (comportement identique au ennemie de base, hp et vitesse change)

    FastEnemy::FastEnemy()
    : Enemy(
        /*hp*/     75,
        /*speed*/  50,
        /*radius*/ Render::Cell::cellSize * 0.25f, // taille = cellule/2
        /*color*/  sf::Color(0, 255, 0)
    ) { ++counter; }

    FastEnemy::~FastEnemy() { --FastEnemy::counter; } 

//----------------------------------------------------------
//Ennemi tank (comportement identique au ennemie de base, hp et vitesse change) 

    TankEnemy::TankEnemy()
    : Enemy(
        /*hp*/     300,
        /*speed*/  1,
        /*radius*/ Render::Cell::cellSize * 0.6f, // taille = cellule *0.6
        /*color*/  sf::Color(0, 0, 255)
    ) { ++counter; }

    TankEnemy::~TankEnemy() { --TankEnemy::counter; } 


//----------------------------------------------------------
//Ennemi target (leur objectif est de détruire les tourelles sur leur passage (chemin le plus cours en ignorant les tourelles)
//hp et vitesse change) 

    TargetEnemy::TargetEnemy()
    : Enemy(
        /*hp*/     150,
        /*speed*/  3,
        /*radius*/ Render::Cell::cellSize * 0.5f,     
        /*color*/  sf::Color(100, 100, 100)
    ) { ++counter; }

    TargetEnemy::~TargetEnemy() { --TargetEnemy::counter; }

//----------------------------------------------------------
//Ennemi volatile (ils ignorent les tourelles, vole par dessus, hp et vitesse change) 

    FlyEnemy::FlyEnemy()
    : Enemy(
        /*hp*/     100,
        /*speed*/  4,
        /*radius*/ Render::Cell::cellSize * 0.4f,
        /*color*/  sf::Color(200, 0, 150)
    ) { ++counter; }

    FlyEnemy::~FlyEnemy() { --FlyEnemy::counter; }


  /*  void Enemy::generateEnemy ()
    {
        r = rand();         // génère un nombre "aléatoire"
        switch (r)
        {
        case 0:
            enemies.push_back(new BasicEnemy());
        case 1:
            enemies.push_back(new FastEnemy());
        case 2:
            enemies.push_back(new TankEnemy());
        case 3:
            enemies.push_back(new FlyEnemy());
        case 4:
            enemies.push_back(new TargetEnemy());
            break;
        
        default:
            break;
        }
    }*/

    /*
    void Enemy::destroyEnemy (std::vector<Enemy*>& enemies)
    {
        // Ici il faut libérer la mémoire à la main
        for (auto e : enemies) {
            delete e;  // appelle le destructeur
        }
    }
*/

    