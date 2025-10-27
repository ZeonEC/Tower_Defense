#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include <SFML/Graphics.hpp>
#include <vector>

class Enemy;      // fwd declarations pour éviter les inclusions circulaires
class Tourelle;   // (pas utilisé ici dans l'API, mais utile si tu en auras besoin plus tard)

class Projectile {
public:
    Projectile(const sf::Vector2f& start,
               const sf::Vector2f& target,
               float speed = 450.f,
               int   damage = 0.0,
               float radius = 4.f);

    void update(float dt);

    void draw(sf::RenderWindow& win) const;

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
};

#endif