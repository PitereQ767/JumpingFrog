//
// Created by 48734 on 05.12.2024.
//

#ifndef TIMER_H
#define TIMER_H
#include "config.h"

void Sleep(unsigned int tui);
void ShowTimer(WIN* W, float pass_time, int life);
TIMER* InitTimer(int pass_time);
int UpdateTimer(TIMER* T, WIN* status, OBJ* ob, int pass_time);

#endif //TIMER_H
