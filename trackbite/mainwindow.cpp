#include "mainwindow.h"
#include <QAbstractItemView>
#include <QDate>
#include <QHeaderView>
#include <QMessageBox>
#include <QProgressBar>
#include <QTableWidgetItem>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    // Wczytanie danych z plikow przy starcie
    wczytajDaneZPlikow();

    ustawDziennikGui();
    if (produkty.empty())
    {
        dodajProduktyTestoweDoCombo();
    }
    odswiezDziennik();
}

MainWindow::~MainWindow()
{
    // Zapisz dane przed zamknieciem
    zapiszDaneDoPlikow();
}

void MainWindow::ustawDziennikGui()
{
    ui.doubleSpinIlosc->setRange(1.0, 5000.0);
    ui.doubleSpinIlosc->setDecimals(0);
    ui.doubleSpinIlosc->setValue(100.0);

    // Walidacja i zakresy pól profilu
    ui.spinWiek->setRange(8, 120); // wiek 8..120
    ui.doubleSpinWaga->setRange(20.0, 300.0); // waga 20..300 kg
    ui.doubleSpinWaga->setDecimals(1);
    ui.doubleSpinWzrost->setRange(50.0, 250.0); // wzrost 50..250 cm
    ui.doubleSpinWzrost->setDecimals(1);
    ui.doubleSpinLimitKalorii->setRange(800.0, 10000.0); // rozsądny zakres kalorii
    ui.doubleSpinLimitKalorii->setDecimals(0);

    ui.tableDziennik->setColumnCount(8);
    ui.tableDziennik->setHorizontalHeaderLabels(
        { "Produkt", "Ilosc", "Jednostka", "Gramy", "kcal", "Bialko", "Weglowodany", "Tluszcz" }
    );

    ui.tableDziennik->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.tableDziennik->verticalHeader()->setVisible(false);
    ui.tableDziennik->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.tableDziennik->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.tableDziennik->setEditTriggers(QAbstractItemView::NoEditTriggers);

    for (QProgressBar* pasek : {
        ui.progressKalorie,
        ui.progressBialko,
        ui.progressWeglowodany,
        ui.progressTluszcz
        })
    {
        pasek->setRange(0, 100);
        pasek->setValue(0);
        pasek->setTextVisible(true);
    }
}

void MainWindow::dodajProduktyTestoweDoCombo()
{
    ui.comboProduktDziennik->clear();
    ui.comboJednostkaDziennik->clear();
    produkty.clear();

    auto dodajProdukt = [this](
        const QString& nazwa,
        double kcal,
        double bialko,
        double weglowodany,
        double tluszcz,
        const std::vector<JednostkaProduktu>& dodatkoweJednostki
        )
        {
            Produkt produkt(
                nazwa.toStdString(),
                Makroskladniki{ kcal, bialko, weglowodany, tluszcz }
            );

            for (const JednostkaProduktu& jednostka : dodatkoweJednostki)
            {
                produkt.dodajJednostke(jednostka.nazwa, jednostka.gramyNaJednostke);
            }

            produkty.push_back(produkt);
            ui.comboProduktDziennik->addItem(
                nazwa,
                static_cast<int>(produkty.size() - 1)
            );
        };

    // Jeżeli istnieje plik produkty.txt w katalogu, wczytaj zamiast testowych
    std::vector<Produkt> zPliku;
    if (PlikManager::wczytajProdukty("produkty.txt", zPliku))
    {
        produkty = std::move(zPliku);
        for (std::size_t i = 0; i < produkty.size(); ++i)
        {
            ui.comboProduktDziennik->addItem(QString::fromStdString(produkty[i].pobierzNazwe()), static_cast<int>(i));
        }
        if (ui.comboProduktDziennik->count() > 0)
        {
            ui.comboProduktDziennik->setCurrentIndex(0);
            on_comboProduktDziennik_currentIndexChanged(ui.comboProduktDziennik->currentIndex());
        }
        return;
        }

        // domyslne produkty (gdy brak pliku)
    dodajProdukt(
        "Jajko",
        155.0, 13.0, 1.1, 11.0,
        std::vector<JednostkaProduktu>{
            { "sztuka", 55.0 }
        }
    );

    dodajProdukt(
        "Ryz bialy",
        130.0, 2.7, 28.0, 0.3,
        std::vector<JednostkaProduktu>{
            { "opakowanie", 500.0 }
        }
    );

    dodajProdukt(
        "Ser zolty",
        350.0, 25.0, 2.0, 27.0,
        std::vector<JednostkaProduktu>{
            { "plaster", 20.0 },
            { "opakowanie", 150.0 }
        }
    );

    dodajProdukt(
        "Banan",
        89.0, 1.1, 23.0, 0.3,
        std::vector<JednostkaProduktu>{
            { "sztuka", 120.0 }
        }
    );

    if (ui.comboProduktDziennik->count() > 0)
    {
        ui.comboProduktDziennik->setCurrentIndex(0);
        on_comboProduktDziennik_currentIndexChanged(ui.comboProduktDziennik->currentIndex());
    }

    // ustaw domyslny index i laduj jednostki
    if (ui.comboProduktDziennik->count() > 0)
    {
        ui.comboProduktDziennik->setCurrentIndex(0);
        on_comboProduktDziennik_currentIndexChanged(0);
    }
}

// Wczytywanie / zapisywanie danych
void MainWindow::wczytajDaneZPlikow()
{
    // Profil
    ProfilUzytkownika p;
    if (PlikManager::wczytajProfil("profil.txt", p))
    {
        profil = p;
    }

    // Baza produktow
    std::vector<Produkt> pb;
    PlikManager::wczytajProdukty("produkty.txt", pb);
    if (!pb.empty()) produkty = std::move(pb);

    // Dziennik
    PlikManager::wczytajDziennik("dziennik.txt", dziennik);
}

void MainWindow::zapiszDaneDoPlikow()
{
    PlikManager::zapiszProfil("profil.txt", profil);
    PlikManager::zapiszProdukty("produkty.txt", produkty);
    PlikManager::zapiszDziennik("dziennik.txt", dziennik);
}

void MainWindow::on_comboProduktDziennik_currentIndexChanged(int index)
{
    if (aktualizujeUi) return;

    ui.comboJednostkaDziennik->clear();

    if (index < 0)
    {
        return;
    }

    const int indeksProduktu = ui.comboProduktDziennik->itemData(index).toInt();

    if (indeksProduktu < 0 || indeksProduktu >= static_cast<int>(produkty.size()))
    {
        return;
    }

    const std::vector<JednostkaProduktu>& jednostki =
        produkty[static_cast<std::size_t>(indeksProduktu)].pobierzJednostki();

    for (std::size_t i = 0; i < jednostki.size(); ++i)
    {
        const JednostkaProduktu& jednostka = jednostki[i];

        ui.comboJednostkaDziennik->addItem(
            QString::fromStdString(jednostka.nazwa),
            static_cast<int>(i)
        );
    }
}

void MainWindow::odswiezDziennik()
{
    const std::vector<PozycjaDziennika>& pozycje = dziennik.pobierzPozycje();

    ui.tableDziennik->setRowCount(static_cast<int>(pozycje.size()));

    for (int i = 0; i < static_cast<int>(pozycje.size()); ++i)
    {
        const PozycjaDziennika& pozycja = pozycje[i];
        const Makroskladniki makro = pozycja.obliczMakro();

        ui.tableDziennik->setItem(i, 0, new QTableWidgetItem(
            QString::fromStdString(pozycja.pobierzNazweProduktu())
        ));

        ui.tableDziennik->setItem(i, 1, new QTableWidgetItem(
            QString::number(pozycja.pobierzIlosc(), 'f', 2)
        ));

        ui.tableDziennik->setItem(i, 2, new QTableWidgetItem(
            QString::fromStdString(pozycja.pobierzJednostke().nazwa)
        ));

        ui.tableDziennik->setItem(i, 3, new QTableWidgetItem(
            QString::number(pozycja.pobierzGramy(), 'f', 0)
        ));

        ui.tableDziennik->setItem(i, 4, new QTableWidgetItem(
            QString::number(makro.kalorie, 'f', 0)
        ));

        ui.tableDziennik->setItem(i, 5, new QTableWidgetItem(
            QString::number(makro.bialko, 'f', 1)
        ));

        ui.tableDziennik->setItem(i, 6, new QTableWidgetItem(
            QString::number(makro.weglowodany, 'f', 1)
        ));

        ui.tableDziennik->setItem(i, 7, new QTableWidgetItem(
            QString::number(makro.tluszcz, 'f', 1)
        ));
    }

    const Makroskladniki suma = dziennik.obliczSume();
    const Makroskladniki limity = dziennik.pobierzLimityDzienne();

    ui.labelDataDnia->setText(
        "Dziennik - " + QDate::currentDate().toString("dd.MM.yyyy")
    );

    ui.labelLimitKalorii_2->setText(
        QString("Limit kalorii: %1 kcal").arg(limity.kalorie, 0, 'f', 0)
    );

    ui.labelSpozytoKalorii->setText(
        QString("Spozyto: %1 kcal").arg(suma.kalorie, 0, 'f', 0)
    );

    ui.labelZostaloKalorii->setText(
        QString("Zostalo: %1 kcal").arg(dziennik.pozostaleKalorie(), 0, 'f', 0)
    );

    ui.labelSumaKalorii->setText(
        QString("Kalorie: %1").arg(suma.kalorie, 0, 'f', 0)
    );

    ui.labelSumaBialka->setText(
        QString("Bialko: %1 g").arg(suma.bialko, 0, 'f', 1)
    );

    ui.labelSumaWeglowodanow->setText(
        QString("Weglowodany: %1 g").arg(suma.weglowodany, 0, 'f', 1)
    );

    ui.labelSumaTluszczu->setText(
        QString("Tluszcze: %1 g").arg(suma.tluszcz, 0, 'f', 1)
    );

    auto ustawProgress = [](QProgressBar* pasek, int procent)
        {
            const int wartoscPaska = std::clamp(procent, 0, 100);

            pasek->setValue(wartoscPaska);
            pasek->setFormat(QString::number(procent) + "%");
        };

    ustawProgress(ui.progressKalorie, dziennik.procentKalorii());
    ustawProgress(ui.progressBialko, dziennik.procentBialka());
    ustawProgress(ui.progressWeglowodany, dziennik.procentWeglowodanow());
    ustawProgress(ui.progressTluszcz, dziennik.procentTluszczu());

    // Jeżeli profil wczytany, ustaw limit kalorii
    if (!profil.pobierzImie().empty())
    {
        // zabezpiecz przed reentry
        aktualizujeUi = true;

        dziennik.ustawLimitKalorii(profil.pobierzLimitKalorii());

        // Wypelnij pola UI danymi profilu
        ui.lineEditImie->setText(QString::fromStdString(profil.pobierzImie()));
        ui.spinWiek->setValue(profil.pobierzWiek());
        ui.doubleSpinWaga->setValue(profil.pobierzWage());
        ui.doubleSpinWzrost->setValue(profil.pobierzWzrost());
        ui.doubleSpinLimitKalorii->setValue(profil.pobierzLimitKalorii());

        aktualizujeUi = false;
    }
}

void MainWindow::on_buttonDodajDoDziennika_clicked()
{
    if (ui.comboProduktDziennik->currentIndex() < 0)
    {
        QMessageBox::warning(this, "Blad", "Nie wybrano produktu.");
        return;
    }

    if (ui.comboJednostkaDziennik->currentIndex() < 0)
    {
        QMessageBox::warning(this, "Blad", "Nie wybrano jednostki.");
        return;
    }

    const int indeksProduktu = ui.comboProduktDziennik->currentData().toInt();

    if (indeksProduktu < 0 || indeksProduktu >= static_cast<int>(produkty.size()))
    {
        QMessageBox::critical(this, "Blad", "Produkt ma niepoprawne dane.");
        return;
    }

    const Produkt& produkt = produkty[static_cast<std::size_t>(indeksProduktu)];
    const std::vector<JednostkaProduktu>& jednostki = produkt.pobierzJednostki();
    const int indeksJednostki = ui.comboJednostkaDziennik->currentData().toInt();

    if (indeksJednostki < 0 || indeksJednostki >= static_cast<int>(jednostki.size()))
    {
        QMessageBox::critical(this, "Blad", "Jednostka ma niepoprawne dane.");
        return;
    }

    const QString nazwaProduktu = ui.comboProduktDziennik->currentText();
    const double ilosc = ui.doubleSpinIlosc->value();
    const Makroskladniki& makro = produkt.pobierzMakroNa100g();
    const JednostkaProduktu& jednostka = jednostki[static_cast<std::size_t>(indeksJednostki)];

    const DziennikZywieniowy::WynikOperacji wynik = dziennik.dodajPozycje(
        nazwaProduktu.toStdString(),
        ilosc,
        jednostka,
        makro.kalorie,
        makro.bialko,
        makro.weglowodany,
        makro.tluszcz
    );

    if (wynik != DziennikZywieniowy::WynikOperacji::Sukces)
    {
        QMessageBox::warning(this, "Blad danych", komunikatBledu(wynik));
        return;
    }

    // Autozapis dziennika po dodaniu
    PlikManager::zapiszDziennik("dziennik.txt", dziennik);

    odswiezDziennik();
}

void MainWindow::on_buttonUsunZDziennika_clicked()
{
    const int zaznaczonyWiersz = ui.tableDziennik->currentRow();

    if (zaznaczonyWiersz < 0)
    {
        QMessageBox::information(this, "Usuwanie", "Zaznacz pozycje do usuniecia.");
        return;
    }

    const bool usunieto = dziennik.usunPozycje(static_cast<std::size_t>(zaznaczonyWiersz));

    if (!usunieto)
    {
        QMessageBox::warning(this, "Blad", "Nie udalo sie usunac pozycji.");
        return;
    }

    // Autozapis dziennika po usunieciu
    PlikManager::zapiszDziennik("dziennik.txt", dziennik);

    odswiezDziennik();
}

void MainWindow::on_buttonWyczyscDziennik_clicked()
{
    dziennik.wyczysc();
    PlikManager::zapiszDziennik("dziennik.txt", dziennik);
    odswiezDziennik();
}

void MainWindow::on_buttonDodajProdukt_clicked()
{
    const QString nazwa = ui.lineEditNazwaProduktu->text().trimmed();
    const double kcal = ui.doubleSpinKalorie->value();
    const double bialko = ui.doubleSpinBialko->value();
    const double weg = ui.doubleSpinWeglowodany->value();
    const double tluszcz = ui.doubleSpinTluszcze->value();

    if (nazwa.isEmpty())
    {
        QMessageBox::warning(this, "Blad", "Nazwa produktu nie moze byc pusta.");
        return;
    }

    if (!(std::isfinite(kcal) && kcal >= 0.0 && kcal <= 10000.0))
    {
        QMessageBox::warning(this, "Blad", "Niepoprawna wartosc kalorii.");
        return;
    }

    if (!(std::isfinite(bialko) && bialko >= 0.0 && bialko <= 1000.0) ||
        !(std::isfinite(weg) && weg >= 0.0 && weg <= 1000.0) ||
        !(std::isfinite(tluszcz) && tluszcz >= 0.0 && tluszcz <= 1000.0))
    {
        QMessageBox::warning(this, "Blad", "Niepoprawne wartosci makroskladnikow.");
        return;
    }

    Produkt p(nazwa.toStdString(), Makroskladniki{ kcal, bialko, weg, tluszcz });
    // produkt ma domyslna jednostke g
    produkty.push_back(p);

    const int idx = static_cast<int>(produkty.size() - 1);
    ui.comboProduktDziennik->addItem(nazwa, idx);

    // ustaw na nowo i zaladuj jednostki
    ui.comboProduktDziennik->setCurrentIndex(idx);
    on_comboProduktDziennik_currentIndexChanged(idx);

    // zapis bazy produktow
    PlikManager::zapiszProdukty("produkty.txt", produkty);

    QMessageBox::information(this, "Produkty", "Produkt dodany.");
}

QString MainWindow::komunikatBledu(DziennikZywieniowy::WynikOperacji wynik) const
{
    switch (wynik)
    {
    case DziennikZywieniowy::WynikOperacji::Sukces:
        return "Operacja wykonana poprawnie.";

    case DziennikZywieniowy::WynikOperacji::PustaNazwa:
        return "Nazwa produktu nie moze byc pusta.";

    case DziennikZywieniowy::WynikOperacji::PustaJednostka:
        return "Nazwa jednostki nie moze byc pusta.";

    case DziennikZywieniowy::WynikOperacji::NiepoprawnaIlosc:
        return "Ilosc musi byc wieksza od 0.";

    case DziennikZywieniowy::WynikOperacji::NiepoprawnaJednostka:
        return "Jednostka produktu ma niepoprawna wage w gramach.";

    case DziennikZywieniowy::WynikOperacji::NiepoprawneMakro:
        return "Dane makroskladnikow sa niepoprawne.";

    case DziennikZywieniowy::WynikOperacji::NiepoprawnyLimit:
        return "Limit dzienny jest niepoprawny.";

    default:
        return "Nieznany blad.";
    }
}

// Zapisz profil (walidacja + autozapis)
void MainWindow::on_buttonZapiszProfil_clicked()
{
    const QString imie = ui.lineEditImie->text().trimmed();
    const int wiek = ui.spinWiek->value();
    const double waga = ui.doubleSpinWaga->value();
    const double wzrost = ui.doubleSpinWzrost->value();
    const double limit = ui.doubleSpinLimitKalorii->value();

    if (imie.isEmpty())
    {
        QMessageBox::warning(this, "Blad", "Imie nie moze byc puste.");
        return;
    }

    // dodatkowa walidacja warunkowa
    if (wiek < 8 || wiek > 120)
    {
        QMessageBox::warning(this, "Blad", "Wiek poza zakresem (8-120). ");
        return;
    }

    if (waga < 20.0 || waga > 300.0)
    {
        QMessageBox::warning(this, "Blad", "Waga poza zakresem (20-300 kg). ");
        return;
    }

    if (wzrost < 50.0 || wzrost > 250.0)
    {
        QMessageBox::warning(this, "Blad", "Wzrost poza zakresem (50-250 cm). ");
        return;
    }

    if (limit < 800.0 || limit > 10000.0)
    {
        QMessageBox::warning(this, "Blad", "Limit kalorii poza zakresem (800-10000). ");
        return;
    }

    profil.ustawImie(imie.toStdString());
    profil.ustawWiek(wiek);
    profil.ustawWage(waga);
    profil.ustawWzrost(wzrost);
    profil.ustawLimitKalorii(limit);

    // Autozapis profilu
    PlikManager::zapiszProfil("profil.txt", profil);

    // Ustaw limit w dzienniku i zapisz
    dziennik.ustawLimitKalorii(limit);
    PlikManager::zapiszDziennik("dziennik.txt", dziennik);

    QMessageBox::information(this, "Profil", "Profil zapisany.");
}
