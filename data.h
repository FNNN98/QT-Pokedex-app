#ifndef DATA_H
#define DATA_H
#include <QString>


struct pokemon {
    int pokeID;
    QString name;
    QString type1;
    QString type2;
    QString type3;
    int total;
    int hp;
    int attack;
    int defense;
    int sp_attack;
    int sp_defense;
    int speed;
    QString jpg_image;
};


#endif
