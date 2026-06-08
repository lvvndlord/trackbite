#pragma once

#include <QtWidgets/QMainWindow>
#include <QDate>
#include <QComboBox>
#include <QDoubleSpinBox>

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
class QDoubleSpinBox;

// Glowne okno aplikacji Fit Plan.
// Laczy warstwe GUI z logika dziennika, baza produktow, profilem uzytkownika i zapisem do plikow.
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Inicjalizuje interfejs, style, dane z plikow oraz pierwsze odswiezenie tabel.
    MainWindow(QWidget* parent = nullptr);

    // Przy zamykaniu okna zapisuje aktualny stan aplikacji do plikow.
    ~MainWindow();

private slots:
    // Sloty nawigacji po dniach dziennika.
    void on_buttonPoprzedniDzien_clicked();
    void on_buttonNastepnyDzien_clicked();

    // Sloty przyciskow dodawania produktu do konkretnej pory posilku.
    void on_buttonDodajSniadanie_clicked();
    void on_buttonDodajDrugieSniadanie_clicked();
    void on_buttonDodajObiad_clicked();
    void on_buttonDodajKolacja_clicked();
    void on_buttonDodajPrzekaski_clicked();

    // Slot dodawania nowego produktu do bazy.
    void on_buttonDodajProdukt_clicked();

    // Slot wyszukiwarki produktow. Odswieza tabele po kazdej zmianie tekstu.
    void on_lineEditSzukajProduktu_textChanged(const QString& tekst);

private:
    // Obiekt wygenerowany z pliku .ui. Zawiera wskazniki do kontrolek z Qt Designera.
    Ui::MainWindowClass ui;

    // Glowny model danych dzisiejszego/wybranego dnia.
    DziennikZywieniowy dziennik;

    // Baza produktow dostepnych do dodawania w dzienniku.
    BazaProduktow bazaProduktow;

    // Dane profilu i celu uzytkownika.
    ProfilUzytkownika profil;

    // Data aktualnie pokazywanego dziennika. Od niej zalezy nazwa pliku dziennika.
    QDate aktualnaData;

    // Flaga blokujaca reakcje na sygnaly w trakcie programowego wypelniania UI.
    // Bez tego latwo przypadkowo zapisac dane podczas inicjalizacji kontrolek.
    bool aktualizujeUi = false;

    // Kontrolki sekcji celu profilu wyszukiwane dynamicznie z UI.
    QComboBox* comboCelTyp = nullptr;
    QDoubleSpinBox* spinWagaDocelowa = nullptr;
    QDoubleSpinBox* spinTempoWagi = nullptr;
    QLabel* labelSzacowanyCzas = nullptr;
    QLabel* labelAutoKalorie = nullptr;

    // Automatycznie wyliczony limit kalorii, synchronizowany z profilem i dziennikiem.
    double limitKaloriiAuto = 2000.0;

    // Ustawia wyglad i podstawowe zachowanie elementow dziennika.
    void ustawDziennikGui();

    // Konfiguruje tabele posilkow: kolumny, naglowki, zaznaczanie i klikniecia.
    void ustawTabelePosilkow();

    // Odswieza wszystkie widoki dziennika, podsumowania, daty i paski postepu.
    void odswiezDziennik();

    // Odswieza opis celu wagowego i dzisiejszy status kalorii w profilu.
    void odswiezSekcjeCelu();

    // Podpina lub odnajduje kontrolki celu profilu i ustawia ich zakresy.
    void utworzSekcjeCeluProfilu();

    // Pobiera dane z UI, waliduje je, zapisuje do profilu i od razu aktualizuje pliki.
    void zastosujProfilZUiIAutozapis(bool pokazKomunikatyBledu);

    // Liczy limit kalorii na podstawie wagi, wzrostu, wieku, celu i tempa zmiany wagi.
    void przeliczAutomatycznyLimitKalorii();

    // Wypelnia jedna tabele posilku pozycjami z konkretnej pory i aktualizuje jej podsumowanie kcal.
    void wypelnijTabeleDlaPory(
        PoraPosilku pora,
        QTableWidget* tabela,
        QLabel* labelKcal
    );

    // Dopasowuje wysokosc tabeli do liczby wierszy, zeby sekcje posilkow nie mialy pustej przestrzeni.
    void dopasujWysokoscTabeli(QTableWidget* tabela);

    // Otwiera wybor produktu i dodaje go do wskazanej pory posilku.
    void dodajProduktDoPory(PoraPosilku pora);

    // Usuwa pozycje z konkretnej pory posilku na podstawie indeksu wiersza w tabeli.
    void usunPozycjeWPorze(PoraPosilku pora, int indeksWiersza);

    // Otwiera dialog edycji ilosci i jednostki dla wybranej pozycji posilku.
    void edytujPozycjeWPorze(PoraPosilku pora, int indeksWiersza);

    // Podpina klikniecie w wiersz tabeli do edycji pozycji.
    void podlaczEdycjePozycji(QTableWidget* tabela, PoraPosilku pora);

    // Wczytuje profil, produkty i dziennik aktualnej daty z plikow JSON.
    void wczytajDaneZPlikow();

    // Zapisuje profil, produkty i dziennik aktualnej daty do plikow JSON.
    void zapiszDaneDoPlikow();

    // Odswieza tabele produktow i ulubionych z uwzglednieniem pola wyszukiwania.
    void odswiezTabeleProduktow();

    // Laduje podany wektor produktow do wskazanej tabeli oraz podpina przyciski akcji w wierszach.
    void zaladujWektorDoTabeli(
        QTableWidget* tabela,
        const std::vector<Produkt>& produkty
    );

    // Otwiera formularz edycji istniejacego produktu.
    void otworzOknoEdycji(Produkt p);

    // Pobiera jednostki produktu po nazwie i opcjonalnie doklada jednostke z edytowanej pozycji.
    // Metoda pilnuje tez, zeby na liscie zawsze byla jednostka "g".
    std::vector<JednostkaProduktu> pobierzJednostkiDlaNazwy(
        const std::string& nazwaProduktu,
        const JednostkaProduktu* domyslnaJednostka = nullptr
    ) const;

    // Pokazuje dialog wyboru ilosci i jednostki, z podgladem przeliczonych makroskladnikow.
    // Wynik jest zwracany przez parametry wyjsciowe wybranaIlosc i wybranaJednostka.
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

    // Zamienia kod bledu z DziennikZywieniowy na tekst zrozumialy dla uzytkownika.
    QString komunikatBledu(DziennikZywieniowy::WynikOperacji wynik) const;
};
