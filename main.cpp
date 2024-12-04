#include "config.h"

//*******************************
//**** WINDOW FUNCTIONS *********
//*******************************


// Funkcja inicjalizująca ncurses
WINDOW* Start() {
    WINDOW* win;

    if((win = initscr()) == NULL) {
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
    W->window = subwin(parent, rows, cols, y, x);
    if(W->window == NULL) {
        fprintf(stderr, "Error creating subwindow.\n");
        exit(EXIT_FAILURE);
    }
    CleanWin(W, border); // pierwsze wyczyszczenie okna z rysowaniem ramki
    if (delay == DELAY_OFF) nodelay(W->window, TRUE);
    wrefresh(W->window);
    return W;
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
    wrefresh(win);
    wgetch(win);
    wclear(win);
    wrefresh(win);
}

// Funkcja Sleep (opóźnienie w milisekundach)
void Sleep(unsigned int tui) {
    usleep(tui * 1000);  // przekształcamy na mikrosekundy (1 ms = 1000 μs)
}


//*******************************
//**** TIMER FUNCTIONS **********
//*******************************


// Funkcja wyświetlająca zegar
void ShowTimer(WIN* W, float pass_time, int life) {
    mvwprintw(W->window, 1, 3, "Time left: %.2f", pass_time);
    mvwprintw(W->window, 1, 28, "Frog life: %d", life);
    mvwprintw(W->window, 1, 50, "PIOTR RATKOWSKI 203285");
    wrefresh(W->window);

}

// Funkcja inicjalizująca timer
TIMER* InitTimer(WIN* status) {
    TIMER* timer = (TIMER*)malloc(sizeof(TIMER));
    if (timer == NULL) {
        fprintf(stderr, "Error creating TIMER.\n");
        exit(EXIT_FAILURE);
    }
    timer->frame_no = 1;
    timer->frame_time = FRAME_TIME; // Czas trwania jednej klatki (w ms)
    timer->pass_time = PASS_TIME; // Czas na zegarze (w sekundach)
    return timer;
}

// Funkcja aktualizująca czas w grze
int UpdateTimer(TIMER* T, WIN* status, OBJ* ob) {
    // Zwiększamy numer klatki
    T->frame_no++;

    // Obliczamy pozostały czas w sekundach
    T->pass_time = PASS_TIME - (T->frame_no * T->frame_time / 1000.0);

    // Jeśli pozostały czas jest mniejszy niż czas trwania jednej klatki, ustawiamy go na 0
    if (T->pass_time < (T->frame_time / 1000.0)) {
        T->pass_time = 0;
    } else {
        // Jeśli czas nie minął, wykonujemy opóźnienie (przeżywanie jednej klatki)
        Sleep(T->frame_time);
    }

    // Wyświetlamy pozostały czas
    ShowTimer(status, T->pass_time, ob->life);

    // Jeśli czas minął, zwróć 1
    if (T->pass_time == 0) return 1;

    // Jeśli czas nie minął, zwróć 0
    return 0;
}

//*******************************
//****** FROG FUNCTIONS *********
//*******************************

void PrintFrog(OBJ* ob){
    wattron(ob->win->window, COLOR_PAIR(ob->color));
    mvwprintw(ob->win->window, ob->y, ob->x, "F");  // Zakładamy, że "F" oznacza żabę
    wattroff(ob->win->window, COLOR_PAIR(ob->color)); // Wyłączamy kolor
    wrefresh(ob->win->window);  // Odśwież okno, aby pokazać zmiany
}

OBJ* InitFrog(WIN* w, int col) {
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
    ob->life = LIFE;
    ob->last_jump = time(NULL);

    PrintFrog(ob);
    return ob;
}
void Show(OBJ* ob, int dx, int dy) {

    if(ob->y == ROWS/2) { //Gdy zaba znajduje sie w safe area
        wattron(ob->win->window, COLOR_PAIR(SAFE_AREA));
        mvwprintw(ob->win->window, ob->y, ob->x,"-");
        wattroff(ob->win->window, COLOR_PAIR(SAFE_AREA));
    }else {
        // Rysujemy żabę w starej pozycji
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

    // Rysujemy żabę w nowej pozycji
    PrintFrog(ob);
}

void moveFrog(OBJ* ob, char ch) {
    time_t current_time = time(NULL);
    if (difftime(current_time, ob->last_jump) >= TIME_BETWEEN_JUMPS) {

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

//*******************************
//***** OBSTACLE FUNCTIONS ******
//*******************************

void PrintObstacle(OBJ *ob) {
        for (int i = 0; i<ob->width; i++) {
            wattron(ob->win->window, COLOR_PAIR(ob->color));
            mvwprintw(ob->win->window, ob->y, ob->x + i, "X");
            wattroff(ob->win->window, COLOR_PAIR(ob->color)); // Wyłączamy kolor
        }
    box(ob->win->window, 0, 0); // Samochody nie nadpisuja ramki
    wrefresh(ob->win->window);  // Odśwież okno, aby pokazać zmiany
}

OBJ* InitObstacle(WIN* w, int x, int y, int color) {
    OBJ* ob = (OBJ*)malloc(sizeof(OBJ));
    if (ob == NULL) {
        fprintf(stderr, "Error creating OBSTACLE.\n");
        exit(EXIT_FAILURE);
    }
    ob ->color = color;
    ob ->win = w;
    ob -> x = x;
    ob -> y = y;
    ob->width = (rand() % 13) + 3;
    ob->height = 1;
    ob->xmin = 1;
    ob->xmax = w->cols-2;
    ob->ymin = 1;
    ob->ymax = w->rows-2;
    ob->speed;

    PrintObstacle(ob);

    return ob;
}

OBJ* obstacle[NUM_OBSTACLES]; // wszytskie przeszkody



int CheckCollision(OBJ* frog, OBJ* obstacle) {
    for(int i = 0; i< obstacle->width; i++) {
        if(frog->x == obstacle->x + i && frog->y == obstacle->y) {
            return 1;
        }
    }
    return 0;
}

int isLineOccupied(OBJ** obstacle, int y) {
    for(int i = 0; i<NUM_OBSTACLES; i++) {
        if(obstacle[i] != NULL && obstacle[i]->y == y) {
            return 1;
        }
    }
    return 0;
}

void GenerateObstacles(WIN* win) {
    for(int i = 0; i<NUM_OBSTACLES; i++) {
        // int x = COLS; //Ustawienie przeszkody poza prawa krawedzia
        int x = rand() % (COLS - 2) + 1;
        int y = rand() % (ROWS - 2) + 1;
        while(y == START_Y || y ==1 || y==ROWS/2 || isLineOccupied(obstacle, y)) { //ROWS/2 - safe area
            y = rand() % (ROWS - 2) + 1;
        }
        int speed = SPEED;
        obstacle[i] = InitObstacle(win, x, y, OBSTACLE_COLOR);
        obstacle[i]->speed = speed;
    }
}

void MoveObstacles(OBJ** obstacle, int numObstacles) {
    for (int i = 0; i < numObstacles; i++) {
        if(obstacle[i]->speed == 0) {
            continue;
        }

         for(int j = 0; j<obstacle[i]->width; j++) {
             mvwprintw(obstacle[i]->win->window, obstacle[i]->y, obstacle[i]->x + j, " ");
         }
        // Przesuwamy przeszkodę w lewo
        obstacle[i]->x -= obstacle[i]->speed;

        // Jeśli przeszkoda wychodzi poza lewą krawędź, przenieś ją na prawą stronę
        if (obstacle[i]->x + obstacle[i]->width < obstacle[i]->xmin) {
            obstacle[i]->x = obstacle[i]->xmax;  // Nowa pozycja X (po prawej stronie)
            int newY = rand() % (ROWS - 2)+1; // Losowa pozycja Y
            while(newY == START_Y || newY ==1 || newY == ROWS/2 || isLineOccupied(obstacle, newY)) {
                newY = rand() % (ROWS - 2) + 1;
            }
            obstacle[i]->y = newY;
        }

        PrintObstacle(obstacle[i]);  // Rysuj przeszkodę w nowej pozycji
    }

    wrefresh(obstacle[0]->win->window);  // Odśwież okno, aby pokazać zmiany
}

void stopObstacle(OBJ** obstacle, OBJ* frog) {
    for(int i = 0; i<NUM_OBSTACLES; i++) {
        if(obstacle[i]->y+1 == frog->y && obstacle[i]->x >= frog->x && obstacle[i]->x < frog->x + frog->width) {
            if(rand()%100 < CHANCE_OF_STOP) {
                obstacle[i]->speed = 0; // szansa na zatrzymanie przeszkody, gdy zaba jest blisko
            }
        }
        if(obstacle[i]->speed == 0) {
            if (difftime(time(NULL), obstacle[i]->stop_time) >=3) {
                obstacle[i]->speed = SPEED;
            }
        }else {
            obstacle[i]->stop_time = time(NULL);
        }
    }
}


//*******************************
//******* GAME FUNCTIONS ********
//*******************************

void endGame(const char* info, WIN* W) {
    CleanWin(W, 1);
    for(int i = 3; i>0; i--) {
        mvwprintw(W->window, 1, 2, "%s Closing the game in %d seconds...", info, i);
        wrefresh(W->window);
        sleep(1);
    }
}

void freeMemory(OBJ** obstacle, OBJ* frog, TIMER* timer) {
    if (frog != NULL) {
        free(frog);
        frog = NULL;
    }
    for(int i = 0; i < NUM_OBSTACLES; i++) {
        if (obstacle[i] != NULL) {
            delwin(obstacle[i]->win->window);
            free(obstacle[i]);
            obstacle[i] = NULL;
        }
    }

    if(timer != NULL) {
        free(timer);
        timer = NULL;
    }
}

void freeWin(WIN* win) {
    if (win != NULL) {
        delwin(win->window);
        free(win);
    }
}

void DrawLine(WIN* W) {
    for(int i = 1; i < W->cols -1 ; i++) { // Zaxzynamy od jeden aby nie rysowac w rogach
        wattron(W->window, COLOR_PAIR(FINISH_LINE));
        mvwprintw(W->window, 1, i, " ");
        mvwprintw(W->window, 1, 36, "FINISH");
        wattroff(W->window, COLOR_PAIR(FINISH_LINE));
    }
    for(int i = 1; i<W->cols-1; i++) {
        wattron(W->window, COLOR_PAIR(SAFE_AREA));
        mvwprintw(W->window, ROWS/2, i, "-");
        wattroff(W->window, COLOR_PAIR(SAFE_AREA));
    }
    wrefresh(W->window);
}

//*******************************
//******* FILE FUNCTIONS ********
//*******************************

void SaveScore(const char* filename, const char* name, int score) {
    FILE* file = fopen(filename, "a");
    if (file == NULL) {
        fprintf(stderr, "Could not open file %s\n", filename);
        return;
    }
    fprintf(file, "%s %d\n", name, score);
    fclose(file);
}

int LoadScores(const char* filename, Score* scores, int max_scores) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open file %s\n", filename);
        return 0;
    }
    int count = 0;
    while(count < max_scores && fscanf(file, "%s %d", scores[count].name, &scores[count].score) == 2) {
        count++;
    }
    fclose(file);
    return count;
}

void SortScores(Score* scores, int count) {
    for (int i = 0; i<count -1; i++) {
        for(int j = 0; j<count-i-1; j++) {
            if(scores[j].score > scores[j+1].score) {
                Score temp = scores[j];
                scores[j] = scores[j+1];
                scores[j+1] = temp;
            }
        }
    }
}

void ShowRanking(WINDOW *win, const Score* scores, int count) {
    wclear(win);
    mvwprintw(win, 1, 1, "RANKING:");
    for(int i =0; i<count; i++) {
        mvwprintw(win, i+2, 1, "%d. %s - %d", i+1, scores[i].name, scores[i].score);
    }
    mvwprintw(win, count + 3, 1, "Press any key to continue...");
    wrefresh(win);
    wgetch(win);
}

int mainLoop(WIN* status, OBJ* frog, TIMER* timer) {
    int ch;

    while((ch = wgetch(status->window)) != QUIT) {
        if (ch == ERR) {
            ch = NOKEY;
        }
        else {
            moveFrog(frog, ch);
        }

        if(frog->y == 1) {
            return 2;
        }

        for(int i = 0; i<NUM_OBSTACLES; i++) {
            if(CheckCollision(frog, obstacle[i])){
                frog->life -= 1;

                if(frog->life == 0) {
                    return 3;
                }
                resetFrogAfterCollision(frog);
                wrefresh(status->window);
            }
        }

        MoveObstacles(obstacle, NUM_OBSTACLES);
        stopObstacle(obstacle, frog);

        if (UpdateTimer(timer, status, frog)) {
            usleep(FRAME_TIME * 1000);
            return 1;
        }
    }

    return 0;
}

void endGameFile() {

}


// Główna funkcja
int main() {
    char player_name[50];
    WINDOW* mainwin = Start();
    StartWin(mainwin, player_name, sizeof(player_name));

    WIN* playwin = Init(mainwin, ROWS, COLS, OFFY, OFFX, MAIN_COLOR, BORDER, DELAY_ON); // Gra
    WIN* statwin = Init(mainwin, 3, COLS, ROWS + OFFY, OFFX, STAT_COLOR, BORDER, DELAY_OFF); // Status

    TIMER* timer = InitTimer(statwin);

    OBJ* frog = InitFrog(playwin, FROG_COLOR);
    GenerateObstacles(playwin);

    DrawLine(playwin);

    int result = mainLoop(statwin, frog, timer);

    if (result == 0) { // Gra zakończona przez gracza
        endGame(" ", statwin);
    } else if (result == 1) { // Gra zakończona przez upłynięcie czasu
        endGame("Time is up! Game over.", statwin);
    } else if (result == 2) {
        endGame("You win!", statwin);
    } else if(result == 3) {
        endGame("The frog was run over!", statwin);
    }

    const char* score_file = "ranikng.txt";

    Score scores[100];
    SaveScore(score_file, player_name, (int)timer->pass_time);
    int score_count = LoadScores(score_file, scores, 100);
    if (score_count > 0) {
        SortScores(scores, score_count);
        ShowRanking(mainwin, scores, score_count);
    }else {
        printf("There is no scores left!");
    }


    // Sprzątanie po grze
    freeMemory(obstacle, frog, timer);
    freeWin(playwin);
    freeWin(statwin);
    delwin(mainwin);
    endwin();
    refresh();
    return 0;
}
