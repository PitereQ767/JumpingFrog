//
// Created by 48734 on 05.12.2024.
//

#ifndef OBSTACLE_H
#define OBSTACLE_H
#include "config.h"

void PrintObstacle(Obstacles *obstacle);
int isLineOccupied(Obstacles** obstacle, int y);
Obstacles* InitObstacle(WIN* w, int x, int y, int color);
Obstacles** GenerateObstacles(WIN* win, int safeArea);
int CheckCollision(OBJ* frog, Obstacles** obstacles);
void MoveObstacles(Obstacles** obstacles, int max_rows,Holes** holes);
void stopObstacle(Obstacles** obstacle, OBJ* frog);


#endif //OBSTACLE_H
