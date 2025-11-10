#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include <SFML/Graphics.hpp>
#include <vector>

// Forward declarations pour éviter les inclusions circulaires
class Enemy;
class Tourelle;
enum class EnemyKind;

// ====================================== ROLE DE LA CLASSE =========================================== //
//
// La classe Projectile représente les tirs lancés par les tourelles OU par les ennemis.
// Chaque projectile se déplace vers une cible et inflige des dégâts quand il entre en collision.
// ==================================================================================================== //

class Projectile {

public:

    // Type principal de cible du projectile
    enum class TargetType {
        Enemies,   // projectile destiné aux ennemis
        Towers     // projectile destiné aux tourelles
    };

    using EnemyMask = unsigned;

    EnemyMask allowedMask() const noexcept { return allowedMask_; }

    // Le projectile ignore toute collision non autorisée (côté ENNEMIS)
    bool accepts(const Enemy* e) const noexcept;

    Projectile(const sf::Vector2f& start,
               const sf::Vector2f& target,
               float speed        = 450.f,
               int   damage       = 0,
               float radius       = 4.f,
               EnemyMask allowedMask = ~EnemyMask{0},
               TargetType targetType = TargetType::Enemies);

    void update(float dt);
    void draw(sf::RenderTarget& win) const;

    bool isAlive() const { return alive; }
    void kill()          { alive = false; }

    // collision simple cercle / cercle
    bool collidesWith(const sf::Vector2f& enemyPos, float enemyRadius) const;

    int          getDamage()   const { return dmg; }
    sf::Vector2f getPosition() const { return shape.getPosition(); }
    float        getRadius()   const { return shape.getRadius(); }

    TargetType getTargetType() const noexcept { return targetType_; }

private:

    sf::CircleShape shape;
    sf::Vector2f    velocity;   // pixels/sec
    int             dmg;
    bool            alive = true;

    static constexpr EnemyMask kindBit(EnemyKind k) {
        return 1u << static_cast<unsigned>(k);
    }

    EnemyMask allowedMask_ = ~0u;                 // pour filtrer les ENNEMIS
    TargetType targetType_ = TargetType::Enemies; // type de cible (ennemis / tourelles)
};

#endif
