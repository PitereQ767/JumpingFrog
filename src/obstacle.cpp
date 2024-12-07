//
// Created by 48734 on 05.12.2024.
//
#include "../include/config.h"
#include "../include/obstacle.h"
#include "../include/hole.h"


//*******************************
//***** OBSTACLE FUNCTIONS ******
//*******************************

void PrintObstacle(Obstacles *obstacle) {
        for (int i = 0; i<obstacle->width; i++) {
            wattron(obstacle->win->window, COLOR_PAIR(obstacle->color));
            mvwprintw(obstacle->win->window, obstacle->y, obstacle->x + i, "X");
            wattroff(obstacle->win->window, COLOR_PAIR(obstacle->color));
        }
    box(obstacle->win->window, 0, 0); // Samochody nie nadpisuja ramki
}

int isLineOccupied(Obstacles** obstacle, int y) {
    for(int i = 0; i<NUM_OBSTACLES; i++) {
        if(obstacle[i] != NULL && obstacle[i]->y == y) {
            return 1;
        }
    }
    return 0;
}

Obstacles* InitObstacle(WIN* w, int x, int y, int color) {
    Obstacles* obstacle = (Obstacles*)malloc(sizeof(Obstacles));
    if (obstacle == NULL) {
        fprintf(stderr, "Error creating OBSTACLE.\n");
        exit(EXIT_FAILURE);
    }
    obstacle->win = w;
    obstacle->x = x;
    obstacle->y = y;
    obstacle->color = color;
    obstacle->width = (rand()%13) + 3;
    obstacle->height = 1;
    obstacle->speed = SPEED;
    obstacle->stop_time = time(NULL);

    PrintObstacle(obstacle);

    return obstacle;
}

Obstacles** GenerateObstacles(WIN* win, int safeArea) {
    Obstacles** obstacles = (Obstacles**)malloc(NUM_OBSTACLES * sizeof(Obstacles*));
    if (obstacles == NULL) {
        fprintf(stderr, "Error allocating memory for obstacles.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NUM_OBSTACLES; i++) {
        int x = rand() % (win->cols - 2) + 1;
        int y = rand() % (win->rows - 2) + 1;
        while (y == START_Y || y == 1 || y == safeArea || isLineOccupied(obstacles, y)) {
            y = rand() % (win->rows - 2) + 1;
        }
        obstacles[i] = InitObstacle(win, x, y, OBSTACLE_COLOR);
    }

    return obstacles;
}

int CheckCollision(OBJ* frog, Obstacles** obstacles) {
    for (int i = 0; i < NUM_OBSTACLES; i++) {
        Obstacles* ob = obstacles[i];
        for (int j = 0; j < ob->width; j++) {
            if (frog->x == ob->x + j && frog->y == ob->y) {
                return 1;
            }
        }
    }
    return 0;
}

void MoveObstacles(Obstacles** obstacles, int max_rows,Holes** holes) {
    for (int i = 0; i < NUM_OBSTACLES; i++) {
        Obstacles* obstacle = obstacles[i];
        for (int j = 0; j < obstacle->width; j++) {
            mvwprintw(obstacle->win->window, obstacle->y, obstacle->x + j, " ");
        }
        obstacle->x -= obstacle->speed;

        if (obstacle->x + obstacle->width < 1) {
            obstacle->x = obstacle->win->cols - 2;
            int newY = rand() % (max_rows - 2) + 1;
            while (newY == START_Y || newY == 1 || newY == max_rows / 2 || isLineOccupied(obstacles, newY)) {
                newY = rand() % (max_rows - 2) + 1;
            }
            obstacle->y = newY;
        }
        PrintObstacle(obstacle);
    }
    for(int i= 0; i < NUM_HOLES; i++) {
        PrintHole(holes[i]);
    }
    wrefresh(obstacles[0]->win->window);
}


void stopObstacle(Obstacles** obstacle, OBJ* frog) {
    for(int i = 0; i<NUM_OBSTACLES; i++) {
        if(obstacle[i]->y+1 == frog->y && obstacle[i]->x >= frog->x && obstacle[i]->x < frog->x + frog->width) {
            if(rand()%100 < CHANCE_OF_STOP) {
                obstacle[i]->speed = 0; // szansa na zatrzymanie przeszkody, gdy zaba jest blisko
            }
        }
        if(obstacle[i]->speed == 0) {
            if (difftime(time(NULL), obstacle[i]->stop_time) >=TIME_STOP_CAR) {
                obstacle[i]->speed = SPEED;
            }
        }else {
            obstacle[i]->stop_time = time(NULL);
        }
    }
}
