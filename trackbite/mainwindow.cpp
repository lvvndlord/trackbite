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
}

MainWindow::~MainWindow()
{
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