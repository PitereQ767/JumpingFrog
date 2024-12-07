//
// Created by 48734 on 05.12.2024.
//

#include "../include/frog.h"
#include "../include/config.h"

//*******************************
//****** FROG FUNCTIONS *********
//*******************************

void PrintFrog(OBJ* ob){
    wattron(ob->win->window, COLOR_PAIR(ob->color));
    mvwprintw(ob->win->window, ob->y, ob->x, "F");
    wattroff(ob->win->window, COLOR_PAIR(ob->color));
    wrefresh(ob->win->window);
}

OBJ* InitFrog(WIN* w, int col, int lives) {
    OBJ* ob = (OBJ*)malloc(sizeof(OBJ));
    if (ob == NULL) {
        fprintf(stderr, "Error creating FROG.\n");
        exit(EXIT_FAILURE);
    }
    ob ->color = col;
    ob ->win = w;
    ob->width = 1;
    ob->height = 1;
    ob -> mv = 0;
    ob -> x = START_X;
    ob -> y = START_Y;
    ob ->xmin = 1;
    ob ->xmax = w->cols-2;
    ob->ymin = 1;
    ob->ymax = w->rows-2;
    ob->lives = lives;
    ob->last_jump = clock();

    PrintFrog(ob);
    return ob;
}
void Show(OBJ* ob, int dx, int dy) {

    if(ob->y == ob->win->rows/2) { //Gdy zaba znajduje sie w safe area
        wattron(ob->win->window, COLOR_PAIR(SAFE_AREA));
        mvwprintw(ob->win->window, ob->y, ob->x,"-");
        wattroff(ob->win->window, COLOR_PAIR(SAFE_AREA));
    }else {
        mvwprintw(ob->win->window, ob->y, ob->x, " "); // Wyczyść poprzednią pozycję
    }

    // Zaktualizuj pozycję
    ob->x += dx;
    ob->y += dy;

    // Upewnij się, że żaba nie wychodzi poza granice
    if (ob->x < ob->xmin) ob->x = ob->xmin;
    if (ob->x > ob->xmax) ob->x = ob->xmax;
    if (ob->y < ob->ymin) ob->y = ob->ymin;
    if (ob->y > ob->ymax) ob->y = ob->ymax;

    PrintFrog(ob);
}

void moveFrog(OBJ* ob, char ch) {
    clock_t current_time = clock(); //mikrosekundy
    if ((current_time - ob->last_jump) >= TIME_BETWEEN_JUMPS) {

        switch (ch) {
            case 'w': Show(ob, 0, -1); break;
            case 's': Show(ob, 0,1); break;
            case 'a': Show(ob, -1,0); break;
            case 'd': Show(ob, 1,0); break;
        }
    }

    ob->last_jump = current_time;
}

void resetFrogAfterCollision(OBJ* ob) {
    mvwprintw(ob->win->window, ob->y, ob->x, " ");
    ob->x = START_X;
    ob->y = START_Y;
    PrintFrog(ob);
}
