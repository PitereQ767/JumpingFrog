//
// Created by 48734 on 06.12.2024.
//
#include "../include/config.h"
#include "../include/stork.h"

Stork* InitStork(WIN* win, int x, int y, int height, int width, int color) {
    Stork* stork = (Stork*)malloc(sizeof(Stork));
    if (stork == NULL) {
        fprintf(stderr, "Error allocating memory for stork.\n");
        exit(EXIT_FAILURE);

    }

    stork->win = win;
    stork->x = x;
    stork->y = y;
    stork->color = color;
    stork->height = height;
    stork->width = width;
    stork->speed = 1;
    stork->last_movement = clock();

    return stork;
}

void PrintStork(Stork* stork) {

    wattron(stork->win->window, COLOR_PAIR(stork->color));
    for (int i = 0; i<stork->height; i++) {
        for (int j = 0; j<stork->width; j++) {
            mvwprintw(stork->win->window, stork->y+i, stork->x+j, "S");
        }
    }
    wattroff(stork->win->window, COLOR_PAIR(stork->color));
}

void ClearStork(Stork* stork) {

    for(int i = 0; i<stork->height; i++) {
        if(stork->y+i == stork->win->rows/2) { // Stork in SafeArea
            for(int j = 0; j<stork->width; j++) {
                wattron(stork->win->window, COLOR_PAIR(SAFE_AREA));
                mvwprintw(stork->win->window, stork->y+i, stork->x+j, " ");
                wattroff(stork->win->window, COLOR_PAIR(SAFE_AREA));
            }
        }
        else {
            for(int j = 0; j<stork->width; j++) {
                mvwprintw(stork->win->window, stork->y+i, stork->x+j, " ");
            }
        }
    }

}


void MoveStork(Stork* stork, OBJ* frog) {
    clock_t current_time = clock();

    if(current_time - stork->last_movement > TIME_BETWEEN_MOVEMENT) {
        ClearStork(stork);

        if (stork->x < frog->x && stork->x+stork->width < stork->win->cols - 1) {
            stork->x += stork->speed;
        }else if (stork->x > frog->x && stork->x > 0) {
            stork->x -= stork->speed;
        }

        if (stork->y < frog->y && stork->y + stork->height < stork->win->rows - 1) {
            stork->y += stork->speed;
        }else if (stork->y > frog->y && stork->y > 0) {
            stork->y -= stork->speed;
        }

        stork->last_movement = current_time;
    }
    PrintStork(stork);
    wrefresh(stork->win->window);

}

int CheckStorkCollision(Stork* stork, OBJ* frog) {
    if (frog->x>= stork->x && frog->x < stork->x + stork->width &&
        frog->y >= stork->y && frog->y < stork->y + stork->height) {
        return 1;
    }
    return 0;
}