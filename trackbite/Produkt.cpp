// Implementacja pojedynczego produktu spożywczego: nazwa, makro na 100 g,
// dodatkowe jednostki oraz walidacja danych wpisywanych przez użytkownika.
#include "Produkt.h"

#include <algorithm>
#include <cmath>

namespace
{
    // Sprawdza tekst po ludzku: sam ciąg spacji nie powinien przejść jako nazwa produktu.
    bool czyTekstNiepusty(const std::string& tekst)
    {
        return tekst.find_first_not_of(" \t\n\r") != std::string::npos;
    }

    // Wspólna walidacja liczb: odrzuca NaN/nieskończoność oraz wartości spoza ustalonego zakresu.
    bool czyLiczbaWZakresie(double wartosc, double minimum, double maksimum)
    {
        return std::isfinite(wartosc)
            && wartosc >= minimum
            && wartosc <= maksimum;
    }
}

// Konstruktor domyślny od razu dodaje gramy, żeby produkt zawsze miał jednostkę bazową do przeliczeń.
Produkt::Produkt()
{
    dodajDomyslnaJednostkeGram();
}

Produkt::Produkt(
    const std::string& nazwa,
    const Makroskladniki& makroNa100g
)
    : nazwa(nazwa),
    makroNa100g(makroNa100g)
{
    dodajDomyslnaJednostkeGram();
}

// Proste metody odczytu zwracają dane bez kopiowania większych struktur.
const std::string& Produkt::pobierzNazwe() const
{
    return nazwa;
}

const Makroskladniki& Produkt::pobierzMakroNa100g() const
{
    return makroNa100g;
}

const std::vector<JednostkaProduktu>& Produkt::pobierzJednostki() const
{
    return jednostki;
}

// Zmiana danych przechodzi przez walidację, dlatego nie da się łatwo zostawić obiektu w niepoprawnym stanie.
bool Produkt::ustawNazwe(const std::string& nowaNazwa)
{
    if (!czyNazwaPoprawna(nowaNazwa))
    {
        return false;
    }

    nazwa = nowaNazwa;
    return true;
}

bool Produkt::ustawMakroNa100g(const Makroskladniki& noweMakro)
{
    if (!czyMakroPoprawne(noweMakro))
    {
        return false;
    }

    makroNa100g = noweMakro;
    return true;
}

// Dodaje własną jednostkę produktu, np. „sztuka” albo „opakowanie”, razem z wagą w gramach.
bool Produkt::dodajJednostke(
    const std::string& nazwaJednostki,
    double gramyNaJednostke
)
{
    if (!czyJednostkaPoprawna(nazwaJednostki, gramyNaJednostke))
    {
        return false;
    }

    if (czyJednostkaIstnieje(nazwaJednostki))
    {
        return false;
    }

    jednostki.push_back({ nazwaJednostki, gramyNaJednostke });
    return true;
}

// Usuwa dodatkową jednostkę, ale nie pozwala skasować bazowych gramów potrzebnych do obliczeń.
bool Produkt::usunJednostke(const std::string& nazwaJednostki)
{
    // Rdzeń kalkulacji zależy od "g", nie możemy tego fizycznie usunąć
    if (nazwaJednostki == "g")
    {
        return false;
    }

    const auto iterator = std::find_if(
        jednostki.begin(),
        jednostki.end(),
        [&nazwaJednostki](const JednostkaProduktu& jednostka)
        {
            return jednostka.nazwa == nazwaJednostki;
        }
    );

    if (iterator == jednostki.end())
    {
        return false;
    }

    jednostki.erase(iterator);
    return true;
}

// Zwraca wskaźnik na jednostkę z wektora albo nullptr, jeśli taka miara nie istnieje.
const JednostkaProduktu* Produkt::znajdzJednostke(
    const std::string& nazwaJednostki
) const
{
    const auto iterator = std::find_if(
        jednostki.begin(),
        jednostki.end(),
        [&nazwaJednostki](const JednostkaProduktu& jednostka)
        {
            return jednostka.nazwa == nazwaJednostki;
        }
    );

    if (iterator == jednostki.end())
    {
        return nullptr;
    }

    return &(*iterator);
}

// Kontrola końcowa obiektu przed zapisem do bazy lub pliku. Produkt musi mieć nazwę, poprawne makro i jednostkę „g”.
bool Produkt::czyPoprawny() const
{
    return czyNazwaPoprawna(nazwa)
        && czyMakroPoprawne(makroNa100g)
        && !jednostki.empty()
        && czyJednostkaIstnieje("g");
}

// Flaga ulubionych jest prostym stanem używanym później przez tabele w interfejsie.
bool Produkt::czyUlubiony() const
{
    return ulubiony;
}

void Produkt::ustawUlubiony(bool stan)
{
    ulubiony = stan;
}

// Prywatne walidatory trzymają zasady poprawności w jednym miejscu, zamiast powielać je w UI.
bool Produkt::czyNazwaPoprawna(const std::string& tekst) const
{
    return czyTekstNiepusty(tekst);
}

bool Produkt::czyMakroPoprawne(const Makroskladniki& makro) const
{
    // Górne limity są celowo szerokie, ale chronią program przed kompletnie absurdalnymi danymi.
    return czyLiczbaWZakresie(makro.kalorie, 0.0, 10000.0)
        && czyLiczbaWZakresie(makro.bialko, 0.0, 1000.0)
        && czyLiczbaWZakresie(makro.weglowodany, 0.0, 1000.0)
        && czyLiczbaWZakresie(makro.tluszcz, 0.0, 1000.0);
}

bool Produkt::czyJednostkaPoprawna(
    const std::string& nazwaJednostki,
    double gramyNaJednostke
) const
{
    return czyNazwaPoprawna(nazwaJednostki)
        && czyLiczbaWZakresie(gramyNaJednostke, 0.01, 5000.0);
}

bool Produkt::czyJednostkaIstnieje(const std::string& nazwaJednostki) const
{
    return znajdzJednostke(nazwaJednostki) != nullptr;
}

// Jednostka „g” jest fundamentem całej matematyki makro, dlatego konstruktorzy wymuszają jej istnienie.
void Produkt::dodajDomyslnaJednostkeGram()
{
    if (!czyJednostkaIstnieje("g"))
    {
        jednostki.push_back({ "g", 1.0 });
    }
}