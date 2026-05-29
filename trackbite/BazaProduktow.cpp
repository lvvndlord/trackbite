#include "BazaProduktow.h"
#include <algorithm> // Do użycia funkcji transform
#include <cctype>    // Do użycia funkcji tolower

using namespace std;

void BazaProduktow::dodajProdukt(const Produkt& p) {
    listaProduktow.push_back(p);
}

bool BazaProduktow::usunProdukt(const string& nazwa) {
    for (auto it = listaProduktow.begin(); it != listaProduktow.end(); ++it) {
        if (it->pobierzNazwe() == nazwa) {
            listaProduktow.erase(it);
            return true;
        }
    }
    return false;
}

bool BazaProduktow::usunProduktPoIndeksie(int indeks) {
    if (indeks >= 0 && indeks < listaProduktow.size()) {
        listaProduktow.erase(listaProduktow.begin() + indeks);
        return true;
    }
    return false;
}

vector<Produkt> BazaProduktow::wyszukajProdukt(const string& fraza) const {
    vector<Produkt> wyniki;

    // 1. Zamieniamy wpisaną przez użytkownika frazę na małe litery
    string frazaMale = fraza;
    transform(frazaMale.begin(), frazaMale.end(), frazaMale.begin(), ::tolower);

    for (const auto& p : listaProduktow) {
        // 2. Pobieramy nazwę aktualnie sprawdzanego produktu i też zamieniamy na małe litery
        string nazwaMale = p.pobierzNazwe();
        transform(nazwaMale.begin(), nazwaMale.end(), nazwaMale.begin(), ::tolower);

        // 3. Sprawdzamy, czy fraza (z małych liter) znajduje się w nazwie (z małych liter)
        if (nazwaMale.find(frazaMale) != string::npos) {
            wyniki.push_back(p);
        }
    }
    return wyniki;
}

vector<Produkt> BazaProduktow::pobierzWszystkie() const {
    return listaProduktow;
}