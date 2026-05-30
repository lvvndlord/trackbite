#include "mainwindow.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QDate>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QFrame>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollArea>
#include <QSizePolicy>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <string>
#include <vector>

namespace
{
    std::string sciezkaProfilu()
    {
        return "profil.json";
    }

    std::string sciezkaProduktow()
    {
        return "produkty.json";
    }

    std::string sciezkaDziennikaDlaDaty(const QDate& data)
    {
        return QString("dziennik_%1.json")
            .arg(data.toString("yyyy_MM_dd"))
            .toStdString();
    }

    QString stylAplikacji()
    {
        return R"(
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

            QFrame#frameStatZostalo {
                background: #edf8e8;
                border: 1px solid #cfe8c2;
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

            QLabel#labelZostaloKalorii {
                color: #2e7d32;
                font-size: 17px;
                font-weight: 800;
            }

            QLabel#labelDataDnia {
                color: #111827;
                font-size: 15px;
                font-weight: 700;
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

            QPushButton#buttonPoprzedniDzien,
            QPushButton#buttonNastepnyDzien {
                background: #eaf6df;
                color: #2e6b1f;
                border: 1px solid #cfe8c2;
                border-radius: 8px;
                font-weight: 700;
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
        )";
    }

    void ustawProgress(QProgressBar* pasek, int procent)
    {
        const int wartoscPaska = std::clamp(procent, 0, 100);

        pasek->setValue(wartoscPaska);
        pasek->setFormat(QString::number(procent) + "%");

        if (procent > 100)
        {
            pasek->setStyleSheet(
                "QProgressBar::chunk { background: #ef4444; border-radius: 5px; }"
            );
        }
        else
        {
            pasek->setStyleSheet(
                "QProgressBar::chunk { background: #6fba44; border-radius: 5px; }"
            );
        }
    }

    bool czyTaSamaJednostka(
        const JednostkaProduktu& pierwsza,
        const JednostkaProduktu& druga
    )
    {
        return pierwsza.nazwa == druga.nazwa
            && std::abs(pierwsza.gramyNaJednostke - druga.gramyNaJednostke) < 0.001;
    }
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    aktualnaData(QDate::currentDate())
{
    ui.setupUi(this);
    setStyleSheet(stylAplikacji());

    ui.doubleSpinLimitKalorii->setRange(800.0, 10000.0);
    ui.doubleSpinWaga->setRange(20.0, 300.0);
    ui.doubleSpinWzrost->setRange(50.0, 250.0);

    ui.lineEditSzukajProduktu->clear();
    ui.lineEditSzukajProduktu->setPlaceholderText("Szukaj produktu...");

    ui.lineEditImie->setPlaceholderText("Imię...");

    wczytajDaneZPlikow();

    ui.lineEditImie->setText(QString::fromStdString(profil.pobierzImie()));
    ui.spinWiek->setValue(profil.pobierzWiek());
    ui.doubleSpinWaga->setValue(profil.pobierzWage());
    ui.doubleSpinWzrost->setValue(profil.pobierzWzrost());
    ui.doubleSpinLimitKalorii->setValue(profil.pobierzLimitKalorii());

    ustawDziennikGui();
    odswiezTabeleProduktow();
    odswiezDziennik();

    ui.tableProdukty->verticalHeader()->setVisible(false);
    ui.tableUlubione->verticalHeader()->setVisible(false);
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
    ui.scrollAreaDziennik->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    if (ui.scrollAreaDziennik->widget() != nullptr)
    {
        ui.scrollAreaDziennik->widget()->setSizePolicy(
            QSizePolicy::Expanding,
            QSizePolicy::Minimum
        );
    }

    ui.frameStatusDnia->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui.framePodsumowanieDolne->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

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

void MainWindow::ustawTabelePosilkow()
{
    auto ustawTabele = [](QTableWidget* tabela)
        {
            tabela->setColumnCount(8);
            tabela->setHorizontalHeaderLabels({
                "Produkt",
                "Ilość",
                "Gramy",
                "kcal",
                "Białko",
                "Węglowodany",
                "Tłuszcz",
                ""
                });

            for (int kolumna = 0; kolumna < 7; ++kolumna)
            {
                tabela->horizontalHeader()->setSectionResizeMode(
                    kolumna,
                    QHeaderView::Stretch
                );
            }

            tabela->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Fixed);
            tabela->setColumnWidth(7, 36);
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
            tabela->setCursor(Qt::PointingHandCursor);

            tabela->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            tabela->setMinimumHeight(0);
            tabela->setMaximumHeight(QWIDGETSIZE_MAX);
        };

    ustawTabele(ui.tableSniadanie);
    ustawTabele(ui.tableDrugieSniadanie);
    ustawTabele(ui.tableObiad);
    ustawTabele(ui.tableKolacja);
    ustawTabele(ui.tablePrzekaski);

    podlaczEdycjePozycji(ui.tableSniadanie, PoraPosilku::Sniadanie);
    podlaczEdycjePozycji(ui.tableDrugieSniadanie, PoraPosilku::DrugieSniadanie);
    podlaczEdycjePozycji(ui.tableObiad, PoraPosilku::Obiad);
    podlaczEdycjePozycji(ui.tableKolacja, PoraPosilku::Kolacja);
    podlaczEdycjePozycji(ui.tablePrzekaski, PoraPosilku::Przekaski);
}

void MainWindow::podlaczEdycjePozycji(QTableWidget* tabela, PoraPosilku pora)
{
    connect(
        tabela,
        &QTableWidget::cellClicked,
        this,
        [this, pora](int wiersz, int kolumna)
        {
            if (kolumna == 7)
            {
                return;
            }

            edytujPozycjeWPorze(pora, wiersz);
        }
    );
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
        QString("<b>%1 kcal</b>  |  B: %2 g  W: %3 g  T: %4 g")
        .arg(sumaPory.kalorie, 0, 'f', 0)
        .arg(sumaPory.bialko, 0, 'f', 1)
        .arg(sumaPory.weglowodany, 0, 'f', 1)
        .arg(sumaPory.tluszcz, 0, 'f', 1)
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
        const PozycjaDziennika& pozycja = pozycje[static_cast<std::size_t>(i)];
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

        auto* przyciskUsun = new QPushButton(QStringLiteral("×"), tabela);
        przyciskUsun->setFixedSize(28, 28);
        przyciskUsun->setCursor(Qt::PointingHandCursor);
        przyciskUsun->setToolTip("Usuń pozycję");
        przyciskUsun->setStyleSheet(R"(
            QPushButton {
                background: transparent;
                color: #b42318;
                border: none;
                font-size: 18px;
                font-weight: 700;
                padding: 0;
            }

            QPushButton:hover {
                color: #7f1d1d;
                background: #fde8e8;
                border-radius: 6px;
            }
        )");

        const int wiersz = i;

        connect(
            przyciskUsun,
            &QPushButton::clicked,
            this,
            [this, pora, wiersz]()
            {
                usunPozycjeWPorze(pora, wiersz);
            }
        );

        tabela->setCellWidget(i, 7, przyciskUsun);
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

    const double zostalo = dziennik.pozostaleKalorie();

    if (zostalo < 0.0)
    {
        ui.labelTytulZostalo->setText("Przekroczono");
        ui.labelZostaloKalorii->setText(
            QString("%1 kcal").arg(std::abs(zostalo), 0, 'f', 0)
        );
        ui.labelZostaloKalorii->setStyleSheet("color: #dc2626;");
        ui.frameStatZostalo->setStyleSheet(
            "QFrame#frameStatZostalo {"
            "background: #fef2f2;"
            "border: 1px solid #fca5a5;"
            "border-radius: 10px;"
            "}"
        );
    }
    else
    {
        ui.labelTytulZostalo->setText("Zostało");
        ui.labelZostaloKalorii->setText(
            QString("%1 kcal").arg(zostalo, 0, 'f', 0)
        );
        ui.labelZostaloKalorii->setStyleSheet("color: #2e7d32;");
        ui.frameStatZostalo->setStyleSheet(
            "QFrame#frameStatZostalo {"
            "background: #edf8e8;"
            "border: 1px solid #cfe8c2;"
            "border-radius: 10px;"
            "}"
        );
    }

    ui.labelSumaKalorii->setText(
        QString("Kalorie: %1").arg(suma.kalorie, 0, 'f', 0)
    );

    ui.labelSumaBialka->setText(
        QString("Białko: %1 g").arg(suma.bialko, 0, 'f', 1)
    );

    ui.labelSumaWeglowodanow->setText(
        QString("Węglowodany: %1 g").arg(suma.weglowodany, 0, 'f', 1)
    );

    ui.labelSumaTluszczu->setText(
        QString("Tłuszcz: %1 g").arg(suma.tluszcz, 0, 'f', 1)
    );

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

std::vector<JednostkaProduktu> MainWindow::pobierzJednostkiDlaNazwy(
    const std::string& nazwaProduktu,
    const JednostkaProduktu* domyslnaJednostka
) const
{
    std::vector<JednostkaProduktu> wynik;

    for (const Produkt& produkt : bazaProduktow.pobierzWszystkie())
    {
        if (produkt.pobierzNazwe() != nazwaProduktu)
        {
            continue;
        }

        for (const JednostkaProduktu& jednostka : produkt.pobierzJednostki())
        {
            JednostkaProduktu doDodania = jednostka;

            if (doDodania.nazwa == "gram")
            {
                doDodania.nazwa = "g";
                doDodania.gramyNaJednostke = 1.0;
            }

            const bool juzIstnieje = std::any_of(
                wynik.begin(),
                wynik.end(),
                [&doDodania](const JednostkaProduktu& obecna)
                {
                    return czyTaSamaJednostka(obecna, doDodania);
                }
            );

            if (!juzIstnieje)
            {
                wynik.push_back(doDodania);
            }
        }

        break;
    }

    if (domyslnaJednostka != nullptr)
    {
        JednostkaProduktu doDodania = *domyslnaJednostka;

        if (doDodania.nazwa == "gram")
        {
            doDodania.nazwa = "g";
            doDodania.gramyNaJednostke = 1.0;
        }

        const bool juzIstnieje = std::any_of(
            wynik.begin(),
            wynik.end(),
            [&doDodania](const JednostkaProduktu& obecna)
            {
                return czyTaSamaJednostka(obecna, doDodania);
            }
        );

        if (!juzIstnieje)
        {
            wynik.push_back(doDodania);
        }
    }

    const bool maGramy = std::any_of(
        wynik.begin(),
        wynik.end(),
        [](const JednostkaProduktu& jednostka)
        {
            return jednostka.nazwa == "g"
                && std::abs(jednostka.gramyNaJednostke - 1.0) < 0.001;
        }
    );

    if (!maGramy)
    {
        wynik.insert(wynik.begin(), { "g", 1.0 });
    }

    return wynik;
}

bool MainWindow::pokazDialogIlosci(
    const QString& tytulOkna,
    const std::string& nazwaProduktu,
    const Makroskladniki& makroNa100g,
    const std::vector<JednostkaProduktu>& dostepneJednostki,
    double domyslnaIlosc,
    const JednostkaProduktu& domyslnaJednostka,
    double& wybranaIlosc,
    JednostkaProduktu& wybranaJednostka
)
{
    if (dostepneJednostki.empty())
    {
        return false;
    }

    QDialog dialog(this);
    dialog.setWindowTitle(tytulOkna);
    dialog.setModal(true);

    auto* layout = new QVBoxLayout(&dialog);

    auto* etykietaProdukt = new QLabel(
        QString("<b>%1</b>").arg(QString::fromStdString(nazwaProduktu)),
        &dialog
    );
    layout->addWidget(etykietaProdukt);

    auto* spinIlosc = new QDoubleSpinBox(&dialog);
    spinIlosc->setRange(0.01, 10000.0);
    spinIlosc->setDecimals(2);
    spinIlosc->setSingleStep(0.5);
    spinIlosc->setValue(domyslnaIlosc);

    auto* comboJednostka = new QComboBox(&dialog);

    int indeksDomyslnejJednostki = 0;

    for (std::size_t i = 0; i < dostepneJednostki.size(); ++i)
    {
        const JednostkaProduktu& jednostka = dostepneJednostki[i];

        comboJednostka->addItem(
            QString("%1 (%2 g)")
            .arg(QString::fromStdString(jednostka.nazwa))
            .arg(jednostka.gramyNaJednostke, 0, 'f',
                jednostka.gramyNaJednostke < 10.0 ? 1 : 0)
        );

        if (czyTaSamaJednostka(jednostka, domyslnaJednostka))
        {
            indeksDomyslnejJednostki = static_cast<int>(i);
        }
    }

    comboJednostka->setCurrentIndex(indeksDomyslnejJednostki);

    auto* etykietaPodgladu = new QLabel(&dialog);

    auto odswiezPodglad = [&]()
        {
            const int indeks = comboJednostka->currentIndex();

            if (indeks < 0)
            {
                return;
            }

            const JednostkaProduktu jednostka =
                dostepneJednostki[static_cast<std::size_t>(indeks)];

            const double gramy = spinIlosc->value() * jednostka.gramyNaJednostke;
            const Makroskladniki makro = makroNa100g.przeliczNaGramy(gramy);

            etykietaPodgladu->setText(
                QString("<b>Razem:</b> %1 g | <b>%2 kcal</b><br/>"
                    "B: %3 g | W: %4 g | T: %5 g")
                .arg(gramy, 0, 'f', 0)
                .arg(makro.kalorie, 0, 'f', 0)
                .arg(makro.bialko, 0, 'f', 1)
                .arg(makro.weglowodany, 0, 'f', 1)
                .arg(makro.tluszcz, 0, 'f', 1)
            );
        };

    connect(
        spinIlosc,
        QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        &dialog,
        [&](double)
        {
            odswiezPodglad();
        }
    );

    connect(
        comboJednostka,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        &dialog,
        [&](int)
        {
            odswiezPodglad();
        }
    );

    auto* formularz = new QFormLayout();
    formularz->addRow("Ilość:", spinIlosc);
    formularz->addRow("Jednostka:", comboJednostka);

    layout->addLayout(formularz);
    layout->addWidget(etykietaPodgladu);

    auto* przyciski = new QDialogButtonBox(
        QDialogButtonBox::Cancel | QDialogButtonBox::Ok,
        &dialog
    );

    layout->addWidget(przyciski);

    connect(przyciski, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    connect(przyciski, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);

    odswiezPodglad();

    if (dialog.exec() != QDialog::Accepted)
    {
        return false;
    }

    const int indeks = comboJednostka->currentIndex();

    if (indeks < 0)
    {
        return false;
    }

    wybranaIlosc = spinIlosc->value();
    wybranaJednostka = dostepneJednostki[static_cast<std::size_t>(indeks)];

    return true;
}

void MainWindow::dodajProduktDoPory(PoraPosilku pora)
{
    const std::vector<Produkt> produkty = bazaProduktow.pobierzWszystkie();

    if (produkty.empty())
    {
        QMessageBox::warning(this, "Brak produktów", "Baza produktów jest pusta.");
        return;
    }

    QDialog dialogListy(this);
    dialogListy.setWindowTitle("Wybierz produkt");
    dialogListy.resize(380, 460);
    dialogListy.setModal(true);

    auto* layoutListy = new QVBoxLayout(&dialogListy);

    auto* poleWyszukiwania = new QLineEdit(&dialogListy);
    poleWyszukiwania->setPlaceholderText("Szukaj produktu...");
    layoutListy->addWidget(poleWyszukiwania);

    auto* listaProduktow = new QListWidget(&dialogListy);

    for (const Produkt& produkt : produkty)
    {
        listaProduktow->addItem(QString::fromStdString(produkt.pobierzNazwe()));
    }

    layoutListy->addWidget(listaProduktow);

    auto* przyciskiListy = new QDialogButtonBox(
        QDialogButtonBox::Cancel | QDialogButtonBox::Ok,
        &dialogListy
    );

    layoutListy->addWidget(przyciskiListy);

    connect(
        poleWyszukiwania,
        &QLineEdit::textChanged,
        &dialogListy,
        [listaProduktow](const QString& tekst)
        {
            for (int i = 0; i < listaProduktow->count(); ++i)
            {
                QListWidgetItem* element = listaProduktow->item(i);

                if (element != nullptr)
                {
                    element->setHidden(
                        !element->text().contains(tekst, Qt::CaseInsensitive)
                    );
                }
            }
        }
    );

    connect(listaProduktow, &QListWidget::itemDoubleClicked, &dialogListy, &QDialog::accept);
    connect(przyciskiListy, &QDialogButtonBox::accepted, &dialogListy, &QDialog::accept);
    connect(przyciskiListy, &QDialogButtonBox::rejected, &dialogListy, &QDialog::reject);

    if (dialogListy.exec() != QDialog::Accepted || listaProduktow->currentItem() == nullptr)
    {
        return;
    }

    const std::string wybranaNazwa =
        listaProduktow->currentItem()->text().toStdString();

    const auto iterator = std::find_if(
        produkty.begin(),
        produkty.end(),
        [&wybranaNazwa](const Produkt& produkt)
        {
            return produkt.pobierzNazwe() == wybranaNazwa;
        }
    );

    if (iterator == produkty.end())
    {
        return;
    }

    const Produkt wybranyProdukt = *iterator;

    const std::vector<JednostkaProduktu> dostepneJednostki =
        pobierzJednostkiDlaNazwy(wybranyProdukt.pobierzNazwe());

    JednostkaProduktu domyslnaJednostka = dostepneJednostki.front();
    double domyslnaIlosc = 1.0;

    for (const JednostkaProduktu& jednostka : dostepneJednostki)
    {
        if (jednostka.nazwa == "g" || jednostka.nazwa == "gram")
        {
            domyslnaJednostka = jednostka;
            domyslnaIlosc = 100.0;
            break;
        }
    }

    double wybranaIlosc = domyslnaIlosc;
    JednostkaProduktu wybranaJednostka = domyslnaJednostka;

    if (!pokazDialogIlosci(
        "Podaj ilość",
        wybranyProdukt.pobierzNazwe(),
        wybranyProdukt.pobierzMakroNa100g(),
        dostepneJednostki,
        domyslnaIlosc,
        domyslnaJednostka,
        wybranaIlosc,
        wybranaJednostka))
    {
        return;
    }

    const Makroskladniki makro = wybranyProdukt.pobierzMakroNa100g();

    const DziennikZywieniowy::WynikOperacji wynik = dziennik.dodajPozycje(
        wybranyProdukt.pobierzNazwe(),
        wybranaIlosc,
        wybranaJednostka,
        makro.kalorie,
        makro.bialko,
        makro.weglowodany,
        makro.tluszcz,
        pora
    );

    if (wynik != DziennikZywieniowy::WynikOperacji::Sukces)
    {
        QMessageBox::warning(this, "Błąd danych", komunikatBledu(wynik));
        return;
    }

    zapiszDaneDoPlikow();
    odswiezDziennik();
}

void MainWindow::edytujPozycjeWPorze(PoraPosilku pora, int indeksWiersza)
{
    const std::vector<PozycjaDziennika> pozycje =
        dziennik.pobierzPozycjeDlaPory(pora);

    if (indeksWiersza < 0 || indeksWiersza >= static_cast<int>(pozycje.size()))
    {
        return;
    }

    const PozycjaDziennika& pozycja =
        pozycje[static_cast<std::size_t>(indeksWiersza)];

    const std::vector<JednostkaProduktu> dostepneJednostki =
        pobierzJednostkiDlaNazwy(
            pozycja.pobierzNazweProduktu(),
            &pozycja.pobierzJednostke()
        );

    double nowaIlosc = pozycja.pobierzIlosc();
    JednostkaProduktu nowaJednostka = pozycja.pobierzJednostke();

    if (!pokazDialogIlosci(
        "Edycja pozycji",
        pozycja.pobierzNazweProduktu(),
        pozycja.pobierzMakroNa100g(),
        dostepneJednostki,
        pozycja.pobierzIlosc(),
        pozycja.pobierzJednostke(),
        nowaIlosc,
        nowaJednostka))
    {
        return;
    }

    const DziennikZywieniowy::WynikOperacji wynik =
        dziennik.edytujPozycjeDlaPory(
            pora,
            static_cast<std::size_t>(indeksWiersza),
            nowaIlosc,
            nowaJednostka
        );

    if (wynik != DziennikZywieniowy::WynikOperacji::Sukces)
    {
        QMessageBox::warning(this, "Błąd danych", komunikatBledu(wynik));
        return;
    }

    zapiszDaneDoPlikow();
    odswiezDziennik();
}

void MainWindow::usunPozycjeWPorze(PoraPosilku pora, int indeksWiersza)
{
    if (indeksWiersza < 0)
    {
        return;
    }

    const bool usunieto =
        dziennik.usunPozycjeDlaPory(
            pora,
            static_cast<std::size_t>(indeksWiersza)
        );

    if (!usunieto)
    {
        QMessageBox::warning(this, "Błąd", "Nie udało się usunąć pozycji.");
        return;
    }

    zapiszDaneDoPlikow();
    odswiezDziennik();
}

void MainWindow::wczytajDaneZPlikow()
{
    ProfilUzytkownika wczytanyProfil;

    if (PlikManager::wczytajProfil(sciezkaProfilu(), wczytanyProfil))
    {
        profil = wczytanyProfil;
    }

    std::vector<Produkt> produkty;

    if (PlikManager::wczytajProdukty(sciezkaProduktow(), produkty) && !produkty.empty())
    {
        for (const Produkt& produkt : produkty)
        {
            bazaProduktow.dodajProdukt(produkt);
        }
    }
    else
    {
        Produkt kurczak("Pierś z kurczaka", { 165.0, 31.0, 0.0, 3.6 });
        Produkt ryz("Ryż biały", { 350.0, 7.0, 79.0, 1.0 });
        ryz.dodajJednostke("opakowanie", 500.0);

        Produkt jajko("Jajko", { 143.0, 12.6, 0.7, 9.5 });
        jajko.dodajJednostke("sztuka", 55.0);

        Produkt banan("Banan", { 89.0, 1.1, 22.8, 0.3 });
        banan.dodajJednostke("sztuka", 120.0);

        Produkt twarog("Twaróg chudy", { 86.0, 18.0, 3.5, 0.5 });
        Produkt oliwa("Oliwa z oliwek", { 884.0, 0.0, 0.0, 100.0 });
        oliwa.dodajJednostke("łyżka", 10.0);

        Produkt platki("Płatki owsiane", { 370.0, 13.0, 62.0, 7.0 });
        Produkt jablko("Jabłko", { 52.0, 0.3, 14.0, 0.2 });
        jablko.dodajJednostke("sztuka", 180.0);

        Produkt chleb("Chleb żytni", { 259.0, 6.5, 48.0, 1.8 });
        chleb.dodajJednostke("kromka", 35.0);

        Produkt maslo("Masło orzechowe", { 588.0, 25.0, 20.0, 50.0 });
        maslo.dodajJednostke("łyżka", 20.0);

        for (const Produkt& produkt : {
            kurczak,
            ryz,
            jajko,
            banan,
            twarog,
            oliwa,
            platki,
            jablko,
            chleb,
            maslo
            })
        {
            bazaProduktow.dodajProdukt(produkt);
        }

        PlikManager::zapiszProdukty(
            sciezkaProduktow(),
            bazaProduktow.pobierzWszystkie()
        );
    }

    if (!PlikManager::wczytajDziennik(
        sciezkaDziennikaDlaDaty(aktualnaData),
        dziennik))
    {
        dziennik = DziennikZywieniowy();
        dziennik.ustawLimitKalorii(profil.pobierzLimitKalorii());
    }
}

void MainWindow::zapiszDaneDoPlikow()
{
    PlikManager::zapiszProfil(sciezkaProfilu(), profil);

    PlikManager::zapiszProdukty(
        sciezkaProduktow(),
        bazaProduktow.pobierzWszystkie()
    );

    PlikManager::zapiszDziennik(
        sciezkaDziennikaDlaDaty(aktualnaData),
        dziennik
    );
}

void MainWindow::on_buttonPoprzedniDzien_clicked()
{
    zapiszDaneDoPlikow();

    aktualnaData = aktualnaData.addDays(-1);
    dziennik = DziennikZywieniowy();
    dziennik.ustawLimitKalorii(profil.pobierzLimitKalorii());

    PlikManager::wczytajDziennik(
        sciezkaDziennikaDlaDaty(aktualnaData),
        dziennik
    );

    odswiezDziennik();
}

void MainWindow::on_buttonNastepnyDzien_clicked()
{
    zapiszDaneDoPlikow();

    aktualnaData = aktualnaData.addDays(1);
    dziennik = DziennikZywieniowy();
    dziennik.ustawLimitKalorii(profil.pobierzLimitKalorii());

    PlikManager::wczytajDziennik(
        sciezkaDziennikaDlaDaty(aktualnaData),
        dziennik
    );

    odswiezDziennik();
}

void MainWindow::on_buttonDodajSniadanie_clicked()
{
    dodajProduktDoPory(PoraPosilku::Sniadanie);
}

void MainWindow::on_buttonDodajDrugieSniadanie_clicked()
{
    dodajProduktDoPory(PoraPosilku::DrugieSniadanie);
}

void MainWindow::on_buttonDodajObiad_clicked()
{
    dodajProduktDoPory(PoraPosilku::Obiad);
}

void MainWindow::on_buttonDodajKolacja_clicked()
{
    dodajProduktDoPory(PoraPosilku::Kolacja);
}

void MainWindow::on_buttonDodajPrzekaski_clicked()
{
    dodajProduktDoPory(PoraPosilku::Przekaski);
}

void MainWindow::on_buttonZapiszProfil_clicked()
{
    const QString imie = ui.lineEditImie->text().trimmed();
    const int wiek = ui.spinWiek->value();
    const double waga = ui.doubleSpinWaga->value();
    const double wzrost = ui.doubleSpinWzrost->value();
    const double limit = ui.doubleSpinLimitKalorii->value();

    if (imie.isEmpty())
    {
        QMessageBox::warning(this, "Błąd", "Imię nie może być puste.");
        return;
    }

    if (wiek < 8 || wiek > 120)
    {
        QMessageBox::warning(this, "Błąd", "Wiek poza zakresem 8-120.");
        return;
    }

    if (waga < 20.0 || waga > 300.0)
    {
        QMessageBox::warning(this, "Błąd", "Waga poza zakresem 20-300 kg.");
        return;
    }

    if (wzrost < 50.0 || wzrost > 250.0)
    {
        QMessageBox::warning(this, "Błąd", "Wzrost poza zakresem 50-250 cm.");
        return;
    }

    if (limit < 800.0 || limit > 10000.0)
    {
        QMessageBox::warning(this, "Błąd", "Limit kalorii poza zakresem 800-10000.");
        return;
    }

    profil.ustawImie(imie.toStdString());
    profil.ustawWiek(wiek);
    profil.ustawWage(waga);
    profil.ustawWzrost(wzrost);
    profil.ustawLimitKalorii(limit);

    dziennik.ustawLimitKalorii(limit);

    zapiszDaneDoPlikow();
    odswiezDziennik();

    QMessageBox::information(this, "Profil", "Profil zapisany.");
}

void MainWindow::zaladujWektorDoTabeli(
    QTableWidget* tabela,
    const std::vector<Produkt>& produkty
)
{
    tabela->setColumnCount(7);
    tabela->setHorizontalHeaderLabels({
        "Nazwa",
        "kcal",
        "Białko",
        "Węglowodany",
        "Tłuszcz",
        "Ulubione",
        "Usuń"
        });

    tabela->setSelectionBehavior(QAbstractItemView::SelectRows);
    tabela->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabela->verticalHeader()->setVisible(false);
    tabela->verticalHeader()->setDefaultSectionSize(42);

    tabela->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tabela->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    tabela->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);

    tabela->setRowCount(0);

    for (int i = 0; i < static_cast<int>(produkty.size()); ++i)
    {
        tabela->insertRow(i);

        const Produkt& produkt = produkty[static_cast<std::size_t>(i)];
        const QString nazwa = QString::fromStdString(produkt.pobierzNazwe());
        const Makroskladniki makro = produkt.pobierzMakroNa100g();

        tabela->setItem(i, 0, new QTableWidgetItem(nazwa));
        tabela->setItem(i, 1, new QTableWidgetItem(QString::number(makro.kalorie, 'f', 0)));
        tabela->setItem(i, 2, new QTableWidgetItem(QString::number(makro.bialko, 'f', 1)));
        tabela->setItem(i, 3, new QTableWidgetItem(QString::number(makro.weglowodany, 'f', 1)));
        tabela->setItem(i, 4, new QTableWidgetItem(QString::number(makro.tluszcz, 'f', 1)));

        auto* przyciskUlubione = new QPushButton(produkt.czyUlubiony() ? "★" : "☆", tabela);
        przyciskUlubione->setCursor(Qt::PointingHandCursor);
        przyciskUlubione->setStyleSheet(
            produkt.czyUlubiony()
            ? "color: #f59e0b; font-size: 22px; border: none; background: transparent;"
            : "color: #9ca3af; font-size: 22px; border: none; background: transparent;"
        );

        tabela->setCellWidget(i, 5, przyciskUlubione);

        connect(
            przyciskUlubione,
            &QPushButton::clicked,
            this,
            [this, nazwa]()
            {
                bazaProduktow.przelaczUlubiony(nazwa.toStdString());
                zapiszDaneDoPlikow();
                odswiezTabeleProduktow();
            }
        );

        auto* przyciskUsun = new QPushButton(QStringLiteral("×"), tabela);
        przyciskUsun->setCursor(Qt::PointingHandCursor);
        przyciskUsun->setStyleSheet(
            "color: #dc2626;"
            "font-size: 20px;"
            "font-weight: 700;"
            "border: none;"
            "background: transparent;"
        );

        tabela->setCellWidget(i, 6, przyciskUsun);

        connect(
            przyciskUsun,
            &QPushButton::clicked,
            this,
            [this, nazwa]()
            {
                const QMessageBox::StandardButton odpowiedz =
                    QMessageBox::question(
                        this,
                        "Usuwanie produktu",
                        "Czy na pewno chcesz trwale usunąć produkt:\n" + nazwa + "?",
                        QMessageBox::Yes | QMessageBox::No
                    );

                if (odpowiedz != QMessageBox::Yes)
                {
                    return;
                }

                bazaProduktow.usunProdukt(nazwa.toStdString());
                zapiszDaneDoPlikow();
                odswiezTabeleProduktow();
            }
        );
    }
}

void MainWindow::odswiezTabeleProduktow()
{
    const std::string fraza = ui.lineEditSzukajProduktu->text().trimmed().toStdString();

    if (fraza.empty())
    {
        zaladujWektorDoTabeli(
            ui.tableProdukty,
            bazaProduktow.pobierzWszystkie()
        );
    }
    else
    {
        zaladujWektorDoTabeli(
            ui.tableProdukty,
            bazaProduktow.wyszukajProdukt(fraza)
        );
    }

    zaladujWektorDoTabeli(
        ui.tableUlubione,
        bazaProduktow.pobierzUlubione()
    );
}

void MainWindow::on_lineEditSzukajProduktu_textChanged(const QString&)
{
    odswiezTabeleProduktow();
}

void MainWindow::on_buttonDodajProdukt_clicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Dodaj nowy produkt");
    dialog.setMinimumWidth(400);
    dialog.setModal(true);

    QFormLayout layout(&dialog);
    layout.setSpacing(12);
    layout.setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QLineEdit editNazwa(&dialog);
    editNazwa.setPlaceholderText("np. Jajko, Ryż biały, Banan");
    editNazwa.setMinimumHeight(30);

    QDoubleSpinBox spinKcal(&dialog);
    QDoubleSpinBox spinBialko(&dialog);
    QDoubleSpinBox spinWegle(&dialog);
    QDoubleSpinBox spinTluszcz(&dialog);

    spinKcal.setRange(0.0, 1000.0);
    spinBialko.setRange(0.0, 100.0);
    spinWegle.setRange(0.0, 100.0);
    spinTluszcz.setRange(0.0, 100.0);

    spinKcal.setDecimals(0);
    spinBialko.setDecimals(1);
    spinWegle.setDecimals(1);
    spinTluszcz.setDecimals(1);

    QComboBox comboJednostka(&dialog);
    comboJednostka.addItems({
        "Brak - wartości są na 100 g",
        "sztuka",
        "opakowanie",
        "porcja",
        "ml",
        "łyżka",
        "szklanka"
        });

    QDoubleSpinBox spinWagaJednostki(&dialog);
    spinWagaJednostki.setRange(0.0, 5000.0);
    spinWagaJednostki.setDecimals(0);
    spinWagaJednostki.setValue(0.0);
    spinWagaJednostki.setSuffix(" g");

    auto dodajWiersz = [&](const QString& tekst, QWidget* pole)
        {
            auto* label = new QLabel(tekst, &dialog);
            label->setMinimumWidth(165);
            label->setStyleSheet("font-weight: 600; color: #444;");
            layout.addRow(label, pole);
        };

    dodajWiersz("Nazwa produktu:", &editNazwa);
    dodajWiersz("Kalorie na 100 g:", &spinKcal);
    dodajWiersz("Białko na 100 g:", &spinBialko);
    dodajWiersz("Węglowodany na 100 g:", &spinWegle);
    dodajWiersz("Tłuszcz na 100 g:", &spinTluszcz);
    dodajWiersz("Dodatkowa jednostka:", &comboJednostka);
    dodajWiersz("Waga jednostki:", &spinWagaJednostki);

    QDialogButtonBox przyciski(
        QDialogButtonBox::Cancel | QDialogButtonBox::Ok,
        &dialog
    );

    przyciski.button(QDialogButtonBox::Ok)->setText("Zapisz produkt");
    przyciski.button(QDialogButtonBox::Cancel)->setText("Anuluj");

    layout.addRow(&przyciski);

    connect(&przyciski, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&przyciski, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }

    const std::string nazwa = editNazwa.text().trimmed().toStdString();

    if (nazwa.empty())
    {
        QMessageBox::warning(this, "Błąd", "Nazwa produktu nie może być pusta.");
        return;
    }

    Produkt nowyProdukt(
        nazwa,
        {
            spinKcal.value(),
            spinBialko.value(),
            spinWegle.value(),
            spinTluszcz.value()
        }
    );

    if (comboJednostka.currentIndex() != 0)
    {
        if (spinWagaJednostki.value() <= 0.0)
        {
            QMessageBox::warning(
                this,
                "Błąd",
                "Dla dodatkowej jednostki podaj wagę większą od 0 g."
            );
            return;
        }

        nowyProdukt.dodajJednostke(
            comboJednostka.currentText().toStdString(),
            spinWagaJednostki.value()
        );
    }

    if (!nowyProdukt.czyPoprawny())
    {
        QMessageBox::warning(
            this,
            "Błąd",
            "Wprowadzone wartości produktu są niepoprawne."
        );
        return;
    }

    const std::size_t liczbaPrzed = bazaProduktow.pobierzWszystkie().size();
    bazaProduktow.dodajProdukt(nowyProdukt);
    const std::size_t liczbaPo = bazaProduktow.pobierzWszystkie().size();

    if (liczbaPo == liczbaPrzed)
    {
        QMessageBox::warning(
            this,
            "Produkt",
            "Nie dodano produktu. Taki produkt może już istnieć albo dane są niepoprawne."
        );
        return;
    }

    zapiszDaneDoPlikow();
    odswiezTabeleProduktow();

    QMessageBox::information(this, "Produkt", "Produkt został dodany.");
}

QString MainWindow::komunikatBledu(
    DziennikZywieniowy::WynikOperacji wynik
) const
{
    switch (wynik)
    {
    case DziennikZywieniowy::WynikOperacji::Sukces:
        return "Operacja wykonana poprawnie.";

    case DziennikZywieniowy::WynikOperacji::PustaNazwa:
        return "Nazwa produktu nie może być pusta.";

    case DziennikZywieniowy::WynikOperacji::PustaJednostka:
        return "Nazwa jednostki nie może być pusta.";

    case DziennikZywieniowy::WynikOperacji::NiepoprawnaIlosc:
        return "Ilość musi być większa od 0.";

    case DziennikZywieniowy::WynikOperacji::NiepoprawnaJednostka:
        return "Jednostka produktu ma niepoprawną wagę w gramach.";

    case DziennikZywieniowy::WynikOperacji::NiepoprawneMakro:
        return "Dane makroskładników są niepoprawne.";

    case DziennikZywieniowy::WynikOperacji::NiepoprawnyLimit:
        return "Limit dzienny jest niepoprawny.";

    default:
        return "Nieznany błąd.";
    }
}
