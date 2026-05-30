#include "DziennikZywieniowy.h"

#include <algorithm>
#include <cmath>
#include <optional>

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

    std::optional<std::size_t> indeksGlobalnyDlaPory(
        const std::vector<PozycjaDziennika>& pozycje,
        PoraPosilku pora,
        std::size_t indeksWPorze
    )
    {
        std::size_t licznik = 0;

        for (std::size_t i = 0; i < pozycje.size(); ++i)
        {
            if (pozycje[i].pobierzPorePosilku() != pora)
            {
                continue;
            }

            if (licznik == indeksWPorze)
            {
                return i;
            }

            ++licznik;
        }

        return std::nullopt;
    }
}

PozycjaDziennika::PozycjaDziennika(
    const std::string& nazwaProduktu,
    double ilosc,
    const JednostkaProduktu& jednostka,
    const Makroskladniki& makroNa100g,
    PoraPosilku poraPosilku
)
    : nazwaProduktu(nazwaProduktu),
    ilosc(ilosc),
    jednostka(jednostka),
    makroNa100g(makroNa100g),
    poraPosilku(poraPosilku)
{
}

PoraPosilku PozycjaDziennika::pobierzPorePosilku() const
{
    return poraPosilku;
}

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
    double tluszczNa100g,
    PoraPosilku poraPosilku
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

    pozycje.emplace_back(
        nazwaProduktu,
        ilosc,
        jednostka,
        makroNa100g,
        poraPosilku
    );

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

bool DziennikZywieniowy::usunPozycjeDlaPory(PoraPosilku pora, std::size_t indeksWPorze)
{
    const std::optional<std::size_t> indeks =
        indeksGlobalnyDlaPory(pozycje, pora, indeksWPorze);

    if (!indeks.has_value())
    {
        return false;
    }

    return usunPozycje(*indeks);
}

DziennikZywieniowy::WynikOperacji DziennikZywieniowy::edytujPozycjeDlaPory(
    PoraPosilku pora,
    std::size_t indeksWPorze,
    double ilosc,
    const JednostkaProduktu& jednostka
)
{
    const std::optional<std::size_t> indeks =
        indeksGlobalnyDlaPory(pozycje, pora, indeksWPorze);

    if (!indeks.has_value())
    {
        return WynikOperacji::NiepoprawnaIlosc;
    }

    const PozycjaDziennika& stara = pozycje[*indeks];

    const WynikOperacji wynikWalidacji = walidujPozycje(
        stara.pobierzNazweProduktu(),
        ilosc,
        jednostka,
        stara.pobierzMakroNa100g()
    );

    if (wynikWalidacji != WynikOperacji::Sukces)
    {
        return wynikWalidacji;
    }

    pozycje[*indeks] = PozycjaDziennika(
        stara.pobierzNazweProduktu(),
        ilosc,
        jednostka,
        stara.pobierzMakroNa100g(),
        stara.pobierzPorePosilku()
    );

    return WynikOperacji::Sukces;
}

std::vector<PozycjaDziennika> DziennikZywieniowy::pobierzPozycjeDlaPory(PoraPosilku pora) const
{
    std::vector<PozycjaDziennika> wynik;

    for (const PozycjaDziennika& pozycja : pozycje)
    {
        if (pozycja.pobierzPorePosilku() == pora)
        {
            wynik.push_back(pozycja);
        }
    }

    return wynik;
}

Makroskladniki DziennikZywieniowy::obliczSumeDlaPory(PoraPosilku pora) const
{
    Makroskladniki suma;

    for (const PozycjaDziennika& pozycja : pozycje)
    {
        if (pozycja.pobierzPorePosilku() == pora)
        {
            suma += pozycja.obliczMakro();
        }
    }

    return suma;
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