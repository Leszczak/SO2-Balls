//max number of balls present at once
#define MAX_BALLS       10
//time to spawn next ball [ms]
#define ADD_INTERVAL    1000 //ms
//horisontal size of playable board (+1 for each border, not counted in)
#define SIZE_X          99
//vartical size of playable board (+1 for each border, not counted in)
#define SIZE_Y          40
//how much slower/faster (going up/down) ball gets every 'tick' 
#define GRAVITY         1
//how much points must ball collect to move further
//each 'tick' ball gets it's velocity
#define SUM_TO_MOVE     100
//max starting velocity of balls
#define MAX_START_VELOCITY  100
//should program show ball stats? (true/false)
#define ifShowStats     false
//should program show frame stats? (true/false)
#define ifShowFrameStats     true

//max number of trapped balls
#define MAX_TRAPPED_BALLS   3
//frame top Y possition
#define FRAME_Y             10
//frame height
#define FRAME_HEIGHT        10
//frame width
#define FRAME_WIDTH         40
//time between frame moves [ms]
#define FRAME_SPEED         1000*250 //4 pixels/s
//trap time for ball [s]
#define MAX_TRAP_TIME       3 //s
