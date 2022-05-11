#pragma once
#ifndef CLIENT_Hpp
#define CLIENT_Hpp

#include"defines.hpp"
#include "Game.hpp"

#include <SFML/Graphics.hpp>
#include<SFML/Network.hpp>
#include<SFML/Audio.hpp>
using namespace std;

class Client :public Game, public sf::RenderWindow {
public:
	sf::Uint16 ID = 1;
	sf::TcpSocket* sConn;
	sf::TcpSocket* rConn;
	sf::Packet rpack;
	sf::Packet spack;
	Player player;
	static sf::Font font;
	string title;

	sf::Texture bgtx;
	sf::RectangleShape background;

	sf::Text info1;
	sf::Text info2;

	MySounds sfx;
	
	Client();
	Client(string);
	~Client();

	bool runClient();

	
	static void receiveData(Client*);
	static void sendData(Client*);
	

	void unpackPlayers(sf::Packet&, bool);
	void unpackWeaponary(sf::Packet&);
	void cliDraw(Player&, bool);

	static void setSFMLWindow(HWND gw);
};

#endif 

