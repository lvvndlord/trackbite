// Punkt startowy aplikacji Qt. Ten plik tylko uruchamia okno główne,
// a cała logika programu znajduje się w klasach domenowych i MainWindow.
#include "mainwindow.h"
#include <QtWidgets/QApplication>

// Funkcja startowa tworzy QApplication, pokazuje MainWindow i oddaje sterowanie pętli zdarzeń Qt.
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}
