#ifndef POKEDEX_H
#define POKEDEX_H
#include <QTableView>
#include <QMainWindow>
#include <QItemSelection>
#include <QListView>
#include <QListWidget>
#include <vector>
#include "data.h"

#include <QtCore>

QT_BEGIN_NAMESPACE
namespace Ui { class Pokedex; }
QT_END_NAMESPACE

class Pokedex : public QMainWindow
{
    Q_OBJECT

public:
    Pokedex(QWidget *parent = nullptr);
    ~Pokedex();

    void updateTable(const std::vector<pokemon>& filteredPokemons);

    QMap<QString, QPixmap> resizedImageCache; // resized image cache for updateTable

private slots:

    void clickedPokemon(QListWidgetItem* item);

private:
    Ui::Pokedex *ui;

    QTableView *tableView;
    QWidget *radarChart;
    QListWidget listWidget;
    QListWidget listWidget_2;

    std::vector<pokemon> allPokemons;
    pokemon selectedPokemonList1;
    pokemon selectedPokemonList2;

    void parseCSV();
    void initializeUI();
    void filterType();
    void initializeQListWidgets();
    void comparePokemons(const pokemon &pokemon1, const pokemon &pokemon2);



};
#endif // POKEDEX_H
