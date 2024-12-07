//
// Created by 48734 on 05.12.2024.
//
#include "../include/hole.h"
#include "../include/config.h"

//*******************************
//******* Hole FUNCTIONS ********
//*******************************

void PrintHole(Holes *hole) {
    for(int i=0; i<hole->height; i++) {
        for(int j=0; j<hole->width; j++) {
            wattron(hole->win->window, COLOR_PAIR(hole->color));
            mvwprintw(hole->win->window, hole->y+i, hole->x + j, "H");
            wattroff(hole->win->window, COLOR_PAIR(hole->color));
        }
    }
}

Holes* InitHole(WIN* w, int x, int y, int color) {
    Holes* hole = (Holes*)malloc(sizeof(Holes));
    if (hole == NULL) {
        fprintf(stderr, "Error creating HOLE.\n");
        exit(EXIT_FAILURE);
    }
    hole->win = w;
    hole->x = x;
    hole->y = y;
    hole->color = color;
    hole->width = 2;
    hole->height = 2;

    PrintHole(hole);

    return hole;
}

Holes** GenerateHoles(WIN* win, int safeArea) {
    Holes** holes = (Holes**)malloc(NUM_HOLES * sizeof(Holes*));
    if (holes == NULL) {
        fprintf(stderr, "Error allocating memory for obstacles.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NUM_HOLES; i++) {
        int x = rand() % (win->cols - 3) + 1; //-3 aby nie byly na ramce
        int y = rand() % (win->rows - 2) + 1;
        while (y == START_Y || y == 1 || y == safeArea || y == safeArea - 1 || y==START_Y-1) {
            y = rand() % (win->rows - 2) + 1;
        }
        holes[i] = InitHole(win, x, y, HOLE_COLOR);
    }

    return holes;
}

int FrogInHole(OBJ* frog, Holes** holes) {
    for(int i=0; i<NUM_HOLES; i++) {
        Holes* hole = holes[i];
        for(int j=0; j<hole->width; j++) {
            for(int z=0; z<hole->height; z++) {
                if (frog->x == hole->x + j && frog->y == hole->y+z) {
                    return 1;
                }
            }
        }
    }
    return 0;
}