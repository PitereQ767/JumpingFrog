//
// Created by 48734 on 05.12.2024.
//

#ifndef WINDOW_H
#define WINDOW_H
#include "config.h"


WINDOW* Start();
void CleanWin(WIN* W, int bo );
WIN* Init(WINDOW* parent, int rows, int cols, int y, int x, int color, int border, int delay);
void StartWin(WINDOW* win, char* player_name, int max_len);

#endif //WINDOW_H
