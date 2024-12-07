//
// Created by 48734 on 05.12.2024.
//

#ifndef CONFIG_H
#define CONFIG_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <cstring>
#include <time.h>  // Zawiera funkcję clock()

#define MAIN_COLOR 1
#define STAT_COLOR 2
#define FROG_COLOR 3
#define OBSTACLE_COLOR 4
#define SAFE_AREA 5
#define FINISH_LINE 6
#define HOLE_COLOR 7
#define FRIENDLY_CAR_COLOR 8
#define STORK_COLOR 9

#define QUIT 'q'
#define NOKEY ' '

#define DELAY_OFF 0
#define BORDER 1
#define DELAY_ON 1


#define OFFY 0
#define OFFX 8

#define FRAME_TIME 25 // Czas trwania jednej klatki w milisekundach


#define START_X 37
#define START_Y 25
#define NUM_OBSTACLES 5
#define NUM_HOLES 3
#define NUM_FRIENDLY_CAR 2
#define STORK_HEIGHT 3
#define STORK_WIDTH 2

#define TIME_BETWEEN_JUMPS 1000 //w mikrosekundach
#define TIME_BETWEEN_MOVEMENT 10000
#define CHANCE_OF_STOP 50 // w %
#define TIME_STOP_CAR 3 //w sekundach
#define SPEED rand()%2+1

#define MAX_SCORES 100 //w pliku do wyswietlania rankingu



// Struktury do przechowywania okien i obiektów
typedef struct {
    WINDOW* window; // ncurses window
    int x, y;
    int rows, cols;
    int color;
} WIN;

typedef struct {
    float pass_time; // Pozostały czas w sekundach
    clock_t start_time_ms; // Czas rozpoczęcia gry (w "tikach" procesora)
    int frame_no; // Numer klatki
    float frame_time; // Czas trwania jednej klatki
} TIMER;

typedef struct {
    WIN* win;
    int color;
    int mv;
    int x, y;
    int width, height;
    int xmin, xmax;
    int ymin, ymax;
    int speed;
    int lives;
    clock_t last_jump;
} OBJ;

typedef struct {
    char name[50];
    int score;
} Score;

typedef struct {
    int x;
    int y;
    int width;
    int height;
    int color;
    int speed;
    time_t stop_time;
    WIN* win;
}Obstacles;

typedef struct {
    int x;
    int y;
    int width;
    int height;
    int color;
    WIN* win;
}Holes;

typedef struct {
    int x;
    int y;
    int width;
    int height;
    int color;
    int speed;
    WIN* win;
}FriendlyCars;

typedef struct {
    WIN* win;
    int x, y;
    int width;
    int height;
    int speed;
    int color;
    clock_t last_movement;
}Stork;

#endif //CONFIG_H
