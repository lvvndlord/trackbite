#include "mainwindow.h"

#include <QAbstractItemView>
#include <QDate>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QProgressBar>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QFrame>
#include <QScrollArea>
#include <QSizePolicy>
#include <algorithm>
#include <QScrollBar>
#include <QVBoxLayout>


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    aktualnaData(QDate::currentDate())
{
    ui.setupUi(this);
    wczytajDaneZPlikow();
    setStyleSheet(R"(
    QMainWindow {
        background: #f5f6f8;
    }

    QTabWidget::pane {
        border: none;
        background: #f5f6f8;
    }

    QTabBar::tab {
        background: #ffffff;
        color: #222;
        padding: 8px 18px;
        border: 1px solid #e2e5ea;
        border-bottom: none;
        border-top-left-radius: 8px;
        border-top-right-radius: 8px;
        margin-right: 4px;
    }

    QTabBar::tab:selected {
        background: #f5f6f8;
        font-weight: 600;
    }

    QScrollArea {
        border: none;
        background: transparent;
    }

    QScrollArea > QWidget > QWidget {
        background: transparent;
    }

    QFrame {
        background: #ffffff;
        border: 1px solid #e1e5ea;
        border-radius: 12px;
    }

    QFrame QLabel {
        background: transparent;
        border: none;
        color: #202124;
    }

    QPushButton {
        background: #eaf6df;
        color: #2e6b1f;
        border: 1px solid #cfe8c2;
        border-radius: 8px;
        padding: 6px 12px;
        font-weight: 600;
    }

    QPushButton:hover {
        background: #dff0d2;
    }

    QTableWidget {
        background: #ffffff;
        border: 1px solid #eef1f4;
        border-radius: 8px;
        gridline-color: transparent;
        selection-background-color: #e8f5e9;
        selection-color: #111;
    }

    QTableWidget::item {
        padding: 6px;
        border: none;
    }

    QHeaderView::section {
        background: #f7f8fa;
        color: #333;
        border: none;
        border-bottom: 1px solid #e1e5ea;
        padding: 6px;
        font-weight: 600;
    }

    QProgressBar {
        border: none;
        background: #edf0f3;
        border-radius: 5px;
        height: 8px;
        text-align: center;
    }

    QProgressBar::chunk {
        background: #6fba44;
        border-radius: 5px;
    }
QFrame#frameStatusDnia {
    background: #ffffff;
    border: 1px solid #e1e5ea;
    border-radius: 12px;
}

QFrame#frameStatZostalo,
QFrame#frameStatSpozyto,
QFrame#frameStatLimit {
    background: #f8faf7;
    border: 1px solid #e2eadf;
    border-radius: 10px;
}

QLabel#labelTytulZostalo,
QLabel#labelTytulSpozyto,
QLabel#labelTytulLimit {
    color: #6b7280;
    font-size: 11px;
    font-weight: 500;
}

QLabel#labelZostaloKalorii,
QLabel#labelSpozytoKalorii,
QLabel#labelLimitKalorii_2 {
    color: #111827;
    font-size: 15px;
    font-weight: 700;
}

QLabel#labelDataDnia {
    font-size: 15px;
    font-weight: 700;
    color: #111827;
}

QPushButton#buttonPoprzedniDzien,
QPushButton#buttonNastepnyDzien {
    background: #eaf6df;
    color: #2e6b1f;
    border: 1px solid #cfe8c2;
    border-radius: 8px;
    font-weight: 700;
}
QFrame#frameStatZostalo {
    background: #edf8e8;
    border: 1px solid #cfe8c2;
    border-radius: 10px;
}

QLabel#labelZostaloKalorii {
    color: #2e7d32;
    font-size: 17px;
    font-weight: 800;
}

QScrollArea {
    border: none;
    background: transparent;
}

QScrollArea > QWidget > QWidget {
    background: transparent;
}

QScrollBar:vertical {
    background: transparent;
    width: 8px;
    margin: 4px 0 4px 0;
}

QScrollBar::handle:vertical {
    background: #cfd6dd;
    border-radius: 4px;
    min-height: 40px;
}

QScrollBar::handle:vertical:hover {
    background: #aeb8c2;
}

QScrollBar::add-line:vertical,
QScrollBar::sub-line:vertical {
    height: 0px;
    background: none;
    border: none;
}

QScrollBar::add-page:vertical,
QScrollBar::sub-page:vertical {
    background: transparent;
}
)");

    dziennik.ustawLimityDzienne({ 2200.0, 120.0, 240.0, 70.0 });

    ustawDziennikGui();
    odswiezDziennik();

    odswiezTabeleProduktow();
}

MainWindow::~MainWindow()
{
    zapiszDaneDoPlikow();
}

void MainWindow::ustawDziennikGui()
{
    ui.scrollAreaDziennik->setWidgetResizable(true);
    ui.scrollAreaDziennik->setFrameShape(QFrame::NoFrame);

    ui.scrollAreaDziennik->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui.scrollAreaDziennik->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    ui.scrollAreaDziennik->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
    );

    if (ui.scrollAreaDziennik->widget() != nullptr)
    {
        ui.scrollAreaDziennik->widget()->setSizePolicy(
            QSizePolicy::Expanding,
            QSizePolicy::Minimum
        );
    }

    ui.frameStatusDnia->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui.framePodsumowanieDolne->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui.scrollAreaDziennik->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    for (QFrame* frame : {
        ui.frameSniadanie,
        ui.frameDrugieSniadanie,
        ui.frameObiad,
        ui.frameKolacja,
        ui.framePrzekaski
        })
    {
        frame->setMinimumHeight(64);
        frame->setMaximumHeight(QWIDGETSIZE_MAX);
        frame->setMinimumWidth(0);
        frame->setMaximumWidth(QWIDGETSIZE_MAX);
        frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    }

    ustawTabelePosilkow();

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

void MainWindow::dopasujWysokoscTabeli(QTableWidget* tabela)
{
    if (tabela->rowCount() <= 0)
    {
        tabela->setMinimumHeight(0);
        tabela->setMaximumHeight(0);
        return;
    }

    const int liczbaWierszy = tabela->rowCount();
    const int wysokoscNaglowka = tabela->horizontalHeader()->height();
    const int wysokoscWiersza = tabela->verticalHeader()->defaultSectionSize();

    const int wysokosc = wysokoscNaglowka + liczbaWierszy * wysokoscWiersza + 10;

    tabela->setMinimumHeight(wysokosc);
    tabela->setMaximumHeight(wysokosc);
}

void MainWindow::ustawTabelePosilkow()
{
    auto ustawTabele = [](QTableWidget* tabela)
        {
            tabela->setColumnCount(7);
            tabela->setHorizontalHeaderLabels(
                { "Produkt", "Ilosc", "Gramy", "kcal", "Bialko", "Weglowodany", "Tluszcz" }
            );

            tabela->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            tabela->horizontalHeader()->setFixedHeight(30);

            tabela->verticalHeader()->setVisible(false);
            tabela->verticalHeader()->setDefaultSectionSize(32);

            tabela->setSelectionBehavior(QAbstractItemView::SelectRows);
            tabela->setSelectionMode(QAbstractItemView::SingleSelection);
            tabela->setEditTriggers(QAbstractItemView::NoEditTriggers);

            tabela->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            tabela->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            tabela->setShowGrid(false);
            tabela->setAlternatingRowColors(true);

            tabela->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            tabela->setMinimumHeight(0);
            tabela->setMaximumHeight(QWIDGETSIZE_MAX);
        };

    ustawTabele(ui.tableSniadanie);
    ustawTabele(ui.tableDrugieSniadanie);
    ustawTabele(ui.tableObiad);
    ustawTabele(ui.tableKolacja);
    ustawTabele(ui.tablePrzekaski);
}

void MainWindow::wypelnijTabeleDlaPory(
    PoraPosilku pora,
    QTableWidget* tabela,
    QLabel* labelKcal
)
{
    const std::vector<PozycjaDziennika> pozycje =
        dziennik.pobierzPozycjeDlaPory(pora);

    const Makroskladniki sumaPory = dziennik.obliczSumeDlaPory(pora);

    labelKcal->setText(
        QString("%1 kcal").arg(sumaPory.kalorie, 0, 'f', 0)
    );

    if (pozycje.empty())
    {
        tabela->clearContents();
        tabela->setRowCount(0);
        tabela->setVisible(false);
        tabela->updateGeometry();

        if (tabela->parentWidget() != nullptr)
        {
            tabela->parentWidget()->updateGeometry();
        }

        return;
    }

    tabela->setVisible(true);
    tabela->setRowCount(static_cast<int>(pozycje.size()));

    for (int i = 0; i < static_cast<int>(pozycje.size()); ++i)
    {
        const PozycjaDziennika& pozycja = pozycje[i];
        const Makroskladniki makro = pozycja.obliczMakro();

        tabela->setItem(i, 0, new QTableWidgetItem(
            QString::fromStdString(pozycja.pobierzNazweProduktu())
        ));

        tabela->setItem(i, 1, new QTableWidgetItem(
            QString("%1 %2")
            .arg(pozycja.pobierzIlosc(), 0, 'f', 2)
            .arg(QString::fromStdString(pozycja.pobierzJednostke().nazwa))
        ));

        tabela->setItem(i, 2, new QTableWidgetItem(
            QString::number(pozycja.pobierzGramy(), 'f', 0)
        ));

        tabela->setItem(i, 3, new QTableWidgetItem(
            QString::number(makro.kalorie, 'f', 0)
        ));

        tabela->setItem(i, 4, new QTableWidgetItem(
            QString::number(makro.bialko, 'f', 1)
        ));

        tabela->setItem(i, 5, new QTableWidgetItem(
            QString::number(makro.weglowodany, 'f', 1)
        ));

        tabela->setItem(i, 6, new QTableWidgetItem(
            QString::number(makro.tluszcz, 'f', 1)
        ));
    }

    dopasujWysokoscTabeli(tabela);

    tabela->updateGeometry();

    if (tabela->parentWidget() != nullptr)
    {
        tabela->parentWidget()->updateGeometry();
    }
}

void MainWindow::odswiezDziennik()
{
    wypelnijTabeleDlaPory(
        PoraPosilku::Sniadanie,
        ui.tableSniadanie,
        ui.labelKcalSniadanie
    );

    wypelnijTabeleDlaPory(
        PoraPosilku::DrugieSniadanie,
        ui.tableDrugieSniadanie,
        ui.labelKcalDrugieSniadanie
    );

    wypelnijTabeleDlaPory(
        PoraPosilku::Obiad,
        ui.tableObiad,
        ui.labelKcalObiad
    );

    wypelnijTabeleDlaPory(
        PoraPosilku::Kolacja,
        ui.tableKolacja,
        ui.labelKcalKolacja
    );

    wypelnijTabeleDlaPory(
        PoraPosilku::Przekaski,
        ui.tablePrzekaski,
        ui.labelKcalPrzekaski
    );

    const Makroskladniki suma = dziennik.obliczSume();
    const Makroskladniki limity = dziennik.pobierzLimityDzienne();

    if (aktualnaData == QDate::currentDate())
    {
        ui.labelDataDnia->setText("Dzisiaj");
    }
    else if (aktualnaData == QDate::currentDate().addDays(-1))
    {
        ui.labelDataDnia->setText("Wczoraj");
    }
    else if (aktualnaData == QDate::currentDate().addDays(1))
    {
        ui.labelDataDnia->setText("Jutro");
    }
    else
    {
        ui.labelDataDnia->setText(aktualnaData.toString("dd.MM.yyyy"));
    }

    ui.labelLimitKalorii_2->setText(
        QString("%1 kcal").arg(limity.kalorie, 0, 'f', 0)
    );

    ui.labelSpozytoKalorii->setText(
        QString("%1 kcal").arg(suma.kalorie, 0, 'f', 0)
    );

    ui.labelZostaloKalorii->setText(
        QString("%1 kcal").arg(dziennik.pozostaleKalorie(), 0, 'f', 0)
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
        QString("Tluszcz: %1 g").arg(suma.tluszcz, 0, 'f', 1)
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

    if (ui.scrollAreaDziennik->widget() != nullptr)
    {
        ui.scrollAreaDziennik->widget()->updateGeometry();

        if (ui.scrollAreaDziennik->widget()->layout() != nullptr)
        {
            ui.scrollAreaDziennik->widget()->layout()->invalidate();
            ui.scrollAreaDziennik->widget()->layout()->activate();
        }
    }

    ui.scrollAreaDziennik->updateGeometry();
}

void MainWindow::dodajProduktTestowyDoPory(PoraPosilku pora)
{
    const JednostkaProduktu jednostka{ "sztuka", 55.0 };

    const DziennikZywieniowy::WynikOperacji wynik = dziennik.dodajPozycje(
        "Jajko",
        1.0,
        jednostka,
        155.0,
        13.0,
        1.1,
        11.0,
        pora
    );

    if (wynik != DziennikZywieniowy::WynikOperacji::Sukces)
    {
        QMessageBox::warning(this, "Blad danych", komunikatBledu(wynik));
        return;
    }

    odswiezDziennik();
}

// Wczytywanie / zapisywanie danych
void MainWindow::wczytajDaneZPlikow()
{
    ProfilUzytkownika p;
    if (PlikManager::wczytajProfil("profil.txt", p))
    {
        profil = p;
    }

    // Ładowanie bazy przez Twoją klasę!
    vector<Produkt> pb;
    PlikManager::wczytajProdukty("produkty.txt", pb);
    if (pb.empty())
    {
        bazaProduktow.dodajProdukt(Produkt("Piers z kurczaka", Makroskladniki{ 165.0, 31.0, 0.0, 3.6 }));
        bazaProduktow.dodajProdukt(Produkt("Ryz bialy", Makroskladniki{ 350.0, 7.0, 79.0, 1.0 }));
        bazaProduktow.dodajProdukt(Produkt("Jajko", Makroskladniki{ 143.0, 12.6, 0.7, 9.5 }));
        bazaProduktow.dodajProdukt(Produkt("Banan", Makroskladniki{ 89.0, 1.1, 22.8, 0.3 }));
        bazaProduktow.dodajProdukt(Produkt("Twarog chudy", Makroskladniki{ 86.0, 18.0, 3.5, 0.5 }));
        bazaProduktow.dodajProdukt(Produkt("Oliwa z oliwek", Makroskladniki{ 884.0, 0.0, 0.0, 100.0 }));
        bazaProduktow.dodajProdukt(Produkt("Platki owsiane", Makroskladniki{ 370.0, 13.0, 62.0, 7.0 }));
        bazaProduktow.dodajProdukt(Produkt("Jablko", Makroskladniki{ 52.0, 0.3, 14.0, 0.2 }));
        bazaProduktow.dodajProdukt(Produkt("Chleb zytni", Makroskladniki{ 259.0, 6.5, 48.0, 1.8 }));
        bazaProduktow.dodajProdukt(Produkt("Maslo orzechowe", Makroskladniki{ 588.0, 25.0, 20.0, 50.0 }));

        // Od razu zapisujemy do pliku, żeby baza została utrwalona
        PlikManager::zapiszProdukty("produkty.txt", bazaProduktow.pobierzWszystkie());
    }
    else
    {
        // Jeśli plik nie był pusty, normalnie przepisujemy produkty do Twojej bazy
        for (const auto& prod : pb) {
            bazaProduktow.dodajProdukt(prod);
        }
    }

    PlikManager::wczytajDziennik("dziennik.txt", dziennik);
}

void MainWindow::zapiszDaneDoPlikow()
{
    PlikManager::zapiszProfil("profil.txt", profil);
    // Zapisywanie bazy przez Twoją klasę!
    PlikManager::zapiszProdukty("produkty.txt", bazaProduktow.pobierzWszystkie());
    PlikManager::zapiszDziennik("dziennik.txt", dziennik);
}

void MainWindow::on_buttonDodajSniadanie_clicked()
{
    dodajProduktTestowyDoPory(PoraPosilku::Sniadanie);
}

void MainWindow::on_buttonDodajDrugieSniadanie_clicked()
{
    dodajProduktTestowyDoPory(PoraPosilku::DrugieSniadanie);
}

void MainWindow::on_buttonDodajObiad_clicked()
{
    dodajProduktTestowyDoPory(PoraPosilku::Obiad);
}

void MainWindow::on_buttonDodajKolacja_clicked()
{
    dodajProduktTestowyDoPory(PoraPosilku::Kolacja);
}

void MainWindow::on_buttonDodajPrzekaski_clicked()
{
    dodajProduktTestowyDoPory(PoraPosilku::Przekaski);
}

void MainWindow::on_buttonPoprzedniDzien_clicked()
{
    aktualnaData = aktualnaData.addDays(-1);
    odswiezDziennik();
}

void MainWindow::on_buttonNastepnyDzien_clicked()
{
    aktualnaData = aktualnaData.addDays(1);
    odswiezDziennik();
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
// --- FUNKCJE ZAKŁADKI PRODUKTY ---

void MainWindow::odswiezTabeleProduktow()
{
    ui.tableProdukty->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.tableProdukty->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.tableProdukty->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui.tableProdukty->setRowCount(0);

    // Użycie Twojej funkcji: pobierzWszystkie()
    vector<Produkt> wszystkie = bazaProduktow.pobierzWszystkie();

    for (size_t i = 0; i < wszystkie.size(); ++i)
    {
        ui.tableProdukty->insertRow(i);

        QString nazwa = QString::fromStdString(wszystkie[i].pobierzNazwe());
        Makroskladniki makro = wszystkie[i].pobierzMakroNa100g();

        ui.tableProdukty->setItem(i, 0, new QTableWidgetItem(nazwa));
        ui.tableProdukty->setItem(i, 1, new QTableWidgetItem(QString::number(makro.kalorie, 'f', 0)));
        ui.tableProdukty->setItem(i, 2, new QTableWidgetItem(QString::number(makro.bialko, 'f', 1)));
        ui.tableProdukty->setItem(i, 3, new QTableWidgetItem(QString::number(makro.weglowodany, 'f', 1)));
        ui.tableProdukty->setItem(i, 4, new QTableWidgetItem(QString::number(makro.tluszcz, 'f', 1)));
    }
}

void MainWindow::on_buttonDodajProdukt_clicked()
{
    string nazwa = ui.lineEditNazwaProduktu->text().trimmed().toStdString();
    double kcal = ui.doubleSpinKalorie->value();
    double bialko = ui.doubleSpinBialko->value();
    double wegle = ui.doubleSpinWeglowodany->value();
    double tluszcz = ui.doubleSpinTluszcze->value();

    if (nazwa.empty() || nazwa == "Nazwa produktu")
    {
        QMessageBox::warning(this, "Błąd", "Podaj prawidłową nazwę produktu!");
        return;
    }

    Makroskladniki noweMakro{ kcal, bialko, wegle, tluszcz };
    Produkt nowyProdukt(nazwa, noweMakro);

    if (nowyProdukt.czyPoprawny())
    {
        // Użycie Twojej funkcji: dodajProdukt()
        bazaProduktow.dodajProdukt(nowyProdukt);
        zapiszDaneDoPlikow();
        odswiezTabeleProduktow();

        QMessageBox::information(this, "Sukces", "Dodano nowy produkt do bazy!");
    }
    else
    {
        QMessageBox::warning(this, "Błąd", "Wprowadzone dane produktu są niepoprawne!");
    }
}

void MainWindow::on_buttonUsunProdukt_clicked()
{
    int zaznaczonyWiersz = ui.tableProdukty->currentRow();

    // Użycie Twojej funkcji: usunProduktPoIndeksie()
    if (bazaProduktow.usunProduktPoIndeksie(zaznaczonyWiersz))
    {
        zapiszDaneDoPlikow();
        odswiezTabeleProduktow();
        QMessageBox::information(this, "Sukces", "Produkt został usunięty.");
    }
    else
    {
        QMessageBox::warning(this, "Błąd", "Najpierw kliknij w produkt na liście, który chcesz usunąć!");
    }
}

void MainWindow::on_buttonSzukajProduktu_clicked()
{
    string szukanaFraza = ui.lineEditSzukajProduktu->text().trimmed().toStdString();

    if (szukanaFraza.empty() || szukanaFraza == "Szukaj produktu") {
        odswiezTabeleProduktow(); // Resetujemy widok na wszystkie
        return;
    }

    // Użycie Twojej funkcji: wyszukajProdukt()
    vector<Produkt> znalezione = bazaProduktow.wyszukajProdukt(szukanaFraza);

    ui.tableProdukty->setRowCount(0);

    for (size_t i = 0; i < znalezione.size(); ++i)
    {
        ui.tableProdukty->insertRow(i);
        QString nazwa = QString::fromStdString(znalezione[i].pobierzNazwe());
        Makroskladniki makro = znalezione[i].pobierzMakroNa100g();

        ui.tableProdukty->setItem(i, 0, new QTableWidgetItem(nazwa));
        ui.tableProdukty->setItem(i, 1, new QTableWidgetItem(QString::number(makro.kalorie, 'f', 0)));
        ui.tableProdukty->setItem(i, 2, new QTableWidgetItem(QString::number(makro.bialko, 'f', 1)));
        ui.tableProdukty->setItem(i, 3, new QTableWidgetItem(QString::number(makro.weglowodany, 'f', 1)));
        ui.tableProdukty->setItem(i, 4, new QTableWidgetItem(QString::number(makro.tluszcz, 'f', 1)));
    }
}