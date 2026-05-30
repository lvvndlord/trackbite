#include "Produkt.h"
#include <algorithm>
#include <cmath>

using namespace std;

Produkt::Produkt()
{
    dodajDomyslnaJednostkeGram();
    ulubiony = false;
}

Produkt::Produkt(
    const string& nazwa,
    const Makroskladniki& makroNa100g
)
    : nazwa(nazwa),
    makroNa100g(makroNa100g),
    ulubiony(false)
{
    dodajDomyslnaJednostkeGram();
}

const string& Produkt::pobierzNazwe() const { return nazwa; }
const Makroskladniki& Produkt::pobierzMakroNa100g() const { return makroNa100g; }
const vector<JednostkaProduktu>& Produkt::pobierzJednostki() const { return jednostki; }

bool Produkt::ustawNazwe(const string& nowaNazwa) {
    if (!czyNazwaPoprawna(nowaNazwa)) return false;
    nazwa = nowaNazwa;
    return true;
}

bool Produkt::ustawMakroNa100g(const Makroskladniki& noweMakro) {
    if (!czyMakroPoprawne(noweMakro)) return false;
    makroNa100g = noweMakro;
    return true;
}

bool Produkt::dodajJednostke(const string& nazwaJednostki, double gramyNaJednostke) {
    if (!czyJednostkaPoprawna(nazwaJednostki, gramyNaJednostke)) return false;
    if (czyJednostkaIstnieje(nazwaJednostki)) return false;
    jednostki.push_back({ nazwaJednostki, gramyNaJednostke });
    return true;
}

bool Produkt::usunJednostke(const string& nazwaJednostki) {
    if (nazwaJednostki == "g") return false;
    const auto iterator = remove_if(jednostki.begin(), jednostki.end(),
        [&nazwaJednostki](const JednostkaProduktu& jednostka) { return jednostka.nazwa == nazwaJednostki; });
    if (iterator == jednostki.end()) return false;
    jednostki.erase(iterator, jednostki.end());
    return true;
}

const JednostkaProduktu* Produkt::znajdzJednostke(const string& nazwaJednostki) const {
    const auto iterator = find_if(jednostki.begin(), jednostki.end(),
        [&nazwaJednostki](const JednostkaProduktu& jednostka) { return jednostka.nazwa == nazwaJednostki; });
    if (iterator == jednostki.end()) return nullptr;
    return &(*iterator);
}

bool Produkt::czyPoprawny() const {
    return czyNazwaPoprawna(nazwa) && czyMakroPoprawne(makroNa100g) && !jednostki.empty() && czyJednostkaIstnieje("g");
}

bool Produkt::czyNazwaPoprawna(const string& tekst) const { return tekst.find_first_not_of(" \t\n\r") != string::npos; }

bool Produkt::czyMakroPoprawne(const Makroskladniki& makro) const {
    // ZMIANA: Zwiększono limity wartości makroskładników i kalorii do 10000, 
    // żeby można było dodawać potężne produkty lub całe dania!
    return isfinite(makro.kalorie) && isfinite(makro.bialko) && isfinite(makro.weglowodany) && isfinite(makro.tluszcz) &&
        makro.kalorie >= 0.0 && makro.kalorie <= 10000.0 && makro.bialko >= 0.0 && makro.bialko <= 10000.0 &&
        makro.weglowodany >= 0.0 && makro.weglowodany <= 10000.0 && makro.tluszcz >= 0.0 && makro.tluszcz <= 10000.0;
}

bool Produkt::czyJednostkaPoprawna(const string& nazwaJednostki, double gramyNaJednostke) const {
    return czyNazwaPoprawna(nazwaJednostki) && isfinite(gramyNaJednostke) && gramyNaJednostke > 0.0 && gramyNaJednostke <= 5000.0;
}

bool Produkt::czyJednostkaIstnieje(const string& nazwaJednostki) const { return znajdzJednostke(nazwaJednostki) != nullptr; }
void Produkt::dodajDomyslnaJednostkeGram() { if (!czyJednostkaIstnieje("g")) jednostki.push_back({ "g", 1.0 }); }

bool Produkt::czyUlubiony() const { return ulubiony; }
void Produkt::ustawUlubiony(bool stan) { ulubiony = stan; }