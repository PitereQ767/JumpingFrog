//
// Created by 48734 on 06.12.2024.
//

#ifndef STORK_H
#define STORK_H
#include "config.h"

Stork* InitStork(WIN* win, int x, int y, int height, int width, int color);
void PrintStork(Stork* stork);
void ClearStork(Stork* stork);
void MoveStork(Stork* stork, OBJ* frog);
int CheckStorkCollision(Stork* stork, OBJ* frog);

#endif //STORK_H
