#include "projectile.hpp"
#include "enemy.hpp"   // ici c’est OK, on est dans le .cpp
#include <cmath>

// ============================================================================
//                      FONCTION UTILITAIRE : normalized()
// ============================================================================
//
// Retourne un vecteur normalisé (même direction, longueur = 1).
// Si le vecteur est nul (longueur 0), on renvoie (0,0) pour éviter une division par 0.
//
// Exemple :
//   v = (3,4)  -> normalized(v) = (0.6, 0.8)
// ============================================================================ //
static sf::Vector2f normalized(const sf::Vector2f& v) {
    float n = std::sqrt(v.x*v.x + v.y*v.y);      // longueur du vecteur (Pythagore)
    return (n > 0.f) ? sf::Vector2f(v.x/n, v.y/n) // si n>0  on normalise
                     : sf::Vector2f(0.f,0.f);     // sinon vecteur nul
}


// ============================================================================
//                           CONSTRUCTEUR Projectile
// ============================================================================
//
// start       : position de départ du projectile
// target      : point vers lequel il est tiré (direction = target - start)
// speed       : vitesse (px/s)
// damage      : dégâts infligés à l’impact
// radius      : taille visuelle / collision du projectile
// allowedMask : quels types d’ennemis ce projectile peut toucher (EnemyMask)
// targetType  : vise des ENNEMIS ou des TOURELLES
//
// Le constructeur initialise la forme SFML (shape) + calcule la vitesse
// à partir de la direction normalisée * speed.
// ============================================================================ //
Projectile::Projectile(const sf::Vector2f& start,
                       const sf::Vector2f& target,
                       float speed,
                       int   damage,
                       float radius,
                       EnemyMask allowedMask,
                       TargetType targetType)
    : dmg(damage),
      alive(true),
      allowedMask_(allowedMask),
      targetType_(targetType)
{
    // -------------------- Configuration visuelle du projectile -------------------- //
    shape.setRadius(radius);                 // rayon graphique du projectile
    shape.setOrigin(radius, radius);         // origine au centre (plus simple pour les collisions)
    shape.setFillColor(sf::Color::Yellow);   // couleur par défaut (peut être changée plus tard)
    shape.setPosition(start);                // position initiale = point de départ du tir

    // -------------------- Calcul de la vitesse -------------------- //
    // On calcule la direction normalisée entre départ et cible,
    // puis on multiplie par speed pour obtenir le vecteur vitesse (px/s).
    auto dir = normalized(target - start);
    velocity = dir * speed;
}



// Filtrage des cibles (côté ENNEMIS)
// Un projectile n’est pas obligé de toucher tous les types d’ennemis.
// Cette fonction vérifie si le type de l’ennemi e est autorisé par allowedMask_.
// Retourne true si le bit correspondant à EnemyKind est présent dans allowedMask_.
// Principalement on s'en sert pour évité que les tir de flytourelle tue les autres ennemies et que les tirs des autres tourelles tue flyenemy
bool Projectile::accepts(const Enemy* e) const noexcept {
    if (!e) return false;                    // sécurité si pointeur nul
    const EnemyKind k = e->getKind();        // type réel de l’ennemi
    return (allowedMask_ & kindBit(k)) != 0; // test du bit associé dans le masque
}


// MISE À JOUR DU PROJECTILE
// - Déplace le projectile en fonction de sa vitesse et de dt.
// - Si le projectile sort très loin de la zone de jeu, on le tue pour éviter qu’il traîne à l’infini en mémoire.
void Projectile::update(float dt) {
    if (!alive) return;                      // projectile déjà mort → on ne fait rien

    // Déplacement = vitesse * temps écoulé
    shape.move(velocity * dt);

    // On récupère la position pour vérifier s’il est trop loin
    auto p = shape.getPosition();

    // Petites bornes pour dire : ok là c’est bon, on peut le supprimer
    if (p.x < -100 || p.y < -100 || p.x > 5000 || p.y > 5000) {
        alive = false;
    }
}


// Dessine le projectile dans la fenêtre SFML tant qu’il est vivant.
void Projectile::draw(sf::RenderTarget& win) const {
    if (alive) win.draw(shape);
}


// ============================================================================
//                  COLLISION CERCLE / CERCLE AVEC UNE CIBLE
// ============================================================================
//
// enemyPos    : centre de la cible (ennemi ou tourelle)
// enemyRadius : rayon de la cible
//
// On considère le projectile comme un cercle et la cible comme un cercle.
// Collision si : distance² entre les centres <= (somme des rayons)².
//
// ============================================================================ //

bool Projectile::collidesWith(const sf::Vector2f& enemyPos, float enemyRadius) const {
    auto p  = shape.getPosition();           // position du projectile
    auto dx = p.x - enemyPos.x;              // écart en X
    auto dy = p.y - enemyPos.y;              // écart en Y
    float r = shape.getRadius() + enemyRadius; // rayon total (projectile + cible)

    // Pythagore sur dx,dy mais sans racine : on compare les carrés
    return (dx*dx + dy*dy) <= (r*r);
}
