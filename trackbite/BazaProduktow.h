#pragma once
#include <vector>
#include <string>
#include "Produkt.h"

using namespace std;

class BazaProduktow {
private:
    vector<Produkt> listaProduktow;

public:
    void dodajProdukt(const Produkt& p);
    bool usunProdukt(const string& nazwa);
    bool usunProduktPoIndeksie(int indeks);
    vector<Produkt> wyszukajProdukt(const string& fraza) const;
    vector<Produkt> pobierzWszystkie() const;

    void przelaczUlubiony(const string& nazwa);
    vector<Produkt> pobierzUlubione() const;
};