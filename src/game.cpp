#include "game.hpp"
#include "enemy.hpp"
#include "tower.hpp"
#include "projectile.hpp"  
#include "Astar.hpp"


#include <algorithm>
#include <random>

// ------------ FONCTIONS DES ENNEMIES ------------ //

void Game::generateEnemy(std::vector<Enemy*>& enemies, const std::vector<Render::Cell> cells) { //Faire spawn les ennemies au centre de certaines cellules ///A FAIRE///
    int r = std::rand() % 5; // 0..4
    switch (r) {
        case 0: enemies.push_back(new BasicEnemy());  break;
        case 1: enemies.push_back(new FastEnemy());   break;
        case 2: enemies.push_back(new TankEnemy());   break;
        case 3: enemies.push_back(new FlyEnemy());    break;
        case 4: enemies.push_back(new TargetEnemy()); break;
    }
std::vector<const Render::Cell*> spawnCells;
    for (const auto& c : cells) {
        if (c.col < 3) // colonnes 0, 1, 2
            spawnCells.push_back(&c);
    }

    if (spawnCells.empty()) {
        std::cout << "Erreur : aucune cellule de spawn disponible !" << std::endl;
        delete enemies.back();
        enemies.pop_back();
        return;
    }

    // Choisir une cellule au hasard dans ces 3 colonnes
    int idx = std::rand() % spawnCells.size();
    const Render::Cell* spawnCell = spawnCells[idx];

    // Positionner l’ennemi au centre de la cellule choisie
    enemies.back()->setPosition(spawnCell->center.x, spawnCell->center.y);

    std::cout << "Ennemi spawné en cellule " << spawnCell->id
              << " (col=" << spawnCell->col
              << ", row=" << spawnCell->row << ")\n";
}

void Game::destroyEnemy(std::vector<Enemy*>& enemies) {
    for (auto*& e : enemies) {
        delete e;    // appelle le destructeur virtuel de Enemy (voir note ci-dessous)
        e = nullptr;
    }
    enemies.clear();
}

// ------------ FONCTIONS DES TOURELLES ------------ //
/*
void Game::generateTourelle(std::vector<Tourelle*>& tourelles, std::vector<Render::Cell>& cells,PathFinding_AStar& pathfinder, float x, float y, int type) { 
    
    if (cells.empty()) return; // pas de cellule, on ne peut rien faire

    // Trouver la cellule qui contient le clic (x,y)
    auto it = std::find_if(cells.begin(), cells.end(),[x,y]( Render::Cell& c){ return c.bounds.contains(x, y); });
           
    if (it == cells.end()) {
        // clic hors grille
        return;
    }

    if (it->col < 3 || it->col == pathfinder.gridCols - 1 ) {
        std::cout << "Impossible de placer une tourelle dans les 3 premières colonnes !" << std::endl;
        return;
    }

    // Génération de la fonction rand (pour l'instant, apres ça sera avec les touches)
    int r = std::rand() % 5; // 0..4

    // On fait ça pour éviter de surcharger la lecture quand on crée des tourelles
    const float cs = Render::Cell::cellSize;
    // Marque la cellule comme occupée pour A*
    int col = it->col;
    int row = it->row;
    // Placement d'une tourelle
    int index = row * pathfinder.gridCols + col;
    pathfinder.Nodes[index].turreted = true;

    sf::Vector2f entryPos = cells.front().center;
    bool pathStillOk = pathfinder.IsPathStillPossible(entryPos);

    if (!pathStillOk) {
        std::cout << "Placement refusé : cela bloquerait le chemin !" << std::endl;
        pathfinder.Nodes[index].turreted = false; // on annule
        return;
    }

    // --- ✅ Vérifie s’il y a un ennemi sur ce node ---
    if (pathfinder.Nodes[index].EnemyPresence) {
        std::cout << "Impossible de placer une tourelle ici : un ennemi occupe la case !" << std::endl;
        return;
    }

    if (it->turreted == true) {
        // déjà une tourelle sur cette cellule
        std::cout << "Cellule " << it->id << " occupée !\n";
        return;
    }

    // Pas de tourelle sur cette cellule alors banco piou piou
    if (it->turreted == false) {
        // la cellule est libre
    switch (type) {
        case 0: tourelles.push_back(new BasicTourelle(Render::Cell::cellSize * 0.5f)); break;
        case 1: tourelles.push_back(new PoisonTourelle(Render::Cell::cellSize * 0.5f)); break;
        case 2: tourelles.push_back(new shotgunTourelle(Render::Cell::cellSize * 0.5f)); break;
        case 3: tourelles.push_back(new TargetTourelle(Render::Cell::cellSize * 0.5f)); break;
        case 4: tourelles.push_back(new FlyTourelle(Render::Cell::cellSize * 0.5f)); break;
    }

        it->turreted = true; // on marque la cellule comme occupée
        std::cout << "Tourelle placée en cellule " << it->id << "\n";
    }
        
    
    // Positionne l'ennemi à la position de la souris
    // Trouver la cellule la plus proche de la position de la souris
    tourelles.back()->setPosition(it->center.x, it->center.y);
}*/

// ====================== Game::generateTourelle ======================
void Game::generateTourelle(
    std::vector<Tourelle*>& tourelles,
    std::vector<Render::Cell>& cells,
    PathFinding_AStar& pathfinder,
    std::vector<Enemy*>& enemies,
    float x, float y,
    int type,
    Player* player
) { 
    if (cells.empty()) return;

    // Trouver la cellule cliquée
    auto it = std::find_if(cells.begin(), cells.end(),
        [x, y](Render::Cell& c){ return c.bounds.contains(x, y); });
    if (it == cells.end()) return; // clic hors grille

    if (it->col < 3 || it->col == pathfinder.gridCols - 1) {
        std::cout << "Impossible de placer une tourelle dans les 3 premières colonnes !\n";
        return;
    }

    if (it->turreted) {
        std::cout << "Cellule " << it->id << " déjà occupée !\n";
        return;
    }

        // 🔹 Vérification si un ennemi est sur cette cellule
    for (auto* e : enemies) {
        if (!e || e->isDead()) continue;

        const sf::Vector2f& pos = e->getPosition();
        if (it->bounds.contains(pos)) {  // si la position de l’ennemi est dans la cellule
            std::cout << "Impossible de placer une tourelle ici : un ennemi est présent !\n";
            return;
        }
    }

    // Marque le node comme occupé
    int index = it->row * pathfinder.gridCols + it->col;
        pathfinder.Nodes[index].turreted = true;

    // ⚡ Vérifie si le chemin reste possible
    sf::Vector2f entryPos = cells.front().center; // point d'entrée des ennemis
    if (!pathfinder.IsPathStillPossible(entryPos)) {
        std::cout << "Placement refusé : cela bloquerait le chemin !\n";
        pathfinder.Nodes[index].turreted = false; // annule la pose
        return;
    }

    // 🔹 Crée une tourelle temporaire pour vérifier le coût
int towerCost = 0;
switch (type) {
    case 0: towerCost = 10; break;
    case 1: towerCost = 30; break;
    case 2: towerCost = 40; break;
    case 3: towerCost = 40; break;
    case 4: towerCost = 50; break;
}

// 🔹 Vérifie si le joueur a assez de ressources
if (player->getRessources() < towerCost) {
    std::cout << "Pas assez de ressources ! Il faut " << towerCost 
              << " mais tu as " << player->getRessources() << ".\n";
    pathfinder.Nodes[index].turreted = false;
    return;
}

// 🔹 Si oui, retire le coût
player->reduceRessources(towerCost);

    // Crée la tourelle
    switch (type) {
        case 0: tourelles.push_back(new BasicTourelle(Render::Cell::cellSize * 0.4f)); break;
        case 1: tourelles.push_back(new PoisonTourelle(Render::Cell::cellSize * 0.4f)); break;
        case 2: tourelles.push_back(new shotgunTourelle(Render::Cell::cellSize * 0.4f)); break;
        case 3: tourelles.push_back(new TargetTourelle(Render::Cell::cellSize * 0.4f)); break;
        case 4: tourelles.push_back(new FlyTourelle(Render::Cell::cellSize * 0.4f)); break;
        default: break;
    }

    // Place la tourelle au centre de la cellule
    tourelles.back()->setPosition(it->center.x, it->center.y);
    it->turreted = true;

    std::cout << "Tourelle placée en cellule " << it->id << "\n";

    // ⚡ Indique à tous les ennemis de recalculer leur chemin au prochain update
    for (auto* e : enemies) {
        if (!e->isDead()) e->needRepath = true;
    }
}

void Game::destroyTourelles(std::vector<Tourelle*>& tourelles) {
    for (auto*& t : tourelles) {
        delete t;    // appelle le destructeur virtuel de tourelles
        t = nullptr;
    }
    tourelles.clear();
}


void Game::update(PathFinding_AStar& pathfinder,
                  const std::vector<Render::Cell>& cells,
                  float dt,
                  std::vector<Enemy*>& enemies,
                  std::vector<Tourelle*>& tourelles,
                  Player* player)
{
    // =======================
    // 1) UPDATE ENNEMIS
    // =======================
    for (auto it = enemies.begin(); it != enemies.end(); )
    {
        Enemy* e = *it;
        if (!e) { it = enemies.erase(it); continue; }

        // Fournir la liste des tours aux FlyEnemy pour leur IA (anti-A*)
        if (auto* fe = dynamic_cast<FlyEnemy*>(e)) {
            fe->setTowerList(&tourelles);
        }

        // Fournir la liste des tours aux TargetEnemy  (ici pas comme les Fly car il garde le astar mais tir quand ils en ont une a porté)

        if (auto* te = dynamic_cast<TargetEnemy*>(e)) {
        te->setTowerList(&tourelles);
        }

        // Mise à jour de l'ennemi (A* pour les ground, poursuite/arrêt à portée pour les Fly)
        e->update(pathfinder, cells);

        // Les Fly et les Target tirent s'ils sont à portée d'une tour
        if (auto* fe = dynamic_cast<FlyEnemy*>(e)) {
            fe->tryShoot(dt, projectiles);
        }

        if (auto* te = dynamic_cast<TargetEnemy*>(e)) {
        te->tryShoot(dt, projectiles);
        }

        // Sorties de boucle selon l'état
        if (e->reachedGoal) {
            player->reduceHealth(e->getDamage());
            delete e;
            it = enemies.erase(it);
            std::cout << "Un ennemi a atteint la fin ! Vie du joueur : "
                      << player->getHealth() << "\n";
            continue;
        }

        if (e->isDead()) {
            const int rv = e->ressourceValue; // sauvegarder AVANT delete
            delete e;
            it = enemies.erase(it);
            player->AddRessources(rv);
            std::cout << "Ennemi tué ! Ressources gagnées : " << rv
                      << " | Total : " << player->getRessources() << "\n";
            continue;
        }

        ++it;
    }

    // =======================
    // 2) TOURELLES : TIR
    // =======================
    for (auto* t : tourelles) {
        if (!t) continue;
        t->tryShoot(dt, enemies, projectiles);
    }

    // =======================
    // 3) PROJECTILES : UPDATE
    // =======================
    for (auto& p : projectiles) {
        p.update(dt);
    }

    // =======================
    // 4) COLLISIONS
    // =======================

    // --- 4a) Projectiles qui VISENT LES TOURELLES ---
    for (auto& p : projectiles) {
        if (!p.isAlive()) continue;
        if (!p.targetsTowers()) continue; // <-- nécessite Projectile::targetsTowers()

        for (auto* t : tourelles) {
            if (!t || t->isDestroyed()) continue;

            if (p.collidesWith(t->getPosition(), t->getRadius())) {
                t->takeDamage(p.getDamage());
                p.kill();
                break; // projectile consommé
            }
        }
    }

    // --- 4b) Projectiles qui VISENT LES ENNEMIS (comportement existant) ---
    for (auto& p : projectiles) {
        if (!p.isAlive()) continue;
        if (p.targetsTowers()) continue; // ignorer ceux destinés aux tours

        for (auto* e : enemies) {
            if (!e || e->isDead()) continue;

            // Respect du masque de ciblage du projectile
            if (!p.accepts(e)) continue;

            const float enemyRadius = e->getRadius();
            if (p.collidesWith(e->getPosition(), enemyRadius)) {
                e->takeDamage(p.getDamage());
                p.kill();
                break; // projectile consommé
            }
        }
    }

    // Nettoyage des projectiles inactifs
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
                       [](const Projectile& pr){ return !pr.isAlive(); }),
        projectiles.end()
    );

    // =======================
    // 5) TOURELLES DÉTRUITES : NETTOYAGE + LIBÉRATION A*
    // =======================
    for (auto tit = tourelles.begin(); tit != tourelles.end(); )
    {
        Tourelle* t = *tit;
        if (t && t->isDestroyed()) {
            // Libérer la cellule (marque visuelle + A*)
            const sf::Vector2f tp = t->getPosition();

            auto& cellsNC = const_cast<std::vector<Render::Cell>&>(cells);
            auto cit = std::find_if(cellsNC.begin(), cellsNC.end(),
                                    [&tp](const Render::Cell& c){ return c.bounds.contains(tp); });

            if (cit != cellsNC.end()) {
                cit->turreted = false;

                // Libérer aussi le node de la grille A*
                const int index = cit->row * pathfinder.gridCols + cit->col;
                if (index >= 0 && index < (int)pathfinder.Nodes.size()) {
                    pathfinder.Nodes[index].turreted = false;
                }
            }

            delete t;
            tit = tourelles.erase(tit);

            // Prévenir les ennemis (les non-fly) qu'un nouveau chemin peut exister
            for (auto* e : enemies) if (e) e->needRepath = true;
        } else {
            ++tit;
        }
    }
}



/*float Game::spawn(float x, float y) {  //Fonction qui marche pas
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distX(20.f, 780.f);
    std::uniform_real_distribution<float> distY(20.f, 580.f);

    return { distX(gen), distY(gen) };
}*/

/*void Game::generateEnemy(std::vector<Enemy*>& enemies, std::vector<Render::Cell> cells) {
    int r = std::rand() % 5; // 0..4
    switch (r) {
        case 0: enemies.push_back(new BasicEnemy());  break;
        case 1: enemies.push_back(new FastEnemy());   break;
        case 2: enemies.push_back(new TankEnemy());   break;
        case 3: enemies.push_back(new FlyEnemy());    break;
        case 4: enemies.push_back(new TargetEnemy()); break;
    }
    // Positionne l'ennemi à une position aléatoire
    float rx = std::rand() % 200; // 0..39
    float ry = std::rand() % 800; // 0..39
    enemies.back()->setPosition(rx,ry);
}*/
