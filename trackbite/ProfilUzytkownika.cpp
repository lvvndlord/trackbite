// Implementacja profilu użytkownika: dane podstawowe, cel wagowy,
// limit kalorii oraz proste obliczenie czasu dojścia do celu.
#include "ProfilUzytkownika.h"

#include <cmath>

namespace
{
    // Wspólna kontrola wartości liczbowych używana przez settery profilu.
    bool czyLiczbaWZakresie(double wartosc, double minimum, double maksimum)
    {
        return std::isfinite(wartosc)
            && wartosc >= minimum
            && wartosc <= maksimum;
    }
}

// Gettery zwracają aktualny stan profilu do UI oraz do zapisu w pliku.
const std::string& ProfilUzytkownika::pobierzImie() const
{
    return imie;
}

int ProfilUzytkownika::pobierzWiek() const
{
    return wiek;
}

double ProfilUzytkownika::pobierzWage() const
{
    return waga;
}

double ProfilUzytkownika::pobierzWzrost() const
{
    return wzrost;
}

const std::string& ProfilUzytkownika::pobierzCel() const
{
    return cel;
}

double ProfilUzytkownika::pobierzLimitKalorii() const
{
    return limitKalorii;
}

double ProfilUzytkownika::pobierzWageDocelowa() const
{
    return wagaDocelowa;
}

double ProfilUzytkownika::pobierzTempoZmianyWagiTygodniowo() const
{
    return tempoZmianyWagiTygodniowo;
}

// Settery aktualizują dane profilu, a przy liczbach odrzucają wartości spoza rozsądnych zakresów.
void ProfilUzytkownika::ustawImie(const std::string& noweImie)
{
    imie = noweImie;
}

void ProfilUzytkownika::ustawWiek(int nowyWiek)
{
    if (nowyWiek < 0 || nowyWiek > 120)
    {
        return;
    }

    wiek = nowyWiek;
}

void ProfilUzytkownika::ustawWage(double nowaWaga)
{
    if (!czyLiczbaWZakresie(nowaWaga, 20.0, 400.0))
    {
        return;
    }

    waga = nowaWaga;
}

void ProfilUzytkownika::ustawWzrost(double nowyWzrost)
{
    if (!czyLiczbaWZakresie(nowyWzrost, 100.0, 250.0))
    {
        return;
    }

    wzrost = nowyWzrost;
}

void ProfilUzytkownika::ustawCel(const std::string& nowyCel)
{
    cel = nowyCel;
}

void ProfilUzytkownika::ustawLimitKalorii(double nowyLimit)
{
    if (!czyLiczbaWZakresie(nowyLimit, 800.0, 10000.0))
    {
        return;
    }

    limitKalorii = nowyLimit;
}

void ProfilUzytkownika::ustawWageDocelowa(double nowaWagaDocelowa)
{
    if (!czyLiczbaWZakresie(nowaWagaDocelowa, 20.0, 400.0))
    {
        return;
    }

    wagaDocelowa = nowaWagaDocelowa;
}

void ProfilUzytkownika::ustawTempoZmianyWagiTygodniowo(double noweTempo)
{
    if (!czyLiczbaWZakresie(noweTempo, 0.1, 2.0))
    {
        return;
    }

    tempoZmianyWagiTygodniowo = noweTempo;
}

// Cel wagi jest poprawny tylko wtedy, gdy obecna waga, cel i tempo dają realną różnicę do policzenia.
bool ProfilUzytkownika::maPoprawnyCelWagi() const
{
    return czyLiczbaWZakresie(waga, 20.0, 400.0)
        && czyLiczbaWZakresie(wagaDocelowa, 20.0, 400.0)
        && czyLiczbaWZakresie(tempoZmianyWagiTygodniowo, 0.1, 2.0)
        && std::abs(wagaDocelowa - waga) >= 0.1;
}

// Szacowanie czasu jest proste: różnica kilogramów podzielona przez tempo tygodniowe i zamieniona na dni.
int ProfilUzytkownika::obliczSzacowaneDniDoCelu() const
{
    if (!maPoprawnyCelWagi())
    {
        return 0;
    }

    const double roznicaKg = std::abs(wagaDocelowa - waga);
    const double tygodnie = roznicaKg / tempoZmianyWagiTygodniowo;
    return static_cast<int>(std::round(tygodnie * 7.0));
}