#include "BazaProduktow.h"

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

using namespace std;

namespace
{
    // Helper zamieniający cały tekst na małe litery - uodparnia bazę na błędy typu "Jabłko" = "jabłko"
    string naMaleLitery(const string& tekst)
    {
        string wynik = tekst;
        transform(
            wynik.begin(),
            wynik.end(),
            wynik.begin(),
            [](unsigned char znak) { return static_cast<char>(tolower(znak)); }
        );
        return wynik;
    }
}

void BazaProduktow::dodajProdukt(const Produkt& produkt)
{
    if (!produkt.czyPoprawny()) { return; }

    const string nazwaNowegoProduktu = naMaleLitery(produkt.pobierzNazwe());

    // Przeszukanie bazy by zabezpieczyć się przed dublowaniem się produktów i awariami systemu
    const auto znaleziony = find_if(
        listaProduktow.begin(),
        listaProduktow.end(),
        [&nazwaNowegoProduktu](const Produkt& obecnyProdukt) {
            return naMaleLitery(obecnyProdukt.pobierzNazwe()) == nazwaNowegoProduktu;
        }
    );

    if (znaleziony != listaProduktow.end()) { return; }

    listaProduktow.push_back(produkt);
}

bool BazaProduktow::usunProdukt(const string& nazwa)
{
    const string szukanaNazwa = naMaleLitery(nazwa);

    const auto znaleziony = find_if(
        listaProduktow.begin(),
        listaProduktow.end(),
        [&szukanaNazwa](const Produkt& produkt) {
            return naMaleLitery(produkt.pobierzNazwe()) == szukanaNazwa;
        }
    );

    if (znaleziony == listaProduktow.end()) { return false; } // Nic nie znaleziono

    listaProduktow.erase(znaleziony);
    return true;
}

bool BazaProduktow::usunProduktPoIndeksie(int indeks)
{
    if (indeks < 0) { return false; }

    const size_t indeksJakoSizeT = static_cast<size_t>(indeks);

    // Ochrona przed usunięciem wiersza, który wykracza poza rozmiar bazy
    if (indeksJakoSizeT >= listaProduktow.size()) { return false; }

    listaProduktow.erase(listaProduktow.begin() + static_cast<ptrdiff_t>(indeksJakoSizeT));
    return true;
}

// Algorytm filtrujący dla "Live Searcha"
vector<Produkt> BazaProduktow::wyszukajProdukt(const string& fraza) const
{
    if (fraza.empty()) { return listaProduktow; } // Puste pole wrzuca wszystko

    vector<Produkt> wyniki;
    const string frazaMale = naMaleLitery(fraza);

    for (const Produkt& produkt : listaProduktow)
    {
        const string nazwaMale = naMaleLitery(produkt.pobierzNazwe());

        // Metoda 'find' zwraca string::npos jeśli NIE odnajdzie powiązania. Różne od 'npos' znaczy, że wpisaliśmy trafnie.
        if (nazwaMale.find(frazaMale) != string::npos)
        {
            wyniki.push_back(produkt);
        }
    }

    return wyniki;
}

vector<Produkt> BazaProduktow::pobierzWszystkie() const { return listaProduktow; }

// Wyszukuje dany produkt i zamienia pole "ulubiony" na przeciwny status
void BazaProduktow::przelaczUlubiony(const string& nazwa)
{
    const string szukanaNazwa = naMaleLitery(nazwa);

    const auto znaleziony = find_if(
        listaProduktow.begin(),
        listaProduktow.end(),
        [&szukanaNazwa](const Produkt& produkt) {
            return naMaleLitery(produkt.pobierzNazwe()) == szukanaNazwa;
        }
    );

    if (znaleziony != listaProduktow.end())
    {
        znaleziony->ustawUlubiony(!znaleziony->czyUlubiony());
    }
}

// Metoda widoku wykorzystywana do wypełniania specjalnej zakładki "Ulubione"
vector<Produkt> BazaProduktow::pobierzUlubione() const
{
    vector<Produkt> ulubione;
    for (const Produkt& produkt : listaProduktow)
    {
        if (produkt.czyUlubiony()) { ulubione.push_back(produkt); }
    }
    return ulubione;
}