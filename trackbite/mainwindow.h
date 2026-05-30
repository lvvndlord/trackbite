#pragma once

#include <QtWidgets/QMainWindow>
#include <QDate>

#include "ui_mainwindow.h"
#include "DziennikZywieniowy.h"
#include "Produkt.h"
#include "BazaProduktow.h"
#include "PlikManager.h"
#include "ProfilUzytkownika.h"

#include <string>
#include <vector>

class QTableWidget;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void on_buttonPoprzedniDzien_clicked();
    void on_buttonNastepnyDzien_clicked();

    void on_buttonDodajSniadanie_clicked();
    void on_buttonDodajDrugieSniadanie_clicked();
    void on_buttonDodajObiad_clicked();
    void on_buttonDodajKolacja_clicked();
    void on_buttonDodajPrzekaski_clicked();

    void on_buttonZapiszProfil_clicked();

    void on_buttonDodajProdukt_clicked();
    void on_lineEditSzukajProduktu_textChanged(const QString& tekst);

private:
    Ui::MainWindowClass ui;

    DziennikZywieniowy dziennik;
    BazaProduktow bazaProduktow;
    ProfilUzytkownika profil;
    QDate aktualnaData;

    bool aktualizujeUi = false;

    void ustawDziennikGui();
    void ustawTabelePosilkow();
    void odswiezDziennik();

    void wypelnijTabeleDlaPory(
        PoraPosilku pora,
        QTableWidget* tabela,
        QLabel* labelKcal
    );

    void dopasujWysokoscTabeli(QTableWidget* tabela);

    void dodajProduktDoPory(PoraPosilku pora);
    void usunPozycjeWPorze(PoraPosilku pora, int indeksWiersza);
    void edytujPozycjeWPorze(PoraPosilku pora, int indeksWiersza);
    void podlaczEdycjePozycji(QTableWidget* tabela, PoraPosilku pora);

    void wczytajDaneZPlikow();
    void zapiszDaneDoPlikow();

    void odswiezTabeleProduktow();
    void zaladujWektorDoTabeli(
        QTableWidget* tabela,
        const std::vector<Produkt>& produkty
    );

    std::vector<JednostkaProduktu> pobierzJednostkiDlaNazwy(
        const std::string& nazwaProduktu,
        const JednostkaProduktu* domyslnaJednostka = nullptr
    ) const;

    bool pokazDialogIlosci(
        const QString& tytulOkna,
        const std::string& nazwaProduktu,
        const Makroskladniki& makroNa100g,
        const std::vector<JednostkaProduktu>& dostepneJednostki,
        double domyslnaIlosc,
        const JednostkaProduktu& domyslnaJednostka,
        double& wybranaIlosc,
        JednostkaProduktu& wybranaJednostka
    );

    QString komunikatBledu(DziennikZywieniowy::WynikOperacji wynik) const;
};