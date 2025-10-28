#ifndef TOWER_HPP
#define TOWER_HPP

#include <SFML/Graphics.hpp>
#include <random>
#include <cstdlib>  // pour rand() et srand()
#include <ctime>    // pour time()

//#include "render.hpp"


class Tourelle // Déclaration de la classe Tourelle
{
    //private:

    protected : //Protégé = accessible aux classes dérivées (les différents types de tourelles)

    //Paramètres de base de l'ennemi
    int   health;
    float speed;
    bool dead; // Variable d'état de l'tourelle (vivant/mort)


    // Tableau de pointeurs vers des tourelles, comprennant la classe Tourelle, il porte le nom de "tourelle"
    std::vector<Tourelle*> tourelle;

    //Définition de la forme des tourelles
    sf::CircleShape shape;

    //On défini a la construction de l'ennemi sa vie,vitesse, sa taille et sa couleur (pour l'instant c'est des forme ronde de couleur)
    Tourelle(int hp, float spd,float radius, sf::Color color); // Constructeur générique d'un ennemi, utile pour les type de tourelle dérivés
    

    public:
    virtual ~Tourelle() = default; // Destructeur virtuel par défaut -- apparement obligatoire

    static int counter; // Compteur de tourelle total

    //Gestion des points de vie
    void hit(int amount); // La tourelle subit des dégats
    bool isDead() const; // La tourelle est morte ?

    //Etat de la tourelle -- De simple getter pas forcément utile
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

class BasicTourelle : public Tourelle
{
public:
static int counter; // Compteur de tourelles de type BasicTourelle
    BasicTourelle(float cellsize) : Tourelle(100, 3, cellsize, sf::Color(255,255,255)) {++counter;} //Oblige de demander cellsize pour que la tourelle prenne la taille d'une cellule
    ~BasicTourelle() { --BasicTourelle::counter; } 
};

//----------------------------------------------------------
//Ennemi rapide (comportement identique au tourelle de base, hp et vitesse change)

class PoisonTourelle : public Tourelle
{
public:
    static int counter; // Compteur d'ennemis de type FastTourelle
    PoisonTourelle(float cellsize) : Tourelle(75, 6, cellsize, sf::Color(0,255,0)) {++counter;}
    ~PoisonTourelle() { --PoisonTourelle::counter; } 
};

//----------------------------------------------------------
//Ennemi tank (comportement identique au tourelle de base, hp et vitesse change) 

class shotgunTourelle : public Tourelle
{
public:
    static int counter; // Compteur d'ennemis de type TankTourelle
    shotgunTourelle(float cellsize) : Tourelle(300, 1, cellsize, sf::Color(0,0,255)) {++counter;}
    ~shotgunTourelle() { --shotgunTourelle::counter; } 
};

//----------------------------------------------------------
//Ennemi target (leur objectif est de détruire les tourelles sur leur passage (chemin le plus cours en ignorant les tourelles)
//hp et vitesse change) 

class TargetTourelle : public Tourelle
{
public:
    static int counter; // Compteur d'ennemis de type TargetTourelle
    TargetTourelle(float cellsize) : Tourelle(150, 3, cellsize, sf::Color(100,100,100)) {++counter;}
    ~TargetTourelle() { --TargetTourelle::counter; }
};

//----------------------------------------------------------
//Ennemi volatile (ils ignorent les tourelles, vole par dessus, hp et vitesse change) 

class FlyTourelle : public Tourelle
{
public:
    static int counter; // Compteur d'ennemis de type FlyTourelle
    FlyTourelle(float cellsize) : Tourelle(100, 4, cellsize, sf::Color(200,0,150)) {++counter;}
    ~FlyTourelle() { --FlyTourelle::counter; }
};


#endif