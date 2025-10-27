#include "game.hpp"
#include "enemy.hpp"
#include "tower.hpp"
#include "projectile.hpp"  // ⬅️ AJOUT

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
    // Positionne l'ennemi à une position aléatoire
    float rx = std::rand() % 200; // 0..39
    float ry = std::rand() % 800; // 0..39
    enemies.back()->setPosition(rx,ry);
}

void Game::destroyEnemy(std::vector<Enemy*>& enemies) {
    for (auto*& e : enemies) {
        delete e;    // appelle le destructeur virtuel de Enemy (voir note ci-dessous)
        e = nullptr;
    }
    enemies.clear();
}

// ------------ FONCTIONS DES TOURELLES ------------ //

// Quand on a une structure ou un tableau de structure en paramètre, on doit passer par référence (&) pour pouvoir modifier les données
// Si on ne passe pas par référence, on travaille sur une copie locale de la structure,
//  et les modifications ne sont pas visibles en dehors de la fonction
// Par contre si on passe par référence, on travaille directement sur la structure d'origine
//  et les modifications sont visibles en dehors de la fonction
// Ici on passe par référence car on veut modifier le booléen turreted de la cellule
// Sinon ça marche juste pas on peut effectivement récupérer l'id de la cellule mais pas modifier son état (turreted = true) (pas le but)
// DONC PITIE RAPPEL TOI DU & POUR LES STRUCTURES/CLASSES/TABLEAUX STP 

void Game::generateTourelle(std::vector<Tourelle*>& tourelles, std::vector<Render::Cell>& cells, float x, float y) { 
    
    if (cells.empty()) return; // pas de cellule, on ne peut rien faire

    // Trouver la cellule qui contient le clic (x,y)
    auto it = std::find_if(cells.begin(), cells.end(),[x,y]( Render::Cell& c){ return c.bounds.contains(x, y); });
           
    if (it == cells.end()) {
        // clic hors grille
        return;
    }

    // Génération de la fonction rand (pour l'instant, apres ça sera avec les touches)
    int r = std::rand() % 5; // 0..4

    // On fait ça pour éviter de surcharger la lecture quand on crée des tourelles
    const float cs = Render::Cell::cellSize;

    
    if (it->turreted) {
        // déjà une tourelle sur cette cellule
        std::cout << "Cellule " << it->id << " déjà occupée !\n";
        return;
    }

    // Pas de tourelle sur cette cellule alors banco piou piou
    if (it->turreted == false) {
        // la cellule est libre
        switch (r) {
            case 0: tourelles.push_back(new BasicTourelle((cs/2)));  break;
            case 1: tourelles.push_back(new PoisonTourelle((cs/2)));   break;
            case 2: tourelles.push_back(new shotgunTourelle((cs/2)));   break;
            case 3: tourelles.push_back(new TargetTourelle((cs/2)));    break;
            case 4: tourelles.push_back(new FlyTourelle((cs/2))); break;
        }
        it->turreted = true; // on marque la cellule comme occupée
        std::cout << "Tourelle placée en cellule " << it->id << "\n";
    }
        
    
    // Positionne l'ennemi à la position de la souris
    // Trouver la cellule la plus proche de la position de la souris
    tourelles.back()->setPosition(it->center.x, it->center.y);
}

void Game::destroyTourelles(std::vector<Tourelle*>& tourelles) {
    for (auto*& t : tourelles) {
        delete t;    // appelle le destructeur virtuel de tourelles
        t = nullptr;
    }
    tourelles.clear();
}


    void Game::update(float dt, std::vector<Enemy*>& enemies, std::vector<Tourelle*>& tourelles)
    {
        // 1) les tourelles tentent de tirer (création éventuelle de projectiles)
        for (auto t : tourelles) {
            if (!t) continue;
            t->tryShoot(dt, enemies, projectiles);
        }

        // 2) avancer les projectiles
        for (auto& p : projectiles) p.update(dt);

        // 3) collisions projectile/ennemi
        for (auto& p : projectiles) {
            if (!p.isAlive()) continue;
            for (auto e : enemies) {
                if (!e || e->isDead()) continue;
                const float enemyRadius = 20.f; // adapte si tu as un vrai rayon
                if (p.collidesWith(e->getPosition(), enemyRadius)) 
                {
                    std::cout << "Hit " << typeid(*e).name() 
                            << " HP avant=" << e->getHp();
                    e->takeDamage(p.getDamage());
                    std::cout << " HP apres=" << e->getHp() 
                            << " dead=" << e->isDead() << "\n";
                    p.kill();
                    break;
                }
            }
        }

        // 4) nettoyage
        projectiles.erase(
            std::remove_if(projectiles.begin(), projectiles.end(),
                        [](const Projectile& pr){ return !pr.isAlive(); }),
            projectiles.end()
        );
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
