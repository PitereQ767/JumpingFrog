#include "../include/config.h"
#include "../include/window.h"
#include "../include/timer.h"
#include "../include/frog.h"
#include "../include/hole.h"
#include "../include/obstacle.h"
#include "../include/friendlyCars.h"
#include "../include/files.h"
#include "../include/stork.h"


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

void freeMemory(Obstacles** obstacle, OBJ* frog, TIMER* timer, Holes** holes, Stork* stork) {
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
    if(stork != NULL) {
        free(stork);
        stork = NULL;
    }
}

void freeWin(WIN* win) {
    if (win != NULL) {
        delwin(win->window);
        free(win);
    }
}

void DrawLine(WIN* W) {
    for(int i = 1; i<W->cols-1; i++) {
        wattron(W->window, COLOR_PAIR(SAFE_AREA));
        mvwprintw(W->window, W->rows/2, i, "-");
        wattroff(W->window, COLOR_PAIR(SAFE_AREA));
    }
    wrefresh(W->window);
}

void DrawFinishLine(WIN* W) {
    for(int i = 1; i < W->cols -1 ; i++) { // Zaczynamy od jeden aby nie rysowac w rogach
        wattron(W->window, COLOR_PAIR(FINISH_LINE));
        mvwprintw(W->window, 1, i, " ");
        mvwprintw(W->window, 1, 36, "FINISH");
        wattroff(W->window, COLOR_PAIR(FINISH_LINE));
    }
    wrefresh(W->window);
}


//*******************************
//********* MAIN LOOP ***********
//*******************************

int mainLoop(WIN* status, OBJ* frog, TIMER* timer, Obstacles** obstacles, Holes** holes, FriendlyCars** cars, Stork* stork, int pass_time) {
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
                frog->lives -= 1;

                if(frog->lives == 0) {
                    return 3;
                }
                resetFrogAfterCollision(frog);
                wrefresh(status->window);
            }
        }

        for(int i = 0; i<NUM_HOLES; i++) {
            if(FrogInHole(frog, holes)) {
                frog->lives -= 1;
                if(frog->lives == 0) {
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
        MoveStork(stork, frog);
        if(CheckStorkCollision(stork, frog)) {
            frog->lives -= 1;
            if(frog->lives == 0) {
                return 3;
            }
            resetFrogAfterCollision(frog);
        }
        if (UpdateTimer(timer, status, frog, pass_time)) {
            usleep(FRAME_TIME * 1000); //przekształcamy na mikrosekundy
            return 1;
        }
    }

    return 0;
}

void endGameFile(TIMER* timer, WINDOW* mainwin,OBJ* frog, char player_name[]) {
    const char* score_file = "ranikng.txt";
    Score scores[100];

    if ((int)timer->pass_time >= 0 && frog->lives > 0) {
        SaveScore(score_file, player_name, (int)timer->pass_time);
        int score_count = LoadScores(score_file, scores, MAX_SCORES);
        if (score_count > 0) {
            SortScores(scores, score_count);
            ShowRanking(mainwin, scores, score_count);
        }else {
            printf("There is no scores left!");
        }
    }
    else if((int)timer->pass_time>=0 && frog->lives == 0) {
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

void InitGame(WIN* playwin, int game_time, int lives, TIMER** timer, OBJ** frog, Holes*** holes, Obstacles*** obstacles,
    FriendlyCars*** cars, Stork** stork) {

    *timer = InitTimer(game_time);
    *frog = InitFrog(playwin, FROG_COLOR, lives);
    *holes = GenerateHoles(playwin, (playwin)->rows/2); //SafeArea = playwin->rows/2
    *obstacles = GenerateObstacles(playwin, playwin->rows/2);
    *cars = GenerateFriendlyCars(playwin, playwin->rows/2);
    *stork = InitStork(playwin, playwin->cols/2, playwin->rows/2, STORK_HEIGHT, STORK_WIDTH, STORK_COLOR);
    DrawFinishLine(playwin);
    DrawLine(playwin);
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

    TIMER* timer = NULL;
    OBJ* frog = NULL;
    Holes** holes = NULL;
    Obstacles** obstacles = NULL;
    FriendlyCars** cars = NULL;
    Stork* stork = NULL;

    InitGame(playwin, game_time, lives, &timer, &frog, &holes, &obstacles, &cars, &stork);

    int result = mainLoop(statwin, frog, timer, obstacles, holes, cars, stork, game_time);
    endGame(result, timer, frog, mainwin, statwin, player_name);


    // Sprzątanie po grze
    freeMemory(obstacles, frog, timer, holes, stork);
    freeWin(playwin);
    freeWin(statwin);
    delwin(mainwin);
    endwin();
    refresh();
    return 0;
}
