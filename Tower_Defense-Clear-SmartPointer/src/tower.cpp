//------------------------ INCLUDE libs ----------------------//
#include <algorithm>
#include <cmath>

//------------------------ INCLUDE prog ----------------------//
#include "tower.hpp"
#include "projectile.hpp"
#include "enemy.hpp"


// PETITES FONCTIONS UTILITAIRES EN INTERNE
// sqr(x)  : renvoie x² (évite de répéter x * x partout)
// dist2() : renvoie la distance² entre deux points 2D (sans sqrt)
//           → utile pour comparer des distances sans payer la racine carrée.
namespace 
{
    // on fait des inline pour demander au compilateur de "refaire le prog" plustot que de faire un appele de la fonction (on lui demande de faire un ctrl c v dans son code)
    inline float sqr(float x) { return x * x; }

    inline float dist2(const sf::Vector2f& a, const sf::Vector2f& b) {
        return sqr(a.x - b.x) + sqr(a.y - b.y);
    }
}


// COMPTEURS STATIQUES DES TOURELLES
int FlyTourelle::counter     = 0;
int BasicTourelle::counter   = 0;
int PoisonTourelle::counter  = 0;
int shotgunTourelle::counter = 0;
int TargetTourelle::counter  = 0;


// ========================================================================== //
//                         FONCTIONS STANDARDS TOURELLES                      // 
// ========================================================================== //
//
// Tout ce qui suit est lié à la classe de base Tourelle (constructeur,
// update, ciblage, tir simple). Les variantes se contentent de changer
// les paramètres ou de override certaines fonctions.
// ========================================================================== //


// ------------------------ Constructeur de base ------------------------ //
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
  cost(cost),
  hp(50 + cost * 2)   // base 50 PV + 2 PV par point de coût
{
    // Représentation graphique de la tourelle
    shape.setRadius(radius);            // rayon visuel (lié à la taille de la cellule)
    shape.setPointCount(4);            // 4 points → carré
    shape.setOrigin(radius, radius);   // origine au centre
    shape.setFillColor(color);         // couleur selon le type
    shape.setRotation(45);             // rotation de 45° → donne un losange
}

Tourelle::~Tourelle() = default;


// ------------------------ Position / Rendu ------------------------ //

// Change la position de la tourelle dans le monde (centre de la forme)
void Tourelle::setPosition(float x, float y) {
    shape.setPosition(x, y);
}

// Retourne la position actuelle de la tourelle
sf::Vector2f Tourelle::getPosition() const {
    return shape.getPosition();
}

// Dessin de la tourelle dans la fenêtre SFML
void Tourelle::draw(sf::RenderTarget& win) const {
    if (isDestroyed()) return;  // si la tourelle est détruite, on ne l’affiche plus
    win.draw(shape);

    // (Optionnel) cercle de portée, pour debug :
    /*
     sf::CircleShape rangeCircle(range);
     rangeCircle.setOrigin(range, range);
     rangeCircle.setPosition(shape.getPosition());
     rangeCircle.setFillColor(sf::Color(255,255,255,20));
     rangeCircle.setOutlineColor(sf::Color(255,255,255,60));
     rangeCircle.setOutlineThickness(1.f);
     win.draw(rangeCircle);
    */
}


// ------------------------ Update générique ------------------------ //
//
// Gestion du cooldown de tir :
//  - dt : temps écoulé depuis la dernière frame
//  - cooldown diminue jusqu’à 0
//
void Tourelle::update(float dt) {
    if (isDestroyed()) return;
    cooldown = std::max(0.f, cooldown - dt);
}


// ------------------------ Ciblage : acquireTarget ------------------------ //
//
// Cherche une cible dans la liste des ennemis :
//  - uniquement ceux encore en vie
//  - uniquement les types autorisés par le EnemyMask
//  - et dans la portée de la tourelle
// On retourne l’ennemi le plus proche, ou nullptr si aucun.
// On retourne un pointeur vers un enemy
Enemy* Tourelle::acquireTarget(const std::vector<Enemy*>& enemies) const {

    // r2 = portée² → on travaille en distance² pour éviter sqrt
    const float r2 = range * range;

    Enemy* best   = nullptr;           // meilleur candidat
    float  bestD2 = r2;                // meilleure distance² trouvée (init = max portée)
    auto   p      = getPosition();     // position de la tourelle

    for (auto e : enemies) {
        if (!e || e->isDead())
            continue;                  // on ignore les null et les morts

        // Vérifie si le masque de la tourelle autorise ce type d’ennemi
        if (!accepts(e))
            continue;

        // Distance² à l’ennemi (Pythagore sans racine)
        float dx = e->getPosition().x - p.x;
        float dy = e->getPosition().y - p.y;
        float d2 = dx * dx + dy * dy;

        // On garde la cible la plus proche dans la portée
        if (d2 <= bestD2) {
            bestD2 = d2;
            best   = e;
        }
    }

    return best; // peut être nullptr si aucune cible valide
}


// ------------------------ Tir standard : Tourelle::tryShoot ------------------------ //
//
// - Met à jour le cooldown
// - Cherche une cible
// - Si tout est ok → crée un projectile simple vers l’ennemi
//
bool Tourelle::tryShoot(float dt,
                        const std::vector<Enemy*>& enemies,
                        std::vector<Projectile>& outProjectiles)
{
    if (isDestroyed()) return false;

    // gestion du cooldown
    update(dt);
    if (cooldown > 0.f) return false; // encore en recharge -> pas de tir

    // ciblage
    Enemy* target = acquireTarget(enemies);
    if (!target) return false;        // aucune cible -> rien à faire

    // 3) création du projectile (tir standard vers ENNEMIS)
    outProjectiles.emplace_back(
        getPosition(),                // point de départ = centre tourelle
        target->getPosition(),        // cible = centre ennemi
        projectileSpeed,              // vitesse
        damage,                       // dégâts
        /*radius*/      4.f,
        /*allowedMask*/ allowedMask   // quels types d’ennemis ce projectile touche
        // TargetType par défaut = Enemies (constructeur projectile)
    );

    // reset du cooldown en fonction de la cadence de tir
    cooldown = (fireRate > 0.f) ? (1.f / fireRate) : 0.25f;
    return true;
}


// ============================================================================
//                  Rotation d’un vecteur 2D (pour le shotgun)
// ============================================================================
//
// rotateVector(v, degrees)
// -> retourne le vecteur v tourné d’un certain angle (en degrés).
//
// Utilisé pour créer les différents projectiles du shotgun autour de la
// direction principale.
// ============================================================================ //
static sf::Vector2f rotateVector(const sf::Vector2f& v, float degrees)
{
    float rad = degrees * 3.14159265f / 180.f;  // conversion degrés -> radians
    float cs = std::cos(rad);
    float sn = std::sin(rad);

    // Formule standard de rotation 2D :
    // x' = x*cos - y*sin
    // y' = x*sin + y*cos
    return sf::Vector2f(v.x * cs - v.y * sn,
                        v.x * sn + v.y * cs);
}


// override de tryShoot :
//  - au lieu d’un seul projectile, la tourelle en envoie plusieurs
//    en éventail autour de la direction principale.
bool shotgunTourelle::tryShoot(float dt,
                               const std::vector<Enemy*>& enemies,
                               std::vector<Projectile>& outProjectiles)
{
    if (isDestroyed()) return false;

    // Gestion locale du cooldown (ici on n’appelle pas Tourelle::update)
    cooldown = std::max(0.f, cooldown - dt);
    if (cooldown > 0.f) return false;

    // On prend une cible comme pour la tourelle standard
    Enemy* target = acquireTarget(enemies);
    if (!target) return false;

    // direction centrale vers la cible
    sf::Vector2f toTarget = target->getPosition() - getPosition();

    // liste des angles de déviation autour de cette direction
    std::vector<float> angles = { 0.f, 15.f, -15.f, 30.f, -30.f };

    // Pour chaque angle, on crée un projectile légèrement décalé
    for (float a : angles)
    {
        sf::Vector2f dir = rotateVector(toTarget, a);      // direction tournée
        sf::Vector2f targetPos = getPosition() + dir;      // point visé un peu plus loin

        outProjectiles.emplace_back(
            getPosition(),   // départ
            targetPos,    // ça tir dans la direction de la cible mais ça garde l'écart angle donné pour partir un peu n'importe ou (SHOTGUN)
            projectileSpeed,
            damage,
            /*radius*/      4.f,
            /*allowedMask*/ allowedMask
        );
    }

    // On remet un cooldown classique basé sur fireRate
    cooldown = (fireRate > 0.f) ? (1.f / fireRate) : 0.25f;
    return true;
}
