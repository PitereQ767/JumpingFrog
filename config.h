//
// Created by 48734 on 01.12.2024.
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

#define TIME_BETWEEN_JUMPS 0.01
#define CHANCE_OF_STOP 50
#define SPEED rand()%2+1



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
    int life;
    int last_jump;
    time_t stop_time;
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

#endif //CONFIG_H
