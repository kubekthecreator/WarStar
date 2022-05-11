#include "Game.hpp"
using namespace std;
sf::Clock Game::clock;

Game::Game() {
    area.setSize(sf::Vector2f(SCREENX - 50, SCREENY - 50));
    area.setPosition(25, 25);
    area.setOutlineColor(sf::Color(0xffff00ff));
    area.setOutlineThickness(2);
    area.setFillColor(sf::Color(0xffffff00));
}
//! SERVER
void Game::runServer() {
    sf::Thread thGame(&Game::runTheGame, this); 
    sf::Thread thconn(&Game::connect, this);
    thconn.launch();
    string opt;
    while (1) {
        cin >> opt;
        if (opt == "k") { // KILL GAME 
            thGame.terminate();
            thconn.terminate();
            break;
        }
        else if (opt == "s") { // START GAME 
            this->status = SEND_ENV;
            while (this->status != RUN) sf::sleep(sf::milliseconds(100)); // czekaj na RUN ...
            // ... a jak RUN to odpal grê
            thGame.launch();

            sf::sleep(sf::seconds(5));
            cout << "Zbity proces oczekiwania na po³¹czenia\n";
            thconn.terminate(); 
        }
    }
}


//! G£ÓWNA GRA OBLICZAJ¥CA WSZYSTKO CO SIÊ DZIEJE itp.
void Game::runTheGame(Game* game) {
    cout << "runTheGame\n";
    Player* pl;
    Player* tarpl;
    sf::Time start = Game::clock.getElapsedTime();
    sf::Time tick = sf::microseconds(0);
    Torpedo* t;
    unsigned int steps = 0;
    sf::Vector2f before;
    while (1) {
        /// PRZEMIESZCZAM GRACZY I ICH OBIEKTY (torpedy)
        for (auto& p : game->players) {
            pl = p.second;
            before = pl->getPosition();
            if (pl->getHp() > 0) { // O ILE ¯YWY
                if (pl->left) {
                    pl->rotate(-(pl->rot));
                }
                else if (pl->right) {
                    pl->rotate(pl->rot);
                }
                pl->move(Game::calcVector(pl->getRotation(), pl->speed * pl->speedMultiple));
                // ODBICIE SIÊ GRACZA OD KRAWÊDZI
                while (!game->area.getGlobalBounds().contains(pl->getPosition())) {
                    pl->setPosition(before);
                    pl->rotate(Game::sign() * (90 + rand() % 90));
                    pl->move(Game::calcVector(pl->getRotation(), pl->speed * pl->speedMultiple));
                    pl->playCollision = true;
                }
                // ODBISIÊ SIÊ GRACZA OD KAMENI
                for (int index = 0; index < STONES; index++) {
                    while (pl->getSmallGlobalBounds(5).contains(game->stones[index].getPosition())) {
                        pl->setPosition(before);
                        pl->rotate(Game::sign() * (90 + rand() % 90));
                        pl->move(Game::calcVector(pl->getRotation(), pl->speed * pl->speedMultiple));
                        pl->playCollision = true;
                    }
                }
            }
            // TORPEDY LEC¥
            if (pl->weaponary->torpedos.size()) {
                for (auto& x : pl->weaponary->torpedos) {
                    t = x.second;
                    before = t->getPosition();
                    t->move(Game::calcVector(t->getRotation(), t->speed));
                    // TORPEDY odbijaj¹ siê równie¿ od œcian !
                    while (!game->area.getGlobalBounds().contains(t->getPosition())) {
                        t->setPosition(before);
                        t->rotate(Game::sign() * (90 + rand() % 90));
                        t->move(Game::calcVector(t->getRotation(), t->speed));
                    }
                    // TEST KOLIZJI TORPEDY z graczami [innymi ni¿ obecny]
                    for (auto& tarp : game->players) {
                        if (pl == tarp.second) continue; 
                        tarpl = tarp.second;
                        if (t->lethal && tarpl->getSmallGlobalBounds(5).contains(t->getPosition())) { /// KOLIZJA Z TORPED¥ !!
                            t->lethal = false;
                            t->speed = 0;
                            if (t->kill) tarpl->setHp((tarpl->getHp() > 2) ? tarpl->getHp() - 3 : 0, false);
                            else if (t->heal && tarpl->getHp() != 0) tarpl->modHp(3, false);
                            else tarpl->setHp((tarpl->getHp() > 0) ? tarpl->getHp() - 1 : 0, false);
                            if (tarpl->getHp() == 0) game->setGG();
                        }
                    }
                    // TEST KOLIZJI TORPEDY z kamieniami
                    for (int index = 0; index < STONES; index++) {
                        if (game->stones[index].getGlobalBounds().contains(t->getPosition())) {
                            t->lethal = false;
                            t->speed = 0;
                        }
                    }
                }
            }
        }
        /// SPRAWDZAM WYSTRZELENIE TORPED (launchTorpedo = true) i ruch luf¹
        for (auto& p : game->players) {
            pl = p.second;
            if (pl->launchTorpedo) {
                pl->weaponary->addTorpedo(pl);
                pl->launchTorpedo = false; 
            }
            if (pl->barrelUp) {
                pl->barrelRotation += pl->barrelStep;
                pl->barrelUp = false; 
            }
        }
        tick = Game::clock.getElapsedTime() - start;//obliczanie ile klatek na s
        if (tick.asMilliseconds() >= 10000) {
            cout << "œrednio: " << steps / 10.f << " k/s\n";
            start = Game::clock.getElapsedTime();
            steps = 0;
        }
        steps++;
        sf::sleep(sf::milliseconds(1000 / (FRAME_LIMIT * 0.6)));
    }
}

//! przypisuje flagê zwyciêzcy, czyli kto wygra³
void Game::setGG() {
    int candidate = 0;
    sf::Uint16 id;
    for (auto& p : this->players) {
        if (p.second->getHp() > 0) { candidate++; id = p.first; }
    }
    if (candidate == 1) this->players[id]->gg = true;
}


//! NAWI¥ZYWANIE PO£¥CZEÑ
void Game::connect(Game* server) {
    sf::TcpListener L;
    if (L.listen(LISTENING_PORT) != sf::Socket::Done) { cout << "Listening error\n"; return; }
    pair<map<sf::Uint16, sf::TcpSocket*>::iterator, bool> rets;
    pair<map<sf::Uint16, Player*>::iterator, bool> retp;
    sf::Packet rec;
    sf::Uint16 id;
    sf::String s;
    while (server->status == WAIT) {
        rec.clear();
        s.clear();
        // PO£¥CZENIE
        sf::TcpSocket* newConn = new sf::TcpSocket;
        if (L.accept(*newConn) != sf::Socket::Done) { cout << "Connection error 2.\n"; delete newConn; newConn = NULL; continue; }
        else {
            cout << "Connection from IP:PORT " << newConn->getRemoteAddress().toString() << ":" << newConn->getRemotePort() << endl;
        }
        // SPAROWANIE ID -> GNIAZDA
        if (newConn->receive(rec) != sf::Socket::Done) { cout << "Connection receive packet error 3.\n"; delete newConn; newConn = NULL; continue; }
        rec >> id >> s;
        if (s.toAnsiString() == "rec") {
            rets = server->rSockets.emplace(make_pair(id, newConn));
            if (rets.second == false) cout << "Nie doda³em w¹tku z gniazdami rSockets, id = " << id << endl;
            else cout << "+ rSockets dla id = " << id << endl;
        }
        else if (s.toAnsiString() == "send") {
            rets = server->sSockets.emplace(make_pair(id, newConn));
            if (rets.second == false) cout << "Nie doda³em w¹tku z gniazdami sSockets, id = " << id << endl;
            else cout << "+ sSockets dla id = " << id << endl;
        }
        else {
            cout << "Klient wys³a³ jakiœ dziwny kod = " << s.toAnsiString() << endl; continue;
        }
        // SPAROWANIE ID -> Player  : je¿eli nie ma gracza o takim ID to dodajê
        retp = server->players.emplace(make_pair(id, new Player()));
        if (retp.second != false) cout << "Create Player ID = " << id << endl;

        // ODPALAM W¥TEK DLA GNIAZDA
        thData* data = new thData;
        data->game = server; 
        data->id = id; 
        data->s = s; 
        cout << data->id << s.toAnsiString() << " server* = " << data->game << endl;
        sf::Thread* thd = new sf::Thread(&Game::createThread, data);
        thd->launch();
        data->game->threads.push_back(thd);
    }
}
//! GNIAZDO-W£ASNY W¥TEK (inny dla wysy³ki, inny dla odbioru)
void Game::createThread(thData* data) {
    sf::Packet pack;
    /// serwer tu wysy³a
    if (data->s.toAnsiString() == "rec") { 
        while (1) {
            /// WYSY£ANIE danych o pozycji itp. ###########  RUN #############
            if (data->game->status == RUN) {
                data->game->getPlayersPacket(pack, false);
                if (data->game->rSockets[data->id]->send(pack) != sf::Socket::Done) { cout << "disconnect\n"; break; }

                data->game->getWeaponaryPacket(pack); // TORPEDY GRACZY
                if (data->game->rSockets[data->id]->send(pack) != sf::Socket::Done) { cout << "disconnect\n"; break; }

                sf::sleep(sf::milliseconds(1000 / (FRAME_LIMIT * 5))); 
            }        
            else if (data->game->status == WAIT) continue;
            /// KONFIGURACJA ŒRODOWISKA - info o graczach i o kamieniach
            else if (data->game->status == SEND_ENV) {
                data->game->getPlayersPacket(pack, true);
                if (data->game->rSockets[data->id]->send(pack) != sf::Socket::Done) { cout << "disconnect\n"; break; }

                data->game->packStonePositions(pack);//poni¿ej
                if (data->game->rSockets[data->id]->send(pack) != sf::Socket::Done) { cout << "disconnect\n"; break; }
              
                data->game->send_env_nr++;
                if (data->game->send_env_nr == data->game->players.size()) data->game->status = RUN;
                cout << "KONFIGURACJA DLA PLAYER = " << data->id << " WYS£ANA\n";
            }
        }
    }
    /// serwer tu odbiera przyciski klawiatury
    else {
        sf::Uint16 id;
        bool left, right, launchT, barrelUp;
        int multi;
        sf::String s;
        Player* p = data->game->players[data->id];
        while (1) {
            if (data->game->status == RUN) {
                pack.clear();//czyœcimy paczkê 
                s.clear();//czyœcimy napis
                if (data->game->sSockets[data->id]->receive(pack) != sf::Socket::Done) { cout << "receive packet error.\n"; break; }//odbieramy paczkê
                pack >> s;
                if (s.toAnsiString() == "PLAYERS") { // PACZKA STEROWANIA GRACZA
                    pack >> id >> multi>> left >> right >> launchT >> barrelUp;

                    p->left = left;
                    p->right = right;
                    p->speedMultiple = multi;
                    p->launchTorpedo = launchT;
                    p->barrelUp = barrelUp;
                }
                else {
                    cout << "Serwer nie wie co odebra³.\n";
                }
                continue;
            }
            sf::sleep(sf::seconds(1)); 
        }
    }
    cout << "Disconnect Client nr = " << data->id << " " << data->s.toAnsiString() << endl;
    delete data;
}


void Game::getPlayersPacket(sf::Packet& pack, bool env) {
    pack.clear();
    string s;
    int number = this->players.size();
    s = (env) ? "PLAYERS_ENV" : "PLAYERS";
    pack << s << number;
    for (auto& p : this->players) {
        Player* pl = p.second;
        pack << p.first << (float)pl->getPosition().x << (float)pl->getPosition().y  << (float)pl->getRotation() << pl->getHp()
            << pl->playCollision << pl->gg << pl->barrelRotation;
        if (pl->playCollision && (++(pl->sendPlayCollision) == this->players.size())) { 
            pl->playCollision = false;
            pl->sendPlayCollision = 0; 
        }
    }
}

void Game::getWeaponaryPacket(sf::Packet& pack) {
    pack.clear();
    Player* pl;
    Torpedo* t;
    string s;
    int number = 0;
    for (auto& p : this->players) {
        pl = p.second;
        number += pl->weaponary->torpedos.size();
    }
    s = "WEAPONARY";
    pack << s << number;
    for (auto& p : this->players) {
        pl = p.second;
        for (auto& tor : pl->weaponary->torpedos) {
            t = tor.second;
            pack << p.first  << tor.first << (float)t->getPosition().x << (float)t->getPosition().y << (float)t->getRotation() << t->lethal << t->heal << t->kill;
        }
    }
    for (auto& p : this->players) {
        pl = p.second;
        for (auto& tor : pl->weaponary->torpedos) {
            t = tor.second;
            if (t->lethal == false) {
                if (++(t->lethalCheck) == this->players.size()) {
                    delete tor.second;
                    pl->weaponary->torpedos.erase(tor.first);
                    break;
                }
            }
        }
    }
}
/// SPAKUJ INFO O POZYCJI KAMIENI, to wysy³a serwer
void Game::packStonePositions(sf::Packet& pack) {
    pack.clear();
    string s = "STONES";
    pack << s;
    for (int i = 0; i < STONES; i++) {
        pack << this->stones[i].getPosition().x << this->stones[i].getPosition().y;
    }
}
//! INFO O KAMIENIACH - ROZPAKUJ, WGRAJ, to u¿ywa nasz klient
void Game::unpackStonePositions(sf::Packet& pack) {
    float x, y;
    for (int i = 0; i < STONES; i++) {
        pack >> x >> y;
        this->stones[i].setPosition(x, y);
    }
}


bool Game::isPointInsidePolygon(vector <sf::Vector2f>& points, const sf::Vector2f& vec) {
    int is = -1;
    int n = points.size();
    float xi, yi, xj, yj, d;
    for (int i = 0; i < n; i++) {
        xi = points[i].x;
        yi = points[i].y;
        xj = (i + 1 == n) ? points[0].x : points[i + 1].x;
        yj = (i + 1 == n) ? points[0].y : points[i + 1].y;
        d = (vec.x - xi) * (yj - yi) - (vec.y - yi) * (xj - xi);
        if (d == 0.0f) continue;
        else {
            if (is == -1) {
                is = d > 0.0;
            }
            else if ((d > 0.0) != is) return false;
        }
    }
    return true;
}


bool Game::isPointInsidePolygon(const sf::Transform& t, vector <sf::Vector2f>& points, const sf::Vector2f& vec) {
    int is = -1;
    int n = points.size();
    float xi, yi, xj, yj, d;
    for (int i = 0; i < n; i++) {
        sf::Vector2f point(t.transformPoint(points[i]));
        xi = point.x;
        yi = point.y;
        if (i + 1 == n) {
            sf::Vector2f point2(t.transformPoint(points[0]));
            xj = point2.x;
            yj = point2.y;
        }
        else {
            sf::Vector2f point2(t.transformPoint(points[i + 1]));
            xj = point2.x;
            yj = point2.y;
        }

        d = (vec.x - xi) * (yj - yi) - (vec.y - yi) * (xj - xi);
        if (d == 0.0f) continue;
        else {
            if (is == -1) {
                is = d > 0.0f;
            }
            else if ((d > 0.0f) != is) return false;
        }
    }
    return true;
}

sf::Vector2f Game::calcVector(float angle, float d) {
    float pices = d - d / sqrt(2);
    int a = ((int)angle) % 90;
    float mod = (a <= 45) ? a / 45 * pices : (a / 45 - 1) * pices;
    int qrt = angle / 90;
    float proportion = a / 90.f;
    float x = 0, y = 0;
    d += mod;
    if (qrt == 0) { x = d * proportion; y = -d * (1 - proportion); } 
    if (qrt == 1) { x = d * (1 - proportion); y = d * proportion; }
    if (qrt == 2) { x = -d * proportion; y = d * (1 - proportion); }
    if (qrt == 3) { x = -d * (1 - proportion); y = -d * proportion; }

    return sf::Vector2f(x, y);
}
//! konsola - konfiguracja
void Game::startConsole() {
    std::stringstream ss;
    ss << "konsola" << time(0);
    std::string s = ss.str();
    std::wstring stemp = std::wstring(s.begin(), s.end());
    LPCWSTR sw = stemp.c_str();
    HWND cw = GetConsoleWindow(); 
    SetWindowText(cw, sw); 
    SetWindowPos(cw, HWND_TOPMOST, 0, 0, 940, 1080, SWP_NOMOVE); 
}

int Game::sign() {
    if (rand() % 2) return 1;
    return -1;
}

Game::~Game() {
    for (auto& p : this->players) {
        delete p.second;
    }
    for (auto& p : this->sSockets) {
        delete p.second;
    }
    for (auto& p : this->rSockets) {
        delete p.second;
    }
    for (auto i : this->threads) delete i;
    this->threads.clear();
}
