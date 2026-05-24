#pragma once

#include <string>

// Wartości odżywcze produktu.
// Dla produktu oznaczają wartości na 100 g.
// Dla pozycji w dzienniku oznaczają wartości już przeliczone.
struct Makroskladniki
{
    double kalorie = 0.0;
    double bialko = 0.0;
    double weglowodany = 0.0;
    double tluszcz = 0.0;

    Makroskladniki operator+(const Makroskladniki& inne) const
    {
        return {
            kalorie + inne.kalorie,
            bialko + inne.bialko,
            weglowodany + inne.weglowodany,
            tluszcz + inne.tluszcz
        };
    }

    Makroskladniki& operator+=(const Makroskladniki& inne)
    {
        kalorie += inne.kalorie;
        bialko += inne.bialko;
        weglowodany += inne.weglowodany;
        tluszcz += inne.tluszcz;

        return *this;
    }

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

// Jednostka produktu, np.:
// g = 1 g
// sztuka = 55 g
// plaster = 20 g
// opakowanie = 500 g
struct JednostkaProduktu
{
    std::string nazwa;
    double gramyNaJednostke = 1.0;
};

// Prosty interfejs do polimorfizmu.
// Każda klasa, która potrafi policzyć makro, może go implementować.
class IMakroObliczalny
{
public:
    virtual ~IMakroObliczalny() = default;

    virtual Makroskladniki obliczMakro() const = 0;
};