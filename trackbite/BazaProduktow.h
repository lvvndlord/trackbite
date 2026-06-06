#pragma once

#include "Produkt.h"

#include <string>
#include <vector>

using namespace std;

// Klasa pełniąca rolę kontenera na wszystkie zdefiniowane w aplikacji produkty
class BazaProduktow
{
public:
    // Dodaje gotowy i zwalidowany produkt chroniąc przed powtórzeniem nazwy
    void dodajProdukt(const Produkt& produkt);

    // Interfejsy usuwania z kolekcji
    bool usunProdukt(const string& nazwa);
    bool usunProduktPoIndeksie(int indeks);

    // Silnik wyszukiwarki (case-insensitive) do okna wyboru posiłku
    vector<Produkt> wyszukajProdukt(const string& fraza) const;
    vector<Produkt> pobierzWszystkie() const;

    // Manipulowanie flagami ulubionych (gwiazdek) do widoku zakładek
    void przelaczUlubiony(const string& nazwa);
    vector<Produkt> pobierzUlubione() const;

private:
    vector<Produkt> listaProduktow; // Centralny wektor przechowujący bazę w pamięci RAM aplikacji
};