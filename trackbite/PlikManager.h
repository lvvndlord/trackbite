#pragma once

#include "Produkt.h"
#include "ProfilUzytkownika.h"
#include "DziennikZywieniowy.h"

#include <string>
#include <vector>

class PlikManager
{
public:
    // Zapisuje bazę produktów do pliku JSON.
    static bool zapiszProdukty(
        const std::string& sciezka,
        const std::vector<Produkt>& produkty
    );

    // Wczytuje bazę produktów z pliku JSON.
    static bool wczytajProdukty(
        const std::string& sciezka,
        std::vector<Produkt>& produkty
    );

    // Zapisuje profil użytkownika do pliku JSON.
    static bool zapiszProfil(
        const std::string& sciezka,
        const ProfilUzytkownika& profil
    );

    // Wczytuje profil użytkownika z pliku JSON.
    static bool wczytajProfil(
        const std::string& sciezka,
        ProfilUzytkownika& profil
    );

    // Zapisuje dziennik żywieniowy do pliku JSON.
    static bool zapiszDziennik(
        const std::string& sciezka,
        const DziennikZywieniowy& dziennik
    );

    // Wczytuje dziennik żywieniowy z pliku JSON.
    static bool wczytajDziennik(
        const std::string& sciezka,
        DziennikZywieniowy& dziennik
    );
};