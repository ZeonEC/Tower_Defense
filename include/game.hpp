#ifndef GAME_HPP
#define GAME_HPP

//------------------------ INCLUDE libs ----------------------//

#include <iostream>                // logs éventuels
#include <vector>                  // stockage des entités
#include <random>                  // RNG
#include <cstdlib>               
#include <SFML/System/Vector2.hpp> // sf::Vector2f  

//------------------------ INCLUDE prog ----------------------//

#include "render.hpp"     // grille & cellules (placement dépend de cellsize)
#include "enemy.hpp"      // création / déplacement / destruction d’ennemis
#include "tower.hpp"      // placement / essais de tir des tourelles
#include "projectile.hpp" // stockage et tirs
#include "Astar.hpp"      // pathfinding 
#include "Player.hpp"     // économie / PV du joueur

// ==================================================================================================== //
//                                        RÔLE DE LA CLASSE GAME                                        //
// ---------------------------------------------------------------------------------------------------- //
// Gestion du jeu : spawns, placements, tir/MAJ, collisions, nettoyage de fin de frame.
// - Possède le stock de projectiles (par valeur).
// - Manipule des *pointeurs* vers ennemis/tourelles (spawn/destroy gérés ici).
// - Coordonne le pathfinding A* (repath partiel pour lisser la charge CPU).

// ==================================================================================================== //

class Game {
public:
    Game()  = default;
    ~Game() = default;

    // ------------------------ État Joueur ------------------------ //

    Player player;  // économie, PV, etc.

    // ------------------------ Gestion des ENNEMIS ------------------------ //
    /**
     *  Génère 1 ennemi et l’ajoute dans `enemies`.
     *  enemies  tableau de pointeurs possédé par Game
     *  cells    grille (positions, walkable) pour calibrer le spawn
     *
     * Remarque:
     * - Le type d’ennemi dépend pour l'instant de la rng RNG.
     * - L’ennemi est positionné en fonction de la carte (cellsize / entrées).
     */

    void generateEnemy(std::vector<Enemy*>& enemies, std::vector<Render::Cell> cells);

    
    // Détruit proprement tous les ennemis du vecteur (delete + clear).
    // Pointeurs invalidés après appel.
      
    void destroyEnemy(std::vector<Enemy*>& enemies);

    // ------------------------ Gestion des TOURELLES ------------------------ //
    /**
     *  Place une tourelle si possible (chemin encore praticable), retire le coût au joueur.
     *  tourelles   stockage des tourelles (pointeurs possédés)
     *  cells       grille (pour aligner le placement sur la map)
     *  pathfinder  utilisé pour vérifier que la pose n’obstrue pas (A*)
     *  enemies     pour marquer EnemyPresence si besoin
     *  x,y         coordonnées monde du clic (seront “snap” sur la grille)
     *  type        index/type de tourelle à créer (Basic/Poison/Shotgun/Fly/Target…)
     *  player      économie (débit si achat)
     * 
     * - En cas d’échec (coût, case occupée, path bloqué), ne rien ajouter et restituer l’état.
     */

    void generateTourelle(std::vector<Tourelle*>& tourelles,
                          std::vector<Render::Cell>& cells,
                          PathFinding_AStar& pathfinder,
                          std::vector<Enemy*>& enemies,
                          float x, float y, int type,
                          Player* player);

    /**
     *  Détruit proprement toutes les tourelles (delete + clear).
     *  Pointeurs invalidés après appel.
     */
    void destroyTourelles(std::vector<Tourelle*>& tourelles);

    // ------------------------ Mise à jour globale ------------------------ //
    /**
     *  Met à jour la logique de jeu :
     *  - Repath partiel de quelques ennemis
     *  - Déplacements ennemis (A* ou vol)
     *  - Tir des tourelles et de certains ennemis (Target/Fly)
     *  - Avancée des projectiles
     *  - Collisions (projectiles ↔ ennemis/tourelles)
     *  - Nettoyage (suppression entités mortes / tirs invalides)
     *
     *  pathfinder  A* partagé (Nodes & heuristique)
     *  cells       grille (walkability, cellsize, centres pour path)
     *  dt          delta time (s)
     *  enemies     population d’ennemis (pointeurs possédés ici)
     *  tourelles   population de tourelles (pointeurs possédés ici)
     *  player      état joueur (or, PV, etc.)
     */
    void update(PathFinding_AStar& pathfinder,
                const std::vector<Render::Cell>& cells,
                float dt,
                std::vector<Enemy*>& enemies,
                std::vector<Tourelle*>& tourelles,
                Player* player);

    // ------------------------ Accès aux projectiles ------------------------ //

    const std::vector<Projectile>& getProjectiles() const { return projectiles; }
    std::vector<Projectile>&       getProjectiles()       { return projectiles; }

private:

    // =================================== PERF & RYTHME =================================== //
    // Repath partiel : on étale le recalcul sur plusieurs frames pour éviter les freezes.
    size_t enemyRepathIndex = 0; // index rotatif dans `enemies`
    int    maxRepathPerFrame = 2;// nombre max d'ennemis qui recalculent leur chemin par frame

    // Stockage par valeur = simple et cache-friendly (peu de new/delete pendant la partie).
    std::vector<Projectile> projectiles;
};

#endif
