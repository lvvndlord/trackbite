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
#include <QPushButton>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QLineEdit>


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

    // 1. Naprawa limitów w QDoubleSpinBox (nadpisanie domyślnego 99.99)
    ui.doubleSpinLimitKalorii->setMaximum(10000.0);
    ui.doubleSpinWaga->setMaximum(300.0);
    ui.doubleSpinWzrost->setMaximum(250.0);
    ui.doubleSpinKalorie->setMaximum(1000.0);
    ui.doubleSpinBialko->setMaximum(100.0);
    ui.doubleSpinWeglowodany->setMaximum(100.0);
    ui.doubleSpinTluszcze->setMaximum(100.0);

    // 2. Naprawa placeholderów (czyszczenie tekstu i ustawienie znaku wodnego)
    ui.lineEditSzukajProduktu->setText("");
    ui.lineEditSzukajProduktu->setPlaceholderText("Szukaj produktu...");
    ui.lineEditNazwaProduktu->setText("");
    ui.lineEditNazwaProduktu->setPlaceholderText("Nazwa produktu...");
    ui.lineEditImie->setText("");
    ui.lineEditImie->setPlaceholderText("Imię...");

    // 3. Automatyczne wyszukiwanie (filtrowanie) produktów w tabeli
    connect(ui.lineEditSzukajProduktu, &QLineEdit::textChanged, this, [this](const QString& tekst) {
        for (int i = 0; i < ui.tableProdukty->rowCount(); ++i) {
            QTableWidgetItem* elementNazwy = ui.tableProdukty->item(i, 0); // Pobieramy komórkę z nazwą (kolumna 0)
            if (elementNazwy) {
                // Ukrywamy wiersz, jeśli jego tekst nie zawiera wpisanej frazy (ignorując wielkość liter)
                bool pasuje = elementNazwy->text().contains(tekst, Qt::CaseInsensitive);
                ui.tableProdukty->setRowHidden(i, !pasuje);
            }
        }
    });

    // 4. Estetyka: ukrycie pionowych numerków (1, 2, 3...) po lewej stronie tabeli produktów
    ui.tableProdukty->verticalHeader()->setVisible(false);
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
            tabela->setColumnCount(8);
            tabela->setHorizontalHeaderLabels(
                { "Produkt", "Ilosc", "Gramy", "kcal", "Bialko", "Weglowodany", "Tluszcz", "" }
            );

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
    connect(tabela, &QTableWidget::cellClicked, this,
        [this, pora](int wiersz, int kolumna)
        {
            if (kolumna == 7)
            {
                return;
            }

            edytujPozycjeWPorze(pora, wiersz);
        });
}

std::vector<JednostkaProduktu> MainWindow::pobierzJednostkiDlaNazwy(
    const std::string& nazwaProduktu,
    const JednostkaProduktu* domyslnaJednostka
) const
{
    std::vector<JednostkaProduktu> suroweJednostki;

    // 1. Pobranie wszystkich jednostek z modelu (tutaj są wymieszane "g" i "gram")
    for (const Produkt& produkt : produkty)
    {
        if (produkt.pobierzNazwe() == nazwaProduktu)
        {
            suroweJednostki = produkt.pobierzJednostki();
            break;
        }
    }

    if (suroweJednostki.empty() && domyslnaJednostka != nullptr)
    {
        suroweJednostki.push_back(*domyslnaJednostka);
    }

    // 2. Filtrowanie i scalanie duplikatów
    std::vector<JednostkaProduktu> przefiltrowane;
    bool dodanoGrama = false;

    for (const JednostkaProduktu& j : suroweJednostki)
    {
        // Jeśli natrafimy na wariację grama (albo "g" z klasy Produkt, albo "gram" z JSONa)
        if (j.nazwa == "g" || j.nazwa == "gram")
        {
            if (!dodanoGrama)
            {
                // Wrzucamy tylko raz i wymuszamy spójną nazwę "gram"
                przefiltrowane.push_back({ "gram", 1.0 });
                dodanoGrama = true;
            }
        }
        else
        {
            // Inne jednostki (sztuka, porcja, ml itp.) przepuszczamy normalnie
            przefiltrowane.push_back(j);
        }
    }

    // 3. Zabezpieczenie: jeśli produkt z jakiegoś powodu w ogóle nie miał gramów
    if (!dodanoGrama)
    {
        przefiltrowane.push_back({ "gram", 1.0 });
    }

    return przefiltrowane;
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
        const JednostkaProduktu& j = dostepneJednostki[i];
        comboJednostka->addItem(
            QString("%1 (%2 g)")
                .arg(QString::fromStdString(j.nazwa))
                .arg(j.gramyNaJednostke, 0, 'f', j.gramyNaJednostke < 10 ? 1 : 0)
        );

        if (j.nazwa == domyslnaJednostka.nazwa &&
            j.gramyNaJednostke == domyslnaJednostka.gramyNaJednostke)
        {
            indeksDomyslnejJednostki = static_cast<int>(i);
        }
    }

    comboJednostka->setCurrentIndex(indeksDomyslnejJednostki);

    auto* etykietaGramy = new QLabel(&dialog);

    auto odswiezPodgladGramow = [&]()
        {
            const JednostkaProduktu wybrana =
                dostepneJednostki[static_cast<std::size_t>(comboJednostka->currentIndex())];
            const double gramy = spinIlosc->value() * wybrana.gramyNaJednostke;
            const Makroskladniki makro = makroNa100g.przeliczNaGramy(gramy);

            etykietaGramy->setText(
                QString("<b>Razem:</b> %1 g | <b>%2 kcal</b><br/>"
                        "B: %3 g | W: %4 g | T: %5 g")
                    .arg(gramy, 0, 'f', 0)
                    .arg(makro.kalorie, 0, 'f', 0)
                    .arg(makro.bialko, 0, 'f', 1)
                    .arg(makro.weglowodany, 0, 'f', 1)
                    .arg(makro.tluszcz, 0, 'f', 1)
            );
        };

    connect(spinIlosc, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        &dialog, [&](double) { odswiezPodgladGramow(); });
    connect(comboJednostka, QOverload<int>::of(&QComboBox::currentIndexChanged),
        &dialog, [&](int) { odswiezPodgladGramow(); });

    odswiezPodgladGramow();

    auto* formularz = new QFormLayout();
    formularz->addRow("Ilosc:", spinIlosc);
    formularz->addRow("Jednostka:", comboJednostka);
    layout->addLayout(formularz);
    layout->addWidget(etykietaGramy);

    auto* przyciski = new QDialogButtonBox(
        QDialogButtonBox::Cancel | QDialogButtonBox::Ok,
        &dialog
    );
    layout->addWidget(przyciski);

    connect(przyciski, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    connect(przyciski, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);

    if (dialog.exec() != QDialog::Accepted)
    {
        return false;
    }

    wybranaIlosc = spinIlosc->value();
    wybranaJednostka =
        dostepneJednostki[static_cast<std::size_t>(comboJednostka->currentIndex())];

    return true;
}

void MainWindow::edytujPozycjeWPorze(PoraPosilku pora, int indeksWiersza)
{
    const std::vector<PozycjaDziennika> pozycje =
        dziennik.pobierzPozycjeDlaPory(pora);

    if (indeksWiersza < 0 || indeksWiersza >= static_cast<int>(pozycje.size()))
    {
        return;
    }

    const PozycjaDziennika& pozycja = pozycje[static_cast<std::size_t>(indeksWiersza)];

    const std::vector<JednostkaProduktu> dostepneJednostki = pobierzJednostkiDlaNazwy(
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

    const DziennikZywieniowy::WynikOperacji wynik = dziennik.edytujPozycjeDlaPory(
        pora,
        static_cast<std::size_t>(indeksWiersza),
        nowaIlosc,
        nowaJednostka
    );

    if (wynik != DziennikZywieniowy::WynikOperacji::Sukces)
    {
        QMessageBox::warning(this, "Blad danych", komunikatBledu(wynik));
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

    if (!dziennik.usunPozycjeDlaPory(pora, static_cast<std::size_t>(indeksWiersza)))
    {
        QMessageBox::warning(this, "Blad", "Nie udalo sie usunac pozycji.");
        return;
    }

    zapiszDaneDoPlikow();
    odswiezDziennik();
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

    // Zmieniony format wyświetlania nagłówka z makroskładnikami
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

        auto* przyciskUsun = new QPushButton(QStringLiteral("×"), tabela);
        przyciskUsun->setFixedSize(28, 28);
        przyciskUsun->setCursor(Qt::PointingHandCursor);
        przyciskUsun->setToolTip("Usun pozycje");
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
        connect(przyciskUsun, &QPushButton::clicked, this,
            [this, pora, wiersz]()
            {
                usunPozycjeWPorze(pora, wiersz);
            });

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

    double zostalo = dziennik.pozostaleKalorie();

    if (zostalo < 0) {
        // Przekroczenie limitu -> kolor czerwony, zmieniony tekst
        ui.labelZostaloKalorii->setText(QString("%1 kcal").arg(std::abs(zostalo), 0, 'f', 0));
        ui.labelTytulZostalo->setText("Przekroczono");
        ui.labelZostaloKalorii->setStyleSheet("color: #dc2626;");
        
        // ZMIANA: Zablokowanie dziedziczenia ramki na tekst
        ui.frameStatZostalo->setStyleSheet(
            "QFrame#frameStatZostalo { background: #fef2f2; border: 1px solid #fca5a5; border-radius: 10px; }"
        );
    } else {
        // Standardowy stan -> kolor zielony
        ui.labelZostaloKalorii->setText(QString("%1 kcal").arg(zostalo, 0, 'f', 0));
        ui.labelTytulZostalo->setText("Zostało");
        ui.labelZostaloKalorii->setStyleSheet("color: #2e7d32;"); 
        
        // ZMIANA: Zablokowanie dziedziczenia ramki na tekst
        ui.frameStatZostalo->setStyleSheet(
            "QFrame#frameStatZostalo { background: #edf8e8; border: 1px solid #cfe8c2; border-radius: 10px; }"
        ); 
    }

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

            // Jeśli procent > 100, malujemy na czerwono. Inaczej na zielono.
            if (procent > 100) {
                pasek->setStyleSheet("QProgressBar::chunk { background: #ef4444; border-radius: 5px; }");
            } else {
                pasek->setStyleSheet("QProgressBar::chunk { background: #6fba44; border-radius: 5px; }");
            }
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

void MainWindow::dodajProduktDoPory(PoraPosilku pora)
{
    if (produkty.empty())
    {
        QMessageBox::warning(this, "Brak produktów", "Baza produktów jest pusta.");
        return;
    }

    // --- ETAP 1: Okno wyszukiwania ---
    QDialog dialogListy(this);
    dialogListy.setWindowTitle("Wybierz produkt");
    dialogListy.resize(350, 450);
    dialogListy.setModal(true);

    auto* layoutListy = new QVBoxLayout(&dialogListy);
    auto* poleWyszukiwania = new QLineEdit(&dialogListy);
    poleWyszukiwania->setPlaceholderText("Szukaj produktu...");
    layoutListy->addWidget(poleWyszukiwania);

    auto* listaProduktow = new QListWidget(&dialogListy);
    for (const Produkt& p : produkty)
    {
        listaProduktow->addItem(QString::fromStdString(p.pobierzNazwe()));
    }
    layoutListy->addWidget(listaProduktow);

    auto* przyciskiListy = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok, &dialogListy);
    layoutListy->addWidget(przyciskiListy);

    connect(poleWyszukiwania, &QLineEdit::textChanged, [&](const QString& tekst) {
        for (int i = 0; i < listaProduktow->count(); ++i) {
            QListWidgetItem* el = listaProduktow->item(i);
            el->setHidden(!el->text().contains(tekst, Qt::CaseInsensitive));
        }
    });

    connect(listaProduktow, &QListWidget::itemDoubleClicked, &dialogListy, &QDialog::accept);
    connect(przyciskiListy, &QDialogButtonBox::accepted, &dialogListy, &QDialog::accept);
    connect(przyciskiListy, &QDialogButtonBox::rejected, &dialogListy, &QDialog::reject);

    if (dialogListy.exec() != QDialog::Accepted || !listaProduktow->currentItem()) return;

    // --- ETAP 2: Wybór ilości (domyślnie 100 gramów) ---
    QString wybranaNazwa = listaProduktow->currentItem()->text();
    auto it = std::find_if(produkty.begin(), produkty.end(), [&](const Produkt& p) {
        return p.pobierzNazwe() == wybranaNazwa.toStdString();
    });
    if (it == produkty.end()) return;

    const Produkt& wybranyProdukt = *it;
    std::vector<JednostkaProduktu> dostepneJednostki = pobierzJednostkiDlaNazwy(wybranyProdukt.pobierzNazwe());

    // Szukamy jednostki "gram" i wymuszamy 100.0 jako start
    JednostkaProduktu domyslnaJednostka = dostepneJednostki.front();
    double domyslnaIlosc = 1.0;
    
    for (const auto& j : dostepneJednostki) {
        if (j.nazwa == "gram" || j.nazwa == "g") {
            domyslnaJednostka = j;
            domyslnaIlosc = 100.0;
            break;
        }
    }

    double wybranaIlosc = domyslnaIlosc;
    JednostkaProduktu wybranaJednostka = domyslnaJednostka;

    if (!pokazDialogIlosci("Podaj ilość", wybranyProdukt.pobierzNazwe(), wybranyProdukt.pobierzMakroNa100g(),
        dostepneJednostki, domyslnaIlosc, domyslnaJednostka, wybranaIlosc, wybranaJednostka))
    {
        return; 
    }

    // --- ETAP 3: Dodanie do dziennika ---
    const Makroskladniki makro = wybranyProdukt.pobierzMakroNa100g();
    dziennik.dodajPozycje(wybranyProdukt.pobierzNazwe(), wybranaIlosc, wybranaJednostka,
                          makro.kalorie, makro.bialko, makro.weglowodany, makro.tluszcz, pora);

    zapiszDaneDoPlikow();
    odswiezDziennik();
}

// Wczytywanie / zapisywanie danych
void MainWindow::wczytajDaneZPlikow()
{
    // Profil
    ProfilUzytkownika p;
    if (PlikManager::wczytajProfil("profil.txt", p))
    {
        profil = p;
        ui.lineEditImie->setText(QString::fromStdString(profil.pobierzImie()));
        ui.spinWiek->setValue(profil.pobierzWiek());
        ui.doubleSpinWaga->setValue(profil.pobierzWage());
        ui.doubleSpinWzrost->setValue(profil.pobierzWzrost());
        ui.doubleSpinLimitKalorii->setValue(profil.pobierzLimitKalorii());
        // --------------------------------
    }

    std::vector<Produkt> pb;
    PlikManager::wczytajProdukty("produkty.json", pb); 
    if (!pb.empty()) produkty = std::move(pb);

    // Dziennik - dynamiczna nazwa pliku na podstawie daty
    QString nazwaPliku = QString("dziennik_%1.txt").arg(aktualnaData.toString("yyyy_MM_dd"));
    PlikManager::wczytajDziennik(nazwaPliku.toStdString(), dziennik);
}

void MainWindow::zapiszDaneDoPlikow()
{
    PlikManager::zapiszProfil("profil.txt", profil);
    
    // ZMIANA NA .json
    PlikManager::zapiszProdukty("produkty.json", produkty); 

    // Dziennik - dynamiczna nazwa pliku na podstawie daty
    QString nazwaPliku = QString("dziennik_%1.txt").arg(aktualnaData.toString("yyyy_MM_dd"));
    PlikManager::zapiszDziennik(nazwaPliku.toStdString(), dziennik);
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

void MainWindow::on_buttonPoprzedniDzien_clicked()
{
    zapiszDaneDoPlikow(); // Zapisujemy modyfikacje bieżącego dnia
    
    aktualnaData = aktualnaData.addDays(-1);
    
    dziennik.wyczysc(); // Czyścimy listę posiłków ze starego dnia
    
    QString nazwaPliku = QString("dziennik_%1.txt").arg(aktualnaData.toString("yyyy_MM_dd"));
    PlikManager::wczytajDziennik(nazwaPliku.toStdString(), dziennik); // Ładujemy nowy dzień
    
    odswiezDziennik();
}

void MainWindow::on_buttonNastepnyDzien_clicked()
{
    zapiszDaneDoPlikow(); // Zapisujemy modyfikacje bieżącego dnia
    
    aktualnaData = aktualnaData.addDays(1);
    
    dziennik.wyczysc(); // Czyścimy listę posiłków ze starego dnia
    
    QString nazwaPliku = QString("dziennik_%1.txt").arg(aktualnaData.toString("yyyy_MM_dd"));
    PlikManager::wczytajDziennik(nazwaPliku.toStdString(), dziennik); // Ładujemy nowy dzień
    
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

    QString nazwaPliku = QString("dziennik_%1.txt").arg(aktualnaData.toString("yyyy_MM_dd"));
    PlikManager::zapiszDziennik(nazwaPliku.toStdString(), dziennik);

    QMessageBox::information(this, "Profil", "Profil zapisany.");
}

void MainWindow::odswiezTabeleProduktow()
{
    ui.tableProdukty->setRowCount(0); 
    ui.tableProdukty->setRowCount(static_cast<int>(produkty.size()));

    for (int i = 0; i < static_cast<int>(produkty.size()); ++i)
    {
        const Produkt& p = produkty[i];
        const Makroskladniki m = p.pobierzMakroNa100g();

        ui.tableProdukty->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(p.pobierzNazwe())));
        ui.tableProdukty->setItem(i, 1, new QTableWidgetItem(QString::number(m.kalorie, 'f', 0)));
        ui.tableProdukty->setItem(i, 2, new QTableWidgetItem(QString::number(m.bialko, 'f', 1)));
        ui.tableProdukty->setItem(i, 3, new QTableWidgetItem(QString::number(m.weglowodany, 'f', 1)));
        ui.tableProdukty->setItem(i, 4, new QTableWidgetItem(QString::number(m.tluszcz, 'f', 1)));
    }
}

void MainWindow::on_buttonDodajProdukt_clicked()
{
    std::string nazwa = ui.lineEditNazwaProduktu->text().toStdString();
    double kcal = ui.doubleSpinKalorie->value();
    double bialko = ui.doubleSpinBialko->value();
    double wegle = ui.doubleSpinWeglowodany->value();
    double tluszcz = ui.doubleSpinTluszcze->value();

    if (nazwa.empty() || nazwa == "Nazwa produktu")
    {
        QMessageBox::warning(this, "Błąd", "Podaj poprawną nazwę produktu.");
        return;
    }

    // Tworzenie nowego produktu
    Produkt nowyProdukt(nazwa, Makroskladniki{kcal, bialko, wegle, tluszcz});
    
    // Dodajemy go do naszej bazy
    produkty.push_back(nowyProdukt);

    // Zapisujemy nowy stan do pliku
    PlikManager::zapiszProdukty("produkty.json", produkty);

    // Odświeżamy tabelę, żeby nowy produkt się w niej pojawił
    odswiezTabeleProduktow();

    QMessageBox::information(this, "Sukces", "Produkt dodany do bazy.");
}