#pragma once

class Player{

private:
    int playerHealth = 20;
    int playerRessoures = 200;

public:
    int getHealth() const { return playerHealth; }
    void reduceHealth(int amount) { 
        playerHealth -= amount;
        if (playerHealth < 0) playerHealth = 0;
        if (playerHealth > 20) playerHealth = 20; 
    }
    
    int getRessources() const { return playerRessoures; }
    void AddRessources(int amount) { playerRessoures += amount; }
    void reduceRessources(int amount) { playerRessoures -= amount; }
};
