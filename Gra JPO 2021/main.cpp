#include "defines.hpp"
#include "Game.hpp"
#include "Client.hpp"

using namespace std;

int main() {
    Game::startConsole();
    srand(time(NULL));
    setlocale(LC_ALL, "");

    string opt;
    cout << "[s]erwer lub login (Liczba 0-255) : ";
    cin >> opt;
    if (opt == "s") {
        Game server;
        server.runServer();
    }
    else { 
        Client::font.loadFromFile("fonts/OpenSans-Regular.ttf");
        Player::font.loadFromFile("fonts/OpenSans-Light.ttf");
        Player::pltx.loadFromFile("images/anim.png");

        Client client("GRA Statki JPO");
        sf::Uint16 key;
        cin >> key; 
        client.ID = key;
        client.runClient();
    }
    cout << "Koniec Gry";
    return 1;
}



