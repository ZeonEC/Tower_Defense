#ifndef GAME_HPP
#define GAME_HPP

//------------------------ INCLUDE libs ----------------------//

#include <iostream>
#include <vector>
#include <random>
#include <cstdlib>
#include <SFML/System/Vector2.hpp>

//------------------------ INCLUDE prog ----------------------//
#include "render.hpp"
#include "enemy.hpp"
#include "tower.hpp"
#include "projectile.hpp"          

class Game {
public:
    Game() = default;
    ~Game() = default;

    // On génère un ennemy a l'appel de la fonction, l'ennemie est rangé dans un tableau et prend en compte cellule pour son emplacement
    void generateEnemy(std::vector<Enemy*>& enemies, std::vector<Render::Cell> cells);
    // Un delete géant mdr
    void destroyEnemy(std::vector<Enemy*>& enemies);

    // // On génère une tourelle a l'appel de la fonction, la tourelle est rangé dans un tableau et prend en compte cellule pour son emplacement
    void generateTourelle(std::vector<Tourelle*>& tourelles, std::vector<Render::Cell>& cells, float x, float y);
    void destroyTourelles(std::vector<Tourelle*>& tourelles);

    // ⬅logique projectiles / tirs / collisions
    void update(float dt, std::vector<Enemy*>& enemies, std::vector<Tourelle*>& tourelles);

    // ⬅accès lecture/écriture aux projectiles (pour dessin)
    const std::vector<Projectile>& getProjectiles() const { return projectiles; }
    std::vector<Projectile>&       getProjectiles()       { return projectiles; }

private:

    std::vector<Projectile> projectiles;
};

#endif
