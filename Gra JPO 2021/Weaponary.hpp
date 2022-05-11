#pragma once
#ifndef WEAPONARY_Hpp
#define WEAPONARY_Hpp
#include "defines.hpp"
class Torpedo;
#include "Torpedo.hpp"
#include <utility>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>
class Player;
#include "Player.hpp"
using namespace std;

class Weaponary {
public:
	map<sf::Uint16, Torpedo*> torpedos;
	unsigned int maxTorpedos = 12;
	Weaponary();

	bool addTorpedo(Player*);
	bool addTorpedoNr(Player*, sf::Uint16);
	virtual ~Weaponary();
};

#endif 

