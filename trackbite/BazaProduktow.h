#pragma once

#include "Produkt.h"

#include <string>
#include <vector>

// Przechowuje produkty dostepne w aplikacji i udostepnia operacje na tej kolekcji.
// Ta klasa nie zajmuje sie GUI ani plikami - jest tylko warstwa logiki dla bazy produktow.
class BazaProduktow
{
public:
    // Dodaje produkt do bazy tylko wtedy, gdy jest poprawny i nie ma juz produktu o tej samej nazwie.
    // Porownywanie nazw jest robione bez rozrozniania wielkosci liter w implementacji .cpp.
    void dodajProdukt(const Produkt& produkt);

    // Usuwa produkt po nazwie. Zwraca false, gdy produktu nie znaleziono.
    bool usunProdukt(const std::string& nazwa);

    // Usuwa produkt po indeksie z tabeli/listy. Zwraca false dla indeksu spoza zakresu.
    bool usunProduktPoIndeksie(int indeks);

    // Zwraca produkty, ktorych nazwa pasuje do podanej frazy. Pusta fraza zwraca cala baze.
    std::vector<Produkt> wyszukajProdukt(const std::string& fraza) const;

    // Zwraca kopie wszystkich produktow, np. do wypelnienia tabeli albo zapisu do pliku.
    std::vector<Produkt> pobierzWszystkie() const;

    // Przelacza status ulubionego produktu znalezionego po nazwie.
    void przelaczUlubiony(const std::string& nazwa);

    // Zwraca tylko produkty oznaczone jako ulubione.
    std::vector<Produkt> pobierzUlubione() const;

private:
    // Glowna kolekcja produktow trzymana w pamieci podczas dzialania programu.
    std::vector<Produkt> listaProduktow;
};
