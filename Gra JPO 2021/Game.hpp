#pragma once
#ifndef GAME_HPP
#define GAME_HPP

#include "defines.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <vector>
#include <utility>
#include <unordered_map>
#include <map>
#include <sstream>
#include <cmath>
#include "Player.hpp"
#include "Torpedo.hpp"
#include "Weaponary.hpp"
#include "Stone.hpp"
using namespace std;

struct thData;
enum Status {WAIT,SEND_ENV,RUN};

class Game {
public:
	static sf::Clock clock;
	Status status = WAIT;

	map <sf::Uint16, Player*>players;
	map <sf::Uint16, sf::TcpSocket*> sSockets;
	map<sf::Uint16, sf::TcpSocket*> rSockets;
	Stone stones[STONES];
	vector <sf::Thread*> threads;

	//Obszar gry
	sf::RectangleShape area;
	unsigned int send_env_nr = 0;

	Game();
	virtual ~Game();
	void runServer();
	static void createThread(thData*);
	static void connect(Game*);
	static void runTheGame(Game*);
	static sf::Vector2f calcVector(float, float);

	void getPlayersPacket(sf::Packet&, bool);
	void getWeaponaryPacket(sf::Packet&);
	void packStonePositions(sf::Packet&);
	void unpackStonePositions(sf::Packet& pack);
	void setGG();

	static void startConsole();
	static bool isPointInsidePolygon(vector <sf::Vector2f>&, const sf::Vector2f&);
	static bool isPointInsidePolygon(const sf::Transform&, vector <sf::Vector2f>&, const sf::Vector2f&);
	static int sign();
};

struct thData {
	Game* game;
	sf::Uint16 id;
	sf::String s; 
};

#endif 

