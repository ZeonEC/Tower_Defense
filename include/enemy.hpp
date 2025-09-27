#ifndef ENEMY_HPP
#define ENEMY_HPP

#include <SFML/Graphics.hpp>
#include <random>
#include <cstdlib>  // pour rand() et srand()
#include <ctime>    // pour time()



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

    //Définition de la forme de l'ennemie
    sf::CircleShape shape;

    //On défini a la construction de l'ennemi sa vie,vitesse, sa taille et sa couleur (pour l'instant c'est des forme ronde de couleur)
    Enemy(int hp, float spd,float radius, sf::Color color); // Constructeur générique d'un ennemi, utile pour les type d'ennemis dérivés
    

    public:
    virtual ~Enemy() = default; // Destructeur virtuel par défaut -- apparement obligatoire

    static int counter; // Compteur d'ennemis total

    //Gestion des points de vie
    void hit(int amount); // L'ennemie subit des dégats
    bool isDead() const; // L'ennemi est-il mort ?

    //Etat de l'ennemi -- De simple getter pas forcément utile
    int getHp() const;
    float getSpeed() const;
    bool hasEaten() const;

//--- SFML --//

    void setPosition(float x, float y) { shape.setPosition(x, y); }

    sf::Vector2f getPosition() const   { return shape.getPosition(); }

    // Boucle de jeu
    virtual void update(float dt);                 // déplacement basique
    
    virtual void draw(sf::RenderWindow& win) const // rendu
    {
        if (!dead) win.draw(shape);
    }
};


//----------------------------------------------------------
//Ennemi de base 

class BasicEnemy : public Enemy
{
public:
static int counter; // Compteur d'ennemis de type BasicEnemy
    BasicEnemy() : Enemy(100, 3, 16.f, sf::Color(255,0,0)) {++counter;}
    ~BasicEnemy() { --BasicEnemy::counter; } 

};

//----------------------------------------------------------
//Ennemi rapide (comportement identique au ennemie de base, hp et vitesse change)

class FastEnemy : public Enemy
{
public:
    static int counter; // Compteur d'ennemis de type FastEnemy
    FastEnemy() : Enemy(75, 6, 12.f, sf::Color(0,255,0)) {++counter;}
    ~FastEnemy() { --FastEnemy::counter; } 
};

//----------------------------------------------------------
//Ennemi tank (comportement identique au ennemie de base, hp et vitesse change) 

class TankEnemy : public Enemy
{
public:
    static int counter; // Compteur d'ennemis de type TankEnemy
    TankEnemy() : Enemy(300, 1, 10.f, sf::Color(0,0,255)) {++counter;}
    ~TankEnemy() { --TankEnemy::counter; } 
};

//----------------------------------------------------------
//Ennemi target (leur objectif est de détruire les tourelles sur leur passage (chemin le plus cours en ignorant les tourelles)
//hp et vitesse change) 

class TargetEnemy : public Enemy
{
public:
    static int counter; // Compteur d'ennemis de type TargetEnemy
    TargetEnemy() : Enemy(150, 3, 4.f, sf::Color(100,100,100)) {++counter;}
    ~TargetEnemy() { --TargetEnemy::counter; }
};

//----------------------------------------------------------
//Ennemi volatile (ils ignorent les tourelles, vole par dessus, hp et vitesse change) 

class FlyEnemy : public Enemy
{
public:
    static int counter; // Compteur d'ennemis de type FlyEnemy
    FlyEnemy() : Enemy(100, 4, 20.f, sf::Color(200,0,150)) {++counter;}
    ~FlyEnemy() { --FlyEnemy::counter; }

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
