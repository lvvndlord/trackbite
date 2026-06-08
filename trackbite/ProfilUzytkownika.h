#pragma once

#include <string>

// Przechowuje dane uzytkownika potrzebne do obliczania celu kalorii i czasu dojscia do wagi docelowej.
// Klasa celowo nie zna Qt ani GUI - jest zwykla klasa modelu danych.
class ProfilUzytkownika
{
public:
    // Tworzy profil z wartosciami domyslnymi ustawionymi w polach prywatnych.
    ProfilUzytkownika() = default;

    // Metody odczytu uzywane przez GUI, PlikManager i obliczenia celu.
    const std::string& pobierzImie() const;
    int pobierzWiek() const;
    double pobierzWage() const;
    double pobierzWzrost() const;
    const std::string& pobierzCel() const;
    double pobierzLimitKalorii() const;
    double pobierzWageDocelowa() const;
    double pobierzTempoZmianyWagiTygodniowo() const;

    // Metody ustawiajace dane profilu. Czesc z nich ignoruje wartosci spoza bezpiecznego zakresu.
    void ustawImie(const std::string& noweImie);
    void ustawWiek(int nowyWiek);
    void ustawWage(double nowaWaga);
    void ustawWzrost(double nowyWzrost);
    void ustawCel(const std::string& nowyCel);
    void ustawLimitKalorii(double nowyLimit);
    void ustawWageDocelowa(double nowaWagaDocelowa);
    void ustawTempoZmianyWagiTygodniowo(double noweTempo);

    // Sprawdza, czy da sie sensownie policzyc czas do celu wagowego.
    bool maPoprawnyCelWagi() const;

    // Zwraca szacowana liczbe dni do osiagniecia wagi docelowej przy wybranym tempie tygodniowym.
    int obliczSzacowaneDniDoCelu() const;

private:
    // Imie pokazywane w profilu i zapisywane do pliku.
    std::string imie;

    // Podstawowe parametry uzytkownika wykorzystywane przy szacowaniu kalorii.
    int wiek = 0;
    double waga = 0.0;
    double wzrost = 0.0;

    // Opis celu z UI, np. utrzymanie, redukcja albo masa.
    std::string cel;

    // Aktualny dzienny limit kalorii. Domyslne 2000 kcal zabezpiecza aplikacje przed pustym profilem.
    double limitKalorii = 2000.0;

    // Parametry celu wagowego uzywane do wyliczenia przyblizonego czasu dojscia do celu.
    double wagaDocelowa = 0.0;
    double tempoZmianyWagiTygodniowo = 0.0;
};
