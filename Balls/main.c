#include "main.h"

int main(int argc, char *argv[])
{
    //isEnd
    pthread_mutex_init( &isEndMutex, NULL);
    isEnd = false;
    //board
    for(int i=0; i<SIZE_Y; i++)
    {
        for(int j=0; j<SIZE_X; j++)
        {
            pthread_mutex_init(&boardMutexes[i][j], NULL);
            board[i][j] = ' ';
        }
    }
    //balls
    for(int i=0; i<MAX_BALLS; i++)
    {
        pthread_mutex_init(&ballMutexes[i], NULL);
        balls[i].position_X = 0;
        balls[i].position_Y = 0;
        balls[i].direction_X = 0;
        balls[i].direction_Y = 0;
        balls[i].velocity_X = 0;
        balls[i].velocity_Y = 0;
        balls[i].move_progress_X = 0;
        balls[i].move_progress_Y = 0;
    }

    //create threads
    pthread_create(&printThread, NULL, print_state, NULL);
    pthread_create(&isEndThread, NULL, watch_for_end, NULL);
    for ( int i = 0; i < MAX_BALLS; i++) 
    {
        int *arg = malloc(sizeof(*arg));
        *arg = i;
        pthread_create(&ballThreads[i], NULL, move_ball, arg);
    }

    //join threads
    pthread_join(isEndThread, NULL);
    pthread_join(printThread, NULL);
    for(int i=0; i<MAX_BALLS; i++)
    {
        pthread_join(ballThreads[i], NULL);
    }

    //destroy mutexes
    pthread_mutex_destroy( &isEndMutex);
    for(int i=0; i<SIZE_Y; i++)
    {
        for(int j=0; j<SIZE_X; j++)
        {
            pthread_mutex_destroy(&boardMutexes[i][j]);
        }
    }
}


void *move_ball(void* ptr)
{
    int number = *((int *) ptr);
    free(ptr);

    while(!ifEnd())
    {
        

    }
}

void *print_state(void* ptr)
{
    //init ncurses
    initscr();
    noecho();
    curs_set(FALSE);

    //print frame
    for(int i=0; i<SIZE_X+2; i++)
    {
        mvaddch(0, i, '#');
        mvaddch(SIZE_Y+1, i, '#');
    }
    for(int i=0; i<SIZE_Y+2; i++)
    {
        mvaddch(i, 0, '#');
        mvaddch(i, SIZE_X+1, '#');
    }
    refresh();

    while(!ifEnd())
    {
        //clear board
        for(int i=0; i<SIZE_Y; i++)
            for(int j=0; j<SIZE_X; j++)
                mvaddch(i+1, j+1, ' ');

        //place balls on new positions
        for(int i=0; i<MAX_BALLS; i++)
        {
            pthread_mutex_lock(&ballMutexes[i]);
            if(balls[i].position_X >= 0 && balls[i].position_Y >= 0)
                mvaddch(balls[i].position_Y+1, balls[i].position_X+1, 'o');
            pthread_mutex_unlock(&ballMutexes[i]);
        }

        refresh();
        usleep(1000*100);
    }

    //end ncurses
    endwin();
}

void *watch_for_end(void* ptr)
{
    //reason to stop
    usleep(1000*1000*5);
    pthread_mutex_lock( &isEndMutex);
    isEnd = true;
    pthread_mutex_unlock( &isEndMutex);
}

bool ifEnd()
{
    bool result;
    pthread_mutex_lock( &isEndMutex);
    result = isEnd;
    pthread_mutex_unlock( &isEndMutex);
    return result;
}
