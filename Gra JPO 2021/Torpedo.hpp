#pragma once
#ifndef TORPEDO_Hpp
#define TORPEDO_Hpp

#include "defines.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>
class Player;
#include "Player.hpp"

class Torpedo : public sf::CircleShape {
public:
    float rot;
    float speed = 5.f;
    bool lethal = true;
    unsigned int lethalCheck = 0;
    bool heal = false;
    bool kill = false;

    Torpedo(Player*);
    virtual ~Torpedo();
};

#endif 

