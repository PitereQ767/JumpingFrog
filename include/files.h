//
// Created by 48734 on 05.12.2024.
//

#ifndef FILES_H
#define FILES_H
#include "config.h"

void SaveScore(const char* filename, const char* name, int score);
int LoadScores(const char* filename, Score* scores, int max_scores);
void SortScores(Score* scores, int count);
void ShowRanking(WINDOW *win, const Score* scores, int count);
void loadSettings(const char* filename, int *lives, int *game_time, int *window_width, int *window_height);

#endif //FILES_H
