#include "ProfilUzytkownika.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

ProfilUzytkownika::ProfilUzytkownika()
    : imie(""), nazwisko(""), wiek(0.0), waga(0.0), wzrost(0.0),
    cel(0.0), wzorCe("MIFFLIN-JEOR")
{
    limityDzienne = { 2000.0, 50.0, 250.0, 65.0 };
}

// ==================== GETTERY ====================

const std::string& ProfilUzytkownika::pobierzImie() const
{
    return imie;
}

const std::string& ProfilUzytkownika::pobierzNazwisko() const
{
    return nazwisko;
}

double ProfilUzytkownika::pobierzWiek() const
{
    return wiek;
}

double ProfilUzytkownika::pobierzWage() const
{
    return waga;
}

double ProfilUzytkownika::pobierzWzrost() const
{
    return wzrost;
}

double ProfilUzytkownika::pobierzCel() const
{
    return cel;
}

const std::string& ProfilUzytkownika::pobierzWzorCe() const
{
    return wzorCe;
}

const Makroskladniki& ProfilUzytkownika::pobierzLimityDzienne() const
{
    return limityDzienne;
}

// ==================== SETTERY ====================

bool ProfilUzytkownika::ustawImie(const std::string& imie_)
{
    if (!czyImiePoprawne(imie_))
        return false;

    imie = imie_;
    return true;
}

bool ProfilUzytkownika::ustawNazwisko(const std::string& nazwisko_)
{
    if (!czySzwiskoPoprawne(nazwisko_))
        return false;

    nazwisko = nazwisko_;
    return true;
}

bool ProfilUzytkownika::ustawWiek(double wiek_)
{
    if (!czyWiekPoprawny(wiek_))
        return false;

    wiek = wiek_;
    return true;
}

bool ProfilUzytkownika::ustawWage(double waga_)
{
    if (!czyWagaPoprawna(waga_))
        return false;

    waga = waga_;
    return true;
}

bool ProfilUzytkownika::ustawWzrost(double wzrost_)
{
    if (!czyWzrostPoprawny(wzrost_))
        return false;

    wzrost = wzrost_;
    return true;
}

bool ProfilUzytkownika::ustawCel(double cel_)
{
    if (!czyCelPoprawny(cel_))
        return false;

    cel = cel_;
    return true;
}

bool ProfilUzytkownika::ustawWzorCe(const std::string& wzor)
{
    if (wzor != "MIFFLIN-JEOR" && wzor != "HARRIS-BENEDICT")
        return false;

    wzorCe = wzor;
    return true;
}

// ==================== OBLICZENIA ====================

bool ProfilUzytkownika::obliczLimityNaPostawieCelu(double cel_)
{
    if (!czyCelPoprawny(cel_))
        return false;

    cel = cel_;
    limityDzienne.kalorie = cel_;

    // Standardowe podziały makroskładników (% kalorii)
    // Białko: 30% (~1,2 g/kg)
    // Węglowodany: 45%
    // Tłuszcz: 25%
    limityDzienne.bialko = (cel_ * 0.30) / 4.0;      // 4 kcal/g
    limityDzienne.weglowodany = (cel_ * 0.45) / 4.0; // 4 kcal/g
    limityDzienne.tluszcz = (cel_ * 0.25) / 9.0;     // 9 kcal/g

    return true;
}

// ==================== WALIDACJA ====================

bool ProfilUzytkownika::czyImiePoprawne(const std::string& imie_) const
{
    if (imie_.empty() || imie_.length() > 50)
        return false;

    for (char c : imie_)
    {
        if (!std::isalpha(c) && c != '-' && c != ' ')
            return false;
    }

    return true;
}

bool ProfilUzytkownika::czySzwiskoPoprawne(const std::string& nazwisko_) const
{
    if (nazwisko_.empty() || nazwisko_.length() > 50)
        return false;

    for (char c : nazwisko_)
    {
        if (!std::isalpha(c) && c != '-' && c != ' ')
            return false;
    }

    return true;
}

bool ProfilUzytkownika::czyWiekPoprawny(double wiek_) const
{
    return wiek_ > 0.0 && wiek_ < 150.0;
}

bool ProfilUzytkownika::czyWagaPoprawna(double waga_) const
{
    return waga_ > 20.0 && waga_ < 300.0;
}

bool ProfilUzytkownika::czyWzrostPoprawny(double wzrost_) const
{
    return wzrost_ > 100.0 && wzrost_ < 250.0;
}

bool ProfilUzytkownika::czyCelPoprawny(double cel_) const
{
    return cel_ > 500.0 && cel_ < 10000.0;
}

bool ProfilUzytkownika::czyProfilPoprawny() const
{
    return !imie.empty() && !nazwisko.empty() &&
        czyWiekPoprawny(wiek) && czyWagaPoprawna(waga) &&
        czyWzrostPoprawny(wzrost) && czyCelPoprawny(cel);
}

// ==================== ZAPIS I ODCZYT ====================

bool ProfilUzytkownika::zapiszProfil(const std::string& sciezkaPliku) const
{
    if (!czyProfilPoprawny())
        return false;

    return zapiszDoPlikuTekstowego(sciezkaPliku);
}

bool ProfilUzytkownika::odczytajProfil(const std::string& sciezkaPliku)
{
    return odczytajZPlikuTekstowego(sciezkaPliku);
}

bool ProfilUzytkownika::zapiszDoPlikuTekstowego(const std::string& sciezkaPliku) const
{
    std::ofstream plik(sciezkaPliku);

    if (!plik.is_open())
        return false;

    plik << "PROFIL_UZYTKOWNIKA" << "\n";
    plik << "imie=" << imie << "\n";
    plik << "nazwisko=" << nazwisko << "\n";
    plik << "wiek=" << wiek << "\n";
    plik << "waga=" << waga << "\n";
    plik << "wzrost=" << wzrost << "\n";
    plik << "cel=" << cel << "\n";
    plik << "wzor=" << wzorCe << "\n";
    plik << "kalorie=" << limityDzienne.kalorie << "\n";
    plik << "bialko=" << limityDzienne.bialko << "\n";
    plik << "weglowodany=" << limityDzienne.weglowodany << "\n";
    plik << "tluszcz=" << limityDzienne.tluszcz << "\n";

    plik.close();
    return true;
}

bool ProfilUzytkownika::odczytajZPlikuTekstowego(const std::string& sciezkaPliku)
{
    std::ifstream plik(sciezkaPliku);

    if (!plik.is_open())
        return false;

    std::string linia;
    if (!std::getline(plik, linia) || linia != "PROFIL_UZYTKOWNIKA")
    {
        plik.close();
        return false;
    }

    std::string imie_temp, nazwisko_temp;
    double wiek_temp = 0.0, waga_temp = 0.0, wzrost_temp = 0.0, cel_temp = 0.0;
    std::string wzor_temp = "MIFFLIN-JEOR";
    Makroskladniki limity_temp;

    while (std::getline(plik, linia))
    {
        std::istringstream iss(linia);
        std::string klucz;
        std::string wartosc_str;

        if (!std::getline(iss, klucz, '='))
            continue;

        if (!std::getline(iss, wartosc_str))
            continue;

        if (klucz == "imie")
            imie_temp = wartosc_str;
        else if (klucz == "nazwisko")
            nazwisko_temp = wartosc_str;
        else if (klucz == "wiek")
            wiek_temp = std::stod(wartosc_str);
        else if (klucz == "waga")
            waga_temp = std::stod(wartosc_str);
        else if (klucz == "wzrost")
            wzrost_temp = std::stod(wartosc_str);
        else if (klucz == "cel")
            cel_temp = std::stod(wartosc_str);
        else if (klucz == "wzor")
            wzor_temp = wartosc_str;
        else if (klucz == "kalorie")
            limity_temp.kalorie = std::stod(wartosc_str);
        else if (klucz == "bialko")
            limity_temp.bialko = std::stod(wartosc_str);
        else if (klucz == "weglowodany")
            limity_temp.weglowodany = std::stod(wartosc_str);
        else if (klucz == "tluszcz")
            limity_temp.tluszcz = std::stod(wartosc_str);
    }

    plik.close();

    // Walidacja przed zatwierdzeniem
    if (!czyImiePoprawne(imie_temp) || !czySzwiskoPoprawne(nazwisko_temp) ||
        !czyWiekPoprawny(wiek_temp) || !czyWagaPoprawna(waga_temp) ||
        !czyWzrostPoprawny(wzrost_temp) || !czyCelPoprawny(cel_temp))
    {
        return false;
    }

    imie = imie_temp;
    nazwisko = nazwisko_temp;
    wiek = wiek_temp;
    waga = waga_temp;
    wzrost = wzrost_temp;
    cel = cel_temp;
    wzorCe = wzor_temp;
    limityDzienne = limity_temp;

    return true;
}
