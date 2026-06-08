#pragma once

#include <string>

// Wspolny zestaw makroskladnikow uzywany w produktach, dzienniku i limitach dziennych.
// Wartosci przechowywane sa w prostych liczbach double, bo aplikacja glownie je sumuje i przelicza.
struct Makroskladniki
{
    // Energia produktu lub posilku wyrazona w kcal.
    double kalorie = 0.0;

    // Ilosc bialka, weglowodanow i tluszczu w gramach.
    double bialko = 0.0;
    double weglowodany = 0.0;
    double tluszcz = 0.0;

    // Zwraca nowy obiekt z suma makro dwoch wartosci, bez zmiany obecnego obiektu.
    Makroskladniki operator+(const Makroskladniki& inne) const
    {
        return {
            kalorie + inne.kalorie,
            bialko + inne.bialko,
            weglowodany + inne.weglowodany,
            tluszcz + inne.tluszcz
        };
    }

    // Dodaje makro do obecnego obiektu. Przydatne przy sumowaniu calego dziennika w petli.
    Makroskladniki& operator+=(const Makroskladniki& inne)
    {
        kalorie += inne.kalorie;
        bialko += inne.bialko;
        weglowodany += inne.weglowodany;
        tluszcz += inne.tluszcz;
        return *this;
    }

    // Przelicza wartosci podane dla 100 g produktu na konkretna ilosc gramow.
    // Np. jezeli produkt ma 200 kcal / 100 g, to dla 50 g metoda zwroci 100 kcal.
    Makroskladniki przeliczNaGramy(double gramy) const
    {
        const double mnoznik = gramy / 100.0;

        return {
            kalorie * mnoznik,
            bialko * mnoznik,
            weglowodany * mnoznik,
            tluszcz * mnoznik
        };
    }
};

// Opis jednej jednostki produktu, np. "g", "sztuka", "opakowanie".
// Pole gramyNaJednostke pozwala sprowadzic kazda jednostke do gramow, czyli wspolnej bazy obliczen.
struct JednostkaProduktu
{
    // Nazwa widoczna dla uzytkownika w UI.
    std::string nazwa;

    // Ile gramow odpowiada jednej takiej jednostce. Dla "g" wartosc powinna wynosic 1.0.
    double gramyNaJednostke = 1.0;
};

// Prosty interfejs dla obiektow, ktore potrafia policzyc swoje makroskladniki.
// Dzieki temu PozycjaDziennika ma jasny kontrakt: musi umiec zwrocic przeliczone makro.
class IMakroObliczalny
{
public:
    virtual ~IMakroObliczalny() = default;

    // Zwraca makroskladniki dla konkretnego obiektu, a nie zawsze dla 100 g.
    virtual Makroskladniki obliczMakro() const = 0;
};
