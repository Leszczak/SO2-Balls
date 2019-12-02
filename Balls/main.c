#include "main.h"

//compile & start with:
// gcc -o balls defines.h main.h main.c -lncurses -lpthread && ./balls

int main(int argc, char *argv[])
{
    setup_and_start();
}

void setup_and_start()
{
    //for randomness
    srand(time(NULL));

    //isEnd & frame
    isEnd = false;
    pthread_mutex_init( &isEndMutex, NULL);
    frame_X = 10;
    trappedBallsNum = 0;
    pthread_mutex_init( &frameMutex, NULL);
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
        balls[i].position_X = -1;
        balls[i].position_Y = -1;
        balls[i].direction_X = 0;
        balls[i].direction_Y = 0;
        balls[i].velocity_X = 0;
        balls[i].velocity_Y = 0;
        balls[i].move_progress_X = 0;
        balls[i].move_progress_Y = 0;
        balls[i].canStart = false;
        balls[i].isTrapped = false;
        balls[i].wasTrapped = false;
    }
    //destructables
    for(int i=0; i<SIZE_X; i++)
        for(int j=0; j<DESTRUCTABLE_Y; j++)
        {
            destructable[i][j] = MAX_DESTRUCTABLE_VALUE;
            pthread_mutex_init(&destructableMutexes[i][j], NULL);
        }

    //create threads
    pthread_create(&printThread, NULL, print_state, NULL);
    for ( int i = 0; i < MAX_BALLS; i++) 
    {
        int *arg = malloc(sizeof(*arg));
        *arg = i;
        pthread_create(&ballThreads[i], NULL, move_ball, arg);
    }
    pthread_create(&isEndThread, NULL, watch_for_end, NULL);
    pthread_create(&ballStarterThread, NULL, balls_starter, NULL);
    pthread_create(&frameThread, NULL, frame_mover, NULL);

    //join threads
    pthread_join(isEndThread, NULL);
    pthread_join(printThread, NULL);
    for(int i=0; i<MAX_BALLS; i++)
    {
        pthread_join(ballThreads[i], NULL);
    }
    pthread_join(ballStarterThread, NULL);
    pthread_join(frameThread, NULL);

    //destroy mutexes
    pthread_mutex_destroy( &isEndMutex);
    for(int i=0; i<SIZE_Y; i++)
    {
        for(int j=0; j<SIZE_X; j++)
        {
            pthread_mutex_destroy(&boardMutexes[i][j]);
        }
    }
    for(int i=0; i<SIZE_X; i++)
        for(int j=0; j<DESTRUCTABLE_Y; j++)
            pthread_mutex_init(&destructableMutexes[i][j], NULL);
}

void *move_ball(void* ptr)
{
    //get number of corresponding ball
    int n = *((int *) ptr);
    free(ptr);

    while(!ifEnd())
    {
        //don't move if is trapped
        if(balls[n].isTrapped)
        {
            //for checks/s if free
            //or frees itself after MAX_TRAP_TIME s
            for(int i=0; i<MAX_TRAP_TIME*4; i++)
            {
                usleep(1000*250);
                //if locked sleep, else wake up
                pthread_mutex_lock(&ballMutexes[n]);
                if(!balls[n].isTrapped)
                {
                    break;
                }
                pthread_mutex_unlock(&ballMutexes[n]);
            }
            balls[n].isTrapped = false;
            balls[n].wasTrapped = true; 
            pthread_mutex_lock(&frameMutex);
            trappedBallsNum--;
            pthread_mutex_unlock(&frameMutex);   
            pthread_mutex_unlock(&ballMutexes[n]);        
        }
        else
            //25 checks per second
            usleep(1000*40); 
        
        pthread_mutex_lock(&ballMutexes[n]);
        
        int x = balls[n].position_X;
        int dir_x = balls[n].direction_X; 
        int y = balls[n].position_Y;
        int dir_y = balls[n].direction_Y;
        int next_x = balls[n].position_X + balls[n].direction_X;
        int next_y = balls[n].position_Y + balls[n].direction_Y;
        //hit vertical
        if( x >= 0
            && x < SIZE_X
            && next_y >= 0
            && next_y < DESTRUCTABLE_Y)
        {
            pthread_mutex_lock(&destructableMutexes[x][next_y]);
            if(destructable[x][next_y] > 0)
            {
                destructable[x][next_y]--;
                balls[n].direction_Y *= -1;
            }
            pthread_mutex_unlock(&destructableMutexes[x][next_y]);
        }
        //hit horizontal
        if( y >= 0
            && y < DESTRUCTABLE_Y
            && next_x >= 0
            && next_x < SIZE_X)
        {
            pthread_mutex_lock(&destructableMutexes[next_x][y]);
            if(destructable[next_x][y] > 0)
            {
                destructable[next_x][y]--;
                balls[n].direction_X *= -1;
            }
            pthread_mutex_unlock(&destructableMutexes[next_x][y]);        
        }

        //remove ball from board if falling
        if(balls[n].position_Y >= SIZE_Y && balls[n].direction_Y == 1)
        {
            balls[n].position_X = -1;
            balls[n].position_X = -1;
            balls[n].move_progress_Y = 0;
            balls[n].move_progress_X = 0;
            balls[n].direction_Y = 0;
            balls[n].direction_X = 0;
            balls[n].velocity_Y = 0;
            balls[n].velocity_X = 0;
            balls[n].canStart = false;
            balls[n].wasTrapped = false;
            balls[n].isTrapped = false;
        }

        //reset ball
        if(balls[n].position_X == -1 && balls[n].canStart)
        {
            balls[n].wasTrapped = false;
            balls[n].isTrapped = false;
            balls[n].canStart = false;
            balls[n].move_progress_Y=0;
            balls[n].move_progress_X=0;
            balls[n].direction_Y = -1;
            balls[n].direction_X = ((rand() % 2) * 2) - 1;
            balls[n].position_Y = SIZE_Y;
            balls[n].position_X = rand() % SIZE_X;
            balls[n].velocity_Y = MAX_START_VELOCITY / 3 + rand() % MAX_START_VELOCITY *2 / 3;
            balls[n].velocity_X = MAX_START_VELOCITY / 3 + rand() % MAX_START_VELOCITY *2 / 3;
        }

        //start falling down
        if(balls[n].direction_Y == -1 && balls[n].velocity_Y == 0)
        {
            balls[n].direction_Y = 1;
        }

        //hit roof
        if(balls[n].direction_Y == -1 && balls[n].position_Y == 0)
        {
            balls[n].direction_Y = 1;
            balls[n].wasTrapped = false;
        }

        //move up/down
        balls[n].move_progress_Y += abs(balls[n].velocity_Y);
        if(balls[n].move_progress_Y - SUM_TO_MOVE >= 0)
        {
            balls[n].position_Y += balls[n].direction_Y;
            balls[n].move_progress_Y -= SUM_TO_MOVE;
        }
        balls[n].velocity_Y -= GRAVITY;

        //bounce from walls
        if(balls[n].position_X==0 && balls[n].direction_X ==-1)
        {
            balls[n].direction_X=1;
            balls[n].wasTrapped = false;
        }
        if(balls[n].position_X==SIZE_X-1 && balls[n].direction_X ==1)
        {
            balls[n].direction_X=-1;
            balls[n].wasTrapped = false;
        }

        //move left/right
        balls[n].move_progress_X += abs(balls[n].velocity_X);
        if(balls[n].move_progress_X - SUM_TO_MOVE >= 0)
        {
            balls[n].position_X += balls[n].direction_X;
            balls[n].move_progress_X -= SUM_TO_MOVE;
        }

        //if got trapped
        if(ifCought(n))
        {
            balls[n].isTrapped = true;
            pthread_mutex_lock(&frameMutex);
            trappedBallsNum++;
            pthread_mutex_unlock(&frameMutex);
        }

        pthread_mutex_unlock(&ballMutexes[n]);
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
    if(ifShowStats)
        mvprintw(SIZE_Y+2, 0, "Thread no. / X_pos / X_dir / X_vel / Y_pos / Y_dir / Y_vel \n");
    else if (ifShowFrameStats)
        mvprintw(SIZE_Y+2, 0, "Balls inside: ");
    refresh();

    //refresh +- 40fps
    while(!ifEnd())
    {
        //clear board
        for(int i=0; i<SIZE_Y; i++)
            for(int j=0; j<SIZE_X; j++)
                mvaddch(i+1, j+1, ' ');

        //draw frame
        pthread_mutex_lock(&frameMutex);
        //upper horisontal
        for(int i=0; i<=FRAME_WIDTH; i++)
            mvaddch(FRAME_Y, i+frame_X, '@');
        //lower horisontal
        for(int i=0; i<=FRAME_WIDTH; i++)
            mvaddch(FRAME_Y+FRAME_HEIGHT+1, i+frame_X, '@');
        //left vertical
        for(int i=0; i<=FRAME_HEIGHT; i++)
            mvaddch(i+FRAME_Y, frame_X, '@');
        //right vertical
        for(int i=0; i<=FRAME_HEIGHT+1; i++)
            mvaddch(i+FRAME_Y, frame_X+FRAME_WIDTH+1, '@');
        if(ifShowFrameStats && !ifShowStats)
            mvprintw(SIZE_Y+2, 14, "%d\n", trappedBallsNum);
        pthread_mutex_unlock(&frameMutex);

        //place balls on new positions and print coordinates
        for(int i=0; i<MAX_BALLS; i++)
        {
            pthread_mutex_lock(&ballMutexes[i]);

            if(balls[i].position_X >= 0 && balls[i].position_Y >= 0)
                mvaddch(balls[i].position_Y+1, balls[i].position_X+1, 'o');

            if(ifShowStats)
                mvprintw(SIZE_Y+3+i,
                    0,
                    "         %d /    %d /    %d /    %d /    %d /    %d /    %d \n",
                    i, 
                    balls[i].position_X, 
                    balls[i].direction_X, 
                    balls[i].velocity_X, 
                    balls[i].position_Y, 
                    balls[i].direction_Y, 
                    balls[i].velocity_Y);

            pthread_mutex_unlock(&ballMutexes[i]);
        }

        //show destructables
        for(int i=0; i<SIZE_X; i++)
            for(int j=0; j<DESTRUCTABLE_Y; j++)
            {
                pthread_mutex_lock(&destructableMutexes[i][j]);
                if(destructable[i][j] != 0)
                    mvaddch(j+1, i+1, (char) destructable[i][j] +'0');
                pthread_mutex_unlock(&destructableMutexes[i][j]);
            }
        
        refresh();
        usleep(1000*25);
    }

    //end ncurses
    endwin();
}

void *watch_for_end(void* ptr)
{
    //reason to stop
    usleep(1000*1000);
    getch();
    pthread_mutex_lock( &isEndMutex);
    isEnd = true;
    pthread_mutex_unlock( &isEndMutex);
}

void *balls_starter()
{
    while(!ifEnd())
    {
        usleep(1000*ADD_INTERVAL);
        for ( int i = 0; i < MAX_BALLS; i++) 
        {
            pthread_mutex_lock( &ballMutexes[i]);
            if(balls[i].position_X == -1 && !balls[i].canStart)
            {
                balls[i].canStart = true;
                pthread_mutex_unlock( &ballMutexes[i]);
                break;
            }
            pthread_mutex_unlock( &ballMutexes[i]);
        }
    }
}

void *frame_mover()
{
    int move = -1;    
    while(!ifEnd())
    {
        usleep(FRAME_SPEED);

        bool shouldFree = false;
        pthread_mutex_lock(&frameMutex);
        if(move == -1 && frame_X == 1)
            move = 1;
        else if (move == 1 && frame_X+FRAME_WIDTH+1>=SIZE_X)
            move = -1;
        frame_X+=move;

        if(trappedBallsNum>=3)
        {
            shouldFree=true;
        }
        pthread_mutex_unlock(&frameMutex);

        for(int i=0; i<MAX_BALLS; i++)
        {
            pthread_mutex_lock(&ballMutexes[i]);
            if(shouldFree)
                balls[i].isTrapped=false;
            else if(balls[i].isTrapped)
                balls[i].position_X+=move;
            pthread_mutex_unlock(&ballMutexes[i]);
        }
    }
}

bool ifEnd()
{
    bool result;
    pthread_mutex_lock( &isEndMutex);
    result = isEnd;
    pthread_mutex_unlock( &isEndMutex);
    return result;
}

//assumes locks of ball
//uses locks on frame
bool ifCought(int ballNum)
{
    //is ball safe?
    if(balls[ballNum].wasTrapped)
        return false;

    pthread_mutex_lock( &frameMutex);
    //is trap full?
    if(trappedBallsNum >= MAX_TRAPPED_BALLS)
    {
        pthread_mutex_unlock( &frameMutex);
        return false;
    }
    //is ball inside trap?
    if(ifInside(balls[ballNum].position_X, balls[ballNum].position_Y))
    {
        pthread_mutex_unlock( &frameMutex);
        return true;
    }
    pthread_mutex_unlock( &frameMutex);
    //if none from above - default false
    return false;
}

//assumes locks of ball and frame
bool ifInside(int X, int Y)
{
    if(Y >= FRAME_Y && Y < FRAME_Y + FRAME_HEIGHT    //Y
        && X >= frame_X && X < frame_X + FRAME_WIDTH)//X
        return true;
    return false;
}