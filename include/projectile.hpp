#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include <SFML/Graphics.hpp>
#include <vector>

#include "enemy.hpp"   // Pour EnemyKind / Enemy

class Enemy;
class Tourelle;

class Projectile {
public:
    enum class TargetType { Enemies, Towers };

    using EnemyMask = unsigned;

    EnemyMask allowedMask() const noexcept { return allowedMask_; }
    bool accepts(const Enemy* e) const noexcept;

    // ✅ Déclaration SEULE (plus de corps ici)
    Projectile(const sf::Vector2f& start,
               const sf::Vector2f& target,
               float speed = 450.f,
               int   damage = 0,
               float radius = 4.f,
               EnemyMask allowedMask = ~EnemyMask{0},
               TargetType tt = TargetType::Enemies);

    void update(float dt);
    void draw(sf::RenderTarget& win) const;

    bool isAlive() const { return alive; }
    void kill()          { alive = false; }

    bool collidesWith(const sf::Vector2f& pos, float radius) const;

    int  getDamage() const { return dmg; }
    sf::Vector2f getPosition() const { return shape.getPosition(); }
    float getRadius() const { return shape.getRadius(); }

    bool targetsTowers() const noexcept { return targetType == TargetType::Towers; }

private:
    sf::CircleShape shape;
    sf::Vector2f    velocity;   // px/s
    int             dmg         = 0;
    bool            alive       = true;

    static constexpr EnemyMask kindBit(EnemyKind k) {
        return 1u << static_cast<unsigned>(k);
    }

    EnemyMask  allowedMask_ = ~0u;
    TargetType targetType   = TargetType::Enemies;
};

#endif
