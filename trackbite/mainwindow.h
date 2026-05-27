#pragma once

#include <QtWidgets/QMainWindow>
#include <QDate>
#include <vector>

#include "ui_mainwindow.h"
#include "DziennikZywieniowy.h"
#include "Produkt.h"
#include "PlikManager.h"
#include "ProfilUzytkownika.h"

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

private:
    Ui::MainWindowClass ui;

    DziennikZywieniowy dziennik;
    std::vector<Produkt> produkty;
    ProfilUzytkownika profil;
    QDate aktualnaData;

    void ustawDziennikGui();
    void odswiezDziennik();

    bool aktualizujeUi = false;

    void ustawTabelePosilkow();
    void wypelnijTabeleDlaPory(
        PoraPosilku pora,
        QTableWidget* tabela,
        QLabel* labelKcal
    );

    void dodajProduktTestowyDoPory(PoraPosilku pora);
    void dopasujWysokoscTabeli(QTableWidget* tabela);

    void wczytajDaneZPlikow();
    void zapiszDaneDoPlikow();

    QString komunikatBledu(DziennikZywieniowy::WynikOperacji wynik) const;
};