#ifndef ENEMY_HPP
#define ENEMY_HPP

//------------------------ INCLUDE libs ----------------------//

#include <SFML/Graphics.hpp>  // sf::Texture, sf::Sprite, sf::Vector2f, sf::RenderTarget
#include <vector>             
#include <string>             
#include <cstdlib>            // rand()
#include <ctime>              // time() (seed éventuel ailleurs)
//#include <random>             

//------------------------ INCLUDE prog ----------------------//

// On a besoin de PathFinding_AStar et Render::Cell dans les signatures de update().
// Pour limiter les dépendances en .hpp, on pourrait *forward-decl* ces types ici
// et n'inclure les headers réels que dans enemy.cpp. Mais je maitrise pas trop (a redemander a Fougerolle)

#include "Astar.hpp"   // pour PathFinding_AStar
#include "render.hpp"  // pour Render::Cell

// Forward-decl pour éviter les inclusions circulaires
class Tourelle;
class Projectile;


// ==================================================================================================== //
//                                       RÔLE DU MODULE ENEMY                                           //
// ---------------------------------------------------------------------------------------------------- //
// La classe de base `Enemy` représente une unité hostile du jeu : mobile, destructible et rendue avec
// SFML. Les classes dérivées (BasicEnemy, FastEnemy, TankEnemy, TargetEnemy, FlyEnemy) ne changent
// que les stats (HP, vitesse, forme...) et dans certain cas le behavior ("vol", tir, etc.)
//
// Le cycle de vie d'un ennemi :
//   - construction (stats, sprite)
//   - update() : déplacement (A* pour les terrestres), logique (tir éventuel), états (mort/goal)
//   - draw()   : rendu SFML
//
// On utilise le polymorphisme donc des virtual-- pour séparer certain comportement des enemies
// ==================================================================================================== //


//------------------------ Typage des types d’ennemis ----------------------//
//
// Utilisé partout (tourelles, projectiles) pour filtrer/identifier rapidement les cibles.
// Avec ça on peut définir les autorisations de tir sur enemy mais commme on a mis les même noms
// pour turret et enemy bah on s'en sert pareil (1 mask plutot que 2);
//
enum class EnemyKind { Basic = 0, Fast, Tank, Target, Fly };


// ===================================== CLASSE DE BASE ===================================== //

class Enemy {

protected:

    // ------------------------ Paramètres de base ------------------------ //
    int   health;   // Points de vie
    float speed;    // Vitesse linéaire (px/s)
    bool  dead;     // État dead/goal
    bool  eat;      // Objectif atteint ? (ex: a traversé la map etc.)

    // Vecteur de la totalité des enemy 
    std::vector<Enemy*> enemies;

    // ------------------------ Gestion de la RNG ------------------------ //
    // Petite variable de variété (pour le spawn typiquement).
    int r = static_cast<int>(rand() % 5); // [0..4]

    // ------------------------ SFML : visuel ------------------------ //
    sf::Texture texture;  // texture du sprite
    sf::Sprite  sprite;   // sprite affiché en jeu

    // ------------------------ Cheminement (path) ------------------------ //
    // Pour les unités terrestres : liste de points à suivre (chemin A* discrétisé)
    std::vector<sf::Vector2f> pathPoints; // points de passage
    int   currentPathIndex;               // index du point courant
    int   enemyID;                        // ID utile pour debug/trace

    // ------------------------ Constructeur ------------------------ //
    // - hp/spd : stats
    // - radius : taille de l'enemy (on s'en sert surtout pour 'calibrer' les sprites et la hitbox)
    // - texturePath : chemin vers l'image pour SFML
    // - ressourceValue : ressources données au joueur à la mort
    Enemy(int hp, float spd, float radius, const std::string& texturePath, int ressourceValue);

public:

    // ------------------------ État public temps réel ------------------------ //
    bool reachedGoal = false; // a atteint l’objectif de la vague ?
    bool needRepath  = false; // doit recalculer son chemin (obstacles, tourelles posées, etc...)

    static int counter; // Compteur global (tous types confondus)
    virtual ~Enemy() = default; // Destructeur 

    // Type dynamique de l'ennemi (obligatoire dans chaque dérivé pour le mask)
    virtual EnemyKind getKind() const = 0;

    // ------------------------ Gestion des PV ------------------------ //
    // Applique des dégâts bruts (dmg) et met à jour l'état mort si nécessaire.
    void hit(int amount);
    void takeDamage(int amount);

    bool isDead() const; // HP <= 0 ?

    // ------------------------ Accesseurs ------------------------ //
    int   getHp()    const;
    float getSpeed() const;
    bool  hasEaten() const;

    // Dégâts infligés au joueur/structure si l’ennemi atteint sa cible (par défaut 1)
    int getDamage() const { return 1; }

    // Ressources données au joueur à la mort 
    int ressourceValue;

    // ------------------------ SFML & Géo ------------------------ //
    void setPosition(float x, float y) { sprite.setPosition(x, y); }
    sf::Vector2f getPosition() const   { return sprite.getPosition(); }

    // Rayon "logique" (collision simple ou scale visuel selon ton implémentation)
    float getRadius() const;

    // Affichage
    virtual void draw(sf::RenderTarget& win) const;

    // ------------------------ Boucle de jeu ------------------------ //
    // Déplacement/comportement de base.
    // - pathfinder : permet de (re)calculer le chemin (A*)
    // - cells      : grille (cellules) fournie par le moteur de rendu (positions, walkable, etc.)
    virtual void update(PathFinding_AStar& pathfinder, const std::vector<Render::Cell>& cells);
};


// ===================================== DÉRIVÉS CONCRETS ===================================== //
//
// Tous héritent de Enemy, changent simplement les stats et parfois update() / tir.

//----------------------------------------------------------
// Ennemi de base : stats équilibrées
class BasicEnemy : public Enemy {
public:
    static int counter;
    BasicEnemy();
    ~BasicEnemy();
    EnemyKind getKind() const override { return EnemyKind::Basic; }
};

//----------------------------------------------------------
// Ennemi rapide : vitesse ↑, PV ↓ (comportement identique sinon)
class FastEnemy : public Enemy {
public:
    static int counter;
    FastEnemy();
    ~FastEnemy();
    EnemyKind getKind() const override { return EnemyKind::Fast; }
};

//----------------------------------------------------------
// Ennemi tank : PV ↑↑, vitesse ↓ (comportement identique sinon)
class TankEnemy : public Enemy {
public:
    static int counter;
    TankEnemy();
    ~TankEnemy();
    EnemyKind getKind() const override { return EnemyKind::Tank; }
};

//----------------------------------------------------------
// Ennemi "Target" : objectif = attaquer les tourelles sur sa route.
// - Peut tirer (tryShoot)
// - Se déplace vers une tourelle acquise (acquireTarget + moveTowards)
class TargetEnemy : public Enemy {
public:
    static int counter;
    TargetEnemy();
    ~TargetEnemy();
    EnemyKind getKind() const override { return EnemyKind::Target; }

    // Ignore le pathfinding global (comportement personnalisé)
    void update(PathFinding_AStar& /*pathfinder*/, const std::vector<Render::Cell>& /*cells*/) override;

    // Le Game injecte la liste des tourelles à chaque frame (ou au spawn)
    void setTowerList(const std::vector<Tourelle*>* list) { towers = list; }

    // Tente un tir si la cadence le permet, pousse le projectile dans outProjectiles
    bool tryShoot(float dt, std::vector<Projectile>& outProjectiles);

private:
    // ------------------------ Contexte combat ------------------------ //
    const std::vector<Tourelle*>* towers = nullptr; // référence externe (non possédée)
    Tourelle* target = nullptr;                      // cible actuelle

    // ------------------------ Stats d'attaque ------------------------ //
    float shootRange   = 50.f;  // portée du tir (px)
    float projSpeed    = 520.f; // vitesse du projectile (px/s)
    int   projDamage   = 6;     // dégâts infligés par projectile
    float fireRate     = 1.5f;  // tirs par seconde
    float fireCooldown = 0.f;   // temps restant avant prochain tir

    // ------------------------ Helpers comportement ------------------------ //
    void acquireTarget();                        // choisit la tourelle la plus proche
    void moveTowards(const sf::Vector2f& dest, float step); // avance vers dest (step=px/frame)
};

//----------------------------------------------------------
// Ennemi volant : ignore les obstacles/tourelles au sol.
// - Peut aussi tirer (tryShoot)
// - update survol simple + acquisition tourelle la plus proche

class FlyEnemy : public Enemy {
public:
    static int counter;
    FlyEnemy();
    ~FlyEnemy();
    EnemyKind getKind() const override { return EnemyKind::Fly; }

    // Vol = pas d'A* sol
    void update(PathFinding_AStar&, const std::vector<Render::Cell>&) override;

    void setTowerList(const std::vector<Tourelle*>* list) { towers = list; }
    bool tryShoot(float dt, std::vector<Projectile>& outProjectiles);

private:
    // ------------------------ Contexte combat ------------------------ //
    const std::vector<Tourelle*>* towers = nullptr;
    Tourelle* target = nullptr;

    // ------------------------ Stats d'attaque ------------------------ //
    float shootRange   = 50.f;
    float projSpeed    = 520.f;
    int   projDamage   = 6;
    float fireRate     = 1.5f;   // tirs/s
    float fireCooldown = 0.f;

    // ------------------------ Helpers comportement ------------------------ //
    void acquireTarget();                        // choisit la tourelle la plus proche
    void moveTowards(const sf::Vector2f& dest, float step);
};

#endif
