#include "../include/window.h"
#include "../include/config.h"

//*******************************
//**** WINDOW FUNCTIONS *********
//*******************************

// Funkcja inicjalizująca ncurses
WINDOW* Start() {
    WINDOW* win;

    if((win = initscr()) == NULL) { //inicjalizacja ncurses
        fprintf(stderr, "Error initializing ncurses.\n");
        exit(EXIT_FAILURE);
    }

    start_color();
    init_pair(MAIN_COLOR, COLOR_WHITE, COLOR_BLACK);
    init_pair(STAT_COLOR, COLOR_WHITE, COLOR_BLUE);
    init_pair(FROG_COLOR, COLOR_GREEN, COLOR_GREEN);
    init_pair(OBSTACLE_COLOR, COLOR_RED, COLOR_RED);
    init_pair(SAFE_AREA, COLOR_WHITE, COLOR_WHITE);
    init_pair(FINISH_LINE, COLOR_BLACK, COLOR_WHITE);
    init_pair(HOLE_COLOR, COLOR_CYAN, COLOR_CYAN);
    init_pair(FRIENDLY_CAR_COLOR, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(STORK_COLOR, COLOR_MAGENTA, COLOR_MAGENTA);

    noecho();
    curs_set(0);
    return win;
}

// Funkcja czyszcząca okno
void CleanWin(WIN* W, int bo ) {
    int i, j;
    wattron(W->window, COLOR_PAIR(W->color));
    if(bo) box(W->window, 0, 0); // Rysowanie ramki
    for (i = bo; i < W->rows - bo; i++) {
        for (j = bo; j < W->cols - bo; j++) {
            mvwprintw(W->window, i, j, " ");
        }
    }
}

// Funkcja inicjalizująca podokno
WIN* Init(WINDOW* parent, int rows, int cols, int y, int x, int color, int border, int delay) {
    WIN* W = (WIN*)malloc(sizeof(WIN));
    if (W == NULL) {
        fprintf(stderr, "Error creating WINDOW.\n");
        exit(EXIT_FAILURE);
    }
    W->x = x;
    W->y = y;
    W->rows = rows;
    W->cols = cols;
    W->color = color;
    W->window = subwin(parent, rows, cols, y, x); // tworzenie podokien
    if(W->window == NULL) {
        fprintf(stderr, "Error creating subwindow.\n");
        exit(EXIT_FAILURE);
    }
    CleanWin(W, border); // pierwsze wyczyszczenie okna z rysowaniem ramki
    if (delay == DELAY_OFF) nodelay(W->window, TRUE);
    wrefresh(W->window);
    return W;
}

void PrintIconObjects(WINDOW* win) {
    wattron(win, COLOR_PAIR(OBSTACLE_COLOR));
    mvwprintw(win, 10, 12, "OOOOO");
    wattroff(win, COLOR_PAIR(OBSTACLE_COLOR));
    mvwprintw(win, 10, 17, " - enemy car");
    wattron(win, COLOR_PAIR(FRIENDLY_CAR_COLOR));
    mvwprintw(win, 13, 12, "FFFFF");
    wattroff(win, COLOR_PAIR(FRIENDLY_CAR_COLOR));
    mvwprintw(win, 13, 17, " - friendly car");
    wattron(win, COLOR_PAIR(FRIENDLY_CAR_COLOR));
    mvwprintw(win, 15, 1, "FFFFF");
    wattroff(win, COLOR_PAIR(FRIENDLY_CAR_COLOR));
    mvwprintw(win, 15, 6, " + 'z' = moving to a new place");
    wattron(win, COLOR_PAIR(SAFE_AREA));
    mvwprintw(win, 10, 31, "SSSSS");
    wattroff(win, COLOR_PAIR(SAFE_AREA));
    mvwprintw(win, 10, 36, " - safe area");
    wattron(win, COLOR_PAIR(STORK_COLOR));
    mvwprintw(win, 17, 1, "SS");
    mvwprintw(win, 18, 1, "SS");
    mvwprintw(win, 19, 1, "SS");
    wattroff(win, COLOR_PAIR(STORK_COLOR));
    mvwprintw(win, 19, 3, " - Stork");

}

// Funkcja wyświetlająca ekran powitalny
void StartWin(WINDOW* win, char* player_name, int max_len) {
    mvwaddstr(win, 1, 1, "Welcome to JumpingFrog!");
    mvwaddstr(win, 2, 1, "Enter your name: ");
    wrefresh(win);
    echo();
    wgetnstr(win, player_name, max_len-1);
    noecho();
    wclear(win);
    mvwprintw(win, 1, 1, "Hello, %s! Good luck!", player_name);
    mvwprintw(win, 3, 1, "Instruction:");
    mvwprintw(win, 4, 1, "W - jump up");
    mvwprintw(win, 5, 1, "S - jump down");
    mvwprintw(win, 6, 1, "A - jump left");
    mvwprintw(win, 7, 1, "D - jump right");
    mvwprintw(win, 8, 1, "Q - quit");
    wattron(win, COLOR_PAIR(FROG_COLOR));
    mvwprintw(win, 10, 1, "F");
    wattroff(win, COLOR_PAIR(FROG_COLOR));
    mvwprintw(win, 10, 2, " - frog");
    wattron(win, COLOR_PAIR(HOLE_COLOR));
    mvwprintw(win, 12, 1, "HH");
    mvwprintw(win, 13, 1, "HH");
    wattroff(win, COLOR_PAIR(HOLE_COLOR));
    mvwprintw(win, 13, 3, " - hole");
    PrintIconObjects(win);
    wrefresh(win);
    wgetch(win);
    wclear(win);
    wrefresh(win);
}

