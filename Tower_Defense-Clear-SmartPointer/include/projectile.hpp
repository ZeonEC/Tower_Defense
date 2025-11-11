#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

//------------------------ INCLUDE libs ----------------------//

#include <SFML/Graphics.hpp> // pour sf::CircleShape, sf::Vector2f, sf::RenderTarget
#include <vector>            // au cas où on manipule des tableaux de projectiles/ennemis


//------------------------ FORWARD DECLARATIONS ----------------------//

// Déclarations anticipées pour éviter les inclusions circulaires
// (on dit juste “ces types existent”, sans avoir besoin d’inclure tous les headers)
class Enemy;
class Tourelle;
enum class EnemyKind;


// ====================================== ROLE DE LA CLASSE =========================================== //
//
// La classe Projectile représente un tir dans le jeu :
//
//  - Il possède : une position, une vitesse, un rayon (collision) et des dégâts
//  - Il peut être tiré soit par une tourelle, soit par un ennemi (projectile adverse)
//  - Il se déplace en ligne droite vers une direction donnée
//  - Quand il entre en collision avec une cible autorisée, il inflige des dégâts
//
// On distingue aussi le type de cible ( Comme on utilise la même méthode pour le tire des ennemies et des tourelles):
//  - TargetType::Enemies -> destiné à toucher les ennemis (projectile de tourelle)
//  - TargetType::Towers  -> destiné à toucher les tourelles (projectile ennemi)
//
// Un système de masque (EnemyMask) permet de dire quels types d’ennemis
// ce projectile est autorisé à toucher (Basic, Fast, Fly, etc)
//
// ==================================================================================================== //

class Projectile {

public:

    // ============================= TYPE DE CIBLE PRINCIPALE ============================= //

    // Permet de savoir si ce projectile vise des ENNEMIS ou des TOURELLES
    // En gros utilisé comme ça cela nous permet de faire une "vérification" de qui tire sur quoi, donc quand c'est a enemy on attribue au projectile le target type Towers
    enum class TargetType {
        Enemies,   // projectile destiné aux ennemis
        Towers     // projectile destiné aux tourelles
    };

    // ============================= TYPE POUR LE MASQUE D'ENNEMIS ============================= //

    // ALIAS de type : EnemyMask = entier non signé
    // On l’utilise comme un "bitmask" pour dire quels types d’ennemis sont autorisés
    using EnemyMask = unsigned;

    // Retourne le masque d’ennemis actuellement autorisés pour ce projectile
    EnemyMask allowedMask() const noexcept { return allowedMask_; }

    // Le projectile ignore les ennemis dont le type n’est pas autorisé par le masque
    // Renvoie true si l’ennemi passé en paramètre est "accepté" par ce projectile
    bool accepts(const Enemy* e) const noexcept;

    // ============================= CONSTRUCTEUR ============================= //

    // start       : position de départ du projectile (en pixels)
    // target      : point vers lequel on tire (direction calculée à partir de start -> target)
    // speed       : vitesse de déplacement en pixels/seconde
    // damage      : dégâts infligés à l’impact
    // radius      : rayon du cercle SFML (sert aussi pour la collision)
    // allowedMask : quels types d’ennemis ce projectile a le droit de toucher
    // targetType  : indique si ce projectile vise les ennemis ou les tourelles

    Projectile(const sf::Vector2f& start,
               const sf::Vector2f& target,
               float speed        = 450.f,
               int   damage       = 0,
               float radius       = 4.f,
               EnemyMask allowedMask = ~EnemyMask{0},
               TargetType targetType = TargetType::Enemies);

    // ============================= MISE A JOUR / RENDU ============================= //

    // Mise à jour de la position en fonction de la vitesse et du temps écoulé (dt)
    // dt : delta time, temps entre deux frames
    void update(float dt);

    // Dessine le projectile dans la fenêtre SFML
    void draw(sf::RenderTarget& win) const;

    // ============================= ETAT DE VIE DU PROJECTILE ============================= //

    // Savoir si le projectile est encore actif (true) ou à détruire (false)
    bool isAlive() const { return alive; }

    // Marque le projectile comme "mort"
    void kill()          { alive = false; }

    // ============================= COLLISION ============================= //

    // Collision simple cercle/cercle :
    // enemyPos    : centre de la cible (ennemi ou tourelle)
    // enemyRadius : rayon de la cible
    // Renvoie true si la distance entre les centres est < somme des rayons
    bool collidesWith(const sf::Vector2f& enemyPos, float enemyRadius) const;

    // ============================= GETTERS SIMPLES ============================= //

    int          getDamage()   const { return dmg; }
    sf::Vector2f getPosition() const { return shape.getPosition(); }
    float        getRadius()   const { return shape.getRadius(); }

    // Retourne le type de cible de ce projectile (ennemis / tourelles)
    TargetType getTargetType() const noexcept { return targetType_; }


private:

    // ------------------------ DONNEES GRAPHIQUES & PHYSIQUES ---------------------- //

    sf::CircleShape shape;      // cercle SFML utilisé pour le rendu (couleur, rayon, position)
    sf::Vector2f    velocity;   // vecteur vitesse en pixels/sec
    int             dmg;        // dégâts infligés à l’impact
    bool            alive = true; // true = projectile actif, false = à supprimer


    // ================================================================================= //
    //                          MASQUE D'ENNEMIS (EnemyMask)                             //
    // ================================================================================= //
    //
    // On utilise le même principe que pour les tourelles :
    // chaque EnemyKind correspond à un bit dans un entier.
    //
    //  EnemyKind = { Basic=0, Fast=1, Tank=2, Target=3, Fly=4 } :
    //
    //  kindBit(EnemyKind::Basic)  -> 00001 (1)
    //  kindBit(EnemyKind::Fast)   -> 00010 (2)
    //  kindBit(EnemyKind::Tank)   -> 00100 (4)
    //  kindBit(EnemyKind::Target) -> 01000 (8)
    //  kindBit(EnemyKind::Fly)    -> 10000 (16)
    //
    // En combinant tout ça dans allowedMask_ :
    //   - si le bit est à 1  -> ce type d’ennemi est autorisé
    //   - si le bit est à 0  -> ce type est ignoré par le projectile
    //
    // Par défaut on met tous les bits à 1 (~0u) : le projectile touche tout.
    //
    // ================================================================================= //

    // Convertit un EnemyKind en bit unique dans le masque.
    static constexpr EnemyMask kindBit(EnemyKind k) {
        return 1u << static_cast<unsigned>(k);
    }

    // Masque des types d’ennemis autorisés pour ce projectile :
    // - utilisé dans accepts()
    // - permet de faire : projectile qui ne touche que les Fly, ou uniquement les Tanks, etc.
    EnemyMask allowedMask_ = ~0u;                 // par défaut : autorise tout

    // Type principal de cible (ennemis / tourelles).
    TargetType targetType_ = TargetType::Enemies;
};

#endif
