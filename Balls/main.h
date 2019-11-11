#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ncurses.h>
#include <time.h>

//max number of balls present at once
#define MAX_BALLS       5
//average time to spawn ball +-0.5
#define ADD_INTERVAL    3000 //ms
//horisontal size of playable board (+1 for each border, not counted in)
#define SIZE_X          99
//vartical size of playable board (+1 for each border, not counted in)
#define SIZE_Y          30
//how much slower/faster (going up/down) ball gets every 'tick' 
#define GRAVITY         1
//how much points must ball collect to move further
//each 'tick' ball gets it's velocity
#define SUM_TO_MOVE     100

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