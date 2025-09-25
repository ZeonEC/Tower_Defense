#include "tower.hpp"


#include <iostream>


// ==== Définition des membres statiques  ====
int Tourelle::counter      = 0;
int BasicTourelle::counter = 0;
int PoisonTourelle::counter  = 0;
int shotgunTourelle::counter  = 0;
int TargetTourelle::counter= 0;
int FlyTourelle::counter   = 0;
// --------------------------------------------


// Constructeur générique : initialise les stats
Tourelle::Tourelle(int hp, float spd, float radius, sf::Color color) : dead(false), health(hp), speed(spd), shape(radius) 
{
    shape.setPointCount(4); // Forme carré pour les tourelles
    shape.setRotation(45.f); // On tourne le carré de 45° parce que ça spawn en losange plutôt qu'en carré
    shape.setFillColor(color);
    shape.setOrigin(radius, radius); // origine au centre pour faciliter la position
    counter++; // Incrémente le compteur d'Tourelle à chaque création
}

// ----------------------------------------------------------
// Gestion des points de vie
    void Tourelle::hit(int amount)
    {
        health -= amount;
        if (health < 0) health = 0; // La vie ne peut pas être négative
        if (health == 0) dead = true; // L'tourelle est mort

        // Option visuelle : griser le cercle pour montrer la mort
        auto c = shape.getFillColor();
        shape.setFillColor(sf::Color(c.r/2, c.g/2, c.b/2, 120));
    }

    void Tourelle::update(float dt)
    {
    if (dead) return;
    // Déplacement simple vers la droite
    }

    bool Tourelle::isDead() const 
    {
        return dead;
    }

    int Tourelle::getHp() const 
    {
        return health;
    }

    float Tourelle::getSpeed() const 
    {
        return speed;
    }
