#pragma once

#include "Produkt.h"
#include "ProfilUzytkownika.h"
#include "DziennikZywieniowy.h"

#include <string>
#include <vector>

// Klasa narzedziowa odpowiedzialna za zapis i odczyt danych aplikacji z plikow JSON.
// Ma tylko metody statyczne, bo nie musi przechowywac zadnego wlasnego stanu.
class PlikManager
{
public:
    // Zapisuje liste produktow do pliku JSON pod wskazana sciezka.
    // Zwraca false, gdy pliku nie da sie otworzyc albo zapis sie nie powiedzie.
    static bool zapiszProdukty(
        const std::string& sciezka,
        const std::vector<Produkt>& produkty
    );

    // Wczytuje produkty z JSON-a do podanego wektora.
    // Niepoprawne rekordy sa odrzucane w implementacji, zeby nie psuc bazy w aplikacji.
    static bool wczytajProdukty(
        const std::string& sciezka,
        std::vector<Produkt>& produkty
    );

    // Zapisuje dane profilu uzytkownika, m.in. imie, wage, wzrost, cel i limit kalorii.
    static bool zapiszProfil(
        const std::string& sciezka,
        const ProfilUzytkownika& profil
    );

    // Wczytuje profil uzytkownika z pliku JSON do przekazanego obiektu.
    static bool wczytajProfil(
        const std::string& sciezka,
        ProfilUzytkownika& profil
    );

    // Zapisuje dziennik zywieniowy konkretnego dnia razem z limitami i pozycjami posilkow.
    static bool zapiszDziennik(
        const std::string& sciezka,
        const DziennikZywieniowy& dziennik
    );

    // Wczytuje dziennik zywieniowy z pliku JSON.
    // Zwraca false, gdy plik nie istnieje albo ma zla strukture.
    static bool wczytajDziennik(
        const std::string& sciezka,
        DziennikZywieniowy& dziennik
    );
};
