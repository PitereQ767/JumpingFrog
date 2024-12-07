#include "../include/timer.h"
#include "../include/config.h"

//*******************************
//**** TIMER FUNCTIONS **********
//*******************************

// Funkcja Sleep (opóźnienie w milisekundach)
void Sleep(unsigned int tui) {
    usleep(tui * 1000);  // przekształcamy na mikrosekundy (1 ms = 1000 μs)
}

// Funkcja wyświetlająca zegar
void ShowTimer(WIN* W, float pass_time, int lives) {
    mvwprintw(W->window, 1, 3, "Time left: %.2f", pass_time);
    mvwprintw(W->window, 1, 28, "Frog life: %d", lives);
    mvwprintw(W->window, 1, 50, "PIOTR RATKOWSKI 203285");
    wrefresh(W->window);

}


TIMER* InitTimer(int pass_time) {
    TIMER* timer = (TIMER*)malloc(sizeof(TIMER));
    if (timer == NULL) {
        fprintf(stderr, "Error creating TIMER.\n");
        exit(EXIT_FAILURE);
    }
    timer->frame_no = 1;
    timer->frame_time = FRAME_TIME; // Czas trwania jednej klatki (w ms)
    timer->pass_time = pass_time; // Czas na zegarze (w sekundach)
    return timer;
}

// Funkcja aktualizująca czas w grze
int UpdateTimer(TIMER* T, WIN* status, OBJ* ob, int pass_time) {
    // Zwiększamy numer klatki
    T->frame_no++;

    // Obliczamy pozostały czas w sekundach
    T->pass_time = pass_time - (T->frame_no * T->frame_time / 1000.0);

    // Jeśli pozostały czas jest mniejszy niż czas trwania jednej klatki, ustawiamy go na 0
    if (T->pass_time < (T->frame_time / 1000.0)) {
        T->pass_time = 0;
    } else {
        // Jeśli czas nie minął, wykonujemy opóźnienie (przeżywanie jednej klatki)
        Sleep(T->frame_time);
    }

    // Wyświetlamy pozostały czas
    ShowTimer(status, T->pass_time, ob->lives);

    // Jeśli czas minął, zwróć 1
    if (T->pass_time == 0) return 1;

    // Jeśli czas nie minął, zwróć 0
    return 0;
}