// Implementacja dziennika żywieniowego: pozycje posiłków, przeliczenia makro,
// dzienne limity oraz operacje dodawania/edycji/usuwania wpisów.
#include "DziennikZywieniowy.h"

#include <cmath>
#include <optional>

namespace
{
    // Pomocnicza kontrola pól tekstowych: nazwa złożona z samych spacji jest traktowana jak pusta.
    bool czyTekstPusty(const std::string& tekst)
    {
        return tekst.find_first_not_of(" \t\n\r") == std::string::npos;
    }

    // Jedna funkcja do walidacji liczb zabezpiecza przed NaN, nieskończonością i wartościami spoza zakresu.
    bool czyLiczbaWZakresie(double wartosc, double minimum, double maksimum)
    {
        return std::isfinite(wartosc)
            && wartosc >= minimum
            && wartosc <= maksimum;
    }

    // Tabele w UI mają indeksy liczone osobno dla każdej pory posiłku.
    // Ta funkcja mapuje taki indeks na prawdziwy indeks w głównym wektorze dziennika.
    std::optional<std::size_t> znajdzIndeksGlobalnyDlaPory(
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

// Pozycja dziennika zapamiętuje stan produktu w momencie dodania, także makro i wybraną jednostkę.
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

// Gettery są używane przez UI i zapis do JSON-a; nie modyfikują obiektu.
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

// Ilość użytkownika przeliczamy na gramy, bo makroskładniki są przechowywane w przeliczeniu na 100 g.
double PozycjaDziennika::pobierzGramy() const
{
    return ilosc * jednostka.gramyNaJednostke;
}

const Makroskladniki& PozycjaDziennika::pobierzMakroNa100g() const
{
    return makroNa100g;
}

PoraPosilku PozycjaDziennika::pobierzPorePosilku() const
{
    return poraPosilku;
}

// Przy edycji pozycji zmieniamy tylko ilość i jednostkę, a nazwa oraz makro produktu zostają bez zmian.
void PozycjaDziennika::zmienIloscIJednostke(
    double nowaIlosc,
    const JednostkaProduktu& nowaJednostka
)
{
    ilosc = nowaIlosc;
    jednostka = nowaJednostka;
}

// Właściwe przeliczenie kalorii i makro dla konkretnej ilości produktu.
Makroskladniki PozycjaDziennika::obliczMakro() const
{
    return makroNa100g.przeliczNaGramy(pobierzGramy());
}

// Domyślne limity pozwalają uruchomić aplikację bez wcześniejszego uzupełniania profilu.
DziennikZywieniowy::DziennikZywieniowy()
    : limityDzienne{ 2200.0, 120.0, 240.0, 70.0 }
{
}

// Dodanie wpisu składa dane makro w strukturę, waliduje całość i dopiero wtedy dopisuje do wektora.
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

// Usunięcie po indeksie globalnym jest najprostszą operacją na głównym wektorze pozycji.
bool DziennikZywieniowy::usunPozycje(std::size_t indeks)
{
    if (indeks >= pozycje.size())
    {
        return false;
    }

    pozycje.erase(
        pozycje.begin() + static_cast<std::ptrdiff_t>(indeks)
    );

    return true;
}

// Usuwanie z konkretnej pory posiłku musi najpierw znaleźć odpowiadający wpis w całym dzienniku.
bool DziennikZywieniowy::usunPozycjeDlaPory(
    PoraPosilku pora,
    std::size_t indeksWPorze
)
{
    const std::optional<std::size_t> indeksGlobalny =
        znajdzIndeksGlobalnyDlaPory(pozycje, pora, indeksWPorze);

    if (!indeksGlobalny.has_value())
    {
        return false;
    }

    return usunPozycje(indeksGlobalny.value());
}

// Edycja pozycji korzysta z tej samej walidacji co dodawanie, żeby nie rozjechały się zasady poprawności danych.
DziennikZywieniowy::WynikOperacji DziennikZywieniowy::edytujPozycjeDlaPory(
    PoraPosilku pora,
    std::size_t indeksWPorze,
    double ilosc,
    const JednostkaProduktu& jednostka
)
{
    const std::optional<std::size_t> indeksGlobalny =
        znajdzIndeksGlobalnyDlaPory(pozycje, pora, indeksWPorze);

    if (!indeksGlobalny.has_value())
    {
        return WynikOperacji::NiepoprawnaIlosc;
    }

    PozycjaDziennika& pozycja = pozycje[indeksGlobalny.value()];

    const WynikOperacji wynikWalidacji = walidujPozycje(
        pozycja.pobierzNazweProduktu(),
        ilosc,
        jednostka,
        pozycja.pobierzMakroNa100g()
    );

    if (wynikWalidacji != WynikOperacji::Sukces)
    {
        return wynikWalidacji;
    }

    pozycja.zmienIloscIJednostke(ilosc, jednostka);
    return WynikOperacji::Sukces;
}

// Czyści tylko wpisy dnia, nie ruszając zapisanych limitów dziennych.
void DziennikZywieniowy::wyczysc()
{
    pozycje.clear();
}

const std::vector<PozycjaDziennika>& DziennikZywieniowy::pobierzPozycje() const
{
    return pozycje;
}

// Tworzy osobną listę pozycji dla wybranej pory, dzięki czemu każda tabela posiłku może być wypełniana niezależnie.
std::vector<PozycjaDziennika> DziennikZywieniowy::pobierzPozycjeDlaPory(
    PoraPosilku pora
) const
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

// Sumowanie całego dnia opiera się na obliczMakro() każdej pozycji, więc jednostki są uwzględniane automatycznie.
Makroskladniki DziennikZywieniowy::obliczSume() const
{
    Makroskladniki suma;

    for (const PozycjaDziennika& pozycja : pozycje)
    {
        suma += pozycja.obliczMakro();
    }

    return suma;
}

// Sumowanie dla jednej pory posiłku zasila nagłówki tabel: kcal, białko, węgle i tłuszcz przy danym posiłku.
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

// Ustawianie limitów ma własną walidację, bo błędny limit zepsułby paski procentowe i podsumowanie dnia.
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

// Limity są zwracane jako kopia, bo UI ma je tylko wyświetlać albo zapisywać do pliku.
Makroskladniki DziennikZywieniowy::pobierzLimityDzienne() const
{
    return limityDzienne;
}

// Różnica między limitem a spożyciem może być ujemna — wtedy UI pokazuje przekroczenie limitu.
double DziennikZywieniowy::pozostaleKalorie() const
{
    return limityDzienne.kalorie - obliczSume().kalorie;
}

// Procenty są przygotowane pod paski postępu w interfejsie.
int DziennikZywieniowy::procentKalorii() const
{
    return procentWartosci(
        obliczSume().kalorie,
        limityDzienne.kalorie
    );
}

int DziennikZywieniowy::procentBialka() const
{
    return procentWartosci(
        obliczSume().bialko,
        limityDzienne.bialko
    );
}

int DziennikZywieniowy::procentWeglowodanow() const
{
    return procentWartosci(
        obliczSume().weglowodany,
        limityDzienne.weglowodany
    );
}

int DziennikZywieniowy::procentTluszczu() const
{
    return procentWartosci(
        obliczSume().tluszcz,
        limityDzienne.tluszcz
    );
}

// Centralna walidacja pozycji dziennika: metoda zwraca konkretny kod błędu zamiast samego true/false.
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

// Wspólna metoda do obliczania procentów makro; chroni przed dzieleniem przez zero i błędnym limitem.
int DziennikZywieniowy::procentWartosci(double wartosc, double limit) const
{
    if (!czyLiczbaWZakresie(limit, 0.01, 100000.0))
    {
        return 0;
    }

    return static_cast<int>(
        std::round((wartosc / limit) * 100.0)
        );
}