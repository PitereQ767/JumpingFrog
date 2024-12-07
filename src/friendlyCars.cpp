//
// Created by 48734 on 05.12.2024.
//
#include "../include/config.h"
#include "../include/friendlyCars.h"
#include "../include/frog.h"
#include "../include/hole.h"

//*******************************
//*** FRIENDLY CAR FUNCTIONS ****
//*******************************
void PrintCar(FriendlyCars* car) {
    for (int i = 0; i<car->width; i++) {
        wattron(car->win->window, COLOR_PAIR(car->color));
        mvwprintw(car->win->window, car->y, car->x+i, "F");
        wattroff(car->win->window, COLOR_PAIR(car->color));
    }

    box(car->win->window, 0, 0);
}

int isLineOccupiedByFriendlyCar(FriendlyCars** cars, int y) {
    for(int i = 0; i<NUM_FRIENDLY_CAR; i++) {
        if(cars[i] != NULL && cars[i]->y == y) {
            return 1;
        }
    }
    return 0;
}

void MoveFrogToNewLocation(OBJ* frog) {
    int newX = rand()% (frog->win->cols - 2) + 1;
    int newY = rand()% (frog->win->rows - 2) + 1;

    while(newY == 1 || newY == frog->y || newX == frog->x) {
        newX = rand()% (frog->win->cols - 2) + 1;
        newY = rand()% (frog->win->rows - 2) + 1;
    }
    frog->x = newX;
    frog->y = newY;
    PrintFrog(frog);
}

FriendlyCars* InitFriendlyCar(WIN* w, int x, int y, int color) {
    FriendlyCars* car = (FriendlyCars*)malloc(sizeof(FriendlyCars));

    if (car == NULL) {
        fprintf(stderr, "Error allocating memory for friendly cars.\n");
        exit(EXIT_FAILURE);
    }

    car->win = w;
    car->color = color;
    car->speed = 1;
    car->x = x;
    car->y = y;
    car->width = 5;
    car->height = 1;

    PrintCar(car);
    return car;
}

FriendlyCars** GenerateFriendlyCars(WIN* win, int safeArea) {
    FriendlyCars** cars = (FriendlyCars**)malloc(sizeof(FriendlyCars*)* NUM_FRIENDLY_CAR);
    if (cars == NULL) {
        fprintf(stderr, "Error allocating memory for friendly cars.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < NUM_FRIENDLY_CAR; i++) {
        int x = rand() % (win->cols - 2) + 1;
        int y = rand() % (win->rows - 2) + 1;
        while (y == START_Y || y == 1 || y == safeArea || isLineOccupiedByFriendlyCar(cars, y)) {
            y = rand() % (win->rows - 2) + 1;
        }
        cars[i] = InitFriendlyCar(win, x, y, FRIENDLY_CAR_COLOR);
    }

    return cars;
}

void MoveFriendlyCars(FriendlyCars** cars, int max_rows, Holes** holes, OBJ* frog, int* key) {
    for (int i = 0; i < NUM_FRIENDLY_CAR; i++) {
        FriendlyCars* car = cars[i];

        if (car->x <= frog->x + frog->width && car->x + car->width >= frog->x && car->y == frog->y) {
            if (*key == 'z') {
                MoveFrogToNewLocation(frog);
            }
        }
        for (int j = 0; j < car->width; j++) {
            mvwprintw(car->win->window, car->y, car->x + j, " ");
        }
        car->x -= car->speed;

        if (car->x + car->width < 1) {
            car->x = car->win->cols - 2;
            int newY = rand() % (max_rows - 2) + 1;
            while (newY == START_Y || newY == 1 || newY == max_rows / 2 || isLineOccupiedByFriendlyCar(cars, newY)) {
                newY = rand() % (max_rows - 2) + 1;
            }
            car->y = newY;
        }
        PrintCar(car);
    }
    for(int i= 0; i < NUM_HOLES; i++) {
        PrintHole(holes[i]);
    }
    PrintFrog(frog);
    wrefresh(cars[0]->win->window);
}