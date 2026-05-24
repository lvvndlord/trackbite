#include "DziennikZywieniowy.h"

#include <algorithm>
#include <cmath>

namespace
{
    bool czyTekstPusty(const std::string& tekst)
    {
        return tekst.find_first_not_of(" \t\n\r") == std::string::npos;
    }

    bool czyLiczbaWZakresie(double wartosc, double minimum, double maksimum)
    {
        return std::isfinite(wartosc) && wartosc >= minimum && wartosc <= maksimum;
    }
}

PozycjaDziennika::PozycjaDziennika(
    const std::string& nazwaProduktu,
    double ilosc,
    const JednostkaProduktu& jednostka,
    const Makroskladniki& makroNa100g
)
    : nazwaProduktu(nazwaProduktu),
    ilosc(ilosc),
    jednostka(jednostka),
    makroNa100g(makroNa100g)
{}

const std::string& PozycjaDziennika::pobierzNazweProduktu() const
{
    return nazwaProduktu;
}

double PozycjaDziennika::pobierzIlosc() const
{
    return ilosc;
}

const JednostkaProduktu& PozycjaDziennika::pobierzJednostke() const
{
    return jednostka;
}

double PozycjaDziennika::pobierzGramy() const
{
    return ilosc * jednostka.gramyNaJednostke;
}

const Makroskladniki& PozycjaDziennika::pobierzMakroNa100g() const
{
    return makroNa100g;
}

Makroskladniki PozycjaDziennika::obliczMakro() const
{
    return makroNa100g.przeliczNaGramy(pobierzGramy());
}

DziennikZywieniowy::DziennikZywieniowy()
    : limityDzienne{ 2200.0, 120.0, 240.0, 70.0 }
{}

DziennikZywieniowy::WynikOperacji DziennikZywieniowy::dodajPozycje(
    const std::string& nazwaProduktu,
    double ilosc,
    const JednostkaProduktu& jednostka,
    double kalorieNa100g,
    double bialkoNa100g,
    double weglowodanyNa100g,
    double tluszczNa100g
)
{
    const Makroskladniki makroNa100g{
        kalorieNa100g,
        bialkoNa100g,
        weglowodanyNa100g,
        tluszczNa100g
    };

    const WynikOperacji wynikWalidacji =
        walidujPozycje(nazwaProduktu, ilosc, jednostka, makroNa100g);

    if (wynikWalidacji != WynikOperacji::Sukces)
    {
        return wynikWalidacji;
    }

    pozycje.emplace_back(nazwaProduktu, ilosc, jednostka, makroNa100g);
    return WynikOperacji::Sukces;
}

bool DziennikZywieniowy::usunPozycje(std::size_t indeks)
{
    if (indeks >= pozycje.size())
    {
        return false;
    }

    pozycje.erase(pozycje.begin() + static_cast<std::ptrdiff_t>(indeks));
    return true;
}

void DziennikZywieniowy::wyczysc()
{
    pozycje.clear();
}

const std::vector<PozycjaDziennika>& DziennikZywieniowy::pobierzPozycje() const
{
    return pozycje;
}

Makroskladniki DziennikZywieniowy::obliczSume() const
{
    Makroskladniki suma;

    for (const PozycjaDziennika& pozycja : pozycje)
    {
        suma += pozycja.obliczMakro();
    }

    return suma;
}

bool DziennikZywieniowy::ustawLimitKalorii(double limitKalorii)
{
    if (!czyLiczbaWZakresie(limitKalorii, 1.0, 10000.0))
    {
        return false;
    }

    limityDzienne.kalorie = limitKalorii;
    return true;
}

bool DziennikZywieniowy::ustawLimityDzienne(const Makroskladniki& limity)
{
    if (!czyLiczbaWZakresie(limity.kalorie, 1.0, 10000.0) ||
        !czyLiczbaWZakresie(limity.bialko, 1.0, 1000.0) ||
        !czyLiczbaWZakresie(limity.weglowodany, 1.0, 1000.0) ||
        !czyLiczbaWZakresie(limity.tluszcz, 1.0, 1000.0))
    {
        return false;
    }

    limityDzienne = limity;
    return true;
}

Makroskladniki DziennikZywieniowy::pobierzLimityDzienne() const
{
    return limityDzienne;
}

double DziennikZywieniowy::pozostaleKalorie() const
{
    return limityDzienne.kalorie - obliczSume().kalorie;
}

int DziennikZywieniowy::procentKalorii() const
{
    return procentWartosci(obliczSume().kalorie, limityDzienne.kalorie);
}

int DziennikZywieniowy::procentBialka() const
{
    return procentWartosci(obliczSume().bialko, limityDzienne.bialko);
}

int DziennikZywieniowy::procentWeglowodanow() const
{
    return procentWartosci(obliczSume().weglowodany, limityDzienne.weglowodany);
}

int DziennikZywieniowy::procentTluszczu() const
{
    return procentWartosci(obliczSume().tluszcz, limityDzienne.tluszcz);
}

DziennikZywieniowy::WynikOperacji DziennikZywieniowy::walidujPozycje(
    const std::string& nazwaProduktu,
    double ilosc,
    const JednostkaProduktu& jednostka,
    const Makroskladniki& makroNa100g
) const
{
    if (czyTekstPusty(nazwaProduktu))
    {
        return WynikOperacji::PustaNazwa;
    }

    if (czyTekstPusty(jednostka.nazwa))
    {
        return WynikOperacji::PustaJednostka;
    }

    if (!czyLiczbaWZakresie(ilosc, 0.01, 10000.0))
    {
        return WynikOperacji::NiepoprawnaIlosc;
    }

    if (!czyLiczbaWZakresie(jednostka.gramyNaJednostke, 0.01, 5000.0))
    {
        return WynikOperacji::NiepoprawnaJednostka;
    }

    if (!czyLiczbaWZakresie(makroNa100g.kalorie, 0.0, 1000.0) ||
        !czyLiczbaWZakresie(makroNa100g.bialko, 0.0, 100.0) ||
        !czyLiczbaWZakresie(makroNa100g.weglowodany, 0.0, 100.0) ||
        !czyLiczbaWZakresie(makroNa100g.tluszcz, 0.0, 100.0))
    {
        return WynikOperacji::NiepoprawneMakro;
    }

    return WynikOperacji::Sukces;
}

int DziennikZywieniowy::procentWartosci(double wartosc, double limit) const
{
    if (!czyLiczbaWZakresie(limit, 0.01, 100000.0))
    {
        return 0;
    }

    return static_cast<int>(std::round((wartosc / limit) * 100.0));
}