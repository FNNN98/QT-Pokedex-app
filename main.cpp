#include "pokedex.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Pokedex w;
    w.show();
    return a.exec();
}
