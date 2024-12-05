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
    init_pair(HOLE_COLOR, COLOR_CYAN, COLOR_CYAN);
    init_pair(FRIENDLY_CAR_COLOR, COLOR_YELLOW, COLOR_YELLOW);

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
TIMER* InitTimer(WIN* status, int PASS_TIME) {
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
int UpdateTimer(TIMER* T, WIN* status, OBJ* ob, int PASS_TIME) {
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
    ob->life = lives;
    ob->last_jump = time(NULL);

    PrintFrog(ob);
    return ob;
}
void Show(OBJ* ob, int dx, int dy) {

    if(ob->y == ob->win->rows/2) { //Gdy zaba znajduje sie w safe area
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
        int x = rand() % (win->cols - 3) + 1;
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

//*******************************
//***** OBSTACLE FUNCTIONS ******
//*******************************

void PrintObstacle(Obstacles *obstacle) {
        for (int i = 0; i<obstacle->width; i++) {
            wattron(obstacle->win->window, COLOR_PAIR(obstacle->color));
            mvwprintw(obstacle->win->window, obstacle->y, obstacle->x + i, "X");
            wattroff(obstacle->win->window, COLOR_PAIR(obstacle->color)); // Wyłączamy kolor
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
        // if (obstacle->speed == 0) {
        //     continue;
        // }
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
            if (difftime(time(NULL), obstacle[i]->stop_time) >=3) {
                obstacle[i]->speed = SPEED;
            }
        }else {
            obstacle[i]->stop_time = time(NULL);
        }
    }
}

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




//*******************************
//******* GAME FUNCTIONS ********
//*******************************

void EndGameStat(const char* info, WIN* W) {
    CleanWin(W, 1);
    for(int i = 3; i>0; i--) {
        mvwprintw(W->window, 1, 2, "%s Closing the game in %d seconds...", info, i);
        wrefresh(W->window);
        sleep(1);
    }
}

void freeMemory(Obstacles** obstacle, OBJ* frog, TIMER* timer, Holes** holes) {
    if (frog != NULL) {
        free(frog);
        frog = NULL;
    }
    for(int i = 0; i < NUM_OBSTACLES; i++) {
        if (obstacle[i] != NULL) {
            free(obstacle[i]);
            obstacle[i] = NULL;
        }
    }

    for(int i = 0; i < NUM_HOLES; i++) {
        if (holes[i] != NULL) {
            free(holes[i]);
            holes[i] = NULL;
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
        mvwprintw(W->window, W->rows/2, i, "-");
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
            if(scores[j].score < scores[j+1].score) {
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

void loadSettings(const char* filename, int *lives, int *game_time, int *window_width, int *window_height) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Błąd otwierania pliku: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    while(!feof(file)) {
        char key[50];
        int value;
        int res = fscanf(file, "%49[^=]=%d\n", key, &value);
        if(res == 2) {
            if(strcmp(key, "lives") == 0) {
                *lives = value;
            }
            else if(strcmp(key, "game_time") == 0) {
                *game_time = value;
            }
            else if(strcmp(key, "window_width") == 0) {
                *window_width = value;
            }
            else if(strcmp(key, "window_height") == 0) {
                *window_height = value;
            }
        }
    }
    fclose(file);
}

//*******************************
//********* MAIN LOOP ***********
//*******************************

int mainLoop(WIN* status, OBJ* frog, TIMER* timer, int PASS_TIME, Obstacles** obstacles, Holes** holes, FriendlyCars** cars) {
    int ch;
    int key = 0;

    while((ch = wgetch(status->window)) != QUIT) {
        if (ch == ERR) {
            ch = NOKEY;
        }
        else {
            moveFrog(frog, ch);
        }

        if(ch == 'z') {
            key = 'z';
        }else {
            key = 0;
        }

        if(frog->y == 1) {
            return 2;
        }

        for(int i = 0; i<NUM_OBSTACLES; i++) {
            if(CheckCollision(frog, obstacles)) {
                frog->life -= 1;

                if(frog->life == 0) {
                    return 3;
                }
                resetFrogAfterCollision(frog);
                wrefresh(status->window);
            }
        }

        for(int i = 0; i<NUM_HOLES; i++) {
            if(FrogInHole(frog, holes)) {
                frog->life -= 1;
                if(frog->life == 0) {
                    return 3;
                }
                resetFrogAfterCollision(frog);
                wrefresh(status->window);
            }
        }

        MoveObstacles(obstacles, frog->win->rows, holes);
        stopObstacle(obstacles, frog);
        MoveFriendlyCars(cars, frog->win->rows, holes, frog, &key);

        for(int i = 0; i<NUM_HOLES; i++) {
            PrintHole(holes[i]);
        }

        if (UpdateTimer(timer, status, frog, PASS_TIME)) {
            usleep(FRAME_TIME * 1000);
            return 1;
        }
    }

    return 0;
}

void endGameFile(TIMER* timer, WINDOW* mainwin,OBJ* frog, char player_name[]) {
    const char* score_file = "ranikng.txt";
    Score scores[100];

    if ((int)timer->pass_time >= 0 && frog->life > 0) {
        SaveScore(score_file, player_name, (int)timer->pass_time);
        int score_count = LoadScores(score_file, scores, MAX_SCORES);
        if (score_count > 0) {
            SortScores(scores, score_count);
            ShowRanking(mainwin, scores, score_count);
        }else {
            printf("There is no scores left!");
        }
    }
    else if((int)timer->pass_time>=0 && frog->life == 0) {
        SaveScore(score_file, player_name, 0);
        int score_count = LoadScores(score_file, scores, MAX_SCORES);
        if (score_count > 0) {
            SortScores(scores, score_count);
            ShowRanking(mainwin, scores, score_count);
        }else {
            printf("There is no scores left!");
        }
    }

}

void endGame(int result, TIMER* timer, OBJ* frog, WINDOW* mainwin, WIN* statwin, char player_name[]) {
    if (result == 0) { // Gra zakończona przez gracza
        EndGameStat(" ", statwin);
    } else if (result == 1) { // Gra zakończona przez upłynięcie czasu
        EndGameStat("Time is up! Game over.", statwin);
        endGameFile(timer, mainwin, frog, player_name);
    } else if (result == 2) {
        EndGameStat("You win!", statwin);
        endGameFile(timer, mainwin, frog, player_name);
    } else if(result == 3) {
        EndGameStat("The frog was run over!", statwin);
        endGameFile(timer, mainwin, frog, player_name);
    }
}


// Główna funkcja
int main() {
    srand(time(NULL));
    char player_name[50];
    WINDOW* mainwin = Start();
    StartWin(mainwin, player_name, sizeof(player_name));

    int lives, game_time, window_width, window_height;
    loadSettings("ustawienia.txt", &lives, &game_time, &window_width, &window_height);

    WIN* playwin = Init(mainwin, window_height, window_width, OFFY, OFFX, MAIN_COLOR, BORDER, DELAY_ON); // Gra
    WIN* statwin = Init(mainwin, 3, window_width, window_height + OFFY, OFFX, STAT_COLOR, BORDER, DELAY_OFF); // Status

    TIMER* timer = InitTimer(statwin, game_time);

    OBJ* frog = InitFrog(playwin, FROG_COLOR, lives);

    Holes** holes = GenerateHoles(playwin, playwin->rows/2);
    Obstacles** obstacles = GenerateObstacles(playwin, playwin->rows/2);
    FriendlyCars** cars = GenerateFriendlyCars(playwin, playwin->rows/2);


    DrawLine(playwin);

    int result = mainLoop(statwin, frog, timer, game_time, obstacles, holes, cars);
    endGame(result, timer, frog, mainwin, statwin, player_name);


    // Sprzątanie po grze
    freeMemory(obstacles, frog, timer, holes);
    freeWin(playwin);
    freeWin(statwin);
    delwin(mainwin);
    endwin();
    refresh();
    return 0;
}
