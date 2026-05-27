#pragma once

#include <string>
#include "TypyZywieniowe.h"

class ProfilUzytkownika
{
public:
    ProfilUzytkownika();

    // Gettery - dostęp do danych profilu
    const std::string& pobierzImie() const;
    const std::string& pobierzNazwisko() const;
    double pobierzWiek() const;
    double pobierzWage() const;
    double pobierzWzrost() const;
    double pobierzCel() const;
    const std::string& pobierzWzorCe() const;
    const Makroskladniki& pobierzLimityDzienne() const;

    // Settery - ustawianie danych profilu
    bool ustawImie(const std::string& imie);
    bool ustawNazwisko(const std::string& nazwisko);
    bool ustawWiek(double wiek);
    bool ustawWage(double waga);
    bool ustawWzrost(double wzrost);
    bool ustawCel(double cel);
    bool ustawWzorCe(const std::string& wzor);

    // Obliczanie limitów kaloryjnych
    bool obliczLimityNaPostawieCelu(double cel);

    // Walidacja
    bool czyProfilPoprawny() const;

    // Persistence - zapis i odczyt z pliku
    bool zapiszProfil(const std::string& sciezkaPliku) const;
    bool odczytajProfil(const std::string& sciezkaPliku);

private:
    std::string imie;
    std::string nazwisko;
    double wiek = 0.0;
    double waga = 0.0;      // w kg
    double wzrost = 0.0;    // w cm
    double cel = 0.0;       // cel kaloryjny
    std::string wzorCe;     // "MIFFLIN-JEOR", "HARRIS-BENEDICT" itp.
    Makroskladniki limityDzienne;

    // Funkcje walidacji
    bool czyImiePoprawne(const std::string& imie) const;
    bool czySzwiskoPoprawne(const std::string& nazwisko) const;
    bool czyWiekPoprawny(double wiek) const;
    bool czyWagaPoprawna(double waga) const;
    bool czyWzrostPoprawny(double wzrost) const;
    bool czyCelPoprawny(double cel) const;

    // Funkcje prywatne do obsługi plików
    bool zapiszDoPlikuTekstowego(const std::string& sciezkaPliku) const;
    bool odczytajZPlikuTekstowego(const std::string& sciezkaPliku);
};

