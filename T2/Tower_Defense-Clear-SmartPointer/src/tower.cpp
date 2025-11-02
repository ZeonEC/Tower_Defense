#include "tower.hpp"
#include "projectile.hpp"
#include "enemy.hpp"

#include <algorithm>
#include <cmath>

// --------- Utilitaires internes ---------
namespace 
{
    inline float sqr(float x) { return x * x; }

    inline float dist2(const sf::Vector2f& a, const sf::Vector2f& b) {
        return sqr(a.x - b.x) + sqr(a.y - b.y);
    }
}

int FlyTourelle::counter = 0;
int BasicTourelle::counter = 0;
int PoisonTourelle::counter = 0;
int shotgunTourelle::counter = 0;
int TargetTourelle::counter = 0;


// ========================================================================== //

//                         FONCTIONS STANDARDS TOURELLES                      // 

// ========================================================================== //


Tourelle::Tourelle(int damage,
                   float range,
                   float fireRate,
                   float projectileSpeed,
                   float radius,
                   sf::Color color,
                   int cost)
: damage(damage),
  range(range),
  fireRate(fireRate),
  cooldown(0.f),
  projectileSpeed(projectileSpeed),
  cost(cost)
{
    shape.setRadius(radius);
    shape.setPointCount(4); 
    shape.setOrigin(radius, radius);
    shape.setFillColor(color);
    shape.setRotation(45);
}

Tourelle::~Tourelle() = default;

void Tourelle::setPosition(float x, float y) {
    shape.setPosition(x, y);
}

sf::Vector2f Tourelle::getPosition() const {
    return shape.getPosition();
}

void Tourelle::draw(sf::RenderTarget& win) const {
    win.draw(shape);

    // (Optionnel) Un petit indicateur de portée en transparence :
     //sf::CircleShape rangeCircle(range);
     //rangeCircle.setOrigin(range, range);
     //rangeCircle.setPosition(shape.getPosition());
     //rangeCircle.setFillColor(sf::Color(255,255,255,20));
     //rangeCircle.setOutlineColor(sf::Color(255,255,255,60));
     //rangeCircle.setOutlineThickness(1.f);
     //win.draw(rangeCircle);
}

void Tourelle::update(float dt) {
    cooldown = std::max(0.f, cooldown - dt);
}

Enemy* Tourelle::acquireTarget(const std::vector<Enemy*>& enemies) const {
    const float r2 = range * range;
    Enemy* best = nullptr;
    float bestD2 = r2;
    auto  p = getPosition();

    for (auto e : enemies) {
        if (!e || e->isDead()) continue;

        // ⬅️ filtre par type d'ennemi
        if (!accepts(e)) continue;

        float d2 = (e->getPosition().x - p.x) * (e->getPosition().x - p.x)
                 + (e->getPosition().y - p.y) * (e->getPosition().y - p.y);

        if (d2 <= bestD2) {
            bestD2 = d2;
            best = e;
        }
    }
    return best;
}


bool Tourelle::tryShoot(float dt,
                        const std::vector<Enemy*>& enemies,
                        std::vector<Projectile>& outProjectiles)
{
    // 1) mettre à jour le cooldown local (et tout état interne)
    update(dt);
    if (cooldown > 0.f) return false;

    // 2) choisir une cible
    Enemy* target = acquireTarget(enemies);
    if (!target) return false;

    // 3) créer un projectile
    // La direction précise/anticipation est gérée dans Projectile (direction vers la cible actuelle).
    outProjectiles.emplace_back(
    getPosition(),
    target->getPosition(),
    projectileSpeed,
    damage,
    /*radius*/ 4.f,
    /*allowedMask*/ allowedMask
);

    // 4) reset cooldown
    cooldown = (fireRate > 0.f) ? (1.f / fireRate) : 0.25f;
    return true;
}

// --------- Utilitaire local pour rotation de vecteur ---------
static sf::Vector2f rotateVector(const sf::Vector2f& v, float degrees)
{
    float rad = degrees * 3.14159265f / 180.f;
    float cs = std::cos(rad);
    float sn = std::sin(rad);
    return sf::Vector2f(v.x * cs - v.y * sn, v.x * sn + v.y * cs);
}

// ===================== shotgunTourelle tir multiple =====================

bool shotgunTourelle::tryShoot(float dt,
                               const std::vector<Enemy*>& enemies,
                               std::vector<Projectile>& outProjectiles)
{
    cooldown = std::max(0.f, cooldown - dt);
    if (cooldown > 0.f) return false;

    Enemy* target = acquireTarget(enemies);
    if (!target) return false;

    // direction principale
    sf::Vector2f toTarget = target->getPosition() - getPosition();

    // liste des angles pour le cône de tir
    std::vector<float> angles = { 0.f, 22.5f, -22.5f, 45.f, -45.f, 67.5f };

    for (float a : angles)
    {
        sf::Vector2f dir = rotateVector(toTarget, a);
        sf::Vector2f targetPos = getPosition() + dir; // point visé légèrement décalé
        
        outProjectiles.emplace_back(
            getPosition(),
            target->getPosition(),
            projectileSpeed,
            damage,
            /*radius*/ 4.f,
            /*allowedMask*/ allowedMask
        );
    }

    cooldown = (fireRate > 0.f) ? (1.f / fireRate) : 0.25f;
    return true;
}

// FONCTION DE CIBLAGE SUR UNE CLASSE UNIQUE (plutot que d'utiliser les mask mais je trouve que c'est plus long car c'est une fonction par type d'ennemie ici)

/*Enemy* FlyTourelle::acquireTarget(const std::vector<Enemy*>& enemies) const {
    const float r2 = range * range;
    Enemy* best = nullptr;
    float bestD2 = r2;
    auto  p = getPosition();

    for (auto e : enemies) {
        if (!e || e->isDead()) continue;

        // On ne vise que les ennemis volants
        if (dynamic_cast<FlyEnemy*>(e) == nullptr)
            continue;

        float d2 = (e->getPosition().x - p.x) * (e->getPosition().x - p.x)
                 + (e->getPosition().y - p.y) * (e->getPosition().y - p.y);

        if (d2 <= bestD2) {
            bestD2 = d2;
            best = e;
        }
    }
    return best;
}*/


// ===================== Variantes =====================

//BasicTourelle::BasicTourelle(float radius)
//: Tourelle(
//    /*damage*/          1000,
//    /*range*/           220.f,
//    /*fireRate*/        1.0f,   // 1 tir / s
//    /*projectileSpeed*/ 450.f,
//    /*radius*/          radius,
//    /*color*/           sf::Color(60, 180, 255)
//) {
//    // Autoriser tout SAUF les volants :
//   forbid(EnemyKind::Fly);
//}

// Poison : dégâts faibles + cadence correcte + portée moyenne
//PoisonTourelle::PoisonTourelle(float radius)
//: Tourelle(
//    /*damage*/          600,
//   /*range*/           200.f,
//    /*fireRate*/        1.2f,
//    /*projectileSpeed*/ 420.f,
//    /*radius*/          radius,
//    /*color*/           sf::Color(150, 255, 150)
//) {
//    // Autoriser tout SAUF les volants :
//    forbid(EnemyKind::Fly);
//}

// Shotgun : cadence élevée, dégâts faibles, faible portée
//shotgunTourelle::shotgunTourelle(float radius)
//: Tourelle(
//    /*damage*/          100,
//    /*range*/           160.f,
//    /*fireRate*/        3.0f,
//    /*projectileSpeed*/ 680.f,
//    /*radius*/          radius,
//    /*color*/           sf::Color(200, 200, 255)
//) 
//{
//    // Autoriser tout SAUF les volants :
//   forbid(EnemyKind::Fly);
//}

//int FlyTourelle::counter = 0;

// Fly : stats équilibrées, couleur violette (et compteur si tu l’utilises)
//FlyTourelle::FlyTourelle(float radius)
//: Tourelle(
 //   /*damage*/          800,
//    /*range*/           220.f,
//    /*fireRate*/        1.2f,
//    /*projectileSpeed*/ 520.f,
//    /*radius*/          radius,
//    /*color*/           sf::Color(200, 0, 0)
//) { ++counter; }

//FlyTourelle::~FlyTourelle() { --counter; }

// Target : portée un peu plus grande, dégâts moyens
//TargetTourelle::TargetTourelle(float radius)
//: Tourelle(
//    /*damage*/          1000,
//    /*range*/           240.f,
//    /*fireRate*/        1.0f,
//    /*projectileSpeed*/ 450.f,
//    /*radius*/          radius,
//    /*color*/           sf::Color(255, 200, 140)
//
//) {
    // Autoriser tout SAUF les volants :
//    forbid(EnemyKind::Fly);
//}
