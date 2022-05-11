#include "Torpedo.hpp"
using namespace std;

Torpedo::Torpedo(Player* p) : sf::CircleShape() {
    this->setRadius(p->torpedoSize);
    this->setPointCount(p->torpedoPoints);
    this->speed = p->speed * p->speedMultiple;
    this->setRotation(p->getRotation() + p->barrelRotation);
    this->setPosition(p->getPosition());
    this->rot = p->rot;
    this->setOrigin(sf::Vector2f(p->torpedoSize / 2, p->torpedoSize / 2));
}

Torpedo::~Torpedo() { }
