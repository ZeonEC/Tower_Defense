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
    Game() : gameOver(false) {};
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

    void upgradeTourelle(std::vector<Tourelle*>& tourelles, Player* player, float mouseX, float mouseY);

    void startWaves();
    void updateWaves(float dt, std::vector<Enemy*>& enemies, const std::vector<Render::Cell>& cells);

    int  getWaveIndex() const { return waveIndex + 1; }   // +1 pour que ça commence à 1 au lieu de 0
    int  getWaveTotal() const { return static_cast<int>(waves.size()); }
    bool isWavesFinished() const { return wavesFinished; }

    struct Wave { int count; float interval; }; // nombre d’ennemis, intervalle de spawn (s)
    
    // Vecteur de créations de vagues avec gestions du nombre d'ennemis et les intervalle
    std::vector<Wave> waves = { {10,0.6f}, {12,0.5f}, {18,0.4f}, {18,0.4f}, {18,0.4f}  };

    int   waveIndex = -1;         // -1 = pas commencé
    int   toSpawn   = 0;          // restants à spawner dans la vague courante
    float spawnIv   = 0.f;        // intervalle de spawn pour la vague
    float spawnT    = 0.f;        // timer cumulatif
    bool  interWave = false;      // on est dans le délai entre deux vagues
    float interT    = 0.f;        // timer du délai
    float interDelay = 3.f;       // 3 secondes entre vagues
    bool  wavesFinished = false;  // toutes les vagues terminées

    bool isGameOver() const { return gameOver; }

private:
    size_t enemyRepathIndex = 0;      // Index rotatif
    int maxRepathPerFrame = 2;        // Nb d'ennemis recalculant par frame
    std::vector<Projectile> projectiles;

    bool gameOver = false;
};

#endif
