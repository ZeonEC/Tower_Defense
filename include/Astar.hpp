#pragma once

#include <string>
#include <algorithm>
#include <vector>
#include <cmath>
#include <SFML/Graphics.hpp>
#include "render.hpp"

class Enemy;

class PathFinding_AStar
{
public :
    int gridCols = 0;
    int gridRows = 0;
    float NodeSize = 0.f;

    struct Node
    {
        bool turreted = false;
        bool bVisited = false;
        bool EnemyPresence = false;
        float fGlobalGoal;
        float fLocalGoal;
        int x, y, col,row;
        std::vector<Node*> Neighbours;
        Node* parent;
    };

    Node* getEnemyNode() const {return EnemyNode;};
    Node* setEnemyNode(const sf::Vector2f& position);

    void CreateNodes(const std::vector<Render::Cell>& cells);


    std::vector<sf::Vector2f> SolveAStar(const sf::Vector2f& position);

    void update(sf::Vector2f position);

    float heuristic(Node* a, Node* b){return sqrtf((a->x - b->x) * (a->x - b->x) + (a->y - b->y) * (a->y - b->y));};
    bool IsPathStillPossible(const sf::Vector2f& startPos);
    void updateEnemyPresence(const std::vector<Enemy*>& enemies);

    Node* findClosestNode(const sf::Vector2f& position);

   // std::vector<sf::Vector2f> getShortestPath(std::vector<std::vector<sf::Vector2f> >& paths);
    std::vector<Node> Nodes;
    std::vector<Node*> EnemyNodes = {};
    std::vector<Node*> nodeEnd = {};
    Node* EnemyNode = nullptr;

    protected :
    Node *nodeStart = nullptr;

    
    
    std::vector<sf::Vector2f> path;
    std::vector<std::vector<sf::Vector2f> > paths;
};