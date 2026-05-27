#pragma once

#include <vector>

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"
#include "DziennikZywieniowy.h"
#include "Produkt.h"
#include "PlikManager.h"
#include "ProfilUzytkownika.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_buttonDodajDoDziennika_clicked();
    void on_buttonUsunZDziennika_clicked();
    void on_buttonWyczyscDziennik_clicked();
    void on_comboProduktDziennik_currentIndexChanged(int index);
    void on_buttonZapiszProfil_clicked();
    void on_buttonDodajProdukt_clicked();

private:
    Ui::MainWindowClass ui;

    DziennikZywieniowy dziennik;
    std::vector<Produkt> produkty;
    ProfilUzytkownika profil;

    // Flaga zabezpieczajaca przed re-entry UI (zapobiega nieskonczonym petlom przy ustawianiu wartosci)
    bool aktualizujeUi = false;

    void ustawDziennikGui();
    void dodajProduktyTestoweDoCombo();
    void odswiezDziennik();

    void wczytajDaneZPlikow();
    void zapiszDaneDoPlikow();

    QString komunikatBledu(DziennikZywieniowy::WynikOperacji wynik) const;
};

