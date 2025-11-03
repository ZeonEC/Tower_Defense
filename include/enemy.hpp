#ifndef ENEMY_HPP
#define ENEMY_HPP

//------------------------ INCLUDE libs ----------------------//
#include <SFML/Graphics.hpp>
#include <random>
#include <cstdlib>  // pour rand() et srand()
#include <ctime>    // pour time()
#include <string>
#include "Astar.hpp"
#include "render.hpp"


//------------------------ INCLUDE prog ----------------------//

//#include "render.hpp"

// ====================================== ROLE DE LA CLASSE =========================================== //

// La classe Enemy représente une unité ennemie que les tourelles doivent attaquer. C’est une entité mobile et destructible du jeu
// Les classes dérivées (BasicEnemy, FastEnemy, TankEnemy, etc.) héritent de Enemy et définissent leurs caractéristiques propres : vitesse, points de vie, apparence…

// ==================================================================================================== //



enum class EnemyKind { Basic = 0, Fast, Tank, Target, Fly };

class Enemy // Déclaration de la classe Ennemy
{
    //private:

    protected : //Protégé = accessible aux classes dérivées (les différents types d'ennemis)

    //Paramètres de base de l'ennemi
    int   health;
    float speed;
    bool dead; // Variable d'état de l'ennemie (vivant/mort)
    bool eat; // Variable d'objetif atteint 


    // Tableau de pointeurs vers des ennemis, comprennant la classe enemy, il porte le nom de "enemies"
    std::vector<Enemy*> enemies;
    //Apparition des ennemies
    int r = static_cast<int>(rand() % 5); // entre 0 et 3

    //Définition du sprite
    sf::Texture texture;
    sf::Sprite  sprite;


    // On défini a la construction de l'ennemi sa vie,vitesse, sa taille et son sprite
    Enemy(int hp, float spd, float radius, const std::string& texturePath, int ressourceValue); // Constructeur générique d'un ennemi, utile pour les type d'ennemis dérivés

    std::vector<sf::Vector2f> pathPoints; // Tableau contenant tous les points de la trajectoire de l'ennemi
    int currentPathIndex; // index actuel du tableau de points
    int enemyID;

    public:

    bool reachedGoal = false;
    bool needRepath;

    virtual ~Enemy() = default; // Destructeur virtuel par défaut -- apparement obligatoire
    virtual EnemyKind getKind() const = 0;  // chaque dérivée le précise

    static int counter; // Compteur d'ennemis total

    //Gestion des points de vie
    void hit(int amount); // L'ennemie subit des dégats
    bool isDead() const; // L'ennemi est-il mort ?

    //Etat de l'ennemi -- De simple getter pas forcément utile
    int getHp() const;
    float getSpeed() const;
    bool hasEaten() const;
    void takeDamage(int amount);
    
    int getDamage() const { return 1; } // valeur par défaut, à adapter selon le type d'ennemi
    int ressourceValue;


//--- SFML --//

    void setPosition(float x, float y) { sprite.setPosition(x, y); }
    float getRadius() const;
    sf::Vector2f getPosition() const   { return sprite.getPosition(); }
    
    virtual void draw(sf::RenderTarget& win) const;

    // Boucle de jeu
    virtual void update(PathFinding_AStar& pathfinder, const std::vector<Render::Cell>& cells);                 // déplacement basique
};

// ---------------------------- DECLARATION DES ENNEMIES ---------------------------- //

// Je ne peux pas faire toute la déclaration ici, malheureusement inclure render ici ferai une loupe d'inclusion 
// (je peux pas recupérer la cellsize si elle est pas construite)
// Donc la déclaration est dans le cpp

//----------------------------------------------------------
//Ennemi de base 

class BasicEnemy : public Enemy {
public:
    static int counter;
    BasicEnemy();                 
    ~BasicEnemy();               
    EnemyKind getKind() const override { return EnemyKind::Basic; }
};

//----------------------------------------------------------
//Ennemi rapide (comportement identique au ennemie de base, hp et vitesse change)

class FastEnemy : public Enemy {
public:
    static int counter;
    FastEnemy();
    ~FastEnemy();
    EnemyKind getKind() const override { return EnemyKind::Fast; }
};

//----------------------------------------------------------
//Ennemi tank (comportement identique au ennemie de base, hp et vitesse change) 

class TankEnemy : public Enemy {
public:
    static int counter;
    TankEnemy();
    ~TankEnemy();
    EnemyKind getKind() const override { return EnemyKind::Tank; }
};

//----------------------------------------------------------
//Ennemi target (leur objectif est de détruire les tourelles sur leur passage (chemin le plus cours en ignorant les tourelles)
//hp et vitesse change) 

class TargetEnemy : public Enemy {
public:
    static int counter;
    TargetEnemy();
    ~TargetEnemy();
    EnemyKind getKind() const override { return EnemyKind::Target; }
};

//----------------------------------------------------------
//Ennemi volatile (ils ignorent les tourelles, vole par dessus, hp et vitesse change) 

class FlyEnemy : public Enemy {
public:
    static int counter;
    FlyEnemy();
    ~FlyEnemy();
    EnemyKind getKind() const override { return EnemyKind::Fly; }
};



#endif


//----------------------------------------------------------
    //Accessors (lecture)

    //int getHp() const;
    //float getSpeed() const;
    //bool isDead() const;
    //bool hasEaten() const;


    //Mutators (écriture)

    //void hit(int amount);

//----------------------------------------------------------
