#pragma once
#ifndef STONE_Hpp
#define STONE_Hpp

#include "defines.hpp"
#include <utility>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>

class Stone : public sf::RectangleShape {
public:

    Stone();
    virtual ~Stone();
};


#endif 

