#include "game.hpp"
#include "enemy.hpp"
#include "tower.hpp"
#include "projectile.hpp"  
#include "Astar.hpp"


#include <algorithm>
#include <random>

// ------------ FONCTIONS DES ENNEMIES ------------ //

void Game::generateEnemy(std::vector<Enemy*>& enemies, const std::vector<Render::Cell> cells) { //Faire spawn les ennemies au centre de certaines cellules 
    int r = std::rand() % 5; // 0..4
    // Apparition des ennemies en fonction de la valeur de rand
    switch (r) {
        case 0: enemies.push_back(new BasicEnemy());  break;
        case 1: enemies.push_back(new FastEnemy());   break;
        case 2: enemies.push_back(new TankEnemy());   break;
        case 3: enemies.push_back(new FlyEnemy());    break;
        case 4: enemies.push_back(new TargetEnemy()); break;
    }

    // On crée une matrice de cellules et on masque les cellules avec un numéro de colonne supérieur à 3
std::vector<const Render::Cell*> spawnCells;
    for (const auto& c : cells) {
        if (c.col < 3) // colonnes 0, 1, 2
            spawnCells.push_back(&c);
    }

    // Si la cellule existe pas alors on supprime l'ennemie qu'on essaye de crée
    if (spawnCells.empty()) {
        std::cout << "Erreur : aucune cellule de spawn disponible !" << std::endl;
        delete enemies.back();
        enemies.pop_back(); // supprime le contenu de l'ennemie
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

// ====================== Game destrcuteur d'ennemies ======================

void Game::destroyEnemy(std::vector<Enemy*>& enemies) {
    for (auto*& e : enemies) {
        delete e;    // appelle le destructeur virtuel de Enemy
        e = nullptr;
    }
    enemies.clear();
}


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
    // On cherche parmis toutes les cellules (éligible au spawn ou non) laquel correspond au coordonées X,Y du clic de souris, on selectionne ensuite cette cellule
    // pour faire apparaitre une nouvelle tourelle
    auto it = std::find_if(cells.begin(), cells.end(),
        [x, y](Render::Cell& c){ return c.bounds.contains(x, y); });
    if (it == cells.end()) return; // clic hors grille

    // Empeche le joueur de positionné des tourelles dans la zone d'apparition des ennemies et dans les zones déja occupées par les tourelles
    if (it->col < 3 || it->col == pathfinder.gridCols - 1) {
        std::cout << "Impossible de placer une tourelle dans les 3 premières colonnes !\n";
        return;
    }

    if (it->turreted) {
        std::cout << "Cellule " << it->id << " déjà occupée !\n";
        return;
    }

        // Vérification si un ennemi est sur cette cellule
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

    // Vérifie si le chemin reste possible
    sf::Vector2f entryPos = cells.front().center; // point d'entrée des ennemis
    if (!pathfinder.IsPathStillPossible(entryPos)) {
        std::cout << "Placement refusé : cela bloquerait le chemin !\n";
        pathfinder.Nodes[index].turreted = false; // annule la pose
        return;
    }

    // Détermine le coût de la tourelle selon son type (0...4)
int towerCost = 0;
switch (type) {
    case 0: towerCost = 10; break;
    case 1: towerCost = 30; break;
    case 2: towerCost = 40; break;
    case 3: towerCost = 40; break;
    case 4: towerCost = 50; break;
}

// Vérifie si le joueur a assez de ressources
if (player->getRessources() < towerCost) {
    std::cout << "Pas assez de ressources ! Il faut " << towerCost 
              << " mais tu as " << player->getRessources() << ".\n";
    pathfinder.Nodes[index].turreted = false;
    return;
}

//  Si oui, retire le coût
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
    tourelles.back()->setGridCoords(it->col, it->row); // définit les coordonnées logiques dans la grille
    it->turreted = true;

    std::cout << "Tourelle placée en cellule " << it->id << "\n";

    // Indique à tous les ennemis de recalculer leur chemin au prochain update
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
                  std::vector<Render::Cell>& cells,
                  float dt,
                  std::vector<Enemy*>& enemies,
                  std::vector<Tourelle*>& tourelles,
                  Player* player)
{
    // ---------------------------------------------------------
    // 1) Donner la liste des tourelles aux ennemis spéciaux
    //    (FlyEnemy / TargetEnemy tirent sur les tourelles)
    // ---------------------------------------------------------
    // On utilse un dynamic_cast pour vérifier le type d'ennemi ( que le pointeur Enemy* (e) pointe bien vers un FlyEnemy ou TargetEnemy )
    for (auto* e : enemies) {
        if (!e) continue;

        if (auto* f = dynamic_cast<FlyEnemy*>(e)) {
            f->setTowerList(&tourelles);
        } 
        else if (auto* t = dynamic_cast<TargetEnemy*>(e)) {
            t->setTowerList(&tourelles);
        }
    }


    // ---------------------------------------------------------
    // 2) Mise à jour des ennemis + gestion arrivée / mort
    // ---------------------------------------------------------
    for (auto it = enemies.begin(); it != enemies.end(); )
    {
        Enemy* e = *it;
        if (!e) {
            it = enemies.erase(it);
            continue;
        }

        e->update(pathfinder, cells);

        // Traitement de la fin ou de la mort de l'ennemi
        if (e->reachedGoal) {
            // L'ennemi a atteint la fin : réduit la vie du joueur
            player->reduceHealth(e->getDamage());
            delete e;
            it = enemies.erase(it);
            std::cout << "Un ennemi a atteint la fin ! Vie du joueur : "
                      << player->getHealth() << "\n";
        } 
        else if (e->isDead()) {
            // L'ennemi est mort avant d'atteindre la fin : donne des ressources
            player->AddRessources(e->ressourceValue);
            delete e;
            it = enemies.erase(it);
            std::cout << "Ennemi tué ! Ressources gagnées : " << e->ressourceValue 
                      << " | Total : " << player->getRessources() << "\n";
        } 
        else {
            ++it;
        }
    }

    // ---------------------------------------------------------
    // 3a) Les tourelles tirent sur les ennemis
    // ---------------------------------------------------------
    for (auto* t : tourelles) {
        if (!t || t->isDestroyed()) continue;
        t->tryShoot(dt, enemies, projectiles);
    }

    // ---------------------------------------------------------
    // 3b) Les ennemis spéciaux tirent sur les tourelles
    // ---------------------------------------------------------
    for (auto* e : enemies) {
        if (!e || e->isDead()) continue;

        // On effectue le try shoot pour spécifiquement les FlyEnemy
        if (auto* f = dynamic_cast<FlyEnemy*>(e)) {
            f->tryShoot(dt, projectiles);
        } 
        // On effectue le try shoot pour spécifiquement les TargetEnemy
        else if (auto* t = dynamic_cast<TargetEnemy*>(e)) {
            t->tryShoot(dt, projectiles);
        }
    }

    // ---------------------------------------------------------
    // 4) Avancer tous les projectiles
    // ---------------------------------------------------------
    for (auto& p : projectiles) {
        p.update(dt);
    }

    // ---------------------------------------------------------
    // 5) Collisions projectile / ennemi
    //    (projectiles tirés par les tourelles)
    // ---------------------------------------------------------
    for (auto& p : projectiles) {
        if (!p.isAlive()) continue;

        for (auto* e : enemies) {
            if (!e || e->isDead()) continue;

            // ignorer les ennemis non autorisés par le projectile
            if (!p.accepts(e)) continue;

            const float enemyRadius = e->getRadius();
            if (p.collidesWith(e->getPosition(), enemyRadius)) {
                e->takeDamage(p.getDamage());
                p.kill();
                break;
            }
        }
    }

    // ---------------------------------------------------------
    // 6) Collisions projectile / tourelle
    //    (projectiles tirés par les ennemis)
    // ---------------------------------------------------------
    for (auto& p : projectiles) {
    if (!p.isAlive()) 
        continue;

    // On ne traite ici que les projectiles qui ciblent les tourelles
    if (p.getTargetType() != Projectile::TargetType::Towers)
        continue;

    for (auto* t : tourelles) {
        if (!t || t->isDestroyed()) 
            continue;

        const float towerRadius = t->getRadius();
        if (p.collidesWith(t->getPosition(), towerRadius)) {
            // La tourelle prend des dégâts
            t->takeDamage(p.getDamage());
            // Le projectile est consommé
            p.kill();
            break; // on arrête de tester ce projectile sur les autres tourelles
        }
    }
}

// Suppression des tourelles détruites
//  + remise à false des flags turreted sur la grille et sur A*
for (auto it = tourelles.begin(); it != tourelles.end(); ) {
    Tourelle* t = *it;

    if (!t || t->isDestroyed()) {
        if (t) {
            // On récupère la position logique de la tourelle dans la grille
            int col = t->getGridCol();
            int row = t->getGridRow();

            if (col >= 0 && row >= 0) {
                // On retrouve la cellule correspondante dans le tableau de cellules
                auto cellIt = std::find_if(
                    cells.begin(), cells.end(),
                    [col, row](const Render::Cell& c) {
                        return c.col == col && c.row == row;
                    }
                );

                if (cellIt != cells.end()) {
                    // Libère la cellule : on peut à nouveau poser une tourelle ici
                    cellIt->turreted = false;

                    // Libère aussi le node côté pathfinding A*
                    int index = cellIt->row * pathfinder.gridCols + cellIt->col;
                    pathfinder.Nodes[index].turreted = false;
                }
            }
        }

        delete t;
        it = tourelles.erase(it);
    } else {
        ++it;
    }
}

    // ---------------------------------------------------------
    // 7) Nettoyage des projectiles morts
    // ---------------------------------------------------------

    // On utilise la fonction remove_if pour supprimer du tableau les projectiles morts, et on le réorganise pour n'avoir que les vivants 
    // ( Evite d'avoir un tableau gigantesque de projectiles morts )
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
                       [](const Projectile& pr){ return !pr.isAlive(); }),
        projectiles.end()
    );

    if (player->getHealth() <= 0) {
            player->reduceHealth(0); // s'assure que c'est exactement 0
            gameOver = true;
            std::cout << "GAME OVER !" << std::endl;
        }
}


    void Game::upgradeTourelle(std::vector<Tourelle*>& tourelles, Player* player, float mouseX, float mouseY)
{
    // Au clic de la moulette de souris, on vérifie si le clic correspond à la position d'une tourelle (sa hitbox) puis on compare les ressources 
    // pour savoir si le joueur peut l'améliorer, si oui alors on fait l'amélioration et on déduit le coût des ressources du joueur.
    sf::Vector2f mousePos(mouseX, mouseY);
    for (auto* t : tourelles) {
        if (!t) continue;
        if (t->getGlobalBounds().contains(mousePos)) {
            int upgradeCost = static_cast<int>(t->getCost()); // coût actuel avant upgrade

            if (player->getRessources() >= upgradeCost) {
                player->reduceRessources(upgradeCost);
                t->upgrade1();
                std::cout << "Tourelle améliorée ! Nouveau coût : " << t->getCost() 
                          << " | Ressources restantes : " << player->getRessources() << "\n";
            } else {
                std::cout << "Pas assez de ressources pour améliorer cette tourelle ! "
                          << "(Besoin : " << upgradeCost << ", Tu as : " << player->getRessources() << ")\n";
            }
            return; // une seule tourelle à la fois
        }
    }

    std::cout << "Aucune tourelle sélectionnée pour upgrade.\n";
}
// ====================== Game::startWaves ======================
// Initialise les variables pour démarrer les vagues d'ennemis
void Game::startWaves() {
    waveIndex = 0;
    toSpawn   = waves[0].count;
    spawnIv   = waves[0].interval;
    spawnT    = 0.f;
    interWave = false;
    interT    = 0.f;
    wavesFinished = false;
}

// ====================== Game::updateWaves ======================
// Gère le timing et le spawn des vagues d'ennemis
void Game::updateWaves(float dt,
                       std::vector<Enemy*>& enemies,
                       const std::vector<Render::Cell>& cells)
{
    // si les vagues sont terminées ou pas encore commencées, ne fait pas d'update 
    if (wavesFinished || waveIndex < 0) return;

    // Phase inter-vague (petit délai avant la suivante)
    if (interWave) {
        interT += dt;
        if (interT >= interDelay) {
            interWave = false;
            interT = 0.f;

            // Passe à la vague suivante
            ++waveIndex;
            if (waveIndex >= (int)waves.size()) {
                wavesFinished = true;
                std::cout << "Toutes les vagues sont terminées !\n";
                return;
            }
            // regarde combien il reste d'ennemis à spawn dans la nouvelle vague
            toSpawn = waves[waveIndex].count;
            // réinitialise le timer de spawn
            spawnIv = waves[waveIndex].interval;
            spawnT  = 0.f;
            std::cout << "Vague " << (waveIndex+1) << " !\n";
        }
        return;
    }

    // Phase de spawn de la vague en cours
    if (toSpawn > 0) {
        spawnT += dt;
        while (toSpawn > 0 && spawnT >= spawnIv) {
            generateEnemy(enemies, cells); // ta fonction existante
            --toSpawn;
            spawnT -= spawnIv;
        }
        return;
    }

    // Vague finie côté "spawn" : on attend que tous les ennemis soient morts/partis
    bool encoreDesEnnemis = false;
    for (auto* e : enemies) { if (e && !e->isDead()) { encoreDesEnnemis = true; break; } }
    if (!encoreDesEnnemis) {
        // lancer le délai avant la prochaine vague
        interWave = true;
        interT = 0.f;
        std::cout << "Vague " << (waveIndex+1) << " terminée. Prochaine dans " << interDelay << "s.\n";
    }
}
