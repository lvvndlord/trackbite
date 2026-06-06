#include "Produkt.h"

#include <algorithm>
#include <cmath>

using namespace std;

namespace
{
    // Upewnia się, że nazwa nie składa się z samych spacji lub pustych znaków
    bool czyTekstNiepusty(const string& tekst)
    {
        return tekst.find_first_not_of(" \t\n\r") != string::npos;
    }

    // Waliduje, czy rzutowana wartość liczbowa mieści się w akceptowalnym matematycznie przedziale
    bool czyLiczbaWZakresie(double wartosc, double minimum, double maksimum)
    {
        return isfinite(wartosc) && wartosc >= minimum && wartosc <= maksimum;
    }
}

Produkt::Produkt()
{
    dodajDomyslnaJednostkeGram();
}

Produkt::Produkt(
    const string& nazwa,
    const Makroskladniki& makroNa100g
)
    : nazwa(nazwa),
    makroNa100g(makroNa100g)
{
    dodajDomyslnaJednostkeGram();
}

const string& Produkt::pobierzNazwe() const { return nazwa; }
const Makroskladniki& Produkt::pobierzMakroNa100g() const { return makroNa100g; }
const vector<JednostkaProduktu>& Produkt::pobierzJednostki() const { return jednostki; }

bool Produkt::ustawNazwe(const string& nowaNazwa)
{
    if (!czyNazwaPoprawna(nowaNazwa)) { return false; } // Zablokuj ustawienie śmieciowych danych
    nazwa = nowaNazwa;
    return true;
}

bool Produkt::ustawMakroNa100g(const Makroskladniki& noweMakro)
{
    if (!czyMakroPoprawne(noweMakro)) { return false; } // Zablokuj absurdalne wartości
    makroNa100g = noweMakro;
    return true;
}

// Rejestruje dodatkową jednostkę (np. "opakowanie" ważące "250" gram)
bool Produkt::dodajJednostke(const string& nazwaJednostki, double gramyNaJednostke)
{
    if (!czyJednostkaPoprawna(nazwaJednostki, gramyNaJednostke)) { return false; }
    if (czyJednostkaIstnieje(nazwaJednostki)) { return false; } // Zabezpieczenie przed wielokrotnym dodaniem np. "sztuka"

    jednostki.push_back({ nazwaJednostki, gramyNaJednostke });
    return true;
}

// Usuwa jednostkę uboczną, z wbudowanym mechanizmem obronnym
bool Produkt::usunJednostke(const string& nazwaJednostki)
{
    // Rdzeń kalkulacji zależy od "g", nie możemy tego fizycznie usunąć
    if (nazwaJednostki == "g") { return false; }

    const auto iterator = find_if(
        jednostki.begin(),
        jednostki.end(),
        [&nazwaJednostki](const JednostkaProduktu& jednostka) {
            return jednostka.nazwa == nazwaJednostki;
        }
    );

    if (iterator == jednostki.end()) { return false; } // Nie ma takiej miary w wektorze

    jednostki.erase(iterator);
    return true;
}

const JednostkaProduktu* Produkt::znajdzJednostke(const string& nazwaJednostki) const
{
    const auto iterator = find_if(
        jednostki.begin(),
        jednostki.end(),
        [&nazwaJednostki](const JednostkaProduktu& jednostka) {
            return jednostka.nazwa == nazwaJednostki;
        }
    );

    if (iterator == jednostki.end()) { return nullptr; }
    return &(*iterator);
}

// Agregator walidacyjny - upewnia się, że obiekt można bezpiecznie zapisać do bazy / UI
bool Produkt::czyPoprawny() const
{
    return czyNazwaPoprawna(nazwa)
        && czyMakroPoprawne(makroNa100g)
        && !jednostki.empty()
        && czyJednostkaIstnieje("g");
}

bool Produkt::czyUlubiony() const { return ulubiony; }
void Produkt::ustawUlubiony(bool stan) { ulubiony = stan; }

bool Produkt::czyNazwaPoprawna(const string& tekst) const { return czyTekstNiepusty(tekst); }

bool Produkt::czyMakroPoprawne(const Makroskladniki& makro) const
{
    // Maksymalny limit upewnia się, że na 100g nie wrzucimy czegoś, co psuje matematykę i program
    return czyLiczbaWZakresie(makro.kalorie, 0.0, 10000.0)
        && czyLiczbaWZakresie(makro.bialko, 0.0, 10000.0)
        && czyLiczbaWZakresie(makro.weglowodany, 0.0, 10000.0)
        && czyLiczbaWZakresie(makro.tluszcz, 0.0, 10000.0);
}

bool Produkt::czyJednostkaPoprawna(const string& nazwaJednostki, double gramyNaJednostke) const
{
    return czyNazwaPoprawna(nazwaJednostki) && czyLiczbaWZakresie(gramyNaJednostke, 0.01, 5000.0);
}

bool Produkt::czyJednostkaIstnieje(const string& nazwaJednostki) const
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