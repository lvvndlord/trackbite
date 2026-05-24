#include "Produkt.h"

#include <algorithm>
#include <cmath>

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

bool Produkt::dodajJednostke(const std::string& nazwaJednostki, double gramyNaJednostke)
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

bool Produkt::usunJednostke(const std::string& nazwaJednostki)
{
    // Jednostki "g" nie usuwamy, bo każdy produkt musi dać się dodać w gramach.
    if (nazwaJednostki == "g")
    {
        return false;
    }

    const auto iterator = std::remove_if(
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

    jednostki.erase(iterator, jednostki.end());
    return true;
}

const JednostkaProduktu* Produkt::znajdzJednostke(const std::string& nazwaJednostki) const
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

bool Produkt::czyPoprawny() const
{
    return czyNazwaPoprawna(nazwa)
        && czyMakroPoprawne(makroNa100g)
        && !jednostki.empty()
        && czyJednostkaIstnieje("g");
}

bool Produkt::czyNazwaPoprawna(const std::string& tekst) const
{
    return tekst.find_first_not_of(" \t\n\r") != std::string::npos;
}

bool Produkt::czyMakroPoprawne(const Makroskladniki& makro) const
{
    return std::isfinite(makro.kalorie)
        && std::isfinite(makro.bialko)
        && std::isfinite(makro.weglowodany)
        && std::isfinite(makro.tluszcz)
        && makro.kalorie >= 0.0
        && makro.kalorie <= 1000.0
        && makro.bialko >= 0.0
        && makro.bialko <= 100.0
        && makro.weglowodany >= 0.0
        && makro.weglowodany <= 100.0
        && makro.tluszcz >= 0.0
        && makro.tluszcz <= 100.0;
}

bool Produkt::czyJednostkaPoprawna(const std::string& nazwaJednostki, double gramyNaJednostke) const
{
    return czyNazwaPoprawna(nazwaJednostki)
        && std::isfinite(gramyNaJednostke)
        && gramyNaJednostke > 0.0
        && gramyNaJednostke <= 5000.0;
}

bool Produkt::czyJednostkaIstnieje(const std::string& nazwaJednostki) const
{
    return znajdzJednostke(nazwaJednostki) != nullptr;
}

void Produkt::dodajDomyslnaJednostkeGram()
{
    if (!czyJednostkaIstnieje("g"))
    {
        jednostki.push_back({ "g", 1.0 });
    }
}