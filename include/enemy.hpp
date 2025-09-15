#ifndef ENNEMY_HPP
#define ENEMY_HPP

#include <SFML/Graphics.hpp>

class Enemy // Déclaration de la classe Ennemy
{
    //private:

    protected : //Protégé = accessible aux classes dérivées (les différents types d'ennemis)

    //Paramètres de base de l'ennemi
    int   health;
    float speed;
    bool dead; // Variable d'état de l'ennemie (vivant/mort)
    bool eat; // Variable d'objetif atteint 

    //Définition de la forme de l'ennemie
    sf::CircleShape shape;

    Enemy(int hp, float spd,float radius, sf::Color color); // Constructeur générique d'un ennemi, utile pour les type d'ennemis dérivés
    

    public:
    virtual ~Enemy() = default; // Destructeur virtuel par défaut

    //Gestion des points de vie
    void hit(int amount); // L'ennemie subit des dégats
    bool isDead() const; // L'ennemi est-il mort ?

    //Etat de l'ennemi
    int getHp() const;
    float getSpeed() const;
    bool hasEaten() const;


//--- Intégration des ennemies dans SFML --//

  // SFML helpers
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
    //Accessors (lecture)

    //int getHp() const;
    //float getSpeed() const;
    //bool isDead() const;
    //bool hasEaten() const;


    //Mutators (écriture)

    //void hit(int amount);

//----------------------------------------------------------



//----------------------------------------------------------
//Ennemi de base 

class BasicEnemy : public Enemy
{
public:
    BasicEnemy() : Enemy(100, 0.3, 16.f, sf::Color(255,0,0)) {} 
};

//----------------------------------------------------------
//Ennemi rapide (comportement identique au ennemie de base, hp et vitesse change)

class FastEnemy : public Enemy
{
public:
    FastEnemy() : Enemy(75, 0.6, 12.f, sf::Color(0,255,0)) {} 
};

//----------------------------------------------------------
//Ennemi tank (comportement identique au ennemie de base, hp et vitesse change) 

class TankEnemy : public Enemy
{
public:
    TankEnemy() : Enemy(300, 0.15, 10.f, sf::Color(0,0,255)) {} 
};

//----------------------------------------------------------
//Ennemi target (leur objectif est de détruire les tourelles sur leur passage (chemin le plus cours en ignorant les tourelles)
//hp et vitesse change) 

class TargetEnemy : public Enemy
{
public:
    TargetEnemy() : Enemy(150, 0.3, 4.f, sf::Color(100,100,100)) {}
};

//----------------------------------------------------------
//Ennemi volatile (ils ignorent les tourelles, vole par dessus, hp et vitesse change) 

class FlyEnemy : public Enemy
{
public:
    FlyEnemy() : Enemy(100, 0.4, 20.f, sf::Color(200,0,150)) {}
};


#endif