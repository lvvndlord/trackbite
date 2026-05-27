#include "PlikManager.h"

#include <fstream>
#include <sstream>

namespace
{
	std::string poraPosilkuNaTekst(PoraPosilku pora)
	{
		switch (pora)
		{
		case PoraPosilku::Sniadanie:
			return "Sniadanie";

		case PoraPosilku::DrugieSniadanie:
			return "DrugieSniadanie";

		case PoraPosilku::Obiad:
			return "Obiad";

		case PoraPosilku::Kolacja:
			return "Kolacja";

		case PoraPosilku::Przekaski:
			return "Przekaski";

		default:
			return "Sniadanie";
		}
	}

	bool tekstNaPorePosilku(const std::string& tekst, PoraPosilku& pora)
	{
		if (tekst == "Sniadanie")
		{
			pora = PoraPosilku::Sniadanie;
			return true;
		}

		if (tekst == "DrugieSniadanie")
		{
			pora = PoraPosilku::DrugieSniadanie;
			return true;
		}

		if (tekst == "Obiad")
		{
			pora = PoraPosilku::Obiad;
			return true;
		}

		if (tekst == "Kolacja")
		{
			pora = PoraPosilku::Kolacja;
			return true;
		}

		if (tekst == "Przekaski")
		{
			pora = PoraPosilku::Przekaski;
			return true;
		}

		return false;
	}

	bool parsujDouble(const std::string& tekst, double& wynik)
	{
		try
		{
			size_t pozycja = 0;
			wynik = std::stod(tekst, &pozycja);
			return pozycja == tekst.size();
		}
		catch (...)
		{
			return false;
		}
	}

	bool parsujJednostke(const std::string& tekst, JednostkaProduktu& jednostka)
	{
		const size_t pozycjaDwukropka = tekst.find(':');

		if (pozycjaDwukropka == std::string::npos)
		{
			jednostka = { tekst, 1.0 };
			return !tekst.empty();
		}

		const std::string nazwa = tekst.substr(0, pozycjaDwukropka);
		const std::string gramyTekst = tekst.substr(pozycjaDwukropka + 1);

		double gramy = 0.0;

		if (nazwa.empty() || !parsujDouble(gramyTekst, gramy) || gramy <= 0.0)
		{
			return false;
		}

		jednostka = { nazwa, gramy };
		return true;
	}
}

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

	if (!out)
	{
		return false;
	}

	const Makroskladniki limity = dziennik.pobierzLimityDzienne();

	out << limity.kalorie << "|"
		<< limity.bialko << "|"
		<< limity.weglowodany << "|"
		<< limity.tluszcz << "\n";

	for (const PozycjaDziennika& pozycja : dziennik.pobierzPozycje())
	{
		const Makroskladniki makroNa100g = pozycja.pobierzMakroNa100g();
		const JednostkaProduktu jednostka = pozycja.pobierzJednostke();

		out << poraPosilkuNaTekst(pozycja.pobierzPorePosilku()) << "|"
			<< pozycja.pobierzNazweProduktu() << "|"
			<< pozycja.pobierzIlosc() << "|"
			<< jednostka.nazwa << ":" << jednostka.gramyNaJednostke << "|"
			<< makroNa100g.kalorie << "|"
			<< makroNa100g.bialko << "|"
			<< makroNa100g.weglowodany << "|"
			<< makroNa100g.tluszcz << "\n";
	}

	return true;
}

bool PlikManager::wczytajDziennik(const std::string& sciezka, DziennikZywieniowy& dziennik)
{
	std::ifstream in(sciezka);

	if (!in)
	{
		return false;
	}

	std::string line;

	if (!std::getline(in, line))
	{
		return false;
	}

	std::stringstream ss(line);
	std::string token;

	double limitKcal = 0.0;
	double limitBialko = 0.0;
	double limitWeglowodany = 0.0;
	double limitTluszcz = 0.0;

	if (!std::getline(ss, token, '|') || !parsujDouble(token, limitKcal)) return false;
	if (!std::getline(ss, token, '|') || !parsujDouble(token, limitBialko)) return false;
	if (!std::getline(ss, token, '|') || !parsujDouble(token, limitWeglowodany)) return false;
	if (!std::getline(ss, token, '|') || !parsujDouble(token, limitTluszcz)) return false;

	DziennikZywieniowy nowyDziennik;

	if (!nowyDziennik.ustawLimityDzienne({
		limitKcal,
		limitBialko,
		limitWeglowodany,
		limitTluszcz
		}))
	{
		return false;
	}

	while (std::getline(in, line))
	{
		if (line.empty())
		{
			continue;
		}

		std::stringstream ls(line);

		std::string poraTekst;
		std::string nazwa;
		std::string iloscTekst;
		std::string jednostkaTekst;
		std::string kcalTekst;
		std::string bialkoTekst;
		std::string weglowodanyTekst;
		std::string tluszczTekst;

		if (!std::getline(ls, poraTekst, '|')) continue;
		if (!std::getline(ls, nazwa, '|')) continue;
		if (!std::getline(ls, iloscTekst, '|')) continue;
		if (!std::getline(ls, jednostkaTekst, '|')) continue;
		if (!std::getline(ls, kcalTekst, '|')) continue;
		if (!std::getline(ls, bialkoTekst, '|')) continue;
		if (!std::getline(ls, weglowodanyTekst, '|')) continue;
		if (!std::getline(ls, tluszczTekst, '|')) continue;

		PoraPosilku pora;

		if (!tekstNaPorePosilku(poraTekst, pora))
		{
			continue;
		}

		double ilosc = 0.0;
		double kcal = 0.0;
		double bialko = 0.0;
		double weglowodany = 0.0;
		double tluszcz = 0.0;

		if (!parsujDouble(iloscTekst, ilosc)) continue;
		if (!parsujDouble(kcalTekst, kcal)) continue;
		if (!parsujDouble(bialkoTekst, bialko)) continue;
		if (!parsujDouble(weglowodanyTekst, weglowodany)) continue;
		if (!parsujDouble(tluszczTekst, tluszcz)) continue;

		JednostkaProduktu jednostka;

		if (!parsujJednostke(jednostkaTekst, jednostka))
		{
			continue;
		}

		const DziennikZywieniowy::WynikOperacji wynik = nowyDziennik.dodajPozycje(
			nazwa,
			ilosc,
			jednostka,
			kcal,
			bialko,
			weglowodany,
			tluszcz,
			pora
		);

		if (wynik != DziennikZywieniowy::WynikOperacji::Sukces)
		{
			continue;
		}
	}

	dziennik = nowyDziennik;
	return true;
}