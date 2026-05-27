#include "PlikManager.h"
#include <fstream>
#include <filesystem>
#include <algorithm>

const std::string PlikManager::KATALOG_DANYCH = "./dane/";

PlikManager::PlikManager()
{
	// Utwórz katalog, jeśli nie istnieje
	if (!std::filesystem::exists(KATALOG_DANYCH))
	{
		std::filesystem::create_directories(KATALOG_DANYCH);
	}
}

bool PlikManager::czytajPlik(const std::string& sciezka, std::vector<std::string>& linie)
{
	if (!czySciezkaPoprawna(sciezka) || !czyPlikIstnieje(sciezka))
		return false;

	std::ifstream plik(sciezka);

	if (!plik.is_open())
		return false;

	std::string linia;
	while (std::getline(plik, linia))
	{
		linie.push_back(linia);
	}

	plik.close();
	return true;
}

bool PlikManager::piszzPlik(const std::string& sciezka, const std::vector<std::string>& linie)
{
	if (!czySciezkaPoprawna(sciezka))
		return false;

	std::ofstream plik(sciezka);

	if (!plik.is_open())
		return false;

	for (const auto& linia : linie)
	{
		plik << linia << "\n";
	}

	plik.close();
	return true;
}

bool PlikManager::czyPlikIstnieje(const std::string& sciezka)
{
	return std::filesystem::exists(sciezka);
}

bool PlikManager::usunPlik(const std::string& sciezka)
{
	if (!czyPlikIstnieje(sciezka))
		return false;

	try
	{
		std::filesystem::remove(sciezka);
		return true;
	}
	catch (...)
	{
		return false;
	}
}

std::string PlikManager::pobierzKatalogDanych()
{
	return KATALOG_DANYCH;
}

bool PlikManager::czySciezkaPoprawna(const std::string& sciezka)
{
	if (sciezka.empty() || sciezka.length() > 260)
		return false;

	// Sprawdź, czy rozszerzenie jest txt
	std::string rozszerzenie = pobierzRozszerzeniePliku(sciezka);
	return czyRozszerzeniePrawidlowe(rozszerzenie);
}

std::string PlikManager::pobierzRozszerzeniePliku(const std::string& sciezka)
{
	size_t pos = sciezka.find_last_of('.');
	if (pos == std::string::npos)
		return "";

	std::string rozszerzenie = sciezka.substr(pos);
	std::transform(rozszerzenie.begin(), rozszerzenie.end(),
		rozszerzenie.begin(), ::tolower);

	return rozszerzenie;
}

bool PlikManager::czyRozszerzeniePrawidlowe(const std::string& rozszerzenie)
{
	return rozszerzenie == ".txt";
}
