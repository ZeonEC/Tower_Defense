#pragma once

// ====================================== ROLE DE LA CLASSE =========================================== //
//
// La classe Player gère les informations essentielles du joueur :
//  - ses points de vie (Health)
//  - ses ressources économiques (Ressources)
//
// Elle est utilisée par la classe Game pour tout ce qui touche à
// la survie du joueur et à la gestion des achats de tourelles.
//
// ==================================================================================================== //

class Player {

private:
    int playerHealth    = 100; // PV du joueur (0 → défaite)
    int playerRessoures = 400; // monnaie utilisée pour acheter les tourelles

public:
    // ------------------------ SANTÉ ------------------------ //

    // Retourne la santé actuelle du joueur
    int getHealth() const { return playerHealth; }

    // Réduit les points de vie du joueur (ex : un ennemi atteint la base)
    // La valeur est bornée entre 0 et 100 pour éviter les dépassements
    void reduceHealth(int amount) { 
        playerHealth -= amount;
        if (playerHealth < 0)   playerHealth = 0;
        if (playerHealth > 100) playerHealth = 100; 
    }

    // ------------------------ RESSOURCES ------------------------ //

    // Retourne la quantité de ressources actuelle (argent du joueur)
    int getRessources() const { return playerRessoures; }

    // Ajoute des ressources (par exemple lors de la mort d’un ennemi)
    void AddRessources(int amount) { playerRessoures += amount; }

    // Retire des ressources (ex : achat de tourelle)
    void reduceRessources(int amount) { playerRessoures -= amount; }
};
