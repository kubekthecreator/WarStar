#pragma once
#ifndef MYSOUNDS_HPP

#define MYSOUNDS_HPP
#include "defines.hpp"

using namespace std;

class MySounds {
public:
	string folder = "sounds";
	string type = "wav";
	unordered_map<string, sf::SoundBuffer> bufor;//kluczem s¹ nazwy plików, wartoœciami bêd¹ bufory na dŸwiêki
	static sf::Sound sounds[MAX_SOUNDS];//sloty na odtwarzane dŸwiêki 

    MySounds();
    virtual ~MySounds();
    MySounds(const string);
    MySounds(const string, std::initializer_list <string>);
    void addFile(const string&);
    int play(string, int, bool, float, bool);
    int play(string, int, bool, float, bool, float, const sf::Vector2f&, float);
    static void stopAll();//zatrzymuje sloty z muzyk¹
    static void pauseAll();
    static void unpauseAll();
    static void playOnce(sf::Sound*, int, bool, float, bool, float, const sf::Vector2f&, float);

};
#endif 