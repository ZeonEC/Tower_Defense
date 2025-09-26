
#include "main.hpp"
#include "tower.hpp"
#include "ennemy.hpp"
#include "perso.hpp"
#include "Astar.hpp"

int main(){
    srand(time(0));
    //Creation de la fenêtre principale
    sf::RenderWindow window(sf::VideoMode(1000,1000),"TowerDefence");

    //Creation de la map
    sf::RenderTexture map;
    map.create(500,500);
    sf::Sprite map_sprite(map.getTexture());
    map_sprite.setPosition(0,0);

    //Creation du panneau de commande
    sf::RenderTexture control_board;
    control_board.create(500,500);
    sf::Sprite control_board_sprite(control_board.getTexture());
    control_board_sprite.setPosition(500,500);


    //Creation du panneau d'information
    sf::RenderTexture info_board;
    info_board.create(500,500);
    sf::Sprite info_board_sprite(info_board.getTexture());
    info_board_sprite.setPosition(0,500);

    

    PathFinding_AStar pathfinder;
    Ennemy zerg("zerg",30,100,50,10,20,1,5,1,pathfinder);

    while (window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                window.close();
            }
        }
        zerg.move(pathfinder);
        //pathfinder.Update(window, zerg.groupe_ennemies[0]->position);

        //Reinitialisation de la fenêtre
        window.clear(sf::Color::Black);        
        map.clear(sf::Color::Black);
        control_board.clear(sf::Color::Black);
        info_board.clear(sf::Color::Black);

        //Dessin sur la fenêtre
        /*
        map.display();
        map_sprite.setTexture(map.getTexture());
        control_board.draw(control_board_sprite);
        info_board.draw(info_board_sprite);
        control_board.display();
        info_board.display();
        */


        

        
        pathfinder.Draw(window);
        zerg.draw(window);
        //window.draw(zerg.groupe_ennemies[0]->shape);
        //window.draw(map_sprite);
        //Affichage de la fenêtre
        window.display();



    }



    
    return 0;
}