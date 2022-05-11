#include "Weaponary.hpp"

Weaponary::Weaponary() {}

bool Weaponary::addTorpedo(Player* p) {
    if (this->torpedos.size() >= this->maxTorpedos) return false;
    sf::Uint16 s = 1;
    while (this->torpedos.find(s) != this->torpedos.end()) {
        s++;
    }
    this->torpedos.emplace(make_pair(s, new Torpedo(p)));
    if (!(rand() % 10)) this->torpedos[s]->heal = true;
    else if (!(rand() % 10)) this->torpedos[s]->kill = true;
    return true;
}
bool Weaponary::addTorpedoNr(Player* p, sf::Uint16 torid) {
    if (this->torpedos.find(torid) == this->torpedos.end())  
        this->torpedos.emplace(make_pair(torid, new Torpedo(p)));
    return true;
}


Weaponary::~Weaponary() {
    for (auto& t : this->torpedos)
        delete t.second;
}

