#include "PlikManager.h"

#include <fstream>
#include <sstream>

bool PlikManager::zapiszProdukty(const std::string& sciezka, const std::vector<Produkt>& produkty)
{
	std::ofstream out(sciezka);
	if (!out) return false;

	// Format: jedna linia na produkt: nazwa|kcal|bialko|weglowodany|tluszcz|jednostka1:gramy;jednostka2:gramy;
	for (const Produkt& p : produkty)
	{
		out << p.pobierzNazwe() << "|";
		const Makroskladniki m = p.pobierzMakroNa100g();
		out << m.kalorie << "|" << m.bialko << "|" << m.weglowodany << "|" << m.tluszcz << "|";

		for (const JednostkaProduktu& j : p.pobierzJednostki())
		{
			out << j.nazwa << ":" << j.gramyNaJednostke << ";";
		}

		out << "\n";
	}

	return true;
}

bool PlikManager::wczytajProdukty(const std::string& sciezka, std::vector<Produkt>& produkty)
{
	std::ifstream in(sciezka);
	if (!in) return false;

	std::string line;

	while (std::getline(in, line))
	{
		if (line.empty()) continue;

		std::stringstream ss(line);
		std::string nazwa;
		if (!std::getline(ss, nazwa, '|')) continue;

		std::string token;
		std::getline(ss, token, '|'); double kcal = std::stod(token);
		std::getline(ss, token, '|'); double bialko = std::stod(token);
		std::getline(ss, token, '|'); double weglowodany = std::stod(token);
		std::getline(ss, token, '|'); double tluszcz = std::stod(token);

		Produkt p(nazwa, Makroskladniki{ kcal, bialko, weglowodany, tluszcz });

		std::string jednostkiStr;
		if (std::getline(ss, jednostkiStr, '|'))
		{
			std::stringstream js(jednostkiStr);
			std::string jtok;
			while (std::getline(js, jtok, ';'))
			{
				if (jtok.empty()) continue;
				const size_t pos = jtok.find(':');
				if (pos == std::string::npos) continue;
				const std::string jn = jtok.substr(0, pos);
				const double gr = std::stod(jtok.substr(pos + 1));
				p.dodajJednostke(jn, gr);
			}
		}

		if (p.czyPoprawny())
		{
			produkty.push_back(p);
		}
	}

	return true;
}

bool PlikManager::zapiszProfil(const std::string& sciezka, const ProfilUzytkownika& profil)
{
	return profil.zapiszDoPliku(sciezka);
}

bool PlikManager::wczytajProfil(const std::string& sciezka, ProfilUzytkownika& profil)
{
	return profil.wczytajZPliku(sciezka);
}

bool PlikManager::zapiszDziennik(const std::string& sciezka, const DziennikZywieniowy& dziennik)
{
	std::ofstream out(sciezka);
	if (!out) return false;

	// Zapisujemy limit jako pierwsza linia: kalorie|bialko|weglowodany|tluszcz
	const Makroskladniki lim = dziennik.pobierzLimityDzienne();
	out << lim.kalorie << "|" << lim.bialko << "|" << lim.weglowodany << "|" << lim.tluszcz << "\n";

	// Kolejne linie: nazwa|ilosc|jednostka:gramy|kcal|bialko|weglowodany|tluszcz
	for (const PozycjaDziennika& p : dziennik.pobierzPozycje())
	{
		const Makroskladniki makro100 = p.pobierzMakroNa100g();
		const JednostkaProduktu je = p.pobierzJednostke();
		out << p.pobierzNazweProduktu() << "|"
			<< p.pobierzIlosc() << "|"
			<< je.nazwa << ":" << je.gramyNaJednostke << "|"
			<< makro100.kalorie << "|" << makro100.bialko << "|" << makro100.weglowodany << "|" << makro100.tluszcz << "\n";
	}

	return true;
}

bool PlikManager::wczytajDziennik(const std::string& sciezka, DziennikZywieniowy& dziennik)
{
	std::ifstream in(sciezka);
	if (!in) return false;

	std::string line;

	if (!std::getline(in, line)) return false;
	std::stringstream ss(line);
	std::string token;
	std::getline(ss, token, '|'); double kcal = std::stod(token);
	std::getline(ss, token, '|'); double bialko = std::stod(token);
	std::getline(ss, token, '|'); double weglowodany = std::stod(token);
	std::getline(ss, token, '|'); double tluszcz = std::stod(token);

	dziennik.ustawLimityDzienne({ kcal, bialko, weglowodany, tluszcz });

	while (std::getline(in, line))
	{
		if (line.empty()) continue;
		std::stringstream ls(line);
		std::string nazwa;
		std::getline(ls, nazwa, '|');
		std::getline(ls, token, '|'); double ilosc = std::stod(token);
		std::string jednostka;
		std::getline(ls, jednostka, '|');

		// obsługa formatu jednostka:gramy
		double gram = 1.0;
		const size_t posj = jednostka.find(":");
		std::string jednostkaNazwa = jednostka;
		if (posj != std::string::npos)
		{
			jednostkaNazwa = jednostka.substr(0, posj);
			gram = std::stod(jednostka.substr(posj + 1));
		}

		std::getline(ls, token, '|'); double mkcal = std::stod(token);
		std::getline(ls, token, '|'); double mbialko = std::stod(token);
		std::getline(ls, token, '|'); double mweg = std::stod(token);
		std::getline(ls, token, '|'); double mtl = std::stod(token);

		dziennik.dodajPozycje(nazwa, ilosc, JednostkaProduktu{ jednostkaNazwa, gram }, mkcal, mbialko, mweg, mtl);
	}

	return true;
}
