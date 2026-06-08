// Implementacja bazy produktów trzymanej w pamięci programu.
// Klasa pilnuje braku duplikatów, wyszukiwania oraz listy ulubionych.
#include "BazaProduktow.h"

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

namespace
{
    // Normalizacja tekstu do porównań: dzięki temu „Jabłko” i „jabłko” są traktowane jak ta sama nazwa.
    std::string naMaleLitery(const std::string& tekst)
    {
        std::string wynik = tekst;

        std::transform(
            wynik.begin(),
            wynik.end(),
            wynik.begin(),
            [](unsigned char znak)
            {
                return static_cast<char>(std::tolower(znak));
            }
        );

        return wynik;
    }
}

// Dodawanie produktu jest defensywne: najpierw walidacja obiektu, potem blokada duplikatu po nazwie.
void BazaProduktow::dodajProdukt(const Produkt& produkt)
{
    if (!produkt.czyPoprawny())
    {
        return;
    }

    const std::string nazwaNowegoProduktu = naMaleLitery(produkt.pobierzNazwe());

    // Szukamy produktu o tej samej nazwie niezależnie od wielkości liter, żeby nie robić duplikatów w tabeli.
    const auto znaleziony = std::find_if(
        listaProduktow.begin(),
        listaProduktow.end(),
        [&nazwaNowegoProduktu](const Produkt& obecnyProdukt)
        {
            return naMaleLitery(obecnyProdukt.pobierzNazwe()) == nazwaNowegoProduktu;
        }
    );

    if (znaleziony != listaProduktow.end())
    {
        return;
    }

    listaProduktow.push_back(produkt);
}

// Usuwanie po nazwie jest używane głównie z UI, gdzie użytkownik widzi nazwę produktu w tabeli.
bool BazaProduktow::usunProdukt(const std::string& nazwa)
{
    const std::string szukanaNazwa = naMaleLitery(nazwa);

    const auto znaleziony = std::find_if(
        listaProduktow.begin(),
        listaProduktow.end(),
        [&szukanaNazwa](const Produkt& produkt)
        {
            return naMaleLitery(produkt.pobierzNazwe()) == szukanaNazwa;
        }
    );

    if (znaleziony == listaProduktow.end())
    {
        return false;
    }

    listaProduktow.erase(znaleziony);
    return true;
}

bool BazaProduktow::usunProduktPoIndeksie(int indeks)
{
    if (indeks < 0)
    {
        return false;
    }

    const std::size_t indeksJakoSizeT = static_cast<std::size_t>(indeks);

    // Indeks z widoku może być błędny, więc przed erase koniecznie sprawdzamy zakres wektora.
    if (indeksJakoSizeT >= listaProduktow.size())
    {
        return false;
    }

    listaProduktow.erase(listaProduktow.begin() + static_cast<std::ptrdiff_t>(indeksJakoSizeT));
    return true;
}

// Filtrowanie do wyszukiwarki „na żywo”: pusta fraza zwraca całą bazę, wpisany tekst zawęża wyniki.
std::vector<Produkt> BazaProduktow::wyszukajProdukt(const std::string& fraza) const
{
    if (fraza.empty())
    {
        return listaProduktow;
    }

    std::vector<Produkt> wyniki;
    const std::string frazaMale = naMaleLitery(fraza);

    for (const Produkt& produkt : listaProduktow)
    {
        const std::string nazwaMale = naMaleLitery(produkt.pobierzNazwe());

        // find != npos oznacza, że wpisana fraza występuje gdzieś w nazwie produktu.
        if (nazwaMale.find(frazaMale) != std::string::npos)
        {
            wyniki.push_back(produkt);
        }
    }

    return wyniki;
}

// Zwracamy kopię listy, żeby zewnętrzny kod nie mógł przypadkiem zmienić wektora bez kontroli klasy.
std::vector<Produkt> BazaProduktow::pobierzWszystkie() const
{
    return listaProduktow;
}

// Przełącza gwiazdkę produktu. Jeśli nazwa nie istnieje, metoda nic nie psuje i po prostu kończy działanie.
void BazaProduktow::przelaczUlubiony(const std::string& nazwa)
{
    const std::string szukanaNazwa = naMaleLitery(nazwa);

    const auto znaleziony = std::find_if(
        listaProduktow.begin(),
        listaProduktow.end(),
        [&szukanaNazwa](const Produkt& produkt)
        {
            return naMaleLitery(produkt.pobierzNazwe()) == szukanaNazwa;
        }
    );

    if (znaleziony != listaProduktow.end())
    {
        znaleziony->ustawUlubiony(!znaleziony->czyUlubiony());
    }
}

// Buduje osobną listę tylko z produktami oznaczonymi gwiazdką, potrzebną do zakładki „Ulubione”.
std::vector<Produkt> BazaProduktow::pobierzUlubione() const
{
    std::vector<Produkt> ulubione;

    for (const Produkt& produkt : listaProduktow)
    {
        if (produkt.czyUlubiony())
        {
            ulubione.push_back(produkt);
        }
    }

    return ulubione;
}