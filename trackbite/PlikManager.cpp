#include "PlikManager.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

namespace
{
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
        QFile plik(QString::fromStdString(sciezka));

        if (!plik.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            return false;
        }

        const QJsonDocument dokument(root);
        plik.write(dokument.toJson(QJsonDocument::Indented));
        plik.close();

        return true;
    }

    bool wczytajJsonZPliku(const std::string& sciezka, QJsonObject& root)
    {
        QFile plik(QString::fromStdString(sciezka));

        if (!plik.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            return false;
        }

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

    QJsonObject makroDoJson(const Makroskladniki& makro)
    {
        QJsonObject obiekt;

        obiekt["kalorie"] = makro.kalorie;
        obiekt["bialko"] = makro.bialko;
        obiekt["weglowodany"] = makro.weglowodany;
        obiekt["tluszcz"] = makro.tluszcz;

        return obiekt;
    }

    Makroskladniki makroZJson(const QJsonObject& obiekt)
    {
        return {
            obiekt["kalorie"].toDouble(),
            obiekt["bialko"].toDouble(),
            obiekt["weglowodany"].toDouble(),
            obiekt["tluszcz"].toDouble()
        };
    }

    QJsonObject jednostkaDoJson(const JednostkaProduktu& jednostka)
    {
        QJsonObject obiekt;

        obiekt["nazwa"] = QString::fromStdString(jednostka.nazwa);
        obiekt["gramyNaJednostke"] = jednostka.gramyNaJednostke;

        return obiekt;
    }

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

bool PlikManager::zapiszProdukty(
    const std::string& sciezka,
    const std::vector<Produkt>& produkty
)
{
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

    return zapiszJsonDoPliku(sciezka, root);
}

bool PlikManager::wczytajProdukty(
    const std::string& sciezka,
    std::vector<Produkt>& produkty
)
{
    QJsonObject root;

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

bool PlikManager::zapiszProfil(
    const std::string& sciezka,
    const ProfilUzytkownika& profil
)
{
    QJsonObject root;

    root["imie"] = QString::fromStdString(profil.pobierzImie());
    root["wiek"] = profil.pobierzWiek();
    root["waga"] = profil.pobierzWage();
    root["wzrost"] = profil.pobierzWzrost();
    root["cel"] = QString::fromStdString(profil.pobierzCel());
    root["limitKalorii"] = profil.pobierzLimitKalorii();

    return zapiszJsonDoPliku(sciezka, root);
}

bool PlikManager::wczytajProfil(
    const std::string& sciezka,
    ProfilUzytkownika& profil
)
{
    QJsonObject root;

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

    profil = wczytanyProfil;
    return true;
}

bool PlikManager::zapiszDziennik(
    const std::string& sciezka,
    const DziennikZywieniowy& dziennik
)
{
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

    return zapiszJsonDoPliku(sciezka, root);
}

bool PlikManager::wczytajDziennik(
    const std::string& sciezka,
    DziennikZywieniowy& dziennik
)
{
    QJsonObject root;

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