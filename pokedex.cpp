#include "pokedex.h"
#include "ui_pokedex.h"
#include "data.h"

#include <QApplication>
#include <QCoreApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QStandardItemModel>
#include <QDir>
#include <QFont>
#include <QtCharts>
#include <QPixmap>


Pokedex::Pokedex(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Pokedex)
{
    ui->setupUi(this);

    tableView = findChild<QTableView*>("tableView");
    radarChart = findChild<QWidget*>("radarChart");

    initializeUI();

    parseCSV();

    initializeQListWidgets(); //listWidgets must be initialized after parsing data to the vector allPokemons

    connect(ui->fire, &QCheckBox::stateChanged, this, &Pokedex::filterType);
    connect(ui->water, &QCheckBox::stateChanged, this, &Pokedex::filterType);
    connect(ui->grass, &QCheckBox::stateChanged, this, &Pokedex::filterType);
    connect(ui->electric, &QCheckBox::stateChanged, this, &Pokedex::filterType);
    connect(ui->psychic, &QCheckBox::stateChanged, this, &Pokedex::filterType);
    connect(ui->rock, &QCheckBox::stateChanged, this, &Pokedex::filterType);
    connect(ui->ground, &QCheckBox::stateChanged, this, &Pokedex::filterType);
    connect(ui->ice, &QCheckBox::stateChanged, this, &Pokedex::filterType);
    connect(ui->flying, &QCheckBox::stateChanged, this, &Pokedex::filterType);
    connect(ui->ghost, &QCheckBox::stateChanged, this, &Pokedex::filterType);
    connect(ui->poison, &QCheckBox::stateChanged, this, &Pokedex::filterType);
    connect(ui->fighting, &QCheckBox::stateChanged, this, &Pokedex::filterType);
    connect(ui->bug, &QCheckBox::stateChanged, this, &Pokedex::filterType);
    connect(ui->normal, &QCheckBox::stateChanged, this, &Pokedex::filterType);
    connect(ui->dark, &QCheckBox::stateChanged, this, &Pokedex::filterType);
    connect(ui->steel, &QCheckBox::stateChanged, this, &Pokedex::filterType);
    connect(ui->fairy, &QCheckBox::stateChanged, this, &Pokedex::filterType);
    connect(ui->dragon, &QCheckBox::stateChanged, this, &Pokedex::filterType);

    // getting signal from lisWidget selection
    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)),
               this, SLOT(clickedPokemon(QListWidgetItem*)));
    connect(ui->listWidget_2, SIGNAL(itemClicked(QListWidgetItem*)),
               this, SLOT(clickedPokemon(QListWidgetItem*)));

}

void Pokedex::initializeUI(){

    // setting UI parameters
    tableView = findChild<QTableView*>("tableView");
    tableView->setFixedSize(975, 700);
    ui->label->setFixedSize(50, 50);
    QFont font = ui->label->font();
    font.setWeight(QFont::Bold);

    ui->label->setFont(font);
    ui->label_2->setFont(font);
    ui->label_3->setFont(font);

    ui->label->setAlignment(Qt::AlignCenter);
    ui->label_2->setAlignment(Qt::AlignCenter);
    ui->label_3->setAlignment(Qt::AlignCenter);

    // enable sorting for the tableView
    //tableView->setSortingEnabled(true);

}

void Pokedex::parseCSV() {

    // getting the path to automatically open pokedex.csv. DESTDIR = $$PWD in PRO file is a MUST for it to work
    QString executablePath = QCoreApplication::applicationDirPath();
    QString csvPath = executablePath + "/pokedex.csv";

    QFile file(csvPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open CSV file:" << file.errorString();
        return;
    }

    QTextStream in(&file);

    // skip first line
    if (!in.atEnd()) {
        in.readLine();
    }

    while (!in.atEnd()) {
        // while the csv file is =/= the end, keep reading
        QString row = in.readLine();

        // save data in QStringList
        QStringList columns;

        QString tmp;
        bool inQuotedRegion = true;

        // parsing method recycled from my January's project (same parsing logic for pokemon types as Uffizi.csv)

        for (int i = 0; i < row.size(); ++i) {
            if (row.at(i) == ',' && inQuotedRegion) {
                columns.append(tmp);
                tmp = "";
            } else if (row.at(i) == '"') {
                inQuotedRegion = !inQuotedRegion;
            } else {
                tmp.append(row.at(i));
            }

            // append if it's the last character
            if (i == row.size() - 1) {
                columns.append(tmp);
            }
        }

        // putting the extracted data in a pokemon object
        pokemon newPokemon;
        newPokemon.jpg_image = columns[0].trimmed();
        newPokemon.pokeID = columns[1].toInt();
        newPokemon.name = columns[2].trimmed();

        // pokemon type extraction
        QString typesString = columns[3];
        typesString.remove("\""); // removing quotation marks
        QStringList types = typesString.split(',');
        if (types.size() >= 1) {
            newPokemon.type1 = types[0].trimmed();
        }
        if (types.size() >= 2) {
            newPokemon.type2 = types[1].trimmed();
        }
        // pokemons theoretically can have up to 3 types, but there's no 3-type pokemon in the CSV so type3 stays empty
        newPokemon.total = columns[4].toInt();
        newPokemon.hp = columns[5].toInt();
        newPokemon.attack = columns[6].toInt();
        newPokemon.defense = columns[7].toInt();
        newPokemon.sp_attack = columns[8].toInt();
        newPokemon.sp_defense = columns[9].toInt();
        newPokemon.speed = columns[10].toInt();

        // adding pokemon to vector
        allPokemons.push_back(newPokemon);
    }

    // resizing images and saving in an image cache to then add them to the table.
    // doing it manually when filtering by type each time was extremely computationally heavy
    for (const pokemon& p : allPokemons) {
        QImage image(p.jpg_image);
        QSize smallerJPEG(48, 48);
        QPixmap pixmap = QPixmap::fromImage(image.scaled(smallerJPEG, Qt::KeepAspectRatio));
        resizedImageCache.insert(p.jpg_image, pixmap);
     }
    file.close();
    updateTable(allPokemons);
}

void Pokedex::updateTable(const std::vector<pokemon>& filteredPokemons) {

    // putting table data in a model
    QStandardItemModel *model = new QStandardItemModel(allPokemons.size(), 12, this);
    // setting headers
    model->setHorizontalHeaderLabels({
        "Image",        // jpg_image
        "ID",           // pokeID
        "Name",         // name
        "Type 1",       // type1
        "Type 2",       // type2
        "Total",        // total
        "HP",           // hp
        "Attack",       // attack
        "Defense",      // defense
        "Sp. ATK",      // sp_attack
        "Sp. DEF",      // sp_defense
        "Speed"         // speed
    });

    for (unsigned int i = 0; i < filteredPokemons.size(); ++i) {
            const pokemon &p = filteredPokemons[i];

                    // retrieving pre-resized image from the cache
                    QPixmap pixmap = resizedImageCache.value(p.jpg_image);

                    QStandardItem *imageItem = new QStandardItem();
                    imageItem->setData(QVariant(pixmap), Qt::DecorationRole);

                    model->setItem(i, 0, imageItem); //OK
                    model->setItem(i, 1, new QStandardItem(QString::number(p.pokeID)));
                    model->setItem(i, 2, new QStandardItem(p.name));
                    model->setItem(i, 3, new QStandardItem(p.type1));
                    model->setItem(i, 4, new QStandardItem(p.type2));
                    model->setItem(i, 5, new QStandardItem(QString::number(p.total)));
                    model->setItem(i, 6, new QStandardItem(QString::number(p.hp)));
                    model->setItem(i, 7, new QStandardItem(QString::number(p.attack)));
                    model->setItem(i, 8, new QStandardItem(QString::number(p.defense)));
                    model->setItem(i, 9, new QStandardItem(QString::number(p.sp_attack)));
                    model->setItem(i, 10, new QStandardItem(QString::number(p.sp_defense)));
                    model->setItem(i, 11, new QStandardItem(QString::number(p.speed)));

        // set background color based on type1
        QModelIndex indexType1 = model->index(i, 3); // column 3 = type1
        QString type1 = model->data(indexType1).toString();
        if (type1 == "Water") {
            model->setData(indexType1, QBrush(Qt::blue), Qt::BackgroundRole);
        } else if (type1 == "Fire") {
            model->setData(indexType1, QBrush(Qt::red), Qt::BackgroundRole);
        } else if (type1 == "Grass") {
            model->setData(indexType1, QBrush(Qt::green), Qt::BackgroundRole);
        } else if (type1 == "Electric") {
            model->setData(indexType1, QBrush(QColor(204, 204, 0)), Qt::BackgroundRole); // can't read white on yellow if I use standard yellow
        } else if (type1 == "Fairy") {
            model->setData(indexType1, QBrush(QColor(255, 182, 193)), Qt::BackgroundRole);
        } else if (type1 == "Poison") {
            model->setData(indexType1, QBrush(Qt::darkMagenta), Qt::BackgroundRole);
        } else if (type1 == "Dragon") {
            model->setData(indexType1, QBrush(Qt::cyan), Qt::BackgroundRole);
        } else if (type1 == "Flying") {
            model->setData(indexType1, QBrush(QColor(115, 147, 179)), Qt::BackgroundRole);
        } else if (type1 == "Normal") {
            model->setData(indexType1, QBrush(QColor(211, 211, 211)), Qt::BackgroundRole);
        } else if (type1 == "Dark") {
            model->setData(indexType1, QBrush(Qt::black), Qt::BackgroundRole);
        } else if (type1 == "Ice") {
            model->setData(indexType1, QBrush(Qt::cyan), Qt::BackgroundRole);
        } else if (type1 == "Ground") {
            model->setData(indexType1, QBrush(Qt::darkYellow), Qt::BackgroundRole);
        } else if (type1 == "Rock") {
            model->setData(indexType1, QBrush(QColor(92, 64, 51)), Qt::BackgroundRole); // QT::brown doesn't exist
        } else if (type1 == "Steel") {
            model->setData(indexType1, QBrush(Qt::darkGray), Qt::BackgroundRole);
        } else if (type1 == "Psychic") {
            model->setData(indexType1, QBrush(Qt::magenta), Qt::BackgroundRole);
        } else if (type1 == "Fighting") {
            model->setData(indexType1, QBrush(Qt::darkRed), Qt::BackgroundRole);
        } else if (type1 == "Ghost") {
            model->setData(indexType1, QBrush(QColor(130, 120, 173)), Qt::BackgroundRole);
        } else if (type1 == "Bug") {
            model->setData(indexType1, QBrush(Qt::darkGreen), Qt::BackgroundRole);
        }
        // Set background color based on type2
        QModelIndex indexType2 = model->index(i, 4); // column 4 = type2
        QString type2 = model->data(indexType2).toString();
        if (type2 == "Water") {
            model->setData(indexType2, QBrush(Qt::blue), Qt::BackgroundRole);
        } else if (type2 == "Fire") {
            model->setData(indexType2, QBrush(Qt::red), Qt::BackgroundRole);
        } else if (type2 == "Grass") {
            model->setData(indexType2, QBrush(Qt::green), Qt::BackgroundRole);
        } else if (type2 == "Electric") {
            model->setData(indexType2, QBrush(QColor(204, 204, 0)), Qt::BackgroundRole);
        } else if (type2 == "Fairy") {
            model->setData(indexType2, QBrush(QColor(255, 182, 193)), Qt::BackgroundRole);
        } else if (type2 == "Poison") {
            model->setData(indexType2, QBrush(Qt::darkMagenta), Qt::BackgroundRole);
        } else if (type2 == "Dragon") {
            model->setData(indexType2, QBrush(Qt::cyan), Qt::BackgroundRole);
        } else if (type2 == "Flying") {
            model->setData(indexType2, QBrush(QColor(115, 147, 179)), Qt::BackgroundRole);
        } else if (type2 == "Normal") {
            model->setData(indexType2, QBrush(QColor(211, 211, 211)), Qt::BackgroundRole);
        } else if (type2 == "Dark") {
            model->setData(indexType2, QBrush(Qt::black), Qt::BackgroundRole);
        } else if (type2 == "Ice") {
            model->setData(indexType2, QBrush(Qt::cyan), Qt::BackgroundRole);
        } else if (type2 == "Ground") {
            model->setData(indexType2, QBrush(Qt::darkYellow), Qt::BackgroundRole);
        } else if (type2 == "Rock") {
            model->setData(indexType2, QBrush(QColor(92, 64, 51)), Qt::BackgroundRole);
        } else if (type2 == "Steel") {
            model->setData(indexType2, QBrush(Qt::darkGray), Qt::BackgroundRole);
        } else if (type2 == "Psychic") {
            model->setData(indexType2, QBrush(Qt::magenta), Qt::BackgroundRole);
        } else if (type2 == "Fighting") {
            model->setData(indexType2, QBrush(Qt::darkRed), Qt::BackgroundRole);
        } else if (type2 == "Ghost") {
            model->setData(indexType2, QBrush(QColor(130, 120, 173)), Qt::BackgroundRole);
        } else if (type1 == "Bug") {
            model->setData(indexType1, QBrush(Qt::darkGreen), Qt::BackgroundRole);
        }

        // setting text color to white for type1 and type2
        model->setData(indexType1, QBrush(Qt::white), Qt::TextColorRole);
        model->setData(indexType2, QBrush(Qt::white), Qt::TextColorRole);

        // bold font for ID, Name, Total, Type1, and Type2
        QModelIndex indexID = model->index(i, 1);
        QModelIndex indexName = model->index(i, 2);
        QModelIndex indexTotal = model->index(i, 5);

        QFont font = model->data(indexID, Qt::FontRole).value<QFont>();
        font.setBold(true);
        model->setData(indexID, font, Qt::FontRole);

        font = model->data(indexName, Qt::FontRole).value<QFont>();
        font.setBold(true);
        model->setData(indexName, font, Qt::FontRole);

        font = model->data(indexTotal, Qt::FontRole).value<QFont>();
        font.setBold(true);
        model->setData(indexTotal, font, Qt::FontRole);

        font = model->data(indexType1, Qt::FontRole).value<QFont>();
        font.setBold(true);
        model->setData(indexType1, font, Qt::FontRole);

        font = model->data(indexType2, Qt::FontRole).value<QFont>();
        font.setBold(true);
        model->setData(indexType2, font, Qt::FontRole);
    }

    // setting model to tableview
    tableView->setModel(model);
    // disable editing from UI
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // resizing column width to better fix the window
    tableView->setColumnWidth(0, 75);  // image
    tableView->setColumnWidth(1, 20);  // ID
    tableView->setColumnWidth(2, 150);  // name
    tableView->setColumnWidth(3, 75);  // type1
    tableView->setColumnWidth(4, 75);  // type2
    tableView->setColumnWidth(5, 30);  // Total
    tableView->setColumnWidth(6, 30);  // HP
    tableView->setColumnWidth(7, 70);  // Attack
    tableView->setColumnWidth(8, 70);  // Defense
    tableView->setColumnWidth(9, 70);  // SP.Attack
    tableView->setColumnWidth(10, 70); // SP.Defense
    tableView->setColumnWidth(11, 70); // Speed
}

void Pokedex::filterType() {
    // read the states of each QCheckBox
    QStringList typesToFilter;
    if (ui->fire->isChecked()) {
        typesToFilter.append("Fire");
    }
    if (ui->grass->isChecked()) {
        typesToFilter.append("Grass");
    }
    if (ui->water->isChecked()) {
        typesToFilter.append("Water");
    }
    if (ui->electric->isChecked()) {
        typesToFilter.append("Electric");
    }
    if (ui->fairy->isChecked()) {
        typesToFilter.append("Fairy");
    }
    if (ui->poison->isChecked()) {
        typesToFilter.append("Poison");
    }
    if (ui->dragon->isChecked()) {
        typesToFilter.append("Dragon");
    }
    if (ui->normal->isChecked()) {
        typesToFilter.append("Normal");
    }
    if (ui->ground->isChecked()) {
        typesToFilter.append("Ground");
    }
    if (ui->bug->isChecked()) {
        typesToFilter.append("Bug");
    }
    if (ui->psychic->isChecked()) {
        typesToFilter.append("Psychic");
    }
    if (ui->fighting->isChecked()) {
        typesToFilter.append("Fighting");
    }
    if (ui->rock->isChecked()) {
        typesToFilter.append("Rock");
    }
    if (ui->ghost->isChecked()) {
        typesToFilter.append("Ghost");
    }
    if (ui->ice->isChecked()) {
        typesToFilter.append("Ice");
    }
    if (ui->steel->isChecked()) {
        typesToFilter.append("Steel");
    }
    if (ui->dark->isChecked()) {
        typesToFilter.append("Dark");
    }
    if (ui->flying->isChecked()) {
        typesToFilter.append("Flying");
    }

    // clear the model associated to QTableView
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->tableView->model());
    if (!model) {
        model = new QStandardItemModel(this);
        ui->tableView->setModel(model);
    } else {
        model->clear(); // clear all data from the model
    }
    // setting the sort role for each column
    for (int column = 0; column < model->columnCount(); ++column) {
        tableView->setSortingEnabled(true);
        tableView->sortByColumn(column, Qt::AscendingOrder);
    }
    // repopulating the model with filtered data
    std::vector<pokemon> filteredPokemons;
    for (const pokemon &p : allPokemons) {
        bool matchesAllTypes = true;
        for (const QString &type : typesToFilter) {
            if (p.type1 != type && p.type2 != type) {
                matchesAllTypes = false;
                break;
            }
        }
        if (matchesAllTypes) {
            filteredPokemons.push_back(p);
        }
    }

    // calling updateTable with the filteredPokemons vector
    updateTable(filteredPokemons);
}

void Pokedex::initializeQListWidgets() {

    for (unsigned int i = 0; i < allPokemons.size(); ++i) {
            const pokemon &p = allPokemons[i];

            // load item (=pokemon name) to the two widgets
            QListWidgetItem *item = new QListWidgetItem(p.name);
            ui->listWidget->addItem(item);
            QListWidgetItem *item2 = new QListWidgetItem(p.name);
            ui->listWidget_2->addItem(item2);

    }
    // enabling sorting
    ui->listWidget->setSortingEnabled(true);

    ui->listWidget_2->setSortingEnabled(true);

    // sorting alphabetically
    ui->listWidget->sortItems(Qt::AscendingOrder);

    ui->listWidget_2->sortItems(Qt::AscendingOrder);

}

void Pokedex::clickedPokemon(QListWidgetItem* item) {
    if (item) {
        QString pokename = item->text();

        // checking if signal comes from first listWidget
        if (sender() == ui->listWidget) {
            // finding the pokemon
            for (const pokemon& p : allPokemons) {
                if (p.name == pokename) {
                    // project requirement: can't selecte the same pokemon to compare, send error!
                    if (selectedPokemonList2.name == pokename) {
                        QMessageBox::warning(this, "Error", "Cannot select the same Pokémon in both lists.");
                        return;
                    }
                    selectedPokemonList1 = p; // store the selected Pokemon for listWidget
                    break;
                }
            }
        } else if (sender() == ui->listWidget_2) { //else ->the signal came from the other listWidget
            // find the pokemon
            for (const pokemon& p : allPokemons) {
                if (p.name == pokename) {
                    // project requirement: can't selecte the same pokemon to compare, send error!
                    if (selectedPokemonList1.name == pokename) {
                        QMessageBox::warning(this, "Error", "Cannot select the same Pokémon in both lists.");
                        return;
                    }
                    selectedPokemonList2 = p; // storing the selected Pokemon for listWidget_2
                    break;
                }
            }
        }
    }
    // calling comparison on the 2 store pokemons
    comparePokemons(selectedPokemonList1, selectedPokemonList2);
}



void Pokedex::comparePokemons(const pokemon &pokemon1, const pokemon &pokemon2) {

    // loading pics for pokemon 1 and pokemon 2
    QPixmap poke1(pokemon1.jpg_image);
    QSize smallerJPEG1(150, 150);  // manual label size approach doesn't appear to work well
    QPixmap scaledPoke1 = poke1.scaled(smallerJPEG1, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->pokemon1label->setPixmap(scaledPoke1.scaled(smallerJPEG1, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QPixmap poke2(pokemon2.jpg_image);
    QSize smallerJPEG2(150, 150);
    QPixmap scaledPoke2 = poke2.scaled(smallerJPEG2, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->pokemon2label->setPixmap(scaledPoke2.scaled(smallerJPEG2, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // clearing radar chart if no Pokémon or only one Pokémon is selected
    if (pokemon1.name.isEmpty() || pokemon2.name.isEmpty()) {
        QLayout *layout = radarChart->layout();
        if (layout) {
            QLayoutItem *item;
            while ((item = layout->takeAt(0)) != nullptr) {
                delete item->widget();
                delete item;
            }
            delete layout;
        }
        return; // exit the function
    }

    // finding the highest stat value among all Pokémon
    int maxPokeStat = std::max({
        pokemon1.speed, pokemon1.hp, pokemon1.sp_attack,
        pokemon1.sp_defense, pokemon1.attack, pokemon1.defense,
        pokemon2.speed, pokemon2.hp, pokemon2.sp_attack,
        pokemon2.sp_defense, pokemon2.attack, pokemon2.defense
    });

    // create a polar chart comparing the stats for the two Pokémon
    QPolarChart *chart = new QPolarChart();
    chart->setTitle("Stat Comparison");

    // create a polar series for each Pokémon
    QLineSeries *pokeSeries = new QLineSeries();

    QCategoryAxis *asse = new QCategoryAxis;
    asse->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    asse->setRange(0, 360);
    // adding labelels every 360/6categories = 60 degrees
    asse->append("SPEED", 0);
    asse->append("HP", 60);
    asse->append("SP.ATK", 120);
    asse->append("SP.DEF", 180);
    asse->append("DEFENSE", 240);
    asse->append("ATTACK", 300);

    chart->addAxis(asse, QPolarChart::PolarOrientationAngular);

    pokeSeries->setName(pokemon1.name);
    pokeSeries->append(0, pokemon1.speed);
    pokeSeries->append(45, pokemon1.hp);
    pokeSeries->append(90, pokemon1.sp_attack);
    pokeSeries->append(135, pokemon1.sp_defense);
    pokeSeries->append(180, pokemon1.attack);
    pokeSeries->append(225, pokemon1.defense);
    pokeSeries->append(270, pokemon1.speed); // End the series at the same point

    QLineSeries *pokeSeries2 = new QLineSeries();
    pokeSeries2->setName(pokemon2.name);

    pokeSeries2->append(0, pokemon2.speed);
    pokeSeries2->append(45, pokemon2.hp);
    pokeSeries2->append(90, pokemon2.sp_attack);
    pokeSeries2->append(135, pokemon2.sp_defense);
    pokeSeries2->append(180, pokemon2.attack);
    pokeSeries2->append(225, pokemon2.defense);
    pokeSeries2->append(270, pokemon2.speed); // End the series at the same point

    // Add the series to the chart
    chart->addSeries(pokeSeries);
    chart->addSeries(pokeSeries2);

    // Set the radial axis
    QValueAxis *axis = new QValueAxis();
    axis->setRange(0, maxPokeStat + 10); // Making range slightly bigger than where the maximum value would touch in the graph.
    chart->addAxis(axis, QPolarChart::PolarOrientationRadial);

    // Loading the series to the radial axis
    pokeSeries->attachAxis(axis);
    pokeSeries2->attachAxis(axis);

    // Create the chart view
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Replace the existing content of the radarChart widget with the new chart
    QLayout *layout = radarChart->layout();
    if (layout) {
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete layout;
    }
    layout = new QVBoxLayout(radarChart);
    layout->addWidget(chartView);
    radarChart->setLayout(layout);
}


Pokedex::~Pokedex()
{
    tableView->setParent(nullptr);
    delete tableView;

    radarChart->setParent(nullptr);
    delete radarChart;

    delete ui;
}

