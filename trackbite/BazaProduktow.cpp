#include "BazaProduktow.h"
#include <algorithm>
#include <cctype>

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

    string frazaMale = fraza;
    transform(frazaMale.begin(), frazaMale.end(), frazaMale.begin(), ::tolower);

    for (const auto& p : listaProduktow) {
        string nazwaMale = p.pobierzNazwe();
        transform(nazwaMale.begin(), nazwaMale.end(), nazwaMale.begin(), ::tolower);

        if (nazwaMale.find(frazaMale) != string::npos) {
            wyniki.push_back(p);
        }
    }
    return wyniki;
}

vector<Produkt> BazaProduktow::pobierzWszystkie() const {
    return listaProduktow;
}

void BazaProduktow::przelaczUlubiony(const string& nazwa) {
    for (auto& p : listaProduktow) {
        if (p.pobierzNazwe() == nazwa) {
            p.ustawUlubiony(!p.czyUlubiony());
            break;
        }
    }
}

vector<Produkt> BazaProduktow::pobierzUlubione() const {
    vector<Produkt> ulubione;
    for (const auto& p : listaProduktow) {
        if (p.czyUlubiony()) {
            ulubione.push_back(p);
        }
    }
    return ulubione;
}