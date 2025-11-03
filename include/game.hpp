#ifndef GAME_HPP
#define GAME_HPP

//------------------------ INCLUDE libs ----------------------//

#include <iostream>
#include <vector>
#include <random>
#include <cstdlib>
#include <SFML/System/Vector2.hpp>

//------------------------ INCLUDE prog ----------------------//

#include "render.hpp" // Pour la gestion de création d'ennemy/tourelle en fonction de la taille de cellule
#include "enemy.hpp" // création/déplacement/destruction
#include "tower.hpp" // placement/essais de tir des tourelles
#include "projectile.hpp"
#include "Astar.hpp" 
#include "Player.hpp"         


// ====================================== ROLE DE LA CLASSE =========================================== //

// Game orchestre le gameplay runtime : spawns, placements, tir/MAJ, collisions, nettoyage. 
// Elle possède le stock de projectiles (par valeur) et manipule des pointeurs vers ennemis/tourelles

// ==================================================================================================== //

class Game {
public:
    Game() = default;
    ~Game() = default;

    Player player;  // ajout du joueur
    // On génère un ennemy a l'appel de la fonction, l'ennemie est rangé dans un tableau et prend en compte cellule pour son emplacement
    void generateEnemy(std::vector<Enemy*>& enemies, std::vector<Render::Cell> cells);
    // Un delete géant mdr
    void destroyEnemy(std::vector<Enemy*>& enemies);

    // // On génère une tourelle a l'appel de la fonction, la tourelle est rangé dans un tableau et prend en compte cellule pour son emplacement
    void generateTourelle(std::vector<Tourelle*>& tourelles, std::vector<Render::Cell>& cells,PathFinding_AStar& pathfinder,std::vector<Enemy*>& enemies, float x, float y,int type, Player* player);
    void destroyTourelles(std::vector<Tourelle*>& tourelles);

    // C'est elle qui met a jour le jeu. logique / projectiles / tirs / collisions
    void update(PathFinding_AStar& pathfinder, const std::vector<Render::Cell>& cells,float dt, std::vector<Enemy*>& enemies, std::vector<Tourelle*>& tourelles, Player* player);

    // accès lecture/écriture aux projectiles (pour dessin)
    const std::vector<Projectile>& getProjectiles() const { return projectiles; }
    std::vector<Projectile>&       getProjectiles()       { return projectiles; }

private:
    size_t enemyRepathIndex = 0;      // Index rotatif
    int maxRepathPerFrame = 2;        // Nb d'ennemis recalculant par frame
    std::vector<Projectile> projectiles;
};

#endif
