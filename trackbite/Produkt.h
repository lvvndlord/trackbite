#pragma once

#include "TypyZywieniowe.h"
#include <string>
#include <vector>

using namespace std; // Zgodnie z poleceniem!

class Produkt
{
public:
    Produkt();

    Produkt(
        const string& nazwa,
        const Makroskladniki& makroNa100g
    );

    const string& pobierzNazwe() const;
    const Makroskladniki& pobierzMakroNa100g() const;
    const vector<JednostkaProduktu>& pobierzJednostki() const;

    bool ustawNazwe(const string& nowaNazwa);
    bool ustawMakroNa100g(const Makroskladniki& noweMakro);

    bool dodajJednostke(const string& nazwaJednostki, double gramyNaJednostke);
    bool usunJednostke(const string& nazwaJednostki);

    const JednostkaProduktu* znajdzJednostke(const string& nazwaJednostki) const;

    bool czyPoprawny() const;

private:
    string nazwa;
    Makroskladniki makroNa100g;
    vector<JednostkaProduktu> jednostki;

    bool czyNazwaPoprawna(const string& tekst) const;
    bool czyMakroPoprawne(const Makroskladniki& makro) const;
    bool czyJednostkaPoprawna(const string& nazwaJednostki, double gramyNaJednostke) const;
    bool czyJednostkaIstnieje(const string& nazwaJednostki) const;

    void dodajDomyslnaJednostkeGram();
};