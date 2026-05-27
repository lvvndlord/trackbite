#include "ProfilUzytkownika.h"

#include <fstream>
#include <sstream>

const std::string& ProfilUzytkownika::pobierzImie() const { return imie; }
int ProfilUzytkownika::pobierzWiek() const { return wiek; }
double ProfilUzytkownika::pobierzWage() const { return waga; }
double ProfilUzytkownika::pobierzWzrost() const { return wzrost; }
const std::string& ProfilUzytkownika::pobierzCel() const { return cel; }
double ProfilUzytkownika::pobierzLimitKalorii() const { return limitKalorii; }

void ProfilUzytkownika::ustawImie(const std::string& i) { imie = i; }
void ProfilUzytkownika::ustawWiek(int w) { wiek = w; }
void ProfilUzytkownika::ustawWage(double w) { waga = w; }
void ProfilUzytkownika::ustawWzrost(double z) { wzrost = z; }
void ProfilUzytkownika::ustawCel(const std::string& c) { cel = c; }
void ProfilUzytkownika::ustawLimitKalorii(double l) { limitKalorii = l; }

bool ProfilUzytkownika::zapiszDoPliku(const std::string& sciezka) const
{
    std::ofstream out(sciezka);

    if (!out)
    {
        return false;
    }

    out << imie << "\n";
    out << wiek << "\n";
    out << waga << "\n";
    out << wzrost << "\n";
    out << cel << "\n";
    out << limitKalorii << "\n";

    return true;
}

bool ProfilUzytkownika::wczytajZPliku(const std::string& sciezka)
{
    std::ifstream in(sciezka);

    if (!in)
    {
        return false;
    }

    std::string line;

    if (!std::getline(in, imie)) return false;

    if (!std::getline(in, line)) return false;
    wiek = std::stoi(line);

    if (!std::getline(in, line)) return false;
    waga = std::stod(line);

    if (!std::getline(in, line)) return false;
    wzrost = std::stod(line);

    if (!std::getline(in, cel)) return false;

    if (!std::getline(in, line)) return false;
    limitKalorii = std::stod(line);

    return true;
}
