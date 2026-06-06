#pragma once

#include <string>

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

struct JednostkaProduktu
{
    std::string nazwa;
    double gramyNaJednostke = 1.0;
};

class IMakroObliczalny
{
public:
    virtual ~IMakroObliczalny() = default;
    virtual Makroskladniki obliczMakro() const = 0;
};