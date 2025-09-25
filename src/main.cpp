////////////////////////////////////////////////////////
//          Programmes de Enzo CHADEVILLE             //
//                                                    //
//                                                    //
//                                                    //
////////////////////////////////////////////////////////

//--------Inclusion des librairies--------//
#include <iostream>
//#include <memory> //Librairie utile pour les unique_ptr (pointeur intelligent, plus avancé que les pointeur de base)
#include <vector>
#include <SFML/Graphics.hpp>

//--------Inclusion des autres programmes--------//

#include "render.hpp"
#include "game.hpp"
#include "enemy.hpp"


//using namespace std;

// Pour les paramètres de base, possible de faire des constantes globales dans un fichier de config !!!!!


int main() {

//---------------------------- INITIALISATION ------------------------------//

    // Initialisation de la fenêtre SFML
    sf::RenderWindow game_window(sf::VideoMode(800, 600), "Enemies + SFML");
    game_window.setFramerateLimit(60);
    //float cellSize = 20.f;

    // Initialisation des acteurs
    std::vector<Enemy*> enemies;      // PAS de new ici pas besoin grace à l'instance de Game
    std::vector<Tourelle*> tourelles;      // PAS de new ici pas besoin grace à l'instance de Game
    std::vector<Render::Cell> cells; // Tableau de cellules pour la grille
    cells = Render::buildCells(game_window); // On construit les cellules une fois pour toute au début (Seul les caractéristiques des cellules compte dans cette fonction)
    Game game;                       

    // Initialisation des aléatoires
    std::srand(static_cast<unsigned>(std::time(nullptr))); // seed une fois

    // Initialisation d'une clock
    sf::Clock clock;
    const float spawnoffset = 0.5f; // spawn toutes les 1.0s


//---------------------------- LOOP DU JEU ------------------------------//

    while (game_window.isOpen()) {
        sf::Event event{};
        while (game_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) game_window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                game_window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A) {
                //game.generateEnemy(enemies);  // exemple d’utilisation
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                int x = event.mouseButton.x;
                int y = event.mouseButton.y;
                game.generateTourelle(tourelles,cells,x,y);
            }     
        }
       
        // --- timing ---
        float dt = clock.restart().asSeconds(); // dt mis à jour A CHAQUE FRAME
        float spawnt;
        spawnt += dt;

        // Spawn tant qu'on a dépassé l'intervalle (robuste si un frame lag)
        while (spawnt >= spawnoffset) {
            game.generateEnemy(enemies,cells);
            spawnt -= spawnoffset;
        }

        // Si l'ennemie atteint le bout de la fenêtre il reviens au début
        /*for (auto& e : enemies) {
            if (e->getPosition().x > 820.f) e->setPosition(-20.f, e->getPosition().y);
        }*/

        //On nettoie la fenêtre de la frame précédente
        game_window.clear(sf::Color(30, 30, 35));


        //------- UPDATE DES ENTITES -------// 

        for (auto& e : enemies) e->update(dt);
        for (auto& t : tourelles) t->update(dt);

        //------- DESSIN DANS LA FENETRE SFML -------// 

        //--A FAIRE AVANT LES AUTRES ENTITES--// 
        //On redessine les grilles
        Render::drawGrid(game_window, sf::Color(80, 80, 80));

        // Mise à jour et dessin des ennemis
        for (auto& e : enemies) e->draw(game_window);
        for (auto& t : tourelles) t->draw(game_window);
        game_window.display();
    }

    // Libérer la mémoire avant de quitter
    game.destroyEnemy(enemies);
    game.destroyTourelles(tourelles);

    return 0;
}



/*int main()
{

    //Création de la fenètre du jeu
sf::RenderWindow game_window(sf::VideoMode(800, 600), "Enemies + SFML");
    game_window.setFramerateLimit(60);

    std::vector<Enemy*>& enemies = *(new std::vector<Enemy*>());
    
    // Initialisation de la graine aléatoire
    srand(static_cast<unsigned>(time(nullptr)));
   
 
    while (game_window.isOpen())
    {
        sf::Event event{};
        while (game_window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed){
                game_window.close();
            }
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                game_window.close();
            }

            //TYPE EVENT

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A) {

                //for (auto& e : enemies) e->spawn();
            }

        }    

        // Reset si trop à droite (boucle visuelle)
        for (auto& e : enemies) {
            if (e->getPosition().x > 820.f) e->setPosition(-20.f, e->getPosition().y);
        }

        //for (auto& e : enemies) e->spawn();

        game_window.clear(sf::Color(30, 30, 35));
        for (auto& e : enemies) e->draw(game_window);
        game_window.display();
    }

    // main.cpp
    Game::destroyEnemy(enemies); // libère la mémoire


    return 0;
}*/





    //Ici les classes des ennemies ont besoin de savoir les hp/ spd ou seulement un des paramètres,
    //  BasicEnemy goblin(20, 1.5f);
    //  TankEnemy orc(0.5f); 
    //  FastEnemy bat(15);

    //Si je crée un ennemie avec une classe qui définie déja chaque statistique (ici hp et spd) alors je ne met pas de ()
    //  TestEnemy neuille; 


//*******************************************************************************/

//                  VERSION AVANCE DES POINTEURS

//*******************************************************************************//

    // Version plus avancée d'une création d'un ennemie, elle utilise les lib <vector> et <memory>
    // L'avantage de cette méthode c'est que c'est plus robuste qu'un pointeur de base (pas besoin de penser au delete/new)
    // Mais c'est plus compliqué a comprendre. A voir à la fin si on fait un truc comme ça mais ça a l'air de marcher pareil
/*
    //std::vector est un tableau dynamique (liste qui peut grandir)
    // unique_ptr (un pointeur intelligent qui remplace le new et delete)
    //enemies est donc un nom de tableau dynamique de pointeur inteligent 
    std::vector<std::unique_ptr<Enemy>> enemies;


    //--------Création des enemies--------//

    enemies.emplace_back(std::make_unique<BasicEnemy>());
    enemies.emplace_back(std::make_unique<FastEnemy>());
    enemies.emplace_back(std::make_unique<TankEnemy>());
    enemies.emplace_back(std::make_unique<TargetEnemy>());
    enemies.emplace_back(std::make_unique<FlyEnemy>());


    // On positionne chaque ennemi en colonne à gauche
    float startX = 60.f;
    float y = 100.f;
    for (auto& e : enemies) {
        e->setPosition(startX, y);
        y += 80.f;
    }
*/
//*******************************************************************************//

//                  TOURELLES POISON ? (clock et loop clock)

//*******************************************************************************//
//
//      Structure permettant d'utiliser une horloge qui inflige des dégats à un ennemie dans le temps
//      La clock est initialisé, on définit un temps avant le dégat via une boucle if (ici 6s)
//      En comparant une variable avec l'horloge on permet de faire un dégat
//      On peut imaginer que cette structure permettrait de faire des tourelles de poisons qui inflige des dmg overtime
//   
/*
        sf::Clock clock;
        float damageTimer = 0.f;
        float dt = clock.restart().asSeconds();

        // Exemple: dégâts automatiques toutes les 0.6s sur le premier ennemi
        damageTimer += dt;
        if (damageTimer >= 0.6f) {
            damageTimer = 0.f;
            if (!enemies.empty()) {
                enemies.front()->hit(10);
                std::cout << "BasicEnemy HP = " << enemies.front()->getHp()
                          << (enemies.front()->isDead() ? " (dead)\n" : "\n");
            }
        }
*/
//*******************************************************************************//

//                  TEMPLATE SFML

//*******************************************************************************//
//
//      Template de base de SFMF     
//   
/*
sf::RenderWindow game_window(sf::VideoMode(800, 600), "Enemies + SFML");
    game_window.setFramerateLimit(60);

    sf::CircleShape shape(50.f);
    shape.setFillColor(sf::Color::Green);

    while (game_window.isOpen())
    {
        sf::Event event;
        while (game_window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                game_window.close();
        }

        game_window.clear();
        game_window.draw(shape);
        game_window.display();
    }
*/
//*******************************************************************************//


   /*** // On crée des ennemis avec new
    for (int i = 0; i < 10; i++)
    {
        enemies.push_back(new BasicEnemy());
        enemies.push_back(new FastEnemy());
        enemies.push_back(new TankEnemy());
        enemies.push_back(new TargetEnemy());
        enemies.push_back(new FlyEnemy());
        enemies.push_back(new BasicEnemy());
        enemies.push_back(new FastEnemy());
        enemies.push_back(new TankEnemy());
        enemies.push_back(new TargetEnemy());
        enemies.push_back(new FlyEnemy());
    }
    */