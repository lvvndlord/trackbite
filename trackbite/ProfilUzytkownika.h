#pragma once

#include <string>

class ProfilUzytkownika
{
public:
    ProfilUzytkownika() = default;

    const std::string& pobierzImie() const;
    int pobierzWiek() const;
    double pobierzWage() const;
    double pobierzWzrost() const;
    const std::string& pobierzCel() const;
    double pobierzLimitKalorii() const;

    void ustawImie(const std::string& imie);
    void ustawWiek(int wiek);
    void ustawWage(double waga);
    void ustawWzrost(double wzrost);
    void ustawCel(const std::string& cel);
    void ustawLimitKalorii(double limit);

    bool zapiszDoPliku(const std::string& sciezka) const;
    bool wczytajZPliku(const std::string& sciezka);

private:
    std::string imie;
    int wiek = 0;
    double waga = 0.0;
    double wzrost = 0.0;
    std::string cel;
    double limitKalorii = 2000.0;
};