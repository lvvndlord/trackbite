#pragma once

#include "TypyZywieniowe.h"

#include <cstddef>
#include <string>
#include <vector>

// Dostepne sekcje dziennika, do ktorych mozna przypisac produkt.
// Enum jest uzywany zarowno w logice, jak i przy zapisie/odczycie dziennika z pliku.
enum class PoraPosilku
{
    Sniadanie,
    DrugieSniadanie,
    Obiad,
    Kolacja,
    Przekaski
};

// Jedna pozycja wpisana do dziennika, np. "Jajko, 2 sztuki, sniadanie".
// Klasa pamieta makro produktu dla 100 g i sama przelicza je na faktyczna porcje.
class PozycjaDziennika final : public IMakroObliczalny
{
public:
    // Tworzy pozycje dziennika z konkretna iloscia, jednostka i przypisana pora posilku.
    PozycjaDziennika(
        const std::string& nazwaProduktu,
        double ilosc,
        const JednostkaProduktu& jednostka,
        const Makroskladniki& makroNa100g,
        PoraPosilku poraPosilku
    );

    // Metody odczytu danych pozycji, uzywane m.in. przez tabele i zapis do JSON.
    const std::string& pobierzNazweProduktu() const;
    double pobierzIlosc() const;
    const JednostkaProduktu& pobierzJednostke() const;

    // Zwraca faktyczna mase porcji: ilosc * gramyNaJednostke.
    double pobierzGramy() const;

    const Makroskladniki& pobierzMakroNa100g() const;
    PoraPosilku pobierzPorePosilku() const;

    // Zmienia tylko porcje. Nazwa produktu, makro i pora posilku zostaja bez zmian.
    void zmienIloscIJednostke(
        double nowaIlosc,
        const JednostkaProduktu& nowaJednostka
    );

    // Przelicza makro dla realnie zjedzonej ilosci, a nie dla bazowych 100 g.
    Makroskladniki obliczMakro() const override;

private:
    // Nazwa produktu skopiowana z bazy w momencie dodania do dziennika.
    std::string nazwaProduktu;

    // Liczba jednostek wpisana przez uzytkownika, np. 2 sztuki albo 150 gramow.
    double ilosc = 0.0;

    // Jednostka wybrana dla tej pozycji. Na jej podstawie liczone sa gramy.
    JednostkaProduktu jednostka;

    // Makro produktu dla 100 g zapisane w pozycji, zeby wpis dziennika byl niezalezny od pozniejszych zmian w bazie.
    Makroskladniki makroNa100g;

    // Sekcja dziennika, w ktorej wyswietla sie ta pozycja.
    PoraPosilku poraPosilku = PoraPosilku::Sniadanie;
};

// Logika dziennego dziennika zywieniowego.
// Klasa przechowuje pozycje, liczy sumy makro, pilnuje limitow i zwraca kody bledow dla GUI.
class DziennikZywieniowy
{
public:
    // Wynik operacji zamiast samego boola, zeby GUI moglo pokazac konkretny komunikat bledu.
    enum class WynikOperacji
    {
        Sukces,
        PustaNazwa,
        PustaJednostka,
        NiepoprawnaIlosc,
        NiepoprawnaJednostka,
        NiepoprawneMakro,
        NiepoprawnyLimit
    };

    // Ustawia domyslne dzienne limity makro.
    DziennikZywieniowy();

    // Dodaje produkt do wybranej pory posilku po walidacji nazwy, ilosci, jednostki i makro.
    WynikOperacji dodajPozycje(
        const std::string& nazwaProduktu,
        double ilosc,
        const JednostkaProduktu& jednostka,
        double kalorieNa100g,
        double bialkoNa100g,
        double weglowodanyNa100g,
        double tluszczNa100g,
        PoraPosilku poraPosilku
    );

    // Usuwa pozycje po globalnym indeksie w wektorze wszystkich wpisow.
    bool usunPozycje(std::size_t indeks);

    // Usuwa pozycje widziana w konkretnej sekcji posilku, czyli po indeksie w tej porze.
    bool usunPozycjeDlaPory(PoraPosilku pora, std::size_t indeksWPorze);

    // Edytuje ilosc i jednostke pozycji w konkretnej porze posilku.
    WynikOperacji edytujPozycjeDlaPory(
        PoraPosilku pora,
        std::size_t indeksWPorze,
        double ilosc,
        const JednostkaProduktu& jednostka
    );

    // Usuwa wszystkie wpisy z dziennika, ale nie kasuje ustawionych limitow.
    void wyczysc();

    // Zwraca wszystkie pozycje jako stala referencje, zeby nie kopiowac calego wektora bez potrzeby.
    const std::vector<PozycjaDziennika>& pobierzPozycje() const;

    // Zwraca kopie pozycji tylko dla jednej pory posilku, wygodne do wypelniania osobnych tabel.
    std::vector<PozycjaDziennika> pobierzPozycjeDlaPory(PoraPosilku pora) const;

    // Liczy sume kalorii i makroskladnikow dla calego dnia.
    Makroskladniki obliczSume() const;

    // Liczy sume kalorii i makroskladnikow tylko dla wskazanej pory posilku.
    Makroskladniki obliczSumeDlaPory(PoraPosilku pora) const;

    // Ustawia tylko limit kalorii, zostawiajac pozostale makroskladniki bez zmian.
    bool ustawLimitKalorii(double limitKalorii);

    // Ustawia caly komplet dziennych limitow po sprawdzeniu zakresow.
    bool ustawLimityDzienne(const Makroskladniki& limity);

    // Zwraca aktualne limity dzienne uzywane przez paski postepu i podsumowania.
    Makroskladniki pobierzLimityDzienne() const;

    // Oblicza, ile kalorii zostalo do limitu. Wartosc ujemna oznacza przekroczenie limitu.
    double pozostaleKalorie() const;

    // Procent realizacji limitow, uzywany glownie przez QProgressBar w interfejsie.
    int procentKalorii() const;
    int procentBialka() const;
    int procentWeglowodanow() const;
    int procentTluszczu() const;

private:
    // Dzienne cele dla kalorii, bialka, weglowodanow i tluszczu.
    Makroskladniki limityDzienne;

    // Wszystkie wpisy danego dnia. Pora posilku jest polem kazdej pozycji, nie osobnym wektorem.
    std::vector<PozycjaDziennika> pozycje;

    // Wspolna walidacja uzywana przy dodawaniu i edycji pozycji.
    WynikOperacji walidujPozycje(
        const std::string& nazwaProduktu,
        double ilosc,
        const JednostkaProduktu& jednostka,
        const Makroskladniki& makroNa100g
    ) const;

    // Pomocniczo liczy procent wykorzystania limitu i zabezpiecza przed dzieleniem przez zera lub zle limity.
    int procentWartosci(double wartosc, double limit) const;
};
