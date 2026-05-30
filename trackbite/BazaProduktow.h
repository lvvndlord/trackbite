#pragma once

#include "Produkt.h"

#include <string>
#include <vector>

class BazaProduktow
{
public:
    void dodajProdukt(const Produkt& produkt);

    bool usunProdukt(const std::string& nazwa);
    bool usunProduktPoIndeksie(int indeks);

    std::vector<Produkt> wyszukajProdukt(const std::string& fraza) const;
    std::vector<Produkt> pobierzWszystkie() const;

    void przelaczUlubiony(const std::string& nazwa);
    std::vector<Produkt> pobierzUlubione() const;

private:
    std::vector<Produkt> listaProduktow;
};