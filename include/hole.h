//
// Created by 48734 on 05.12.2024.
//

#ifndef HOLE_H
#define HOLE_H
#include "config.h"

void PrintHole(Holes *hole);
Holes* InitHole(WIN* w, int x, int y, int color);
Holes** GenerateHoles(WIN* win, int safeArea);
int FrogInHole(OBJ* frog, Holes** holes);

#endif //HOLE_H
