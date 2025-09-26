#include "tower.hpp"

TOWER::TOWER(std::string name, int HitPoint, int Shield, int Defence, int attack_damage, int attack_speed, int attack_range){
    
}

TOWER::~TOWER(){

}

sf::RectangleShape TOWER::CreateShape(){
    sf::RectangleShape shape;
    shape.setSize(sf::Vector2f(40.f,40.f));
    shape.setFillColor(sf::Color(50,150,250));
    shape.setOrigin(sf::Vector2f(20.f,20.f));
    shape.setOutlineThickness(1.f);
    shape.setOutlineColor(sf::Color::Black);
    return shape;
}

void TOWER::draw(sf::RenderWindow& rendertexture){
    rendertexture.draw(this->CreateShape());
}