//
// Created by 48734 on 05.12.2024.
//

#ifndef FROG_H
#define FROG_H
#include "config.h"

void PrintFrog(OBJ* ob);
OBJ* InitFrog(WIN* w, int col, int lives);
void Show(OBJ* ob, int dx, int dy);
void moveFrog(OBJ* ob, char ch);
void resetFrogAfterCollision(OBJ* ob);

#endif //FROG_H
