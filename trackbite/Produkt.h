#pragma once

#include "TypyZywieniowe.h"

#include <string>
#include <vector>

using namespace std;

// Klasa reprezentująca konkretny produkt spożywczy dostępny w bazie aplikacji
class Produkt
{
public:
    // Konstruktor domyślny
    Produkt();

    // Konstruktor główny ustawiający od razu nazwę i makro wyliczone na 100g
    Produkt(
        const string& nazwa,
        const Makroskladniki& makroNa100g
    );

    // --- Gettery (pobieranie danych o obiekcie) ---
    const string& pobierzNazwe() const;
    const Makroskladniki& pobierzMakroNa100g() const;
    const vector<JednostkaProduktu>& pobierzJednostki() const;

    // --- Settery (zmiana danych z wbudowaną walidacją błędów) ---
    bool ustawNazwe(const string& nowaNazwa);
    bool ustawMakroNa100g(const Makroskladniki& noweMakro);

    // Zarządzanie dodatkowymi, własnymi miarami produktu (np. plaster = 15g)
    bool dodajJednostke(const string& nazwaJednostki, double gramyNaJednostke);
    bool usunJednostke(const string& nazwaJednostki);

    // Sprawdza czy produkt posiada zapisaną w wektorze konkretną miarę
    const JednostkaProduktu* znajdzJednostke(const string& nazwaJednostki) const;

    // Sprawdza czy cały obiekt produktu jest logicznie spójny (np. czy kalorie nie są na minusie)
    bool czyPoprawny() const;

    // Zarządzanie stanem ulubionych widocznym w tabeli
    bool czyUlubiony() const;
    void ustawUlubiony(bool stan);

private:
    string nazwa;                             // Nazwa (np. "Chleb żytni")
    Makroskladniki makroNa100g;               // Wartości odżywcze wyliczone sztywno na 100 gramów
    vector<JednostkaProduktu> jednostki;      // Lista opcjonalnych miar towarzyszących (sztuka, plaster itp.)
    bool ulubiony = false;                    // Przełącznik definiujący czy produkt ląduje w zakładce Ulubione

    // Wewnętrzne metody weryfikujące, używane wewnątrz klasy
    bool czyNazwaPoprawna(const string& tekst) const;
    bool czyMakroPoprawne(const Makroskladniki& makro) const;
    bool czyJednostkaPoprawna(const string& nazwaJednostki, double gramyNaJednostke) const;
    bool czyJednostkaIstnieje(const string& nazwaJednostki) const;

    // Metoda upewniająca się, że każdy produkt ZAWSZE ma bazową jednostkę "g" o wadze 1.0
    void dodajDomyslnaJednostkeGram();
};