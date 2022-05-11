#include "Player.hpp"
using namespace std;
sf::Texture Player::pltx;
sf::Font Player::font;

Player::Player() : sf::ConvexShape(4) {
    this->setPointCount(4);
    this->setPoint(0, sf::Vector2f(0, 40));
    this->setPoint(1, sf::Vector2f(5, 0));
    this->setPoint(2, sf::Vector2f(35, 0));
    this->setPoint(3, sf::Vector2f(40, 40));
    this->setOrigin(sf::Vector2f(20, 20));
    this->setPosition(sf::Vector2f(150 + rand() % (SCREENX - 200), 150 + rand() % (SCREENY - 200)));
    this->setRotation(0);
    this->torpedoPoints = 3 + rand() % 3;
    this->weaponary = new Weaponary();
    this->setTexture(&Player::pltx);

    this->hpinfo.setFillColor(sf::Color(0x00ff00ff));
    this->torinfo.setFillColor(sf::Color(0xffff00ff));

    this->sfx = MySounds("sounds", { "heal","notorp","shoot","hit","collp","collt" });
    this->hp = 10;

    this->barrel.setSize(sf::Vector2f(5, 20));
    this->barrel.setPosition(this->getPosition().x + this->getOrigin().x, this->getPosition().y + this->getOrigin().y);
    this->barrel.setOrigin(3, 18);
    this->barrel.setFillColor(sf::Color(0x323232ff));
    this->barrel.setOutlineColor(sf::Color(0xc8c8ffff));
    this->barrel.setOutlineThickness(1);
}

void Player::updateInfo() {
    this->hpinfo.setSize(sf::Vector2f(this->getHp() * 2, 5));
    this->hpinfo.setPosition(this->getPosition().x - 24, this->getPosition().y - 40);
    this->torinfo.setSize(sf::Vector2f((this->weaponary->maxTorpedos - this->weaponary->torpedos.size()) * 2, 5));
    this->torinfo.setPosition(this->getPosition().x - 5, this->getPosition().y + 30);
}

sf::FloatRect Player::getSmallGlobalBounds(float x = 5) {
    sf::FloatRect rect = this->getGlobalBounds();
    rect.top += x;
    rect.left += x;
    rect.width -= 2 * x;
    rect.height -= 2 * x;
    return rect;
}

void Player::modHp(sf::Int16 hpmod, bool play) {
    if (this->gg) return; 
    this->hp += hpmod;
    if (play && hpmod > 0) this->sfx.play("heal", 100, false, 1.f, false);
    if (play && hpmod < 0) this->sfx.play("hit", 100, false, 1.f, false);

}
//! zwraca HP
sf::Int16 Player::getHp() { return this->hp; }
//! ustawia zdrowie, \param play odpala dŸwiêk jak prada
void Player::setHp(sf::Int16 shp, bool play) {
    if (this->gg) return;
    if (play && this->hp > shp) this->sfx.play("hit", 100, false, 1.f, false);
    else if (play && this->hp < shp) this->sfx.play("heal", 100, false, 1.f, false);
    this->hp = shp;
}

Player::~Player() {
    delete this->weaponary;
}
