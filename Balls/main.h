#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ncurses.h>
#include <time.h>
#include "defines.h"

struct Ball
{
    int position_X;
    int position_Y;
    int direction_X; //-1 for left, 1 for right
    int direction_Y; //-1 for up, 1 for down
    int velocity_X;
    int velocity_Y;
    int move_progress_X;
    int move_progress_Y;
};

volatile bool isEnd;
char board[SIZE_Y][SIZE_X];
struct Ball balls[MAX_BALLS];

pthread_mutex_t isEndMutex;
pthread_mutex_t boardMutexes[SIZE_Y][SIZE_X];
pthread_mutex_t ballMutexes[MAX_BALLS];

pthread_t isEndThread;
pthread_t printThread;
pthread_t ballThreads[MAX_BALLS];

int main(int argc, char *argv[]);
void setup_and_start();
void *move_ball(void*);
void *print_state(void*);
void *watch_for_end(void*);
bool ifEnd();