#include "Astar.hpp"
#include <iostream>
#include <algorithm>
#include <limits>
#include <cmath>
#include "enemy.hpp"


//========================== CREATION DES NOEUDS ==============================================================
void PathFinding_AStar::CreateNodes(const std::vector<Render::Cell>& cells)
{
    if (cells.empty()) return;              //Si pas de cellules, ne créer pas de noeuds
    NodeSize = Render::Cell::cellSize;      //Récupère la dimension des cellules

    //Déterminer la taille de la grille
    int maxRow = 0;
    int maxCol = 0;
    for (const auto& c : cells) {
        if (c.row > maxRow) maxRow = c.row;
        if (c.col > maxCol) maxCol = c.col;
    }
    gridRows = maxRow + 1;
    gridCols = maxCol + 1;

    Nodes.clear();
    Nodes.reserve(cells.size());

    // Création des nodes
    for (const auto& c : cells) {
        Node node;                                                  //Créer un noeud
        node.x = c.center.x;                                        //Attribut la position x du centre de la cellule
        node.y = c.center.y;                                        //Attribut la position y du centre de la cellule
        node.row = c.row;                                           //Attribut la ligne de la cellule
        node.col = c.col;                                           //Attribut la colonne de la cellule
        node.turreted = c.turreted;                                 //Attribut l'état occupé par une tourelle
        node.bVisited = false;                                      //Noeud déjà visité
        node.EnemyPresence = false;                                 //Noeud occupé par au moins un ennemi
        node.fGlobalGoal = std::numeric_limits<float>::infinity();  //Distance par rapport au noeud de fin INCONNUE (d'où infini)
        node.fLocalGoal = std::numeric_limits<float>::infinity();   //Distance par rapport au prochain noeud INCONNUE (d'où infini)
        node.parent = nullptr;                                      //Pas de parent initialement
        node.Neighbours.clear();                                    //Pas de voisin initialement
        Nodes.push_back(node);                                      //Ajoute le noeud au vecteur de noeuds
    }

    // Détermination des voisins (8 directions)
    for (size_t i = 0; i < Nodes.size(); ++i) {                     //Noeud i
        for (size_t j = 0; j < Nodes.size(); ++j) {                 //Noeud j
            if (i == j) continue;                                   //Si c'est le même noeud, ne l'ajoute pas à son vecteur de voisins
            if (std::abs(Nodes[j].row - Nodes[i].row) <= 1 &&       //Limite la recherche de voisins à 1 ligne de distance
                std::abs(Nodes[j].col - Nodes[i].col) <= 1) {       //Limite la recherche de voisins à 1 colonne de distance
                Nodes[i].Neighbours.push_back(&Nodes[j]);           //Ajoute à son vecteur de voisins
            }
        }
    }

    // Détermination des noeuds de fin (bord droit)
    nodeEnd.clear();                //Vide le vecteur de noeuds de fin
    for (auto& n : Nodes) {
        if (n.col >= gridCols - 1)  //Les noeuds de la dernière colonne sont des noeuds de fin
            nodeEnd.push_back(&n);  //Remplit le vecteur de noeuds de fin
    }
    std::cout << "Nodes: " << Nodes.size() << ", nodeEnd: " << nodeEnd.size() << std::endl;

}

//========================== ATTRIBUTION D'UN NOEUD A UN ENNEMI ===============================================
PathFinding_AStar::Node* PathFinding_AStar::findClosestNode(const sf::Vector2f& position)
{
    if (Nodes.empty()) return nullptr;                          //Si le vecteur de noeuds est vide, pas de noeud à associer à l'ennemi

    Node* closest = nullptr;                                    //Initialisation du pointeur
    float bestDist = std::numeric_limits<float>::infinity();    //Initialisation de la distance (infini)

    for (auto& n : Nodes) {
        float dx = n.x - position.x;
        float dy = n.y - position.y;
        float d = dx * dx + dy * dy;
        if (d < bestDist) {
            bestDist = d;
            closest = &n;
        }
    }
    return closest;
}

//========================== DECLARATION DE L'HEURISTIC =======================================================
float PathFinding_AStar::heuristic(Node* a, Node* b){
    if (!a || !b) return std::numeric_limits<float>::infinity();
    return sqrtf((a->x - b->x) * (a->x - b->x) + (a->y - b->y) * (a->y - b->y));
};

//========================== CALCUL DU CHEMIN LE PLUS COURT ===================================================
std::vector<sf::Vector2f> PathFinding_AStar::SolveAStar(const sf::Vector2f& startPos)
{
    std::vector<sf::Vector2f> emptyPath;
    paths.clear();
    path.clear();

    if (Nodes.empty() || nodeEnd.empty() || NodeSize <= 0.f)
        return emptyPath;

    // Trouve le noeud de départ le plus proche
    Node* startNode = findClosestNode(startPos);                        //Attribution du noeud ennemi pour le noeud de départ du chemin
    if (!startNode) {                                                   //Si différent de 0, le noeud n'existe pas
        std::cerr << "[AStar] startNode introuvable pour (" << startPos.x << "," << startPos.y << ")\n";
        return emptyPath;
    }

    // Pour chaque nodeEnd possible, on calcule un chemin
    for (Node* targetEnd : nodeEnd) {
        if (!targetEnd) continue;

        // Reset des nodes
        for (auto& n : Nodes) {
            n.bVisited = false;
            n.fGlobalGoal = std::numeric_limits<float>::infinity();
            n.fLocalGoal = std::numeric_limits<float>::infinity();
            n.parent = nullptr;
        }

        Node* nodeCurrent = startNode;                              //Initialise le noeud actuel à analyser le noeud de départ
        startNode->fLocalGoal = 0.0f;                               //Distance entre le noeud et lui même (d'où 0)
        startNode->fGlobalGoal = heuristic(startNode, targetEnd);   //Calcule la distance entre noeud de départ et noeud de fin

        std::vector<Node*> listNotTestedNodes;                      //Vecteur de noeuds à explorer
        listNotTestedNodes.push_back(startNode);                    //Ajoute startNode comme premier noeud à explorer

        while (!listNotTestedNodes.empty() && nodeCurrent != targetEnd) {       //Tant que la fin n'est pas atteinte et que tous les noeuds n'ont pas été explorés
            std::sort(listNotTestedNodes.begin(), listNotTestedNodes.end(),     //Trie avec la fonction lambda
                [](Node* LowCostNode, Node* HighCostNode) { return LowCostNode->fGlobalGoal < HighCostNode->fGlobalGoal; }); //Fontion lambda : trie par ordre croissant des coûts de distance

            while (!listNotTestedNodes.empty() && listNotTestedNodes.front()->bVisited)     //Tant que tous les noeuds n'ont pas été visités et que le premier noeud a été visité
                listNotTestedNodes.erase(listNotTestedNodes.begin());                       //Enlève le premier noeud (car déjà visité)
            if (listNotTestedNodes.empty()) break;

            nodeCurrent = listNotTestedNodes.front();       //Définit le premier noeud de la liste comme étant le prochain à être analysé
            nodeCurrent->bVisited = true;                   //Noeud actuel maintenant visité

            for (Node* neighbour : nodeCurrent->Neighbours) {                   //Va déterminer le meilleur voisin
                if (!neighbour || neighbour->bVisited || neighbour->turreted)   //Si il n'y a pas de noeud | noeud déjà visité | noeud occupé par une tourelle, ignore le noeud voisin
                    continue;

                float dist = 0.f;       //Initialisation de dist
                float penalty = 1.0f;   //Valeur par défaut (pas de pénalité)

                for (const Node& n : Nodes)  // supposons que tu as une liste de tours
                {
                        float dx = neighbour->x - n.x;
                        float dy = neighbour->y - n.y;
                        dist = std::sqrt(dx * dx + dy * dy);;
                
                    //Si une tour est proche
                    if (dist < 50.f) {
                        //Plus on est proche, plus la pénalité augmente
                        penalty += (50.f - dist) / 5.f;   //A 0 de distance → +10 ; à 50 → +0
                    }
                }

                float stepCost = ((neighbour->row != nodeCurrent->row && neighbour->col != nodeCurrent->col)
                                  ? 1.414f : 1.0f) * penalty;                   //Déplacement en diagonale, pas le même coût (normalise le déplacement)

                float possibleLowerGoal = nodeCurrent->fLocalGoal + stepCost;   //Coût du possible nouveau chemin

                if (possibleLowerGoal < neighbour->fLocalGoal) {                //Si coût inférieur au coût connu
                    neighbour->parent = nodeCurrent;                            //Désigne comme noeud parent
                    neighbour->fLocalGoal = possibleLowerGoal;                  //Assigne le nouveau coût local
                    neighbour->fGlobalGoal = neighbour->fLocalGoal + heuristic(neighbour, targetEnd);   //Assigne le nouveau coût global  
                    listNotTestedNodes.push_back(neighbour);                    //Ajoute les noeuds à la liste des noeuds à tester
                }
            }
        }

        if (targetEnd->parent == nullptr && targetEnd != startNode) //Si le noeud de fin est impossible à atteindre et qu'il n'est pas le noeud de départ
            continue;

        // Reconstruction du chemin
        path.clear();
        Node* p = targetEnd;                            //Parcour de la fin au début (noeud de fin au noeud de départ | on remonte le chemin)
        while (p) {                                     //Tant que le noeud p existe
            path.push_back(sf::Vector2f(p->x, p->y));   //Ajoute les coordonnées du noeud p au chemin
            p = p->parent;                              //On refait avec le noeud parent
        }
        std::reverse(path.begin(), path.end());         //Inverse le chemin (pour partir du début)

        if (!path.empty())          //Si le chemin n'est pas vide, on l'ajoute aux chemins possibles
            paths.push_back(path);
    }

    if (paths.empty())          //Si pas de chemin existant
        return emptyPath;       //Retourne pas de chemin

    //Choisir le chemin le plus court avec une fonction lambda
    auto pathCost = [](const std::vector<sf::Vector2f>& p) -> float { //Fonction lambda : détermine le coût du chemin
        if (p.size() < 2) return 0.f;
        float cost = 0.f;
        for (size_t i = 1; i < p.size(); ++i) {
            float dx = p[i].x - p[i - 1].x;
            float dy = p[i].y - p[i - 1].y;
            cost += std::sqrt(dx * dx + dy * dy);
        }
        return cost;
    };

    size_t bestIndex = 0;
    float bestCost = pathCost(paths[0]);
    for (size_t i = 1; i < paths.size(); ++i) {     //Permet de garder le chemin le moins coûteux
        float c = pathCost(paths[i]);
        if (c < bestCost) {
            bestCost = c;
            bestIndex = i;
        }
    }

    std::vector<sf::Vector2f> finalPath = paths[bestIndex];     //Assigne le chemin le moins coûteux
    
    finalPath.erase(finalPath.begin());                         //Supprime le noeud de l'ennemi
    
    finalPath.insert(finalPath.begin(), startPos);              //Insère la position réelle du départ (pas celle du noeud de l'ennemi)

    return finalPath;
}

//========================== VERIFICATION DU CHEMIN APRES POSITIONNEMENT TOURELLE =============================
bool PathFinding_AStar::IsPathStillPossible(const sf::Vector2f& startPos)
{
    if (Nodes.empty() || nodeEnd.empty()) return false;

    Node* startNode = findClosestNode(startPos);
    if (!startNode) return false;

    //Réinitialise les noeuds
    for (auto& n : Nodes) {
        n.bVisited = false;
        n.parent = nullptr;
    }

    std::vector<Node*> open;
    open.push_back(startNode);

    //Cherche si il existe encore au moins un chemin
    while (!open.empty()) {
        Node* current = open.back();
        open.pop_back();
        current->bVisited = true;

        if (std::find(nodeEnd.begin(), nodeEnd.end(), current) != nodeEnd.end())    //Si il existe un chemin, retourne vrai
            return true;
        //Sinon cherche au niveau des voisins
        for (auto* n : current->Neighbours) {
            if (!n || n->bVisited || n->turreted) continue;
            open.push_back(n);
        }
    }
    //Si rien trouvé, retourne faux => permet de bloquer le positionnement de la tourelle
    return false;
}

//========================== MET A JOUR LES NOEUDS OCCUPES PAR DES ENNEMIS ====================================
void PathFinding_AStar::updateEnemyPresence(const std::vector<Enemy*>& enemies)
{   
    //Réinitialisation de la présence des ennemis dans les noeuds
    for (auto& node : Nodes)
        node.EnemyPresence = false;

    // Influence continue : chaque ennemi affecte les nodes autour de lui
    for (auto* e : enemies) {
        if (!e) continue;                       //Si pas d'ennemi, igonre le reste
        sf::Vector2f pos = e->getPosition();    //Sinon récupère sa psoition

        //Compare la position de l'ennemi à tous les noeuds
        for (auto& n : Nodes) {
            float dx = n.x - pos.x;
            float dy = n.y - pos.y;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist < NodeSize * 0.8f) {   //Si l'ennemi est à une certaine distance du centre d'un noeud
                n.EnemyPresence = true;     //Le noeud devient occupé par l'ennemi
            }
        }
    }
}

