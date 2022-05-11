#include "Stone.hpp"
using namespace std;

Stone::Stone() :sf::RectangleShape() {
    this->setSize(sf::Vector2f(16, 16));
    this->setOutlineColor(sf::Color(0xffff64ff));
    this->setOutlineThickness(1);
    this->setOrigin(8, 8);
    this->setFillColor(sf::Color(0x323232ff));
    this->setPosition(SCREENX * 0.1 + rand() % SCREENX * 0.8, SCREENY * 0.1 + rand() % SCREENY * 0.8);
}
Stone::~Stone() {}