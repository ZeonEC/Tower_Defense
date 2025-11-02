#pragma once

class Player{

private:
    int playerHealth = 100;
    int playerRessoures = 400;

public:
    int getHealth() const { return playerHealth; }
    void reduceHealth(int amount) { 
        playerHealth -= amount;
        if (playerHealth < 0) playerHealth = 0;
        if (playerHealth > 100) playerHealth = 100; 
    }
    
    int getRessources() const { return playerRessoures; }
    void AddRessources(int amount) { playerRessoures += amount; }
    void reduceRessources(int amount) { playerRessoures -= amount; }
};
