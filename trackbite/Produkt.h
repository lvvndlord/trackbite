#pragma once

#include "TypyZywieniowe.h"

#include <string>
#include <vector>

// Reprezentuje jeden produkt spozywczy w bazie aplikacji.
// Produkt przechowuje nazwe, makro dla 100 g, liste dostepnych jednostek oraz flage ulubionego.
class Produkt
{
public:
    // Tworzy pusty produkt, ale od razu dodaje domyslna jednostke "g".
    Produkt();

    // Tworzy produkt z nazwa i makro dla 100 g. Jednostka "g" nadal jest dodawana automatycznie.
    Produkt(
        const std::string& nazwa,
        const Makroskladniki& makroNa100g
    );

    // Podstawowe metody odczytu danych produktu uzywane przez tabele, zapis do pliku i dziennik.
    const std::string& pobierzNazwe() const;
    const Makroskladniki& pobierzMakroNa100g() const;
    const std::vector<JednostkaProduktu>& pobierzJednostki() const;

    // Zmienia dane produktu tylko wtedy, gdy przejda walidacje.
    bool ustawNazwe(const std::string& nowaNazwa);
    bool ustawMakroNa100g(const Makroskladniki& noweMakro);

    // Dodaje wlasna jednostke produktu, np. "sztuka = 55 g" albo "opakowanie = 500 g".
    // Zwraca false, gdy nazwa jest pusta, masa jest zla albo jednostka juz istnieje.
    bool dodajJednostke(const std::string& nazwaJednostki, double gramyNaJednostke);

    // Usuwa jednostke po nazwie. Jednostka "g" jest chroniona, bo cala matematyka opiera sie na gramach.
    bool usunJednostke(const std::string& nazwaJednostki);

    // Szuka jednostki po nazwie i zwraca wskaznik do elementu w wektorze.
    // nullptr oznacza, ze produkt nie ma takiej jednostki.
    const JednostkaProduktu* znajdzJednostke(const std::string& nazwaJednostki) const;

    // Sprawdza, czy produkt ma sensowne dane i mozna go bezpiecznie pokazac albo zapisac.
    bool czyPoprawny() const;

    // Obsluga oznaczenia produktu jako ulubionego w tabeli produktow.
    bool czyUlubiony() const;
    void ustawUlubiony(bool stan);

private:
    // Nazwa produktu widoczna w bazie i w dzienniku.
    std::string nazwa;

    // Wartosc odzywcza zawsze zapisana dla 100 g, zeby latwo przeliczac dowolna porcje.
    Makroskladniki makroNa100g;

    // Lista jednostek dostepnych dla produktu. Powinna zawsze zawierac "g".
    std::vector<JednostkaProduktu> jednostki;

    // Flaga zapamietywana w pliku, uzywana do budowania zakladki ulubionych.
    bool ulubiony = false;

    // Wewnetrzne metody walidacyjne. Sa prywatne, bo reszta programu ma korzystac z bezpiecznych setterow.
    bool czyNazwaPoprawna(const std::string& tekst) const;
    bool czyMakroPoprawne(const Makroskladniki& makro) const;
    bool czyJednostkaPoprawna(const std::string& nazwaJednostki, double gramyNaJednostke) const;
    bool czyJednostkaIstnieje(const std::string& nazwaJednostki) const;

    // Pilnuje, zeby kazdy produkt mial bazowa jednostke "g" o masie 1.0.
    void dodajDomyslnaJednostkeGram();
};
