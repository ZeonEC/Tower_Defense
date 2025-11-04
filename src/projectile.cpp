#include "projectile.hpp"
#include <cmath>

// Fonction utilitaire pour normaliser un vecteur
static sf::Vector2f normalized(const sf::Vector2f& v) {
    float n = std::sqrt(v.x * v.x + v.y * v.y);
    return (n > 0.f) ? sf::Vector2f(v.x / n, v.y / n) : sf::Vector2f(0.f, 0.f);
}

// ----------------------------------------------------------
// Constructeur complet (correspond à la déclaration du header)
// ----------------------------------------------------------
Projectile::Projectile(const sf::Vector2f& start,
                       const sf::Vector2f& target,
                       float speed,
                       int   damage,
                       float radius,
                       EnemyMask allowedMask,
                       TargetType tt)     // ✅ paramètre ajouté pour correspondre au header
: dmg(damage),
  allowedMask_(allowedMask),
  targetType(tt)          // ✅ on initialise le type de cible
{
    // Définition de l'apparence du projectile
    shape.setRadius(radius);
    shape.setOrigin(radius, radius);
    shape.setFillColor(sf::Color::Yellow);
    shape.setPosition(start);

    // Calcul de la direction normalisée et de la vitesse
    auto dir = normalized(target - start);
    velocity = dir * speed;
}

// ----------------------------------------------------------
// Vérifie si le projectile peut toucher un type d'ennemi
// ----------------------------------------------------------
bool Projectile::accepts(const Enemy* e) const noexcept {
    if (!e) return false;
    const EnemyKind k = e->getKind();
    return (allowedMask_ & kindBit(k)) != 0;
}

// ----------------------------------------------------------
// Mise à jour du projectile chaque frame
// ----------------------------------------------------------
void Projectile::update(float dt) {
    if (!alive) return;
    shape.move(velocity * dt);

    // Si le projectile sort de l’écran → suppression
    auto p = shape.getPosition();
    if (p.x < -100 || p.y < -100 || p.x > 5000 || p.y > 5000) {
        alive = false;
    }
}

// ----------------------------------------------------------
// Dessine le projectile (si actif)
// ----------------------------------------------------------
void Projectile::draw(sf::RenderTarget& win) const {
    if (alive) win.draw(shape);
}

// ----------------------------------------------------------
// Détection de collision (cercle / cercle)
// ----------------------------------------------------------
bool Projectile::collidesWith(const sf::Vector2f& pos, float radius) const {
    const sf::Vector2f p = shape.getPosition();
    const float dx = p.x - pos.x;
    const float dy = p.y - pos.y;
    const float r = shape.getRadius() + radius;
    return (dx * dx + dy * dy) <= (r * r);
}
