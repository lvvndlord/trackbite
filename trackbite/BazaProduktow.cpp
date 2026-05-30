#include "BazaProduktow.h"

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

namespace
{
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

void BazaProduktow::dodajProdukt(const Produkt& produkt)
{
    if (!produkt.czyPoprawny())
    {
        return;
    }

    const std::string nazwaNowegoProduktu = naMaleLitery(produkt.pobierzNazwe());

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

    if (indeksJakoSizeT >= listaProduktow.size())
    {
        return false;
    }

    listaProduktow.erase(listaProduktow.begin() + static_cast<std::ptrdiff_t>(indeksJakoSizeT));
    return true;
}

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

        if (nazwaMale.find(frazaMale) != std::string::npos)
        {
            wyniki.push_back(produkt);
        }
    }

    return wyniki;
}

std::vector<Produkt> BazaProduktow::pobierzWszystkie() const
{
    return listaProduktow;
}

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