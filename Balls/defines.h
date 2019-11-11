//max number of balls present at once
#define MAX_BALLS       15
//average time to spawn ball +-0.5
#define ADD_INTERVAL    5000 //ms
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