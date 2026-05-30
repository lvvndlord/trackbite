#pragma once

#include "TypyZywieniowe.h"

#include <string>
#include <vector>

class Produkt
{
public:
    Produkt();

    Produkt(
        const std::string& nazwa,
        const Makroskladniki& makroNa100g
    );

    const std::string& pobierzNazwe() const;
    const Makroskladniki& pobierzMakroNa100g() const;
    const std::vector<JednostkaProduktu>& pobierzJednostki() const;

    bool ustawNazwe(const std::string& nowaNazwa);
    bool ustawMakroNa100g(const Makroskladniki& noweMakro);

    bool dodajJednostke(const std::string& nazwaJednostki, double gramyNaJednostke);
    bool usunJednostke(const std::string& nazwaJednostki);

    const JednostkaProduktu* znajdzJednostke(const std::string& nazwaJednostki) const;

    bool czyPoprawny() const;

    bool czyUlubiony() const;
    void ustawUlubiony(bool stan);

private:
    std::string nazwa;
    Makroskladniki makroNa100g;
    std::vector<JednostkaProduktu> jednostki;
    bool ulubiony = false;

    bool czyNazwaPoprawna(const std::string& tekst) const;
    bool czyMakroPoprawne(const Makroskladniki& makro) const;
    bool czyJednostkaPoprawna(const std::string& nazwaJednostki, double gramyNaJednostke) const;
    bool czyJednostkaIstnieje(const std::string& nazwaJednostki) const;

    void dodajDomyslnaJednostkeGram();
};