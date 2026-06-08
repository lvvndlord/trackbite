// Implementacja zapisu i odczytu danych aplikacji w formacie JSON.
// PlikManager oddziela logikę plików od klas domenowych i interfejsu.
#include "PlikManager.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

namespace
{
    // Zamienia enum pory posiłku na tekst zapisywany w JSON-ie.
    QString poraPosilkuNaTekst(PoraPosilku pora)
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

    // Odwrotność funkcji powyżej: tekst z pliku zmienia z powrotem na enum używany w programie.
    bool tekstNaPorePosilku(const QString& tekst, PoraPosilku& pora)
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

    bool zapiszJsonDoPliku(const std::string& sciezka, const QJsonObject& root)
    {
        // Otwieramy plik tekstowo, bo JSON ma być czytelny również poza programem.
        QFile plik(QString::fromStdString(sciezka));

        if (!plik.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            return false;
        }

        const QJsonDocument dokument(root);
        // Indented zostawia plik czytelny dla człowieka, co pomaga przy testach i debugowaniu.
        plik.write(dokument.toJson(QJsonDocument::Indented));
        plik.close();

        return true;
    }

    bool wczytajJsonZPliku(const std::string& sciezka, QJsonObject& root)
    {
        // Brak pliku nie jest awarią programu — metoda zwróci false, a aplikacja użyje danych domyślnych.
        QFile plik(QString::fromStdString(sciezka));

        if (!plik.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            return false;
        }

        // Czytamy cały plik naraz, bo dane aplikacji są małe i proste.
        const QByteArray dane = plik.readAll();
        plik.close();

        QJsonParseError bladParsowania;
        const QJsonDocument dokument = QJsonDocument::fromJson(dane, &bladParsowania);

        if (bladParsowania.error != QJsonParseError::NoError || !dokument.isObject())
        {
            return false;
        }

        root = dokument.object();
        return true;
    }

    // Proste mapowanie struktury makro na obiekt JSON. Nazwy pól muszą być zgodne przy zapisie i odczycie.
    QJsonObject makroDoJson(const Makroskladniki& makro)
    {
        QJsonObject obiekt;

        obiekt["kalorie"] = makro.kalorie;
        obiekt["bialko"] = makro.bialko;
        obiekt["weglowodany"] = makro.weglowodany;
        obiekt["tluszcz"] = makro.tluszcz;

        return obiekt;
    }

    // Odczyt makro z JSON-a. Brakujące wartości Qt zamieni na 0, a dalsza walidacja odfiltruje błędne rekordy.
    Makroskladniki makroZJson(const QJsonObject& obiekt)
    {
        return {
            obiekt["kalorie"].toDouble(),
            obiekt["bialko"].toDouble(),
            obiekt["weglowodany"].toDouble(),
            obiekt["tluszcz"].toDouble()
        };
    }

    // Jednostka produktu jest zapisywana jako nazwa oraz przelicznik gramów.
    QJsonObject jednostkaDoJson(const JednostkaProduktu& jednostka)
    {
        QJsonObject obiekt;

        obiekt["nazwa"] = QString::fromStdString(jednostka.nazwa);
        obiekt["gramyNaJednostke"] = jednostka.gramyNaJednostke;

        return obiekt;
    }

    // Przy odczycie jednostki od razu odrzucamy puste nazwy i niepoprawne przeliczniki.
    bool jednostkaZJson(const QJsonObject& obiekt, JednostkaProduktu& jednostka)
    {
        const QString nazwa = obiekt["nazwa"].toString();
        const double gramyNaJednostke = obiekt["gramyNaJednostke"].toDouble();

        if (nazwa.trimmed().isEmpty() || gramyNaJednostke <= 0.0)
        {
            return false;
        }

        jednostka = {
            nazwa.toStdString(),
            gramyNaJednostke
        };

        return true;
    }
}

// Zapis produktów obejmuje nazwę, makro, stan ulubionego oraz wszystkie zdefiniowane jednostki.
bool PlikManager::zapiszProdukty(
    const std::string& sciezka,
    const std::vector<Produkt>& produkty
)
{
    // Budujemy tablicę JSON, w której każdy element odpowiada jednemu produktowi z bazy.
    QJsonArray produktyArray;

    for (const Produkt& produkt : produkty)
    {
        QJsonObject produktJson;

        produktJson["nazwa"] = QString::fromStdString(produkt.pobierzNazwe());
        produktJson["makroNa100g"] = makroDoJson(produkt.pobierzMakroNa100g());
        produktJson["ulubiony"] = produkt.czyUlubiony();

        QJsonArray jednostkiArray;

        for (const JednostkaProduktu& jednostka : produkt.pobierzJednostki())
        {
            jednostkiArray.append(jednostkaDoJson(jednostka));
        }

        produktJson["jednostki"] = jednostkiArray;

        produktyArray.append(produktJson);
    }

    QJsonObject root;
    root["produkty"] = produktyArray;

    // Fizyczny zapis jest przeniesiony do wspólnej funkcji pomocniczej.
    return zapiszJsonDoPliku(sciezka, root);
}

// Wczytywanie produktów jest odporne na częściowo uszkodzony plik: błędne rekordy są pomijane.
bool PlikManager::wczytajProdukty(
    const std::string& sciezka,
    std::vector<Produkt>& produkty
)
{
    QJsonObject root;

    // Jeśli pliku nie ma albo JSON jest uszkodzony, zwracamy false i nie nadpisujemy aktualnych danych.
    if (!wczytajJsonZPliku(sciezka, root))
    {
        return false;
    }

    if (!root.contains("produkty") || !root["produkty"].isArray())
    {
        return false;
    }

    std::vector<Produkt> wczytaneProdukty;
    const QJsonArray produktyArray = root["produkty"].toArray();

    for (const QJsonValue& produktValue : produktyArray)
    {
        if (!produktValue.isObject())
        {
            continue;
        }

        const QJsonObject produktJson = produktValue.toObject();

        const std::string nazwa = produktJson["nazwa"].toString().toStdString();

        if (!produktJson["makroNa100g"].isObject())
        {
            continue;
        }

        const Makroskladniki makroNa100g =
            makroZJson(produktJson["makroNa100g"].toObject());

        Produkt produkt(nazwa, makroNa100g);
        produkt.ustawUlubiony(produktJson["ulubiony"].toBool(false));

        if (produktJson["jednostki"].isArray())
        {
            const QJsonArray jednostkiArray = produktJson["jednostki"].toArray();

            for (const QJsonValue& jednostkaValue : jednostkiArray)
            {
                if (!jednostkaValue.isObject())
                {
                    continue;
                }

                JednostkaProduktu jednostka;

                if (jednostkaZJson(jednostkaValue.toObject(), jednostka))
                {
                    produkt.dodajJednostke(
                        jednostka.nazwa,
                        jednostka.gramyNaJednostke
                    );
                }
            }
        }

        if (produkt.czyPoprawny())
        {
            wczytaneProdukty.push_back(produkt);
        }
    }

    produkty = wczytaneProdukty;
    return true;
}

// Profil użytkownika zapisujemy jako pojedynczy obiekt JSON, bo to jeden zestaw danych, a nie lista.
bool PlikManager::zapiszProfil(
    const std::string& sciezka,
    const ProfilUzytkownika& profil
)
{
    // Każde pole profilu dostaje własny klucz, co upraszcza późniejsze wczytywanie.
    QJsonObject root;

    root["imie"] = QString::fromStdString(profil.pobierzImie());
    root["wiek"] = profil.pobierzWiek();
    root["waga"] = profil.pobierzWage();
    root["wzrost"] = profil.pobierzWzrost();
    root["cel"] = QString::fromStdString(profil.pobierzCel());
    root["limitKalorii"] = profil.pobierzLimitKalorii();
    root["wagaDocelowa"] = profil.pobierzWageDocelowa();
    root["tempoZmianyWagiTygodniowo"] = profil.pobierzTempoZmianyWagiTygodniowo();

    // Zwracamy wynik zapisu, żeby wywołujący mógł zareagować na problem z plikiem.
    return zapiszJsonDoPliku(sciezka, root);
}

// Przy odczycie profilu korzystamy z setterów, więc zakresy liczb są nadal pilnowane przez klasę profilu.
bool PlikManager::wczytajProfil(
    const std::string& sciezka,
    ProfilUzytkownika& profil
)
{
    QJsonObject root;

    // Niepoprawny lub brakujący plik oznacza powrót do danych domyślnych w aplikacji.
    if (!wczytajJsonZPliku(sciezka, root))
    {
        return false;
    }

    ProfilUzytkownika wczytanyProfil;

    wczytanyProfil.ustawImie(root["imie"].toString().toStdString());
    wczytanyProfil.ustawWiek(root["wiek"].toInt());
    wczytanyProfil.ustawWage(root["waga"].toDouble());
    wczytanyProfil.ustawWzrost(root["wzrost"].toDouble());
    wczytanyProfil.ustawCel(root["cel"].toString().toStdString());
    wczytanyProfil.ustawLimitKalorii(root["limitKalorii"].toDouble(2000.0));
    wczytanyProfil.ustawWageDocelowa(root["wagaDocelowa"].toDouble(0.0));
    wczytanyProfil.ustawTempoZmianyWagiTygodniowo(root["tempoZmianyWagiTygodniowo"].toDouble(0.0));

    profil = wczytanyProfil;
    return true;
}

// Dziennik zapisuje limity oraz listę pozycji z podziałem na pory posiłku.
bool PlikManager::zapiszDziennik(
    const std::string& sciezka,
    const DziennikZywieniowy& dziennik
)
{
    // Limity zapisujemy razem z pozycjami, aby każdy dzień mógł mieć własny stan podsumowania.
    QJsonObject root;

    root["limityDzienne"] = makroDoJson(dziennik.pobierzLimityDzienne());

    QJsonArray pozycjeArray;

    for (const PozycjaDziennika& pozycja : dziennik.pobierzPozycje())
    {
        QJsonObject pozycjaJson;

        pozycjaJson["poraPosilku"] = poraPosilkuNaTekst(pozycja.pobierzPorePosilku());
        pozycjaJson["nazwaProduktu"] =
            QString::fromStdString(pozycja.pobierzNazweProduktu());
        pozycjaJson["ilosc"] = pozycja.pobierzIlosc();
        pozycjaJson["jednostka"] = jednostkaDoJson(pozycja.pobierzJednostke());
        pozycjaJson["makroNa100g"] = makroDoJson(pozycja.pobierzMakroNa100g());

        pozycjeArray.append(pozycjaJson);
    }

    root["pozycje"] = pozycjeArray;

    // Zapis trafia do pliku zależnego od daty, co pozwala przełączać dni w aplikacji.
    return zapiszJsonDoPliku(sciezka, root);
}

// Odczyt dziennika odtwarza obiekt przez publiczne metody, dzięki czemu działa ta sama walidacja co w UI.
bool PlikManager::wczytajDziennik(
    const std::string& sciezka,
    DziennikZywieniowy& dziennik
)
{
    QJsonObject root;

    // Jeśli plik dziennika nie istnieje dla danej daty, aplikacja zacznie od pustego dnia.
    if (!wczytajJsonZPliku(sciezka, root))
    {
        return false;
    }

    if (!root["limityDzienne"].isObject())
    {
        return false;
    }

    DziennikZywieniowy wczytanyDziennik;

    const Makroskladniki limity =
        makroZJson(root["limityDzienne"].toObject());

    if (!wczytanyDziennik.ustawLimityDzienne(limity))
    {
        return false;
    }

    if (root["pozycje"].isArray())
    {
        const QJsonArray pozycjeArray = root["pozycje"].toArray();

        for (const QJsonValue& pozycjaValue : pozycjeArray)
        {
            if (!pozycjaValue.isObject())
            {
                continue;
            }

            const QJsonObject pozycjaJson = pozycjaValue.toObject();

            PoraPosilku pora;

            if (!tekstNaPorePosilku(
                pozycjaJson["poraPosilku"].toString(),
                pora
            ))
            {
                continue;
            }

            if (!pozycjaJson["jednostka"].isObject() ||
                !pozycjaJson["makroNa100g"].isObject())
            {
                continue;
            }

            JednostkaProduktu jednostka;

            if (!jednostkaZJson(pozycjaJson["jednostka"].toObject(), jednostka))
            {
                continue;
            }

            const std::string nazwaProduktu =
                pozycjaJson["nazwaProduktu"].toString().toStdString();

            const double ilosc = pozycjaJson["ilosc"].toDouble();

            const Makroskladniki makroNa100g =
                makroZJson(pozycjaJson["makroNa100g"].toObject());

            const DziennikZywieniowy::WynikOperacji wynik =
                wczytanyDziennik.dodajPozycje(
                    nazwaProduktu,
                    ilosc,
                    jednostka,
                    makroNa100g.kalorie,
                    makroNa100g.bialko,
                    makroNa100g.weglowodany,
                    makroNa100g.tluszcz,
                    pora
                );

            if (wynik != DziennikZywieniowy::WynikOperacji::Sukces)
            {
                continue;
            }
        }
    }

    dziennik = wczytanyDziennik;
    return true;
}