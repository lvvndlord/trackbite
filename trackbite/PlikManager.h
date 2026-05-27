#pragma once

#include <string>
#include <vector>
#include "Produkt.h"
#include "ProfilUzytkownika.h"
#include "DziennikZywieniowy.h"

class PlikManager
{
public:
	static bool zapiszProdukty(const std::string& sciezka, const std::vector<Produkt>& produkty);
	static bool wczytajProdukty(const std::string& sciezka, std::vector<Produkt>& produkty);

	static bool zapiszProfil(const std::string& sciezka, const ProfilUzytkownika& profil);
	static bool wczytajProfil(const std::string& sciezka, ProfilUzytkownika& profil);

	static bool zapiszDziennik(const std::string& sciezka, const DziennikZywieniowy& dziennik);
	static bool wczytajDziennik(const std::string& sciezka, DziennikZywieniowy& dziennik);
};
