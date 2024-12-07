//
// Created by 48734 on 05.12.2024.
//

#ifndef FRIENDLYCARS_H
#define FRIENDLYCARS_H
#include "config.h"

void PrintCar(FriendlyCars* car);
int isLineOccupiedByFriendlyCar(FriendlyCars** cars, int y);
void MoveFrogToNewLocation(OBJ* frog);
FriendlyCars* InitFriendlyCar(WIN* w, int x, int y, int color);
FriendlyCars** GenerateFriendlyCars(WIN* win, int safeArea);
void MoveFriendlyCars(FriendlyCars** cars, int max_rows, Holes** holes, OBJ* frog, int* key);


#endif //FRIENDLYCARS_H
