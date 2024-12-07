//
// Created by 48734 on 05.12.2024.
//

#include "../include/config.h"
#include "../include/files.h"

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
    while(count < max_scores && fscanf(file, "%s %d", scores[count].name, &scores[count].score) == 2) { //jesli fscanf nie zwroci 2 => koniec pliku
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
