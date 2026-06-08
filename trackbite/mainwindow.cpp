// Implementacja głównego okna aplikacji Fit Plan.
// Ten plik łączy interfejs Qt z logiką dziennika, bazy produktów, profilu i zapisu do plików.
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
    // Stałe nazwy plików trzymamy w funkcjach pomocniczych, żeby nie rozrzucać stringów po całej klasie.
    std::string sciezkaProfilu()
    {
        return "profil.json";
    }

    // Plik z produktami jest wspólny dla całej aplikacji, niezależnie od wybranego dnia.
    std::string sciezkaProduktow()
    {
        return "produkty.json";
    }

    // Dziennik ma osobny plik dla każdej daty, np. dziennik_2026_06_08.json.
    std::string sciezkaDziennikaDlaDaty(const QDate& data)
    {
        return QString("dziennik_%1.json")
            .arg(data.toString("yyyy_MM_dd"))
            .toStdString();
    }

    // Centralny arkusz stylów Qt. Dzięki temu wygląd aplikacji jest ustawiany w jednym miejscu.
    QString stylAplikacji()
    {
        return R"(
            QMainWindow {
                background: #f5f7fb;
            }

            QWidget {
                color: #0f172a;
                font-family: "Segoe UI";
                font-size: 9.5pt;
            }

            QWidget#centralWidget,
            QWidget#dziennikTab,
            QWidget#produktyTab,
            QWidget#profilTab,
            QWidget#scrollAreaWidgetContents,
            QWidget#produktyContent,
            QWidget#profilContent {
                background: #f5f7fb;
            }

            QTabWidget::pane {
                border: none;
                background: #f5f7fb;
                top: -1px;
            }

            QTabBar::tab {
                background: #e2e8f0;
                color: #334155;
                padding: 10px 22px;
                border: 1px solid #cbd5e1;
                border-bottom: none;
                border-top-left-radius: 12px;
                border-top-right-radius: 12px;
                margin-right: 4px;
                min-width: 92px;
                font-weight: 800;
            }

            QTabBar::tab:selected {
                background: #ffffff;
                color: #15803d;
                border-color: #dbe4ee;
            }

            QTabBar::tab:hover:!selected {
                background: #edf2f7;
                color: #0f172a;
            }

            QScrollArea,
            QScrollArea > QWidget,
            QScrollArea > QWidget > QWidget {
                background: transparent;
                border: none;
            }

            QLabel {
                background: transparent;
                border: none;
                color: #334155;
            }

            QFrame {
                background: #ffffff;
                border: 1px solid #e5e7eb;
                border-radius: 20px;
            }

            QLabel {
                background: transparent;
                border: none;
                color: #334155;
            }

            QLineEdit,
            QSpinBox,
            QDoubleSpinBox,
            QComboBox {
                background: #ffffff;
                color: #0f172a;
                border: 1px solid #cbd5e1;
                border-radius: 14px;
                padding: 8px 12px;
                min-height: 28px;
                selection-background-color: #22c55e;
                selection-color: #ffffff;
            }

            QLineEdit:focus,
            QSpinBox:focus,
            QDoubleSpinBox:focus,
            QComboBox:focus {
                border: 2px solid #22c55e;
                padding: 7px 11px;
            }

            QComboBox::drop-down {
                border: none;
                width: 30px;
            }

            QComboBox QAbstractItemView {
                background: #ffffff;
                color: #0f172a;
                border: 1px solid #cbd5e1;
                selection-background-color: #dcfce7;
                selection-color: #14532d;
            }

            QPushButton {
                background: #22c55e;
                color: #ffffff;
                border: none;
                border-radius: 14px;
                padding: 9px 16px;
                font-weight: 800;
            }

            QPushButton:hover {
                background: #16a34a;
            }

            QPushButton:pressed {
                background: #15803d;
            }

            QTableWidget {
                background: #ffffff;
                color: #0f172a;
                border: 1px solid #e5e7eb;
                border-radius: 14px;
                gridline-color: #e5e7eb;
                selection-background-color: #dcfce7;
                selection-color: #14532d;
                alternate-background-color: #f8fafc;
            }

            QTableWidget::item {
                color: #0f172a;
                padding: 8px;
                border: none;
            }

            QHeaderView::section {
                background: #f8fafc;
                color: #475569;
                border: none;
                border-bottom: 1px solid #e5e7eb;
                padding: 8px;
                font-weight: 800;
            }

            QProgressBar {
                background: #e2e8f0;
                border: none;
                border-radius: 7px;
                min-height: 14px;
                max-height: 14px;
                text-align: center;
                color: #0f172a;
                font-size: 9px;
                font-weight: 800;
            }

            QProgressBar::chunk {
                background: #22c55e;
                border-radius: 7px;
            }

            QScrollBar:vertical {
                background: transparent;
                width: 10px;
                margin: 4px 0 4px 0;
            }

            QScrollBar::handle:vertical {
                background: #cbd5e1;
                border-radius: 5px;
                min-height: 40px;
            }

            QScrollBar::handle:vertical:hover {
                background: #94a3b8;
            }

            QScrollBar::add-line:vertical,
            QScrollBar::sub-line:vertical,
            QScrollBar::add-page:vertical,
            QScrollBar::sub-page:vertical {
                height: 0px;
                background: none;
                border: none;
            }

            /* DZIENNIK - style kart, nagłówka dnia i podsumowania posiłków */
            QFrame#frameStatusDnia,
            QFrame#frameProduktyHero,
            QFrame#frameProfilHero {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #16a34a, stop:1 #22c55e);
                border: none;
                border-radius: 26px;
            }

            QLabel#labelDziennikTitle,
            QLabel#labelProduktyTitle,
            QLabel#label_3 {
                color: #ffffff;
                font-size: 26px;
                font-weight: 900;
            }

            QLabel#labelDziennikSubtitle,
            QLabel#labelProduktySubtitle,
            QLabel#labelProfilSubtitle {
                color: #dcfce7;
                font-size: 11pt;
                font-weight: 600;
            }

            QLabel#labelDataDnia {
                color: #ffffff;
                font-size: 20px;
                font-weight: 900;
            }

            QPushButton#buttonPoprzedniDzien,
            QPushButton#buttonNastepnyDzien {
                background: #dcfce7;
                color: #15803d;
                border: 1px solid #bbf7d0;
                border-radius: 22px;
                padding: 0;
                font-size: 28px;
                font-weight: 900;
            }

            QPushButton#buttonPoprzedniDzien:hover,
            QPushButton#buttonNastepnyDzien:hover {
                background: #bbf7d0;
            }

            QFrame#frameStatZostalo,
            QFrame#frameStatSpozyto,
            QFrame#frameStatLimit {
                background: #ffffff;
                border: 1px solid #bbf7d0;
                border-radius: 18px;
            }

            QLabel#labelTytulZostalo,
            QLabel#labelTytulSpozyto,
            QLabel#labelTytulLimit {
                color: #64748b;
                font-size: 10pt;
                font-weight: 800;
            }

            QLabel#labelZostaloKalorii,
            QLabel#labelSpozytoKalorii,
            QLabel#labelLimitKalorii_2 {
                color: #0f172a;
                font-size: 17px;
                font-weight: 900;
            }

            QLabel#labelZostaloKalorii {
                color: #15803d;
            }

            QFrame#frameSniadanie,
            QFrame#frameDrugieSniadanie,
            QFrame#frameObiad,
            QFrame#framePrzekaski,
            QFrame#frameKolacja,
            QFrame#framePodsumowanieDolne,
            QFrame#frameProduktySearch,
            QFrame#frameProduktyTable,
            QFrame#frameProfilDane,
            QFrame#frameProfilCel,
            QFrame#frameProfilSummary {
                background: #ffffff;
                border: 1px solid #e5e7eb;
                border-radius: 22px;
            }

            QFrame#framePodsumowanieDolne {
                background: #ffffff;
                border: 1px solid #dbe4ee;
                border-radius: 22px;
            }

            QWidget#headerSniadanie,
            QWidget#headerDrugieSniadanie,
            QWidget#headerObiad,
            QWidget#headerPrzekaski,
            QWidget#headerKolacja {
                background: #ffffff;
                border: none;
            }

            QLabel#labelTytulSniadanie,
            QLabel#labelTytulDrugieSniadanie,
            QLabel#labelTytulObiad,
            QLabel#labelTytulPrzekaski,
            QLabel#labelTytulKolacja,
            QLabel#labelProduktySearchTitle,
            QLabel#labelProduktyWszystkieTitle,
            QLabel#labelProduktyUlubioneTitle,
            QLabel#labelProfilSekcjaDane,
            QLabel#labelProfilSekcjaCel,
            QLabel#labelProfilSummaryTitle {
                color: #0f172a;
                font-size: 16px;
                font-weight: 900;
            }

            QLabel#labelKcalSniadanie,
            QLabel#labelKcalDrugieSniadanie,
            QLabel#labelKcalObiad,
            QLabel#labelKcalPrzekaski,
            QLabel#labelKcalKolacja,
            QLabel#labelProfilOpisDane,
            QLabel#labelProfilOpisCel,
            QLabel#labelProfilMetricTitleCzas,
            QLabel#labelProfilMetricTitleKcal {
                color: #64748b;
                font-size: 10pt;
                font-weight: 800;
            }

            QPushButton#buttonDodajSniadanie,
            QPushButton#buttonDodajDrugieSniadanie,
            QPushButton#buttonDodajObiad,
            QPushButton#buttonDodajPrzekaski,
            QPushButton#buttonDodajKolacja {
                background: #22c55e;
                color: #ffffff;
                border: none;
                border-radius: 21px;
                padding: 0px;
                min-width: 42px;
                max-width: 42px;
                min-height: 42px;
                max-height: 42px;
                font-size: 22px;
                font-weight: 900;
                text-align: center;
            }

            QPushButton#buttonDodajSniadanie:hover,
            QPushButton#buttonDodajDrugieSniadanie:hover,
            QPushButton#buttonDodajObiad:hover,
            QPushButton#buttonDodajPrzekaski:hover,
            QPushButton#buttonDodajKolacja:hover,
            QPushButton#buttonDodajProdukt:hover {
                background: #16a34a;
            }

            QFrame#frameMakroKalorie,
            QFrame#frameMakroBialko,
            QFrame#frameMakroWeglowodany,
            QFrame#frameMakroTluszcz,
            QFrame#frameProfilMetricCzas,
            QFrame#frameProfilMetricKcal {
                background: #f8fafc;
                border: 1px solid #e2e8f0;
                border-radius: 18px;
            }

            QFrame#frameProfilMetricKcal {
                background: #ecfdf5;
                border: 1px solid #bbf7d0;
            }

            QLabel#labelSumaKalorii,
            QLabel#labelSumaBialka,
            QLabel#labelSumaWeglowodanow,
            QLabel#labelSumaTluszczu,
            QLabel#label_5,
            QLabel#label_6 {
                color: #0f172a;
                font-weight: 900;
            }

            QLabel#label_6 {
                color: #15803d;
            }

            /* PRODUKTY - styl listy produktów, ulubionych i przycisku dodawania */
            QPushButton#buttonDodajProdukt {
                background: #ffffff;
                color: #15803d;
                border: 1px solid #bbf7d0;
                border-radius: 16px;
                padding: 10px 18px;
                font-weight: 900;
            }

            QPushButton#buttonDodajProdukt:hover {
                background: #ecfdf5;
            }

            QTableWidget#tableProdukty,
            QTableWidget#tableUlubione {
                min-height: 360px;
            }

            /* PROFIL - formularz danych użytkownika i celów wagowych */
            QLabel#labelWiek,
            QLabel#labelWaga,
            QLabel#labelWzrost,
            QLabel#labelWiek_2,
            QLabel#label,
            QLabel#label_2,
            QLabel#label_4 {
                color: #475569;
                font-weight: 800;
            }


            /* POPRAWKI: etykiety w kartach nie mogą dziedziczyć tła i obramowania QFrame */
            QLabel,
            QFrame QLabel {
                background: transparent;
                border: none;
            }

            QFrame#frameProduktyTable {
                background: #ffffff;
                border: 1px solid #e5e7eb;
                border-radius: 22px;
            }

            QTabWidget#tabWidgetProduktyListy::pane {
                background: #ffffff;
                border: none;
                top: -1px;
            }

            QTabWidget#tabWidgetProduktyListy QTabBar::tab {
                background: #f1f5f9;
                color: #475569;
                border: 1px solid #dbe4ee;
                border-bottom: none;
                border-top-left-radius: 14px;
                border-top-right-radius: 14px;
                padding: 10px 24px;
                margin-right: 6px;
                min-width: 120px;
                font-weight: 900;
            }

            QTabWidget#tabWidgetProduktyListy QTabBar::tab:selected {
                background: #ffffff;
                color: #15803d;
                border-color: #bbf7d0;
            }

            QTabWidget#tabWidgetProduktyListy QWidget {
                background: #ffffff;
            }

            QFrame#framePodsumowanieDolne {
                border-radius: 18px;
            }

            QFrame#frameMakroKalorie,
            QFrame#frameMakroBialko,
            QFrame#frameMakroWeglowodany,
            QFrame#frameMakroTluszcz {
                border-radius: 16px;
            }

            QLabel#labelSumaKalorii,
            QLabel#labelSumaBialka,
            QLabel#labelSumaWeglowodanow,
            QLabel#labelSumaTluszczu {
                color: #0f172a;
                font-size: 13px;
                font-weight: 900;
            }

            QDialog#dialogFit {
                background: #f5f7fb;
            }

            QDialog#dialogFit QLabel {
                color: #334155;
                background: transparent;
                border: none;
            }

            QDialog#dialogFit QLabel#dialogTitle {
                color: #0f172a;
                font-size: 20px;
                font-weight: 900;
            }

            QDialog#dialogFit QLabel#dialogSubtitle,
            QDialog#dialogFit QLabel#dialogPreview {
                color: #64748b;
                font-size: 10pt;
                font-weight: 700;
            }

            QDialog#dialogFit QLineEdit,
            QDialog#dialogFit QSpinBox,
            QDialog#dialogFit QDoubleSpinBox,
            QDialog#dialogFit QComboBox {
                background: #ffffff;
                color: #0f172a;
                border: 1px solid #cbd5e1;
                border-radius: 14px;
                padding: 9px 12px;
                min-height: 30px;
            }

            QDialog#dialogFit QLineEdit:focus,
            QDialog#dialogFit QDoubleSpinBox:focus,
            QDialog#dialogFit QComboBox:focus {
                border: 2px solid #22c55e;
                padding: 8px 11px;
            }

            QDialog#dialogFit QListWidget {
                background: #ffffff;
                color: #0f172a;
                border: 1px solid #e5e7eb;
                border-radius: 16px;
                padding: 8px;
            }

            QDialog#dialogFit QListWidget::item {
                min-height: 34px;
                padding: 8px 10px;
                border-radius: 10px;
            }

            QDialog#dialogFit QListWidget::item:selected {
                background: #dcfce7;
                color: #14532d;
            }

            QDialog#dialogFit QPushButton {
                background: #22c55e;
                color: #ffffff;
                border: none;
                border-radius: 14px;
                padding: 10px 18px;
                min-width: 96px;
                font-weight: 900;
            }

            QDialog#dialogFit QPushButton:hover {
                background: #16a34a;
            }

            QDialog#dialogFit QPushButton#buttonSecondary {
                background: #e2e8f0;
                color: #334155;
            }

            QDialog#dialogFit QPushButton#buttonSecondary:hover {
                background: #cbd5e1;
            }

        )";
    }

    // Ustawia pasek postępu i koloruje go na czerwono po przekroczeniu 100%.
    void ustawProgress(QProgressBar* pasek, int procent)
    {
        const int wartoscPaska = std::clamp(procent, 0, 100);

        pasek->setValue(wartoscPaska);
        pasek->setFormat(QString::number(procent) + "%");

        const QString kolor = procent > 100 ? "#ef4444" : "#22c55e";

        pasek->setStyleSheet(
            QString(
                "QProgressBar {"
                "background: #e2e8f0;"
                "border: none;"
                "border-radius: 7px;"
                "min-height: 14px;"
                "max-height: 14px;"
                "text-align: center;"
                "color: #0f172a;"
                "font-size: 9px;"
                "font-weight: 700;"
                "}"
                "QProgressBar::chunk {"
                "background: %1;"
                "border-radius: 7px;"
                "}"
            ).arg(kolor)
        );
    }

    // Porównuje jednostki z tolerancją dla liczb zmiennoprzecinkowych, bo wartości typu 1.0 mogą mieć drobne różnice.
    bool czyTaSamaJednostka(
        const JednostkaProduktu& pierwsza,
        const JednostkaProduktu& druga
    )
    {
        return pierwsza.nazwa == druga.nazwa
            && std::abs(pierwsza.gramyNaJednostke - druga.gramyNaJednostke) < 0.001;
    }
}

// Konstruktor składa całe okno: ustawia styl, ładuje dane, podpina profil i odświeża wszystkie widoki.
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    aktualnaData(QDate::currentDate())
{
    ui.setupUi(this);
    setStyleSheet(stylAplikacji());

    ui.lineEditSzukajProduktu->clear();
    ui.lineEditSzukajProduktu->setPlaceholderText("Szukaj produktu...");

    ui.lineEditImie->setPlaceholderText("Imię...");

    utworzSekcjeCeluProfilu();

    wczytajDaneZPlikow();

    // Flaga blokuje autozapis podczas programowego ustawiania pól, żeby start aplikacji nie wywoływał niepotrzebnych zmian.
    aktualizujeUi = true;

    limitKaloriiAuto = std::round(profil.pobierzLimitKalorii());

    if (comboCelTyp != nullptr)
    {
        const QString cel = QString::fromStdString(profil.pobierzCel());
        const int indeks = comboCelTyp->findText(cel);
        comboCelTyp->setCurrentIndex(indeks >= 0 ? indeks : 0);
    }

    if (spinWagaDocelowa != nullptr)
    {
        spinWagaDocelowa->setValue(std::round(profil.pobierzWageDocelowa()));
    }

    if (spinTempoWagi != nullptr)
    {
        spinTempoWagi->setValue(profil.pobierzTempoZmianyWagiTygodniowo());
    }

    if (ui.lineEditImie != nullptr)
    {
        ui.lineEditImie->setText(QString::fromStdString(profil.pobierzImie()));
    }

    if (ui.spinWiek != nullptr)
    {
        ui.spinWiek->setRange(8, 120);
        ui.spinWiek->setValue(profil.pobierzWiek());
    }

    if (ui.doubleSpinWaga != nullptr)
    {
        ui.doubleSpinWaga->setRange(20.0, 400.0);
        ui.doubleSpinWaga->setDecimals(0);
        ui.doubleSpinWaga->setSuffix(" kg");
        ui.doubleSpinWaga->setValue(std::round(profil.pobierzWage()));
    }

    if (ui.doubleSpinWzrost != nullptr)
    {
        ui.doubleSpinWzrost->setRange(100.0, 250.0);
        ui.doubleSpinWzrost->setDecimals(0);
        ui.doubleSpinWzrost->setSuffix(" cm");
        ui.doubleSpinWzrost->setValue(std::round(profil.pobierzWzrost()));
    }

    aktualizujeUi = false;

    ustawDziennikGui();
    odswiezTabeleProduktow();
    odswiezDziennik();
    odswiezSekcjeCelu();

    ui.tableProdukty->verticalHeader()->setVisible(false);
    ui.tableUlubione->verticalHeader()->setVisible(false);
}

// Destruktor zapisuje aktualny stan, żeby zamknięcie programu nie zgubiło zmian użytkownika.
MainWindow::~MainWindow()
{
    zapiszDaneDoPlikow();
}

// Sekcja celu jest wyszukiwana po nazwach kontrolek z pliku .ui, a brakujące etykiety są tworzone awaryjnie w kodzie.
void MainWindow::utworzSekcjeCeluProfilu()
{
    comboCelTyp = ui.profilTab->findChild<QComboBox*>("comboBoxCel");

    if (comboCelTyp == nullptr)
    {
        comboCelTyp = ui.profilTab->findChild<QComboBox*>("comboCelTyp");
    }

    spinWagaDocelowa = ui.profilTab->findChild<QDoubleSpinBox*>("doubleSpinBoxDocelowa");

    if (spinWagaDocelowa == nullptr)
    {
        spinWagaDocelowa = ui.profilTab->findChild<QDoubleSpinBox*>("doubleSpinWagaDocelowa");
    }

    spinTempoWagi = ui.profilTab->findChild<QDoubleSpinBox*>("doubleSpinBoxTempo");

    if (spinTempoWagi == nullptr)
    {
        spinTempoWagi = ui.profilTab->findChild<QDoubleSpinBox*>("doubleSpinTempoWagi");
    }

    labelSzacowanyCzas = ui.profilTab->findChild<QLabel*>("label_5");

    if (labelSzacowanyCzas == nullptr)
    {
        labelSzacowanyCzas = ui.profilTab->findChild<QLabel*>("labelSzacowanyCzas");
    }

    labelAutoKalorie = ui.profilTab->findChild<QLabel*>("label_6");

    if (labelAutoKalorie == nullptr)
    {
        labelAutoKalorie = ui.profilTab->findChild<QLabel*>("labelAutoKcalProfil");
    }

    if (labelSzacowanyCzas == nullptr)
    {
        labelSzacowanyCzas = new QLabel(ui.profilTab);
        labelSzacowanyCzas->setObjectName("label_5");
        labelSzacowanyCzas->setGeometry(QRect(40, 260, 460, 20));
        labelSzacowanyCzas->show();
    }

    if (labelAutoKalorie == nullptr)
    {
        labelAutoKalorie = new QLabel(ui.profilTab);
        labelAutoKalorie->setObjectName("label_6");
        labelAutoKalorie->setGeometry(QRect(40, 285, 460, 20));
        labelAutoKalorie->show();
    }

    if (spinWagaDocelowa != nullptr)
    {
        spinWagaDocelowa->setRange(20.0, 400.0);
        spinWagaDocelowa->setDecimals(0);
        spinWagaDocelowa->setSuffix(" kg");
    }

    if (spinTempoWagi != nullptr)
    {
        spinTempoWagi->setRange(0.1, 2.0);
        spinTempoWagi->setDecimals(1);
        spinTempoWagi->setSingleStep(0.1);
        spinTempoWagi->setSuffix(" kg/tydz");
    }

    if (labelSzacowanyCzas != nullptr)
    {
        labelSzacowanyCzas->setText("Szacowany czas do celu: -");
        labelSzacowanyCzas->raise();
    }

    if (labelAutoKalorie != nullptr)
    {
        labelAutoKalorie->setText("Automatyczny cel kalorii: -");
        labelAutoKalorie->raise();
    }

    if (comboCelTyp != nullptr)
    {
        connect(
            comboCelTyp,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            [this](int)
            {
                zastosujProfilZUiIAutozapis(false);
            }
        );
    }

    if (spinWagaDocelowa != nullptr)
    {
        connect(
            spinWagaDocelowa,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            [this](double)
            {
                zastosujProfilZUiIAutozapis(false);
            }
        );
    }

    if (spinTempoWagi != nullptr)
    {
        connect(
            spinTempoWagi,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            [this](double)
            {
                zastosujProfilZUiIAutozapis(false);
            }
        );
    }

    connect(
        ui.lineEditImie,
        &QLineEdit::editingFinished,
        this,
        [this]()
        {
            zastosujProfilZUiIAutozapis(false);
        }
    );

    connect(
        ui.spinWiek,
        QOverload<int>::of(&QSpinBox::valueChanged),
        this,
        [this](int)
        {
            zastosujProfilZUiIAutozapis(false);
        }
    );

    connect(
        ui.doubleSpinWaga,
        QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        this,
        [this](double)
        {
            zastosujProfilZUiIAutozapis(false);
        }
    );

    connect(
        ui.doubleSpinWzrost,
        QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        this,
        [this](double)
        {
            zastosujProfilZUiIAutozapis(false);
        }
    );
}

// Odświeża informacje o celu wagowym i dzisiejszym bilansie kalorii widoczne w zakładce profilu.
void MainWindow::odswiezSekcjeCelu()
{
    if (labelSzacowanyCzas == nullptr || spinWagaDocelowa == nullptr || spinTempoWagi == nullptr)
    {
        return;
    }

    ProfilUzytkownika tymczasowy = profil;
    tymczasowy.ustawWage(ui.doubleSpinWaga->value());
    tymczasowy.ustawWageDocelowa(spinWagaDocelowa->value());
    tymczasowy.ustawTempoZmianyWagiTygodniowo(spinTempoWagi->value());

    const Makroskladniki suma = dziennik.obliczSume();
    const Makroskladniki limity = dziennik.pobierzLimityDzienne();

    if (!tymczasowy.maPoprawnyCelWagi())
    {
        labelSzacowanyCzas->setText(
            QString("Szacowany czas do celu: - | Dzisiaj: %1 / %2 kcal")
            .arg(suma.kalorie, 0, 'f', 0)
            .arg(limity.kalorie, 0, 'f', 0)
        );
        return;
    }

    const int dni = tymczasowy.obliczSzacowaneDniDoCelu();
    const int tygodnie = dni / 7;
    const int pozostaleDni = dni % 7;

    labelSzacowanyCzas->setText(
        QString("Szacowany czas do celu: ok. %1 tyg. %2 dni | Dzisiaj: %3 / %4 kcal")
        .arg(tygodnie)
        .arg(pozostaleDni)
        .arg(suma.kalorie, 0, 'f', 0)
        .arg(limity.kalorie, 0, 'f', 0)
    );
}

// Wylicza automatyczny limit kcal z danych profilu i kierunku celu; wynik trafia do profilu oraz dziennika.
void MainWindow::przeliczAutomatycznyLimitKalorii()
{
    if (comboCelTyp == nullptr || spinWagaDocelowa == nullptr || spinTempoWagi == nullptr)
    {
        return;
    }

    const double waga = ui.doubleSpinWaga->value();
    const double wzrost = ui.doubleSpinWzrost->value();
    const int wiek = ui.spinWiek->value();
    const double wagaDocelowa = spinWagaDocelowa->value();

    if (waga < 20.0 || waga > 400.0 || wzrost < 100.0 || wzrost > 250.0 || wiek < 8 || wiek > 120)
    {
        return;
    }

    // Uproszczony wzór na BMR; w projekcie wystarcza jako automatyczna propozycja limitu kcal.
    const double bmr = 10.0 * waga + 6.25 * wzrost - 5.0 * static_cast<double>(wiek) + 5.0;
    const double tdee = bmr * 1.4;

    const double tempo = spinTempoWagi->value();
    // Przyjmujemy orientacyjnie 7700 kcal na 1 kg masy i rozbijamy zmianę na dni tygodnia.
    const double kcalNaDzienZmiana = (tempo * 7700.0) / 7.0;

    const double roznicaKg = wagaDocelowa - waga;
    double docelowyLimit = tdee;

    if (roznicaKg < -0.1)
    {
        docelowyLimit = tdee - kcalNaDzienZmiana;
    }
    else if (roznicaKg > 0.1)
    {
        docelowyLimit = tdee + kcalNaDzienZmiana;
    }

    docelowyLimit = std::clamp(docelowyLimit, 800.0, 10000.0);

    limitKaloriiAuto = std::round(docelowyLimit);

    profil.ustawLimitKalorii(limitKaloriiAuto);
    dziennik.ustawLimitKalorii(limitKaloriiAuto);

    if (labelAutoKalorie != nullptr)
    {
        labelAutoKalorie->setText(
            QString("Automatyczny cel kalorii: %1 kcal").arg(limitKaloriiAuto, 0, 'f', 0)
        );
    }
}

// Zbiera dane z formularza profilu, waliduje je, aktualizuje model i od razu zapisuje zmiany do plików.
void MainWindow::zastosujProfilZUiIAutozapis(bool pokazKomunikatyBledu)
{
    // Gdy UI jest właśnie wypełniane z pliku, nie zapisujemy tych samych danych z powrotem.
    if (aktualizujeUi)
    {
        return;
    }

    if (comboCelTyp == nullptr || spinWagaDocelowa == nullptr || spinTempoWagi == nullptr)
    {
        return;
    }

    const QString imie = ui.lineEditImie->text().trimmed();
    const int wiek = ui.spinWiek->value();
    const double waga = ui.doubleSpinWaga->value();
    const double wzrost = ui.doubleSpinWzrost->value();
    const QString cel = comboCelTyp->currentText();
    const double wagaDocelowa = spinWagaDocelowa->value();
    const double tempo = spinTempoWagi->value();

    if (imie.isEmpty())
    {
        if (pokazKomunikatyBledu)
        {
            QMessageBox::warning(this, "Błąd", "Imię nie może być puste.");
        }
        return;
    }

    if (wiek < 8 || wiek > 120)
    {
        if (pokazKomunikatyBledu)
        {
            QMessageBox::warning(this, "Błąd", "Wiek poza zakresem 8-120.");
        }
        return;
    }

    if (waga < 20.0 || waga > 400.0)
    {
        if (pokazKomunikatyBledu)
        {
            QMessageBox::warning(this, "Błąd", "Waga poza zakresem 20-400 kg.");
        }
        return;
    }

    if (wzrost < 100.0 || wzrost > 250.0)
    {
        if (pokazKomunikatyBledu)
        {
            QMessageBox::warning(this, "Błąd", "Wzrost poza zakresem 100-250 cm.");
        }
        return;
    }

    if (wagaDocelowa < 20.0 || wagaDocelowa > 400.0)
    {
        if (pokazKomunikatyBledu)
        {
            QMessageBox::warning(this, "Błąd", "Waga docelowa poza zakresem 20-400 kg.");
        }
        return;
    }

    if (tempo < 0.1 || tempo > 2.0)
    {
        if (pokazKomunikatyBledu)
        {
            QMessageBox::warning(this, "Błąd", "Tempo musi być w zakresie 0.1-2.0 kg/tydzień.");
        }
        return;
    }

    profil.ustawImie(imie.toStdString());
    profil.ustawWiek(wiek);
    profil.ustawWage(waga);
    profil.ustawWzrost(wzrost);
    profil.ustawCel(cel.toStdString());
    profil.ustawWageDocelowa(wagaDocelowa);
    profil.ustawTempoZmianyWagiTygodniowo(tempo);

    przeliczAutomatycznyLimitKalorii();

    profil.ustawLimitKalorii(limitKaloriiAuto);
    dziennik.ustawLimitKalorii(limitKaloriiAuto);

    zapiszDaneDoPlikow();
    odswiezDziennik();
    odswiezSekcjeCelu();
}

// Jednorazowa konfiguracja wyglądu i zachowania tabel dziennika po utworzeniu interfejsu.
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
    ui.framePodsumowanieDolne->setMinimumHeight(118);
    ui.framePodsumowanieDolne->setMaximumHeight(142);

    ui.buttonPoprzedniDzien->setFixedSize(44, 44);
    ui.buttonNastepnyDzien->setFixedSize(44, 44);

    for (QPushButton* przyciskDodawania : {
        ui.buttonDodajSniadanie,
        ui.buttonDodajDrugieSniadanie,
        ui.buttonDodajObiad,
        ui.buttonDodajKolacja,
        ui.buttonDodajPrzekaski
        })
    {
        przyciskDodawania->setFixedSize(42, 42);
        przyciskDodawania->setCursor(Qt::PointingHandCursor);
        przyciskDodawania->setText("+");
    }

    for (QFrame* frame : {
        ui.frameSniadanie,
        ui.frameDrugieSniadanie,
        ui.frameObiad,
        ui.frameKolacja,
        ui.framePrzekaski
        })
    {
        frame->setMinimumHeight(82);
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

// Ustawia wszystkie tabele posiłków według jednego schematu kolumn, selekcji i rozmiarów.
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
                "Węgle",
                "Tłuszcz",
                ""
                });

            QHeaderView* naglowek = tabela->horizontalHeader();
            naglowek->setSectionResizeMode(0, QHeaderView::Stretch);

            for (int kolumna = 1; kolumna <= 6; ++kolumna)
            {
                naglowek->setSectionResizeMode(kolumna, QHeaderView::ResizeToContents);
            }

            naglowek->setSectionResizeMode(7, QHeaderView::Fixed);
            tabela->setColumnWidth(7, 48);
            naglowek->setMinimumSectionSize(58);
            naglowek->setFixedHeight(34);

            tabela->verticalHeader()->setVisible(false);
            tabela->verticalHeader()->setDefaultSectionSize(40);

            tabela->setSelectionBehavior(QAbstractItemView::SelectRows);
            tabela->setSelectionMode(QAbstractItemView::SingleSelection);
            tabela->setEditTriggers(QAbstractItemView::NoEditTriggers);
            tabela->setTextElideMode(Qt::ElideRight);
            tabela->setWordWrap(false);

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

// Podłącza kliknięcie w wiersz tabeli do edycji pozycji z odpowiedniej pory posiłku.
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

// Dynamiczna wysokość sprawia, że puste tabele znikają, a wypełnione nie pokazują wewnętrznego scrolla.
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
    const int wysokosc = wysokoscNaglowka + liczbaWierszy * wysokoscWiersza + 14;

    tabela->setMinimumHeight(wysokosc);
    tabela->setMaximumHeight(wysokosc);
}

// Wypełnia jedną tabelę posiłku: liczy sumę pory, ustawia wiersze produktów i dodaje przyciski usuwania.
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

    // Pusta pora posiłku ma schowaną tabelę, dzięki czemu ekran nie marnuje miejsca.
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
        przyciskUsun->setFixedSize(32, 32);
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

// Główne odświeżenie dziennika po każdej zmianie: tabele, data, kafelki kcal i paski makro.
void MainWindow::odswiezDziennik()
{
    odswiezSekcjeCelu();

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

    // Ujemna wartość oznacza przekroczenie limitu — dlatego UI zmienia tekst i kolor sekcji.
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
            "border: 1px solid #fecaca;"
            "border-radius: 18px;"
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
            "background: #f0fdf4;"
            "border: 1px solid #bbf7d0;"
            "border-radius: 18px;"
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

// Pobiera jednostki produktu z bazy i dokłada jednostkę z edytowanej pozycji, żeby nie zgubić starych danych.
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

// Wspólny dialog wyboru ilości i jednostki, używany zarówno przy dodawaniu, jak i edycji pozycji posiłku.
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
    dialog.setObjectName("dialogFit");
    dialog.setWindowTitle(tytulOkna);
    dialog.setModal(true);
    dialog.setStyleSheet(styleSheet());
    dialog.setMinimumWidth(440);
    dialog.setMinimumHeight(330);

    auto* layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(24, 22, 24, 20);
    layout->setSpacing(14);

    auto* etykietaProdukt = new QLabel(QString::fromStdString(nazwaProduktu), &dialog);
    etykietaProdukt->setObjectName("dialogTitle");
    etykietaProdukt->setWordWrap(true);
    layout->addWidget(etykietaProdukt);

    auto* etykietaOpis = new QLabel("Wybierz ilość i jednostkę. Podgląd makro przelicza się automatycznie.", &dialog);
    etykietaOpis->setObjectName("dialogSubtitle");
    etykietaOpis->setWordWrap(true);
    layout->addWidget(etykietaOpis);

    auto* spinIlosc = new QDoubleSpinBox(&dialog);
    spinIlosc->setRange(0.01, 10000.0);
    spinIlosc->setDecimals(2);
    spinIlosc->setSingleStep(0.5);
    spinIlosc->setValue(domyslnaIlosc);
    spinIlosc->setMinimumHeight(46);

    auto* comboJednostka = new QComboBox(&dialog);
    comboJednostka->setMinimumHeight(46);

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

    auto* formularz = new QFormLayout();
    formularz->setContentsMargins(0, 4, 0, 0);
    formularz->setSpacing(12);
    formularz->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    formularz->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    formularz->addRow("Ilość", spinIlosc);
    formularz->addRow("Jednostka", comboJednostka);
    layout->addLayout(formularz);

    auto* etykietaPodgladu = new QLabel(&dialog);
    etykietaPodgladu->setObjectName("dialogPreview");
    etykietaPodgladu->setWordWrap(true);
    layout->addWidget(etykietaPodgladu);

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
                QString("Razem: <b>%1 g</b>  |  <b>%2 kcal</b><br/>B: %3 g  |  W: %4 g  |  T: %5 g")
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

    auto* przyciski = new QDialogButtonBox(
        QDialogButtonBox::Cancel | QDialogButtonBox::Ok,
        &dialog
    );

    przyciski->button(QDialogButtonBox::Ok)->setText("OK");
    przyciski->button(QDialogButtonBox::Cancel)->setText("Anuluj");
    przyciski->button(QDialogButtonBox::Cancel)->setObjectName("buttonSecondary");
    layout->addWidget(przyciski);

    connect(przyciski, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    connect(przyciski, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);

    odswiezPodglad();

    // Jeśli użytkownik anuluje dialog, nie zmieniamy modelu danych.
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

// Obsługuje pełny proces dodania produktu do posiłku: wybór produktu, ilość, walidację, zapis i odświeżenie.
void MainWindow::dodajProduktDoPory(PoraPosilku pora)
{
    const std::vector<Produkt> produkty = bazaProduktow.pobierzWszystkie();

    if (produkty.empty())
    {
        QMessageBox::warning(this, "Brak produktów", "Baza produktów jest pusta.");
        return;
    }

    QDialog dialogListy(this);
    dialogListy.setObjectName("dialogFit");
    dialogListy.setWindowTitle("Wybierz produkt");
    dialogListy.setStyleSheet(styleSheet());
    dialogListy.setMinimumSize(520, 560);
    dialogListy.setModal(true);

    auto* layoutListy = new QVBoxLayout(&dialogListy);
    layoutListy->setContentsMargins(24, 22, 24, 20);
    layoutListy->setSpacing(14);

    auto* tytul = new QLabel("Wybierz produkt", &dialogListy);
    tytul->setObjectName("dialogTitle");
    layoutListy->addWidget(tytul);

    auto* opis = new QLabel("Wyszukaj produkt i zatwierdź wybór. Dwuklik też dodaje produkt do posiłku.", &dialogListy);
    opis->setObjectName("dialogSubtitle");
    opis->setWordWrap(true);
    layoutListy->addWidget(opis);

    auto* poleWyszukiwania = new QLineEdit(&dialogListy);
    poleWyszukiwania->setPlaceholderText("Szukaj produktu...");
    poleWyszukiwania->setMinimumHeight(46);
    layoutListy->addWidget(poleWyszukiwania);

    auto* listaProduktow = new QListWidget(&dialogListy);
    listaProduktow->setMinimumHeight(330);
    listaProduktow->setSpacing(2);

    for (const Produkt& produkt : produkty)
    {
        listaProduktow->addItem(QString::fromStdString(produkt.pobierzNazwe()));
    }

    listaProduktow->setCurrentRow(0);
    layoutListy->addWidget(listaProduktow, 1);

    auto* przyciskiListy = new QDialogButtonBox(
        QDialogButtonBox::Cancel | QDialogButtonBox::Ok,
        &dialogListy
    );

    przyciskiListy->button(QDialogButtonBox::Ok)->setText("Wybierz");
    przyciskiListy->button(QDialogButtonBox::Cancel)->setText("Anuluj");
    przyciskiListy->button(QDialogButtonBox::Cancel)->setObjectName("buttonSecondary");
    layoutListy->addWidget(przyciskiListy);

    connect(
        poleWyszukiwania,
        &QLineEdit::textChanged,
        &dialogListy,
        [listaProduktow](const QString& tekst)
        {
            int pierwszyWidoczny = -1;

            for (int i = 0; i < listaProduktow->count(); ++i)
            {
                QListWidgetItem* element = listaProduktow->item(i);

                if (element == nullptr)
                {
                    continue;
                }

                const bool ukryty = !element->text().contains(tekst, Qt::CaseInsensitive);
                element->setHidden(ukryty);

                if (!ukryty && pierwszyWidoczny < 0)
                {
                    pierwszyWidoczny = i;
                }
            }

            if (pierwszyWidoczny >= 0)
            {
                listaProduktow->setCurrentRow(pierwszyWidoczny);
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

    if (listaProduktow->currentItem()->isHidden())
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

// Edycja pozycji korzysta z indeksu w konkretnej tabeli posiłku, a dziennik mapuje go później na główny wektor.
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

// Usuwa wpis z wybranej pory posiłku i od razu zapisuje nowy stan dnia.
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

// Startowe ładowanie danych: profil, produkty i dziennik bieżącego dnia; przy braku produktów tworzy przykładową bazę.
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

    // Brak pliku dla daty oznacza nowy pusty dzień z limitem pobranym z profilu.
    if (!PlikManager::wczytajDziennik(
        sciezkaDziennikaDlaDaty(aktualnaData),
        dziennik))
    {
        dziennik = DziennikZywieniowy();
        dziennik.ustawLimitKalorii(profil.pobierzLimitKalorii());
    }
}

// Jeden punkt zapisu całego stanu aplikacji, używany po zmianach i przy przełączaniu dni.
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

// Przełączenie dnia zapisuje obecny dziennik, tworzy pusty model dla nowej daty i próbuje wczytać jej plik.
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

// Analogiczna obsługa przejścia do kolejnego dnia w dzienniku.
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

// Sloty przycisków dodawania przekazują tylko właściwą porę posiłku do wspólnej metody.
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

// Wczytuje listę produktów do tabeli, tworząc też przyciski ulubionych, usuwania i obsługę edycji.
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
        "Węgle",
        "Tłuszcz",
        "★",
        "×"
        });

    tabela->setSelectionBehavior(QAbstractItemView::SelectRows);
    tabela->setSelectionMode(QAbstractItemView::SingleSelection);
    tabela->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabela->setTextElideMode(Qt::ElideRight);
    tabela->setWordWrap(false);
    tabela->setAlternatingRowColors(true);
    tabela->setShowGrid(false);

    tabela->verticalHeader()->setVisible(false);
    tabela->verticalHeader()->setDefaultSectionSize(44);

    QHeaderView* naglowek = tabela->horizontalHeader();
    naglowek->setFixedHeight(36);
    naglowek->setMinimumSectionSize(54);
    naglowek->setSectionResizeMode(0, QHeaderView::Stretch);

    for (int kolumna = 1; kolumna <= 4; ++kolumna)
    {
        naglowek->setSectionResizeMode(kolumna, QHeaderView::ResizeToContents);
    }

    naglowek->setSectionResizeMode(5, QHeaderView::Fixed);
    naglowek->setSectionResizeMode(6, QHeaderView::Fixed);
    tabela->setColumnWidth(5, 56);
    tabela->setColumnWidth(6, 56);

    tabela->setRowCount(0);

    // Przed ponownym podłączeniem sygnału odpinamy stare połączenia, żeby dwuklik nie uruchamiał edycji kilka razy.
    disconnect(tabela, &QTableWidget::cellDoubleClicked, this, nullptr);

    // Dwuklik w nazwę produktu otwiera formularz edycji konkretnego rekordu.
    connect(tabela, &QTableWidget::cellDoubleClicked, this, [this, tabela](int row, int column) {
        if (column == 0) {
            std::string nazwaDoEdycji = tabela->item(row, 0)->text().toStdString();
            for (const auto& p : bazaProduktow.pobierzWszystkie()) {
                if (p.pobierzNazwe() == nazwaDoEdycji) {
                    otworzOknoEdycji(p);
                    break;
                }
            }
        }
        });


    for (int i = 0; i < static_cast<int>(produkty.size()); ++i)
    {
        tabela->insertRow(i);
        tabela->setRowHeight(i, 44);

        const Produkt& produkt = produkty[static_cast<std::size_t>(i)];
        const QString nazwa = QString::fromStdString(produkt.pobierzNazwe());
        const Makroskladniki makro = produkt.pobierzMakroNa100g();

        tabela->setItem(i, 0, new QTableWidgetItem(nazwa));
        tabela->setItem(i, 1, new QTableWidgetItem(QString::number(makro.kalorie, 'f', 0)));
        tabela->setItem(i, 2, new QTableWidgetItem(QString::number(makro.bialko, 'f', 1)));
        tabela->setItem(i, 3, new QTableWidgetItem(QString::number(makro.weglowodany, 'f', 1)));
        tabela->setItem(i, 4, new QTableWidgetItem(QString::number(makro.tluszcz, 'f', 1)));

        for (int kolumna = 1; kolumna <= 4; ++kolumna)
        {
            if (QTableWidgetItem* element = tabela->item(i, kolumna))
            {
                element->setTextAlignment(Qt::AlignCenter);
            }
        }

        auto* przyciskUlubione = new QPushButton(produkt.czyUlubiony() ? "★" : "☆", tabela);
        przyciskUlubione->setFixedSize(34, 34);
        przyciskUlubione->setCursor(Qt::PointingHandCursor);
        przyciskUlubione->setToolTip(produkt.czyUlubiony() ? "Usuń z ulubionych" : "Dodaj do ulubionych");
        przyciskUlubione->setStyleSheet(
            produkt.czyUlubiony()
            ? "QPushButton { color: #f59e0b; font-size: 22px; border: none; background: transparent; padding: 0px; } QPushButton:hover { background: #fef3c7; border-radius: 10px; }"
            : "QPushButton { color: #94a3b8; font-size: 22px; border: none; background: transparent; padding: 0px; } QPushButton:hover { background: #f1f5f9; border-radius: 10px; }"
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
        przyciskUsun->setFixedSize(34, 34);
        przyciskUsun->setCursor(Qt::PointingHandCursor);
        przyciskUsun->setToolTip("Usuń produkt");
        przyciskUsun->setStyleSheet(
            "QPushButton {"
            "color: #dc2626;"
            "font-size: 22px;"
            "font-weight: 900;"
            "border: none;"
            "background: transparent;"
            "padding: 0px;"
            "}"
            "QPushButton:hover {"
            "background: #fee2e2;"
            "border-radius: 10px;"
            "}"
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

    tabela->resizeRowsToContents();
}

// Odświeża zakładkę produktów: główna tabela respektuje wyszukiwarkę, a ulubione zawsze pokazują same gwiazdki.
void MainWindow::odswiezTabeleProduktow()
{
    const std::string fraza = ui.lineEditSzukajProduktu->text().trimmed().toStdString();

    // Pusta wyszukiwarka pokazuje wszystkie produkty, wpisany tekst uruchamia filtrowanie po nazwie.
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

// Każda zmiana tekstu w wyszukiwarce natychmiast przebudowuje listę produktów.
void MainWindow::on_lineEditSzukajProduktu_textChanged(const QString&)
{
    odswiezTabeleProduktow();
}

// Formularz dodawania produktu buduje Produkt z danych użytkownika, waliduje go i dopiero wtedy dopisuje do bazy.
void MainWindow::on_buttonDodajProdukt_clicked()
{
    QDialog dialog(this);
    dialog.setObjectName("dialogFit");
    dialog.setWindowTitle("Dodaj nowy produkt");
    dialog.setStyleSheet(styleSheet());
    dialog.setMinimumWidth(560);
    dialog.setModal(true);

    auto* layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(24, 22, 24, 20);
    layout->setSpacing(14);

    auto* tytul = new QLabel("Dodaj produkt", &dialog);
    tytul->setObjectName("dialogTitle");
    layout->addWidget(tytul);

    auto* opis = new QLabel("Wpisz wartości odżywcze dla 100 g produktu. Poniżej możesz opcjonalnie zdefiniować własne miary (np. plaster, opakowanie).", &dialog);
    opis->setObjectName("dialogSubtitle");
    opis->setWordWrap(true);
    layout->addWidget(opis);

    auto* formularz = new QFormLayout();
    formularz->setContentsMargins(0, 4, 0, 0);
    formularz->setSpacing(12);
    formularz->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    formularz->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    QLineEdit editNazwa(&dialog);
    editNazwa.setPlaceholderText("np. Jajko, Ryż biały, Banan");
    editNazwa.setMinimumHeight(46);

    QDoubleSpinBox spinKcal(&dialog), spinBialko(&dialog), spinWegle(&dialog), spinTluszcz(&dialog);

    for (QDoubleSpinBox* spin : { &spinKcal, &spinBialko, &spinWegle, &spinTluszcz })
    {
        spin->setMinimumHeight(46);
        spin->setRange(0.0, 10000.0);
        spin->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    spinKcal.setDecimals(0);
    spinBialko.setDecimals(1);
    spinWegle.setDecimals(1);
    spinTluszcz.setDecimals(1);

    auto dodajWiersz = [&](const QString& tekst, QWidget* pole)
        {
            auto* label = new QLabel(tekst, &dialog);
            label->setMinimumWidth(170);
            label->setStyleSheet("font-weight: 800; color: #475569;");
            formularz->addRow(label, pole);
        };

    dodajWiersz("Nazwa produktu", &editNazwa);
    dodajWiersz("Kalorie / 100 g", &spinKcal);
    dodajWiersz("Białko / 100 g", &spinBialko);
    dodajWiersz("Węglowodany / 100 g", &spinWegle);
    dodajWiersz("Tłuszcz / 100 g", &spinTluszcz);

    layout->addLayout(formularz);

    QFrame* linia = new QFrame(&dialog);
    linia->setFrameShape(QFrame::HLine);
    linia->setStyleSheet("color: #e2e8f0; margin-top: 4px; margin-bottom: 4px;");
    layout->addWidget(linia);

    auto* opisMiar = new QLabel("Dodatkowe jednostki (opcjonalnie):", &dialog);
    opisMiar->setStyleSheet("font-weight: 800; color: #475569; font-size: 14px;");
    layout->addWidget(opisMiar);

    QVBoxLayout* jednostkiLayout = new QVBoxLayout();
    jednostkiLayout->setSpacing(10);
    layout->addLayout(jednostkiLayout);

    struct MiaraRow { QComboBox* combo; QDoubleSpinBox* spin; };
    std::vector<MiaraRow> wierszeMiar;

    auto dodajWierszMiary = [&](const std::string& domyslnaNazwa = "sztuka", double domyslnaWaga = 0.0) {
        QHBoxLayout* row = new QHBoxLayout();

        QComboBox* combo = new QComboBox(&dialog);
        combo->addItems({ "sztuka", "opakowanie", "porcja", "plaster", "ml", "łyżka", "szklanka" });
        combo->setMinimumHeight(46);
        combo->setCurrentText(QString::fromStdString(domyslnaNazwa));

        QDoubleSpinBox* spin = new QDoubleSpinBox(&dialog);
        spin->setRange(0.0, 5000.0);
        spin->setDecimals(0);
        spin->setMinimumHeight(46);
        spin->setSuffix(" g");
        spin->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        spin->setValue(domyslnaWaga);

        row->addWidget(combo, 1);
        row->addWidget(spin, 1);

        jednostkiLayout->addLayout(row);
        wierszeMiar.push_back({ combo, spin });
        dialog.adjustSize();
        };

    dodajWierszMiary();

    QPushButton* btnDodajMiare = new QPushButton("+ Dodaj kolejną miarę", &dialog);
    btnDodajMiare->setStyleSheet(
        "QPushButton { background: #f8fafc; color: #15803d; border: 1px dashed #bbf7d0; border-radius: 14px; padding: 8px; font-weight: 800; }"
        "QPushButton:hover { background: #f0fdf4; }"
    );
    layout->addWidget(btnDodajMiare);

    connect(btnDodajMiare, &QPushButton::clicked, [&]() {
        dodajWierszMiary();
        });

    QDialogButtonBox przyciski(
        QDialogButtonBox::Cancel | QDialogButtonBox::Ok,
        &dialog
    );

    przyciski.button(QDialogButtonBox::Ok)->setText("Zapisz produkt");
    przyciski.button(QDialogButtonBox::Cancel)->setText("Anuluj");
    przyciski.button(QDialogButtonBox::Cancel)->setObjectName("buttonSecondary");

    layout->addWidget(&przyciski);

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

    Produkt nowyProdukt(nazwa, { spinKcal.value(), spinBialko.value(), spinWegle.value(), spinTluszcz.value() });

    for (const auto& wiersz : wierszeMiar) {
        if (wiersz.spin->value() > 0.0) {
            nowyProdukt.dodajJednostke(wiersz.combo->currentText().toStdString(), wiersz.spin->value());
        }
    }

    if (!nowyProdukt.czyPoprawny())
    {
        QMessageBox::warning(this, "Błąd", "Wprowadzone wartości produktu są niepoprawne.");
        return;
    }

    // Baza nie zwraca osobnego kodu błędu przy duplikacie, więc porównujemy rozmiar przed i po dodaniu.
    const std::size_t liczbaPrzed = bazaProduktow.pobierzWszystkie().size();
    bazaProduktow.dodajProdukt(nowyProdukt);
    const std::size_t liczbaPo = bazaProduktow.pobierzWszystkie().size();

    if (liczbaPo == liczbaPrzed)
    {
        QMessageBox::warning(this, "Produkt", "Nie dodano produktu. Taki produkt może już istnieć albo dane są niepoprawne.");
        return;
    }

    zapiszDaneDoPlikow();
    odswiezTabeleProduktow();

    QMessageBox::information(this, "Produkt", "Produkt został pomyślnie dodany.");
}

// Formularz edycji pracuje na kopii produktu; stary rekord jest przywracany, jeśli nowe dane okażą się błędne.
void MainWindow::otworzOknoEdycji(Produkt p)
{
    QDialog dialog(this);
    dialog.setObjectName("dialogFit");
    dialog.setWindowTitle("Edytuj produkt");
    dialog.setStyleSheet(styleSheet());
    dialog.setMinimumWidth(560);
    dialog.setModal(true);

    auto* layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(24, 22, 24, 20);
    layout->setSpacing(14);

    auto* tytul = new QLabel("Edytuj produkt", &dialog);
    tytul->setObjectName("dialogTitle");
    layout->addWidget(tytul);

    auto* opis = new QLabel("Zaktualizuj wartości odżywcze dla 100 g produktu. Możesz też edytować lub dopisać nowe własne miary (np. plaster, opakowanie).", &dialog);
    opis->setObjectName("dialogSubtitle");
    opis->setWordWrap(true);
    layout->addWidget(opis);

    auto* formularz = new QFormLayout();
    formularz->setContentsMargins(0, 4, 0, 0);
    formularz->setSpacing(12);
    formularz->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    formularz->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    QLineEdit editNazwa(&dialog);
    editNazwa.setText(QString::fromStdString(p.pobierzNazwe()));
    editNazwa.setMinimumHeight(46);

    QDoubleSpinBox spinKcal(&dialog), spinBialko(&dialog), spinWegle(&dialog), spinTluszcz(&dialog);

    for (QDoubleSpinBox* spin : { &spinKcal, &spinBialko, &spinWegle, &spinTluszcz })
    {
        spin->setMinimumHeight(46);
        spin->setRange(0.0, 10000.0);
        spin->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    spinKcal.setDecimals(0);
    spinBialko.setDecimals(1);
    spinWegle.setDecimals(1);
    spinTluszcz.setDecimals(1);

    // Formularz edycji startuje od dotychczasowych wartości produktu.
    spinKcal.setValue(p.pobierzMakroNa100g().kalorie);
    spinBialko.setValue(p.pobierzMakroNa100g().bialko);
    spinWegle.setValue(p.pobierzMakroNa100g().weglowodany);
    spinTluszcz.setValue(p.pobierzMakroNa100g().tluszcz);

    auto dodajWiersz = [&](const QString& tekst, QWidget* pole)
        {
            auto* label = new QLabel(tekst, &dialog);
            label->setMinimumWidth(170);
            label->setStyleSheet("font-weight: 800; color: #475569;");
            formularz->addRow(label, pole);
        };

    dodajWiersz("Nazwa produktu", &editNazwa);

    auto* labelMakroTitle = new QLabel("Wartości dla 100g:", &dialog);
    labelMakroTitle->setStyleSheet("font-size: 14px; font-weight: 900; color: #15803d; margin-top: 10px; padding-bottom: 5px; border-bottom: 2px solid #bbf7d0;");
    labelMakroTitle->setAlignment(Qt::AlignCenter);
    formularz->addRow(labelMakroTitle);

    dodajWiersz("Kalorie (kcal)", &spinKcal);
    dodajWiersz("Białko (g)", &spinBialko);
    dodajWiersz("Węglowodany (g)", &spinWegle);
    dodajWiersz("Tłuszcz (g)", &spinTluszcz);

    layout->addLayout(formularz);

    // Sekcja dynamicznych miar pozwala dopisywać jednostki bez stałej liczby pól w UI.
    QFrame* linia = new QFrame(&dialog);
    linia->setFrameShape(QFrame::HLine);
    linia->setStyleSheet("color: #e5e7eb; margin-top: 5px; margin-bottom: 5px;");
    layout->addWidget(linia);

    auto* opisMiar = new QLabel("Dodatkowe jednostki (opcjonalnie):", &dialog);
    opisMiar->setStyleSheet("font-weight: 900; color: #0f172a; font-size: 14px;");
    layout->addWidget(opisMiar);

    QVBoxLayout* jednostkiLayout = new QVBoxLayout();
    jednostkiLayout->setSpacing(10);
    layout->addLayout(jednostkiLayout);

    struct MiaraRow { QComboBox* combo; QDoubleSpinBox* spin; };
    std::vector<MiaraRow> wierszeMiar;

    // Lokalna lambda dodaje jeden wiersz miary i zapamiętuje wskaźniki do późniejszego odczytu.
    auto dodajWierszMiary = [&](const std::string& domyslnaNazwa = "sztuka", double domyslnaWaga = 0.0) {
        QHBoxLayout* row = new QHBoxLayout();

        QComboBox* combo = new QComboBox(&dialog);
        combo->addItems({ "sztuka", "opakowanie", "porcja", "plaster", "ml", "łyżka", "szklanka" });
        combo->setMinimumHeight(46);
        combo->setCurrentText(QString::fromStdString(domyslnaNazwa));

        QDoubleSpinBox* spin = new QDoubleSpinBox(&dialog);
        spin->setRange(0.0, 5000.0);
        spin->setDecimals(0);
        spin->setMinimumHeight(46);
        spin->setSuffix(" g");
        spin->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        spin->setValue(domyslnaWaga);

        row->addWidget(combo, 1);
        row->addWidget(spin, 1);

        jednostkiLayout->addLayout(row);
        wierszeMiar.push_back({ combo, spin });
        };

    // Przy edycji odtwarzamy wcześniejsze miary, ale pomijamy bazowe gramy.
    bool mialInne = false;
    for (const auto& jedn : p.pobierzJednostki()) {
        // Bazowe gramy są dodawane automatycznie przez Produkt, więc nie pokazujemy ich jako dodatkowej miary.
        if (jedn.nazwa != "g" && jedn.nazwa != "gram") {
            dodajWierszMiary(jedn.nazwa, jedn.gramyNaJednostke);
            mialInne = true;
        }
    }

    // Jeśli produkt nie miał dodatkowych miar, zostawiamy jeden pusty wiersz dla wygody użytkownika.
    if (!mialInne) {
        dodajWierszMiary();
    }

    QPushButton* btnDodajMiare = new QPushButton("+ Dodaj kolejną miarę", &dialog);
    btnDodajMiare->setStyleSheet(
        "QPushButton { background: #f8fafc; color: #15803d; border: 1px dashed #bbf7d0; border-radius: 14px; padding: 10px; font-weight: 800; }"
        "QPushButton:hover { background: #f0fdf4; }"
    );
    layout->addWidget(btnDodajMiare);

    connect(btnDodajMiare, &QPushButton::clicked, [&]() {
        dodajWierszMiary();
        dialog.adjustSize();
        });

    QDialogButtonBox przyciski(
        QDialogButtonBox::Cancel | QDialogButtonBox::Ok,
        &dialog
    );

    przyciski.button(QDialogButtonBox::Ok)->setText("Zapisz zmiany");
    przyciski.button(QDialogButtonBox::Cancel)->setText("Anuluj");
    przyciski.button(QDialogButtonBox::Cancel)->setObjectName("buttonSecondary");

    layout->addWidget(&przyciski);

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

    // Edycję realizujemy jako podmianę: usuwamy starą wersję i po walidacji dodajemy nową.
    bazaProduktow.usunProdukt(p.pobierzNazwe());

    Produkt nowyProdukt(nazwa, { spinKcal.value(), spinBialko.value(), spinWegle.value(), spinTluszcz.value() });

    // Gwiazdka ulubionych nie powinna zniknąć tylko dlatego, że użytkownik edytował makro.
    nowyProdukt.ustawUlubiony(p.czyUlubiony());

    for (const auto& wiersz : wierszeMiar) {
        if (wiersz.spin->value() > 0.0) {
            nowyProdukt.dodajJednostke(wiersz.combo->currentText().toStdString(), wiersz.spin->value());
        }
    }

    // Jeśli nowe dane są błędne, przywracamy starą wersję, żeby nie utracić produktu.
    if (!nowyProdukt.czyPoprawny())
    {
        bazaProduktow.dodajProdukt(p);
        QMessageBox::warning(this, "Błąd", "Wprowadzone wartości produktu są niepoprawne.");
        return;
    }

    // Po pozytywnej walidacji nowy obiekt zastępuje poprzedni w bazie.
    bazaProduktow.dodajProdukt(nowyProdukt);

    zapiszDaneDoPlikow();
    odswiezTabeleProduktow();

    QMessageBox::information(this, "Produkt", "Produkt został pomyślnie zaktualizowany.");
}

// Tłumaczy kody błędów z logiki dziennika na komunikaty zrozumiałe dla użytkownika.
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