#pragma once
#ifndef PLAYER_Hpp
#define PLAYER_Hpp

#include "defines.hpp"

class Weaponary;
#include "Weaponary.hpp"
#include "MySounds.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>

class Player : public sf::ConvexShape {
private:
	sf::Int16 hp;
public:
	float speed = 2.f;
	int speedMultiple = 1;
	float rot = 2.f;
	bool left = false;
	bool right = false;
	float torpedoSize = 5.f;
	size_t torpedoPoints;
	bool launchTorpedo = false;
	//Zbrojownia
	Weaponary* weaponary;
	static sf::Texture pltx;
	static sf::Font font;
	bool gg = false;

	MySounds sfx;

	sf::RectangleShape hpinfo; 
	sf::RectangleShape torinfo; 
	sf::RectangleShape barrel; 
	float barrelRotation = 180.f;
	float barrelStep = 90.f;
	bool barrelUp = false;

	sf::FloatRect getSmallGlobalBounds(float);
	void modHp(sf::Int16, bool);
	sf::Int16 getHp();
	void setHp(sf::Int16, bool);

	void updateInfo();

	bool playCollision = false;
	unsigned int sendPlayCollision = 0;

	Player();
	virtual ~Player();
};


#endif 

