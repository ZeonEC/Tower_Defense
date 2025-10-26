#include "enemy.hpp"
#include <cmath> // pour sqrtf
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
Enemy::Enemy(int hp, float spd, float radius, sf::Color color) : dead(false), health(hp), speed(spd), shape(radius) 
{
    shape.setFillColor(color);
    shape.setOrigin(radius, radius); // origine au centre pour faciliter la position
    counter++; // Incrémente le compteur d'ennemis à chaque création
}


void Enemy::update(PathFinding_AStar& pathfinder,const std::vector<Render::Cell>& cells) {
    // Recalcule le chemin si on en a pas ou si on a atteint la fin du chemin courant
    if (pathPoints.empty() || currentPathIndex >= static_cast<int>(pathPoints.size()) || needRepath) {
        pathPoints = pathfinder.SolveAStar(getPosition());
        currentPathIndex = 0;
        needRepath = false;
    }

    // Debug (optionnel) : affiche le chemin
    //for (size_t j = 0; j < pathPoints.size(); ++j)
    //    std::cout << "path[" << j << "] = (" << pathPoints[j].x << ", " << pathPoints[j].y << ")\n";

    if (currentPathIndex < static_cast<int>(pathPoints.size())) {
        sf::Vector2f pos = getPosition();
        sf::Vector2f target = pathPoints[currentPathIndex];
        sf::Vector2f dir = target - pos;
        float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

        float step = getSpeed(); // distance à parcourir par appel (tu peux multiplier par dt si tu changes la signature pour en avoir un)

        if (pathfinder.EnemyNode) {
        pathfinder.EnemyNode->EnemyPresence = false; // libère l’ancien node
        }


        if (dist <= step || dist == 0.f) {
            // On snappe exactement au centre du nœud et on passe au suivant
            shape.setPosition(target);
            ++currentPathIndex;
        } else {
            // Normalise et avance
            dir.x /= dist;
            dir.y /= dist;
            pos.x += dir.x * step;
            pos.y += dir.y * step;
            shape.setPosition(pos);
        }
    } else {
        // Pas de chemin trouvé ou déjà arrivé : rien à faire
    }
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



    