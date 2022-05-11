#include "Client.hpp"
sf::Font Client::font;
using namespace std;

Client::Client() {}

Client::Client(string t) : sf::RenderWindow() {
    this->title = t;
    this->create(sf::VideoMode(SCREENX, SCREENY, CBITS), "");
    this->setTitle(title);
    this->setActive(true);
    this->setFramerateLimit(0); 
    this->setPosition(sf::Vector2i(POSX, POSY));
    this->setKeyRepeatEnabled(false);
    this->setVerticalSyncEnabled(true);

    this->player.setFillColor(sf::Color(0xffffffff)); 

    this->bgtx.loadFromFile("images/background.png", sf::IntRect(0, 0, SCREENX, SCREENY));
    this->background.setSize(sf::Vector2f(SCREENX, SCREENY));
    this->background.setTexture(&(this->bgtx));

    // napisy
    this->info1.setFont(Client::font);
    this->info2.setFont(Client::font);
    this->info1.setPosition(sf::Vector2f(SCREENX * 0.3, SCREENY * 0.4));
    this->info2.setPosition(sf::Vector2f(SCREENX * 0.3, SCREENY * 0.4));
    this->info2.setFillColor(sf::Color(0xffa6a6ff));
    this->info2.setFillColor(sf::Color(0x00c6c6ff));
    this->info1.setCharacterSize(32);
    this->info2.setCharacterSize(32);
    this->info1.setString("YOU ARE LOOSER");
    this->info2.setString("EZ Z NOOBAMI, JESTEM KOTEM");
}

//! CLIENT
bool Client::runClient() {
    Client::setSFMLWindow(this->getSystemHandle());
    sf::Thread ths(&Client::sendData, this);
    sf::Thread thr(&Client::receiveData, this);
    ths.launch();
    thr.launch();

    sf::Time start = Game::clock.getElapsedTime();
    sf::Time tick = sf::microseconds(0);
    sf::Event e;
    unsigned int steps = 0;
    while (this->isOpen()) {
        while (this->pollEvent(e)) {
            if (e.type == sf::Event::Closed || (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))) { this->close(); }
            // STEROWANIE GRACZEM (torpedy)
            if (this->player.getHp() > 0 && e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Space) {
                this->player.launchTorpedo = true;
                if (this->player.weaponary->torpedos.size() < this->player.weaponary->maxTorpedos)  this->player.sfx.play("shoot", 100, false, 1.f, false);//je¿eli mamy torpedy to przy wystrzale dzwiek strzalu
               else this->player.sfx.play("notorp", 100, false, 1.f, false);
            }
            if (this->player.getHp() > 0 && e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Up) {
                this->player.barrelUp = true;
                this->player.sfx.play("collt", 100, false, 1.f, false);
            }
        }
        // STEROWANIE GRACZEM
        if (this->hasFocus()) {
            // W - speed
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) this->player.speedMultiple = 2;
            else this->player.speedMultiple = 1;
            // A - lewo
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                this->player.left = true;  this->player.right = false;
            }
            // D - prawo
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                this->player.right = true;  this->player.left = false;
            }
            else {
                this->player.right = this->player.left = false;
            }
        }
        /// GENEROWANIE OBRAZU - DRAW !
        this->clear(sf::Color(0x000000ff));
        this->draw(this->background); 
        this->draw(this->area); 

           /// kamienie
        for (int index = 0; index < STONES; index++) 
            this->draw(this->stones[index]);

        /// wygra³/przegra³ ?
        if (this->player.getHp() == 0) this->draw(this->info1);
        else if (this->player.gg) this->draw(this->info2);

        this->cliDraw(this->player, false); 
        for (auto& p : this->players) { 
            this->cliDraw(*(p.second), true);
        }
        sf::sleep(sf::milliseconds(10));
        this->display();


        tick = Game::clock.getElapsedTime() - start;
        if (tick.asMilliseconds() >= 5000) {
            cout << "œrednia klatek: " << steps / 5.f << " k/s\n";
            start = Game::clock.getElapsedTime();
            steps = 0;
        }
        steps++;
    }
    cout << "Zamkniêto okno gry\n";
    ths.terminate();
    thr.terminate();
    return true;
}
void Client::cliDraw(Player& plr, bool enemy) {
    int step = 40;
    int m = (enemy) ? 0 : 6 * 40;
    if (!enemy) {
        // gracz
    }
    if (enemy) {//jak wróg
        plr.setTextureRect(sf::IntRect(0, ((rand() % 3 + 2) * step) + m, step, step));
    }
    else {//je¿eli gracz
        if (plr.left) player.setTextureRect(sf::IntRect(0, 0 * step + m, step, step));
        else if (plr.right) plr.setTextureRect(sf::IntRect(0, 1 * step + m, step, step));
        else if (plr.speedMultiple == 2) plr.setTextureRect(sf::IntRect(0, ((rand() % 3 + 2) * step) + m, step, step));
        else plr.setTextureRect(sf::IntRect(0, (5 * step) + m, step, step));
    }
    plr.updateInfo(); 

    this->draw(plr);
    this->draw(plr.hpinfo);
    this->draw(plr.torinfo);
    this->draw(plr.barrel);
    for (auto& t : plr.weaponary->torpedos) {
        if (t.second->lethal)
            this->draw(*(t.second));
    }
}

//! w¹tek ODBIORU informacji
void Client::receiveData(Client* cli) {
    // PO£¥CZENIE
    cout << "receiveData\n";
    cli->rConn = new sf::TcpSocket;
    if (cli->rConn->connect(SERWER_IP, LISTENING_PORT, sf::seconds(10.0f)) != sf::Socket::Done) { cout << "Connection to server error.\n "; delete cli->rConn; return; }
    else {
        cout << "serwer: " << cli->rConn->getRemoteAddress().toString() << " : " << cli->rConn->getRemotePort() << " Lokalny port: " << cli->rConn->getLocalPort() << endl;
    }
    // WYS£ANIE ID
    cli->rpack.clear();
    sf::String s = "rec";
    cli->rpack << cli->ID << s;
    if (cli->rConn->send(cli->rpack) != sf::Socket::Done) { cout << "Nieudana synchronizacja.\n"; delete cli->rConn; return; }
    else { cout << "Sychronizacja klienta...\n"; }
    // PÊTLA ODBIERAJ¥CA
    string type;
    while (1) {
        cli->rpack.clear();
        if (cli->rConn->receive(cli->rpack) != sf::Socket::Done) { cout << "disconnect\n"; delete cli->rConn; return; }
        else {
            cli->rpack >> type;
            if (type == "PLAYERS") cli->unpackPlayers(cli->rpack, false);
            else if (type == "PLAYERS_ENV") cli->unpackPlayers(cli->rpack, true);
            else if (type == "WEAPONARY") cli->unpackWeaponary(cli->rpack);
            else if (type == "STONES") cli->unpackStonePositions(cli->rpack);
            else cout << "serwer pijany bredzi g³upoty" << type << endl;
        }
    }
}
//! w¹tek wysy³ania
void Client::sendData(Client* cli) {
    cout << "sendData\n";
    cli->sConn = new sf::TcpSocket;
    if (cli->sConn->connect(SERWER_IP, LISTENING_PORT, sf::seconds(10.0f)) != sf::Socket::Done) { cout << "Connection to server error.\n "; delete cli->sConn; return; }
    else {
        cout << "serwer: " << cli->sConn->getRemoteAddress().toString() << " : " << cli->sConn->getRemotePort() << " Lokalny port: " << cli->sConn->getLocalPort() << endl;
    }
    cli->spack.clear();
    sf::String s = "send";
    cli->spack << cli->ID << s;
    if (cli->sConn->send(cli->spack) != sf::Socket::Done) { cout << "sync error.\n"; delete cli->sConn; return; }
    else { cout << "synchronization player=" << cli->ID << " ...\n"; }
    // PÊTLA WYSY£AJ¥CA INFORMACJE (sterowanie)
    while (1) {
        cli->spack.clear();
        s.clear();
        s = "PLAYERS";//paczka z graczem
        cli->spack << s << cli->ID << cli->player.speedMultiple << cli->player.left << cli->player.right
            << cli->player.launchTorpedo << cli->player.barrelUp; //! SEND
        if (cli->player.launchTorpedo) cli->player.launchTorpedo = false; // raz ! czy wystrzelilismy torpede
        if (cli->player.barrelUp) cli->player.barrelUp = false; // raz !
        if (cli->sConn->send(cli->spack) != sf::Socket::Done) { cout << "sync error.\n"; delete cli->sConn; return; }
        sf::sleep(sf::milliseconds(1000 / (FRAME_LIMIT * 0.4)));
    }
}

//! aktualizacja playersów
void Client::unpackPlayers(sf::Packet& packet, bool env) {
    int number;
    packet >> number;
    for (int i = 0; i < number; i++) {
        sf::Uint16 id;
        sf::Int16 hp;
        float x, y, a, ba;
        bool playcoll, gg;
        packet >> id >> x >> y >> a >> hp >> playcoll >> gg >> ba;
        if (id == this->ID) { 
            this->player.setPosition(sf::Vector2f(x, y));
            this->player.setRotation(a);
            this->player.barrel.setRotation(a + ba);
            this->player.barrel.setPosition(x, y);
            this->player.setHp(hp, true);
            this->player.gg = gg; 
            if (playcoll) this->player.sfx.play("collp", 100, false, 1.f, true); 

        }
        else { // nie - gracz
        // dodajê jak nie ma
            if (this->players.find(id) == this->players.end()) this->players.emplace(make_pair(id, new Player()));
            this->players[id]->setPosition(sf::Vector2f(x, y));
            this->players[id]->setRotation(a);
            this->players[id]->barrel.setRotation(a + ba);
            this->players[id]->barrel.setPosition(x, y);
            this->players[id]->setHp(hp, true);
            this->players[id]->gg = gg;
            if (env)  this->players[id]->setFillColor(sf::Color(0xffffffff));
        }
    }
}

//! aktualizacja pocisków dla graczy
void Client::unpackWeaponary(sf::Packet& packet) {
    int number;
    packet >> number;
    for (int i = 0; i < number; i++) {
        sf::Uint16 id;
        sf::Uint16 torid;
        float x, y, a;
        bool lethal, heal, kill;
        packet >> id >> torid >> x >> y  >> a >> lethal >> heal >> kill;
        if (id == this->ID) { 
            if (lethal) {
                this->player.weaponary->addTorpedoNr(&(this->player), torid);
                this->player.weaponary->torpedos[torid]->setPosition(sf::Vector2f(x, y));
                this->player.weaponary->torpedos[torid]->setRotation(a);
                this->player.weaponary->torpedos[torid]->setFillColor(sf::Color(0x64ff64ff)); 
                this->player.weaponary->torpedos[torid]->lethal = lethal;
                this->player.weaponary->torpedos[torid]->heal = heal;
                this->player.weaponary->torpedos[torid]->kill = kill;
            }
            if (!lethal) {
                delete this->player.weaponary->torpedos[torid]; 
                this->player.weaponary->torpedos.erase(torid); 
            }
        }
        else { // nie - gracz
            if (lethal) {
                this->players[id]->weaponary->addTorpedoNr(this->players[id], torid); 
                this->players[id]->weaponary->torpedos[torid]->setPosition(sf::Vector2f(x, y));
                this->players[id]->weaponary->torpedos[torid]->setRotation(a);
                this->players[id]->weaponary->torpedos[torid]->lethal = lethal;
                this->players[id]->weaponary->torpedos[torid]->heal = heal;
                this->players[id]->weaponary->torpedos[torid]->kill = kill;
                if (heal) this->players[id]->weaponary->torpedos[torid]->setFillColor(sf::Color(255, 255, 0, 255)); 
                else  this->players[id]->weaponary->torpedos[torid]->setFillColor(sf::Color(255, 0, rand() % 256, 255)); 
                if (kill) {
                    this->players[id]->weaponary->torpedos[torid]->setOutlineColor(sf::Color(0x00ffffff)); 
                    this->players[id]->weaponary->torpedos[torid]->setOutlineThickness(2); 
                }
            }
            if (!lethal) {
                delete this->players[id]->weaponary->torpedos[torid]; 
                this->players[id]->weaponary->torpedos.erase(torid); 
            }
        }
    }
}

Client::~Client() {
    this->sConn->disconnect();
    this->rConn->disconnect();
    delete this->sConn;
    delete this->rConn;
}

void Client::setSFMLWindow(HWND gw) {
    RECT rect;
    GetWindowRect(gw, &rect);
    SetWindowPos(gw, HWND_TOPMOST, rect.left, rect.top, rect.right, rect.bottom, SWP_NOMOVE);
    SetWindowLong(gw, GWL_EXSTYLE, GetWindowLong(gw, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(gw, 0x000000, 0xff, LWA_ALPHA);
}

