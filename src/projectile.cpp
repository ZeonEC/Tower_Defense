#include "projectile.hpp"
#include <cmath>

static sf::Vector2f normalized(const sf::Vector2f& v) {
    float n = std::sqrt(v.x*v.x + v.y*v.y);
    return (n > 0.f) ? sf::Vector2f(v.x/n, v.y/n) : sf::Vector2f(0.f,0.f);
}

Projectile::Projectile(const sf::Vector2f& start,
                       const sf::Vector2f& target,
                       float speed,
                       int   damage,
                       float radius,
                       EnemyMask allowedMask) : dmg(damage), allowedMask_(allowedMask)
{
    shape.setRadius(radius);
    shape.setOrigin(radius, radius);
    shape.setFillColor(sf::Color::Yellow);
    shape.setPosition(start);

    auto dir = normalized(target - start);
    velocity = dir * speed;
}

bool Projectile::accepts(const Enemy* e) const noexcept {
    if (!e) return false;
    const EnemyKind k = e->getKind();
    return (allowedMask_ & kindBit(k)) != 0;
}

void Projectile::update(float dt) {
    if (!alive) return;
    shape.move(velocity * dt);

    auto p = shape.getPosition();
    if (p.x < -100 || p.y < -100 || p.x > 5000 || p.y > 5000) {
        alive = false;
    }
}

void Projectile::draw(sf::RenderWindow& win) const {
    if (alive) win.draw(shape);
}

bool Projectile::collidesWith(const sf::Vector2f& enemyPos, float enemyRadius) const {
    auto p  = shape.getPosition();
    auto dx = p.x - enemyPos.x;
    auto dy = p.y - enemyPos.y;
    float r = shape.getRadius() + enemyRadius;
    return (dx*dx + dy*dy) <= (r*r);
}
