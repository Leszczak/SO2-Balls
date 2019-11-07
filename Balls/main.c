#include "main.h"

int main()
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
            board[i][j]=" ";
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


    pthread_t thread1;
    const char *message1 = "Thread 1";
    int  iret1;

    iret1 = pthread_create( &thread1, NULL, do_stuff, (void*) message1);
    if(iret1)
    {
        fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
        exit(EXIT_FAILURE);
    }

    printf("pthread_create() for thread 1 returns: %d\n",iret1);

    pthread_join( thread1, NULL);
    
    exit(EXIT_SUCCESS);
}


void *move_ball(void* ptr)
{

}

void *print_state(void* ptr)
{

}

void *watch_for_end(void* ptr)
{

}

void *do_stuff (void *ptr)
{
    char *message;
    message = (char *) ptr;
    printf("%s \n", message);
    
}