#pragma once

#include <string>

class ProfilUzytkownika
{
public:
    ProfilUzytkownika() = default;

    // Gettery używane przy zapisie profilu do pliku.
    const std::string& pobierzImie() const;
    int pobierzWiek() const;
    double pobierzWage() const;
    double pobierzWzrost() const;
    const std::string& pobierzCel() const;
    double pobierzLimitKalorii() const;
    double pobierzWageDocelowa() const;
    double pobierzTempoZmianyWagiTygodniowo() const;

    // Settery używane przy odczycie profilu z pliku.
    void ustawImie(const std::string& noweImie);
    void ustawWiek(int nowyWiek);
    void ustawWage(double nowaWaga);
    void ustawWzrost(double nowyWzrost);
    void ustawCel(const std::string& nowyCel);
    void ustawLimitKalorii(double nowyLimit);
    void ustawWageDocelowa(double nowaWagaDocelowa);
    void ustawTempoZmianyWagiTygodniowo(double noweTempo);

    bool maPoprawnyCelWagi() const;
    int obliczSzacowaneDniDoCelu() const;

private:
    // Pola profilu zapisywane i odczytywane z pliku.
    std::string imie;
    int wiek = 0;
    double waga = 0.0;
    double wzrost = 0.0;
    std::string cel;
    double limitKalorii = 2000.0;
    double wagaDocelowa = 0.0;
    double tempoZmianyWagiTygodniowo = 0.0;
};