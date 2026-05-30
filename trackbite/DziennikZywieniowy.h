#pragma once

#include "TypyZywieniowe.h"

#include <cstddef>
#include <string>
#include <vector>

enum class PoraPosilku
{
    Sniadanie,
    DrugieSniadanie,
    Obiad,
    Kolacja,
    Przekaski
};

class PozycjaDziennika final : public IMakroObliczalny
{
public:
    PozycjaDziennika(
        const std::string& nazwaProduktu,
        double ilosc,
        const JednostkaProduktu& jednostka,
        const Makroskladniki& makroNa100g,
        PoraPosilku poraPosilku
    );

    const std::string& pobierzNazweProduktu() const;
    double pobierzIlosc() const;
    const JednostkaProduktu& pobierzJednostke() const;
    double pobierzGramy() const;
    const Makroskladniki& pobierzMakroNa100g() const;
    PoraPosilku pobierzPorePosilku() const;

    void zmienIloscIJednostke(
        double nowaIlosc,
        const JednostkaProduktu& nowaJednostka
    );

    Makroskladniki obliczMakro() const override;

private:
    std::string nazwaProduktu;
    double ilosc = 0.0;
    JednostkaProduktu jednostka;
    Makroskladniki makroNa100g;
    PoraPosilku poraPosilku = PoraPosilku::Sniadanie;
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
        double tluszczNa100g,
        PoraPosilku poraPosilku
    );

    bool usunPozycje(std::size_t indeks);
    bool usunPozycjeDlaPory(PoraPosilku pora, std::size_t indeksWPorze);

    WynikOperacji edytujPozycjeDlaPory(
        PoraPosilku pora,
        std::size_t indeksWPorze,
        double ilosc,
        const JednostkaProduktu& jednostka
    );

    void wyczysc();

    const std::vector<PozycjaDziennika>& pobierzPozycje() const;

    std::vector<PozycjaDziennika> pobierzPozycjeDlaPory(PoraPosilku pora) const;

    Makroskladniki obliczSume() const;
    Makroskladniki obliczSumeDlaPory(PoraPosilku pora) const;

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