#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

//------------------------ INCLUDE libs ----------------------//

#include <SFML/Graphics.hpp>   // sf::CircleShape, sf::RenderTarget, sf::Vector2f
#include <vector>              // (potentiellement utile pour des interactions multiples)

//------------------------ INCLUDE prog ----------------------//

#include "enemy.hpp"   // Pour EnemyKind / Enemy (ciblage par bitmask)

// Forward-declarations pour éviter les inclusions circulaires inutiles
class Enemy;
class Tourelle;


// ====================================== ROLE DE LA CLASSE =========================================== //
//
// La classe Projectile représente les tirs générés par les tourelles (ou par certains ennemis).
// Elle stocke :
//   - sa forme graphique (un cercle SFML mais on pourrait mettre un sprite (jsp si on fera ça)),
//   - sa vitesse et direction (sf::Vector2f),
//   - ses dégâts,
//   - les types de cibles qu’elle peut toucher (EnemyMask),
//   - et enfin son "type de cible" général : ennemis OU tourelles.
//
// Chaque projectile est autonome :
//   - Il avance à chaque frame via update(dt)
//   - Il vérifie les collisions (avec les ennemis ou tourelles selon son type)
//   - Il peut être détruit (flag alive = false)
// On peut surment faire autrement parce que a terme bah ça fait enormément de rendu 
// ==================================================================================================== //


class Projectile {
public:

// ===================================== TYPES & ENUM ===================================== //

    // Type logique du projectile :
    // - Enemies : tir d’une tourelle vers un ennemi
    // - Towers  : tir d’un ennemi vers une tourelle
    enum class TargetType { Enemies, Towers };

    // Masque de ciblage (même principe que dans Tourelle)
    using EnemyMask = unsigned;

    // Accès en lecture au masque actuel (utile pour déboguer ou dupliquer un tir)
    EnemyMask allowedMask() const noexcept { return allowedMask_; }

    // Vérifie si le projectile peut toucher un ennemi donné (en fonction du mask)
    bool accepts(const Enemy* e) const noexcept;


// ===================================== CONSTRUCTION ===================================== //

    /**
     *  Construit un projectile simple
     * 
     *  start        Position de départ du tir (souvent position de la tourelle)
     *  target       Position visée (souvent centre de l’ennemi)
     *  speed        Vitesse linéaire (px/s)
     *  damage       Dégâts infligés
     *  radius       Rayon visuel du projectile (px)
     *  allowedMask  Masque des types d’ennemis autorisés (bitmask EnemyKind)
     *  tt           Type de cible (Enemies ou Towers)
     * Le constructeur calcule automatiquement la direction du projectile à partir de
     * `target - start`, puis normalise et multiplie par `speed`.
     */
    Projectile(const sf::Vector2f& start,
               const sf::Vector2f& target,
               float speed = 450.f,
               int   damage = 0,
               float radius = 4.f,
               EnemyMask allowedMask = ~EnemyMask{0},
               TargetType tt = TargetType::Enemies);


// ===================================== MÉTHODES PRINCIPALES ===================================== //

    // Déplacement du projectile (simple translation : pos += velocity * dt)
    void update(float dt);

    // Dessin du projectile à l’écran
    void draw(sf::RenderTarget& win) const;

    // ------------------------ Gestion de vie ------------------------ //

    bool isAlive() const { return alive; }
    void kill()          { alive = false; }

    // ------------------------ Collisions ------------------------ //
    /**
     * Vérifie si le projectile touche une sphère simple
     * pos     Position du centre à tester
     * radius  Rayon de l’objet testé
     * true   si la distance < somme des rayons
     */
    bool collidesWith(const sf::Vector2f& pos, float radius) const;


// ===================================== GETTERS UTILES ===================================== //

    int  getDamage() const              { return dmg; }
    sf::Vector2f getPosition() const    { return shape.getPosition(); }
    float getRadius() const             { return shape.getRadius(); }

    // Indique si ce projectile est destiné à tirer sur des tourelles
    bool targetsTowers() const noexcept { return targetType == TargetType::Towers; }


// ===================================== DONNÉES PRIVÉES ===================================== //

private:
    // --- Représentation graphique --- //
    sf::CircleShape shape;    // cercle jaune (ou autre couleur selon usage)
    sf::Vector2f    velocity; // direction * vitesse
    int             dmg      = 0;   // dégâts infligés
    bool            alive    = true; // flag de vie (si false → supprimé du jeu)

    // ------------------- Masque de ciblage ------------------- //
    // Convertit le type d’ennemi (EnemyKind) en bit unique.
    // Même principe que dans Tourelle.
    static constexpr EnemyMask kindBit(EnemyKind k) {
        return 1u << static_cast<unsigned>(k);
    }

    EnemyMask  allowedMask_ = ~0u;               // autorise tout par défaut
    TargetType targetType   = TargetType::Enemies; // tir vers ennemis (défaut)
};

#endif // PROJECTILE_HPP

