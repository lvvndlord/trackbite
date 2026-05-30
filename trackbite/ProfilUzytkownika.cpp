#include "ProfilUzytkownika.h"

#include <cmath>

namespace
{
    bool czyLiczbaWZakresie(double wartosc, double minimum, double maksimum)
    {
        return std::isfinite(wartosc)
            && wartosc >= minimum
            && wartosc <= maksimum;
    }
}

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
    if (!czyLiczbaWZakresie(nowaWaga, 0.0, 500.0))
    {
        return;
    }

    waga = nowaWaga;
}

void ProfilUzytkownika::ustawWzrost(double nowyWzrost)
{
    if (!czyLiczbaWZakresie(nowyWzrost, 0.0, 250.0))
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
    if (!czyLiczbaWZakresie(nowyLimit, 1.0, 10000.0))
    {
        return;
    }

    limitKalorii = nowyLimit;
}