#ifndef ASTAR_HPP
#define ASTAR_HPP

//------------------------ INCLUDE libs ----------------------//

#include <string>
#include <algorithm>
#include <vector>
#include <cmath>

// SFML : uniquement pour sf::Vector2f (positions, conversions grille↔monde)
#include <SFML/Graphics.hpp>

//------------------------ INCLUDE prog ----------------------//

// On a besoin de Render::Cell (grille) pour construire les noeuds.
// NOTE perf/compile-time : on pourrait forward-declare `namespace Render { struct Cell; }`
// ici et n'inclure "render.hpp" que dans le .cpp. Pour l’instant on garde ton include.

#include "render.hpp"

// Forward-decl pour éviter l’inclusion d'Enemy ici (utile pour updateEnemyPresence)
class Enemy;


// ==================================================================================================== //
//                                      RÔLE DU MODULE A* (Astar)                                       //
// ---------------------------------------------------------------------------------------------------- //
// Ce module fournit un pathfinding A* discretisé sur une grille (Nodes).
// - `CreateNodes(cells)` construit la grille à partir des cellules de rendu.
// - `SolveAStar(position)` renvoie un chemin (liste de points monde) depuis `position` vers une
//   destination (typiquement la sortie), en tenant compte des cases bloquées (turrets).
// - `updateEnemyPresence(...)` et `IsPathStillPossible(...)` aident à réagir dynamiquement aux
//   changements (nouvelle tourelle posée, ennemis présents, etc.).
//
// Glossaire rapide :
//   • Node          : case de la grille (walkable, coûts, voisins, parent).
//   • fLocalGoal    : coût depuis le départ vers ce noeud (g).
//   • fGlobalGoal   : fLocalGoal + heuristique vers la cible (g + h).
//   • parent        : lien pour remonter le chemin une fois la cible atteinte.
// ==================================================================================================== //

class PathFinding_AStar
{
public:
    // ------------------------ Paramètres de grille ------------------------ //
    int   gridCols  = 0;   // nombre de colonnes
    int   gridRows  = 0;   // nombre de lignes
    float NodeSize  = 0.f; // taille d’une cellule (px) pour conversion grille↔monde

    // ===================================== STRUCT NODE ===================================== //
    struct Node
    {
        // État de la cellule
        bool turreted     = false; // case occupée par une tourelle → non franchissable
        bool bVisited     = false; // drapeau A* (déjà traité)
        bool EnemyPresence= false; // présence d'ennemi (optionnel : éviter les collisions)

        // Coûts A* (g et f)
        float fGlobalGoal = 0.f;   // g + h (coût total estimé jusqu’à la cible)
        float fLocalGoal  = 0.f;   // g (coût exact depuis le départ)

        // Position dans la grille
        int x = 0, y = 0;          // indices ou coordonnées entières (selon ton usage)
        int col = 0, row = 0;      // redondant mais pratique pour lisibilité

        // Graphe
        std::vector<Node*> Neighbours; // voisins (4 ou 8 selon init)
        Node* parent = nullptr;        // pour reconstruire le chemin
    };

    // ------------------------ Accès/maj du noeud Ennemi ------------------------ //
    // Node *actuel* correspondant à la position d’un ennemi (utilitaire)
    Node* getEnemyNode() const { return EnemyNode; }

    // Calcule et retourne le Node le plus proche de `position` et met à jour EnemyNode
    Node* setEnemyNode(const sf::Vector2f& position);

    // ------------------------ Construction / Mise à jour de la grille ------------------------ //
    // À partir des cellules de la map (walkable, taille, etc.), crée et connecte les Nodes.
    void CreateNodes(const std::vector<Render::Cell>& cells);

    // ------------------------ Pathfinding principal ------------------------ //
    // Renvoie un chemin (liste de points monde) depuis `position` (départ) vers l’objectif.
    // L’objectif (nodeEnd) est défini ailleurs (ex: bord de map ou case “sortie”).
    std::vector<sf::Vector2f> SolveAStar(const sf::Vector2f& position);

    // Mise à jour périodique (si tu veux recalculer EnemyNode / coûts / états)
    void update(sf::Vector2f position);

    // Heuristique A* (distance euclidienne) — h(n, goal)
    float heuristic(Node* a, Node* b) {
        return sqrtf((a->x - b->x) * (a->x - b->x) +
                          (a->y - b->y) * (a->y - b->y));
    };

    // Vérifie si un chemin reste possible depuis `startPos` (utile après pose de tourelle)
    bool IsPathStillPossible(const sf::Vector2f& startPos);

    // Met à jour le flag EnemyPresence sur les Nodes à partir des positions d’ennemis
    void updateEnemyPresence(const std::vector<Enemy*>& enemies);

    // Trouve le Node de grille le plus proche d’une position monde
    Node* findClosestNode(const sf::Vector2f& position);

    // ------------------------ Données exposées (si besoin debug/outils) ------------------------ //
    std::vector<Node>   Nodes;        // stockage contigu (les pointeurs restent stables tant que pas de reallocation)
    std::vector<Node*>  EnemyNodes = {}; // éventuel suivi multi-ennemis (optionnel)
    std::vector<Node*>  nodeEnd    = {}; // objectifs possibles (sorties)
    Node*               EnemyNode  = nullptr; // dernier noeud "ennemi" résolu

protected:
    // Noeud départ pour le dernier calcul
    Node* nodeStart = nullptr;

    // Chemin courant et (si tu explores) plusieurs variantes
    std::vector<sf::Vector2f>               path;   // chemin résolu (points monde)
    std::vector<std::vector<sf::Vector2f> > paths;  // si tu gardes un historique ou multi-essais
};

#endif