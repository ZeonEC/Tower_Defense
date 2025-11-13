#pragma once

#include <string>
#include <algorithm>
#include <vector>
#include <cmath>
#include <SFML/Graphics.hpp>
#include "render.hpp"

class Enemy; //Prédéclaration de la classe Enemy

class PathFinding_AStar
{
public :
    int gridCols = 0;                   //Nombre de colonnes
    int gridRows = 0;                   //Nombre de lignes
    float NodeSize = 0.f;               //Dimension des noeuds

    //Définition d'un noeud
    struct Node
    {
        bool turreted = false;          //Noeud occupé par une tourelle
        bool bVisited = false;          //Noeud déjà visité
        bool EnemyPresence = false;     //Noeud occupé minimum par un ennemi
        float fGlobalGoal;              //Distance par rapport aux noeuds finaux
        float fLocalGoal;               //Distance par rapport au prochain noeud
        int x, y, col,row;              //Position du noeud 
        std::vector<Node*> Neighbours;  //Vecteur contenant les voisins du noeud (maximum 8)
        Node* parent;                   //Pointeur en direction du noeud parent (noeud précédent du chemin)
    };

    Node* getEnemyNode() const {return EnemyNode;}; //Récupère le noeud de l'ennemi

    void CreateNodes(const std::vector<Render::Cell>& cells); //Créer les neouds à partir des cellules de la classe Render


    std::vector<sf::Vector2f> SolveAStar(const sf::Vector2f& position); //Résolveur de A* (Cherche le chemin le plus court)

    float heuristic(Node* a, Node* b); //Calcule la distance entre deux noeuds
    bool IsPathStillPossible(const sf::Vector2f& startPos); //Vérifie si il reste au moins un chemin
    void updateEnemyPresence(const std::vector<Enemy*>& enemies);   //Réintialise les noeuds en fonction de la présence d'ennemi

    Node* findClosestNode(const sf::Vector2f& position);    //Attribut à un ennemi un noeud (le noeud le plus proche)

    std::vector<Node> Nodes;                //Vecteur de noeuds
    std::vector<Node*> EnemyNodes = {};     //Vecteur de noeuds occupés par des ennemis
    std::vector<Node*> nodeEnd = {};        //Vecteur de noeuds de fin
    Node* EnemyNode = nullptr;              //Pointeur du noeud de l'ennemi

    protected :
    Node *nodeStart = nullptr;              //Pointeur de noeud de départ d'un ennemi (pour le calcul de chemin)

    std::vector<sf::Vector2f> path;                     //Vecteur de positions de noeuds pour le chemin
    std::vector<std::vector<sf::Vector2f> > paths;      //Vecteur de chemins (pour chercher le chemin le plus court)
};