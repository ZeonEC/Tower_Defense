#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

//------------------------ INCLUDE libs ----------------------//

#include <SFML/Graphics.hpp>
#include <vector>

//------------------------ INCLUDE prog ----------------------//

#include "enemy.hpp"     // Pour EnemyKind et Enemy::getKind()


// ====================================== ROLE DE LA CLASSE =========================================== //

// La classe Projectile représente les tirs lancés par les tourelles. 
// Chaque projectile se déplace vers une cible et inflige des dégâts quand il entre en collision avec un ennemi.
// Ses propriété peuvent changer en fonction du type de tourelle

// ==================================================================================================== //

class Enemy;      // fwd declarations pour éviter les inclusions circulaires
class Tourelle;   // (pas utilisé ici dans l'API, mais utile si tu en auras besoin plus tard)

class Projectile {

    public:
//--------------------------------------------------------------------------------//

    using EnemyMask = unsigned;

    // Optionnel: exposer un accesseur si ça t’aide à debugger
    EnemyMask allowedMask() const noexcept { return allowedMask_; }

    // Le projectile ignore toute collision non autorisée
    bool accepts(const Enemy* e) const noexcept;

//--------------------------------------------------------------------------------//

    Projectile(const sf::Vector2f& start,
               const sf::Vector2f& target,
               float speed = 450.f,
               int   damage = 0.0,
               float radius = 4.f,
               EnemyMask allowedMask = ~EnemyMask{0});

    void update(float dt);

    void draw(sf::RenderTarget& win) const;

    bool isAlive() const { return alive; }
    void kill()          { alive = false; }

    // collision simple cercle (projectile) / cercle (ennemi)
    bool collidesWith(const sf::Vector2f& enemyPos, float enemyRadius) const;

    int  getDamage() const { return dmg; }
    sf::Vector2f getPosition() const { return shape.getPosition(); }
    float getRadius() const { return shape.getRadius(); }


private:

    sf::CircleShape shape;
    sf::Vector2f    velocity;   // pixels/sec
    int             dmg;
    bool            alive = true;

    static constexpr EnemyMask kindBit(EnemyKind k) {
        return 1u << static_cast<unsigned>(k);
    }

    EnemyMask allowedMask_ = ~0u; // par défaut: tout autorisé (sécurise l’ancien code)
};

#endif