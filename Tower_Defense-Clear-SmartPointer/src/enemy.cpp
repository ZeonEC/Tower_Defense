#include "enemy.hpp"

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
Enemy::Enemy(int hp, float spd, float radius, const std::string& texturePath, int ressourceValue)
: health(hp), speed(spd), dead(false), eat(false), ressourceValue(ressourceValue)
{
    // Charger la texture
    if (!texture.loadFromFile(texturePath)) {
        std::cerr << "Erreur : impossible de charger la texture : " << texturePath << "\n";
        // Fallback minimal : texture 1x1 blanche pour éviter un sprite vide
        sf::Image img; img.create(1, 1, sf::Color::White);
        texture.loadFromImage(img);
    }
    sprite.setTexture(texture);

    // Centrer l'origine
    const auto local = sprite.getLocalBounds(); // taille en pixels de la texture
    sprite.setOrigin(local.width * 0.5f, local.height * 0.5f);

    // Mise à l’échelle pour que le sprite ait "radius" de rayon visuel
    // => diamètre visuel = 2 * radius
    const float targetDiameter = radius * 2.f;
    // éviter div/0 si texture très petite
    float sx = targetDiameter / std::max(1.f, local.width);
    float sy = targetDiameter / std::max(1.f, local.height);
    sprite.setScale(sx, sy);

    ++counter; // compteur global d'ennemis
}

// ----------------------------------------------------------
// Gestion des points de vie

   void Enemy::hit(int amount) {
    health -= amount;
    if (health < 0) health = 0;
    if (health == 0) dead = true;

    // Feedback visuel simple : assombrir le sprite
    sf::Color c = sprite.getColor();
    sprite.setColor(sf::Color(c.r / 2, c.g / 2, c.b / 2, 180));
    }

    void Enemy::draw(sf::RenderTarget& win) const {
    if (!dead) win.draw(sprite);
    }


void Enemy::update(PathFinding_AStar& pathfinder,const std::vector<Render::Cell>& cells) {
    
    if (dead) {return;};

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
            setPosition(target.x,target.y);
            ++currentPathIndex;
        } else {
            // Normalise et avance
            dir.x /= dist;
            dir.y /= dist;
            pos.x += dir.x * step;
            pos.y += dir.y * step;
            setPosition(pos.x,pos.y);
        }
    }
        // ✅ Si on a dépassé la fin du chemin, l'ennemi a atteint la goal
    if (currentPathIndex >= static_cast<int>(pathPoints.size())) {
        reachedGoal = true;
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

    float Enemy::getRadius() const 
    {
        const float r = Render::Cell::cellSize * 0.5f;
        return r;
    }

    void Enemy::takeDamage(int amount) {
    if (dead) return;
    health -= amount;
    if (health <= 0) {
        health = 0;
        dead = true;
        sprite.setColor(sf::Color(60, 60, 60, 180));
    }
}

//----------------------------------------------------------
//Ennemi de base 

BasicEnemy::BasicEnemy()
    : Enemy(
        /*hp*/     100,
        /*speed*/  1,
        /*radius*/ Render::Cell::cellSize * 0.5f, // taille = cellule
        /*texturePath*/ "../src/assets/enemies/basic.png",
        1
    ) { ++counter; }
    
    BasicEnemy::~BasicEnemy() { --BasicEnemy::counter; }

//----------------------------------------------------------
//Ennemi rapide (comportement identique au ennemie de base, hp et vitesse change)

    FastEnemy::FastEnemy()
    : Enemy(
        /*hp*/     75,
        /*speed*/  2,
        /*radius*/ Render::Cell::cellSize * 0.25f, // taille = cellule/2
        /*texturePath*/ "../src/assets/enemies/Fast.png",
        2
    ) { ++counter; }

    FastEnemy::~FastEnemy() { --FastEnemy::counter; } 

//----------------------------------------------------------
//Ennemi tank (comportement identique au ennemie de base, hp et vitesse change) 

    TankEnemy::TankEnemy()
    : Enemy(
        /*hp*/     300,
        /*speed*/  0.5,
        /*radius*/ Render::Cell::cellSize * 0.6f, // taille = cellule *0.6
        /*texturePath*/ "../src/assets/enemies/Tank.png",
        5
    ) { ++counter; }

    TankEnemy::~TankEnemy() { --TankEnemy::counter; } 


//----------------------------------------------------------
//Ennemi target (leur objectif est de détruire les tourelles sur leur passage (chemin le plus cours en ignorant les tourelles)
//hp et vitesse change) 

    TargetEnemy::TargetEnemy()
    : Enemy(
        /*hp*/     150,
        /*speed*/  0.75,
        /*radius*/ Render::Cell::cellSize * 0.5f,     
        /*texturePath*/ "../src/assets/enemies/Target.png",
        3
    ) { ++counter; }

    TargetEnemy::~TargetEnemy() { --TargetEnemy::counter; }

//----------------------------------------------------------
//Ennemi volatile (ils ignorent les tourelles, vole par dessus, hp et vitesse change) 

    FlyEnemy::FlyEnemy()
    : Enemy(
        /*hp*/     100,
        /*speed*/  2,
        /*radius*/ Render::Cell::cellSize * 0.4f,
        /*texturePath*/ "../src/assets/enemies/Fly.png",
        2
    ) { ++counter; }

    FlyEnemy::~FlyEnemy() { --FlyEnemy::counter; }


  /*  void Enemy::generateEnemy ()
    {
        r = rand();         // génère un nombre "aléatoire"
        switch (r)
        {
        case 0:
            enemies.push_back(new BasicEnemy());
        case 1:
            enemies.push_back(new FastEnemy());
        case 2:
            enemies.push_back(new TankEnemy());
        case 3:
            enemies.push_back(new FlyEnemy());
        case 4:
            enemies.push_back(new TargetEnemy());
            break;
        
        default:
            break;
        }
    }*/

    /*
    void Enemy::destroyEnemy (std::vector<Enemy*>& enemies)
    {
        // Ici il faut libérer la mémoire à la main
        for (auto e : enemies) {
            delete e;  // appelle le destructeur
        }
    }
*/

    