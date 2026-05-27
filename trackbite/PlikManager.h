#pragma once

#include <string>
#include <vector>

// Menedżer do obsługi operacji na plikach
class PlikManager
{
public:
	// Typy plików wspierane
	enum class TypPliku
	{
		TekstowyProdukt,
		TekstowyProfil,
		TekstowyDziennik
	};

	PlikManager();

	// Odczyt pliku tekstowego
	static bool czytajPlik(const std::string& sciezka, std::vector<std::string>& linie);

	// Zapis pliku tekstowego
	static bool piszzPlik(const std::string& sciezka, const std::vector<std::string>& linie);

	// Czy plik istnieje
	static bool czyPlikIstnieje(const std::string& sciezka);

	// Usunięcie pliku
	static bool usunPlik(const std::string& sciezka);

	// Pobranie domyślnego katalogu dla danych
	static std::string pobierzKatalogDanych();

	// Walidacja ścieżki pliku
	static bool czySciezkaPoprawna(const std::string& sciezka);

private:
	static const std::string KATALOG_DANYCH;

	// Funkcje pomocnicze
	static std::string pobierzRozszerzeniePliku(const std::string& sciezka);
	static bool czyRozszerzeniePrawidlowe(const std::string& rozszerzenie);
};
