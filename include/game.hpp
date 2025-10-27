#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <vector>
#include <random>
#include <cstdlib>
#include <SFML/System/Vector2.hpp>

#include "render.hpp"
#include "enemy.hpp"
#include "tower.hpp"
#include "projectile.hpp"          // ⬅️ AJOUT

class Game {
public:
    Game() = default;
    ~Game() = default;

    void generateEnemy(std::vector<Enemy*>& enemies, std::vector<Render::Cell> cells);
    void destroyEnemy(std::vector<Enemy*>& enemies);
    void generateTourelle(std::vector<Tourelle*>& tourelles, std::vector<Render::Cell>& cells, float x, float y);
    void destroyTourelles(std::vector<Tourelle*>& tourelles);

    // ⬅️ AJOUT : logique projectiles / tirs / collisions
    void update(float dt, std::vector<Enemy*>& enemies, std::vector<Tourelle*>& tourelles);

    // ⬅️ AJOUT : accès lecture/écriture aux projectiles (pour dessin)
    const std::vector<Projectile>& getProjectiles() const { return projectiles; }
    std::vector<Projectile>&       getProjectiles()       { return projectiles; }

private:
    // ⬅️ AJOUT
    std::vector<Projectile> projectiles;
};

#endif
