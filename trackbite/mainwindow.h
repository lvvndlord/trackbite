#pragma once

#include <vector>

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"
#include "DziennikZywieniowy.h"
#include "Produkt.h"

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

private:
    Ui::MainWindowClass ui;

    DziennikZywieniowy dziennik;
    std::vector<Produkt> produkty;

    void ustawDziennikGui();
    void dodajProduktyTestoweDoCombo();
    void odswiezDziennik();

    QString komunikatBledu(DziennikZywieniowy::WynikOperacji wynik) const;
};

