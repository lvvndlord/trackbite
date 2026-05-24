#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include "TypyZywieniowe.h"

// Jedna pozycja w dzienniku, np.:
// "Ser żółty, 2 plastry, 40 g".
class PozycjaDziennika final : public IMakroObliczalny
{
public:
    PozycjaDziennika(
        const std::string& nazwaProduktu,
        double ilosc,
        const JednostkaProduktu& jednostka,
        const Makroskladniki& makroNa100g
    );

    const std::string& pobierzNazweProduktu() const;
    double pobierzIlosc() const;
    const JednostkaProduktu& pobierzJednostke() const;
    double pobierzGramy() const;
    const Makroskladniki& pobierzMakroNa100g() const;

    Makroskladniki obliczMakro() const override;

private:
    std::string nazwaProduktu;
    double ilosc;
    JednostkaProduktu jednostka;
    Makroskladniki makroNa100g;
};

class DziennikZywieniowy
{
public:
    enum class WynikOperacji
    {
        Sukces,
        PustaNazwa,
        PustaJednostka,
        NiepoprawnaIlosc,
        NiepoprawnaJednostka,
        NiepoprawneMakro,
        NiepoprawnyLimit
    };

    DziennikZywieniowy();

    WynikOperacji dodajPozycje(
        const std::string& nazwaProduktu,
        double ilosc,
        const JednostkaProduktu& jednostka,
        double kalorieNa100g,
        double bialkoNa100g,
        double weglowodanyNa100g,
        double tluszczNa100g
    );

    bool usunPozycje(std::size_t indeks);
    void wyczysc();

    const std::vector<PozycjaDziennika>& pobierzPozycje() const;

    Makroskladniki obliczSume() const;

    bool ustawLimitKalorii(double limitKalorii);
    bool ustawLimityDzienne(const Makroskladniki& limity);

    Makroskladniki pobierzLimityDzienne() const;

    double pozostaleKalorie() const;

    int procentKalorii() const;
    int procentBialka() const;
    int procentWeglowodanow() const;
    int procentTluszczu() const;

private:
    Makroskladniki limityDzienne;
    std::vector<PozycjaDziennika> pozycje;

    WynikOperacji walidujPozycje(
        const std::string& nazwaProduktu,
        double ilosc,
        const JednostkaProduktu& jednostka,
        const Makroskladniki& makroNa100g
    ) const;

    int procentWartosci(double wartosc, double limit) const;
};