/***************************************************************************** 
Created by: Jash Shah 
Date Created: 25 Nov, 2025 
Last Modified: 5 Dec, 2025
*****************************************************************************/ 
 
#define AO_GAME 
 
#include "qpn_port.h" 
#include "bsp.h" 
#include "game.h" 
#include <stdio.h> 
#include "xparameters.h" 
#include "xil_cache.h" 
#include "xintc.h" 
#include "xtmrctr.h" 
#include "xtmrctr_l.h" 
#include "xil_printf.h" 
#include "xgpio.h" 
#include "xspi.h" 
#include "xspi_l.h" 
#include "lcd.h" 
#include <math.h>
 
void initVars(); 
void rotatePlayerClockwise();
void rotatePlayerAntiClockwise();
void moveForward(int lvl_no);
 
typedef struct GameTag  {               //Game State machine 
	QActive super; 
}  Game; 
 
/* Setup state machines */ 
/**********************************************************************/ 
static QState Game_initial(Game *me); 
static QState Title_Page(Game *me); 
static QState Home_Page(Game *me); 
static QState Running  (Game *me); 
static QState Result  (Game *me); 
 
Game AO_Game; 
 
volatile int custom_tick = 0; 
volatile int buttonPressed = 0; 
volatile int running   = 0; 
volatile int counter   = 0; 
volatile int encoder_twist = 0;	// +1 for ACW and 0 for CW 
volatile int gameCounter = 0;
volatile int spawnTimer = 0;

int gameDistance = 0;
int gameDifficulty = 0;
int lvl_no = 0; 
int game_no = 0; 
int game_win = 0; 
char buf[12]; // To Print "Level " + number 
int speed = 1000; 
int score = 0;
extern DinoPlayer dino;
extern Obstacle obstacles[MAX_OBSTACLES];

void Game_ctor(void)  { 
	Game *me = &AO_Game; 
	QActive_ctor(&me->super, (QStateHandler)&Game_initial); 
} 
 
 
QState Game_initial(Game *me) { 
    initLCD(); 
	return Q_TRAN(&Title_Page); 
} 
 
QState Title_Page(Game *me){ 
	switch (Q_SIG(me)){ 
		case Q_ENTRY_SIG: { 
		    initVars(); 
		    drawWelcomePage(); 
			return Q_HANDLED(); 
		} 
		case BUTTON: { 
			if (buttonPressed == 1) {game_no = 1;}
			else if(buttonPressed == 2) {game_no = 2;}
     		else if(buttonPressed == 3) {game_no = 3;}
	     	else if(buttonPressed == 4) {game_no = 4;}
			clrScr(); 
			return Q_TRAN(&Home_Page); 
		} 
	} 
	return Q_SUPER(&QHsm_top); 
} 
 
QState Home_Page(Game *me) { 
		switch (Q_SIG(me)) { 
			case Q_ENTRY_SIG: { 
				switch (game_no){ 
					case 1:
						drawMazeMenu();
						break;
					case 2:
						drawCaveMenu();
						break;
					case 3:
						drawDinoMenu();
						break;
					case 4:
						drawGoalMenu();
						break;
				} 
				showLevel();			// Show the Level Select Option 
				return Q_HANDLED(); 
			} 
			case ENCODER_TWIST:  { 
				if (encoder_twist == 0){ 
				    setColor(0, 0, 0); 
				    fillRect(20, 200 + 50*(lvl_no) - 40, 135, 200+ 50*(lvl_no) - 20); 
				    setFont(SmallFont); 
				    setColor(255, 255, 255); 
				    snprintf(buf, sizeof(buf), "Level %d", lvl_no + 1); 
				    lcdPrint(buf, 20, 200 + 50*(lvl_no) - 40); 
 
					lvl_no += 1; 
					if(lvl_no == 3) lvl_no = 0; 
 
					setColor(0, 0, 0); 
				    fillRect(20, 200 + 50*(lvl_no) - 40, 135, 200+ 50*(lvl_no) - 20); 
				    setFont(BigFont); 
				    setColor(255, 255, 255); 
				    snprintf(buf, sizeof(buf), "Level %d", lvl_no + 1); 
				    lcdPrint(buf, 20, 200 + 50*(lvl_no) - 40); 
 
				}else if(encoder_twist == 1){ 
				    setColor(0, 0, 0); 
				    fillRect(20, 200 + 50*(lvl_no) - 40, 135, 200+ 50*(lvl_no) - 20); 
				    setFont(SmallFont); 
				    setColor(255, 255, 255); 
				    snprintf(buf, sizeof(buf), "Level %d", lvl_no + 1); 
				    lcdPrint(buf, 20, 200 + 50*(lvl_no) - 40); 
 
					if(lvl_no == 0) lvl_no = 2; 
					else lvl_no -= 1; 
 
				    setColor(0, 0, 0); 
				    fillRect(20, 200 + 50*(lvl_no) - 40, 135, 200+ 50*(lvl_no) - 20); 
				    setFont(BigFont); 
				    setColor(255, 255, 255); 
				    snprintf(buf, sizeof(buf), "Level %d", lvl_no + 1); 
				    lcdPrint(buf, 20, 200 + 50*(lvl_no) - 40); 
				} 
				return Q_HANDLED(); 
			} 
			case ENCODER_CLICK:  { 
				clrScr(); 
				return Q_TRAN(&Running); 
			} 
		} 
		return Q_SUPER(&QHsm_top); 
} 
 
QState Running(Game *me) { 
		switch (Q_SIG(me)) { 
			case Q_ENTRY_SIG: { 
				counter = 0;	// Start the Seven Segment Display for Counter 
				running = 1;	// Notify the System that Game has started 
				custom_tick = 0;
				switch (game_no){ 
					case 1: {initMaze(lvl_no); speed = 1000;break;}
					case 2: {initCave(); speed = 500;break;}
					case 3: {initDino(); speed = 100; gameCounter = 0; spawnTimer = 0;break;}
					case 4: {
						initGoal(lvl_no);
						drawPlayerAtAngle(player_football.x, player_football.y, player_football.angle, PLAYER_SIZE);
						speed = 800;
						break;
					}
				}
				return Q_HANDLED(); 
			} 
			case ENCODER_TWIST: { 
				switch (game_no){ 
					case 1: { 
						if(encoder_twist) speed = speed/2; 
						else speed = speed*2; 
						break;
					} 
					case 2: {
						if (encoder_twist == 1) {
							ship.angle += 5;
							if(ship.angle > MAX_ANGLE) ship.angle = MAX_ANGLE;
						} else if (encoder_twist == 0) {
							ship.angle -= 5;
							if(ship.angle < -MAX_ANGLE) ship.angle = -MAX_ANGLE;
						}
						break;
					}
					case 4: {
						if (encoder_twist == 1) {
							rotatePlayerClockwise();
						    encoder_twist = -1; // Reset
						} else if (encoder_twist == 0) {
						     rotatePlayerAntiClockwise();
						     encoder_twist = -1; // Reset
						}
						break;
					}
				} 
				return Q_HANDLED(); 
			} 
			case BUTTON: { 
				if(buttonPressed == 1) 
				{ 
					switch (game_no){ 
						case 1: {player.direction = DIR_UP; moveForward(lvl_no);break;} // Move Up
						case 2: {ship.speed = SHIP_SPEED_FAST; speed = 250;break;}	// Speed Increase
						case 3: {	// Dino Jumps
			                if (!dino.jumping && !dino.ducking) {
			                    dino.jumping = 1;
			                    dino.jumpVelocity = -12;  // Initial jump velocity
			                }
			                break;
						}
						case 4: {player_football.speed = PLAYER_SPEED_FAST;break;}
					} 
				} else if(buttonPressed == 2) 
				{ 
					switch (game_no){ 
						case 1: {player.direction = DIR_RIGHT; moveForward(lvl_no);break;} // Move Rights
						case 2: {ship.speed = SHIP_SPEED_NORMAL; speed = 500;break;}	// Speed Normal
						case 3: {	// Dino Duck
			                if (!dino.jumping) {
			                    int oldY = dino.y;
			                    int wasDucking = dino.ducking;
			                    dino.ducking = 1;
			                    if (!wasDucking) {
			                        clearDino(oldY, 0);
			                        drawDino();
			                    }
			                }
			                break;
						}
						case 4: {player_football.speed = PLAYER_SPEED_NORMAL;break;}
					} 
				} else if(buttonPressed == 3) 
				{ 
					switch (game_no){ 
						case 1: {player.direction = DIR_DOWN; moveForward(lvl_no);break;} // Move Down
						case 2: {ship.speed = SHIP_SPEED_NORMAL; speed = 750;break;}	// Speed Normal
						case 3: {	//Dino Normal after ducking
			                if (dino.ducking && !dino.jumping) {
			                    int oldY = dino.y;
			                    dino.ducking = 0;
			                    clearDino(oldY, 1);
			                    drawDino();
			                }
			                break;
						}
						case 4: {player_football.speed = PLAYER_SPEED_NORMAL;break;}
					} 
				} else if(buttonPressed == 4) 
				{ 
					switch (game_no){ 
						case 1: {player.direction = DIR_LEFT; moveForward(lvl_no);break;} // Move Left
						case 2: {ship.speed = SHIP_SPEED_SLOW; speed = 1000;break;}	// Speed Slow
						case 3: {	// Dino Normal After Ducking
			                if (dino.ducking && !dino.jumping) {
			                    int oldY = dino.y;
			                    dino.ducking = 0;
			                    clearDino(oldY, 1);
			                    drawDino();
			                }
			                break;
						}
						case 4: {player_football.speed = PLAYER_SPEED_SLOW;break;}
					} 
				} 
				return Q_HANDLED(); 
			} 
			case GAME_TICK: { 
				switch (game_no){ 
					case 1: {updateEnemyPosition(lvl_no);break;}
					case 2: {
						scrollCave(gameDifficulty, lvl_no);
						updateShip();
						gameDistance++;
						if(gameDistance % 100 == 0) gameDifficulty++;
						drawCave();
						drawShip();
						drawHUD(gameDistance);
						break;
					}
					case 3: {
			            gameCounter++; spawnTimer++;
			            updateDinoPhysics();
			            updateObstacles();
			            if (spawnTimer > 60 - (4*lvl_no * 5)) {
			                spawnObstacle();
			                spawnTimer = 0;
			            }
			            if (gameCounter % 10 == 0) {
			                score++;
			                updateScore();
			            }
			            break;
					}
					case 4: {
			            // Erase old positions
			            setColor(34, 139, 34); // Field green
			            fillCircle((int)ball.x, (int)ball.y, BALL_RADIUS + 1);
						
			            int oldPlayerX = (int)player_football.x;
			            int oldPlayerY = (int)player_football.y;
			            updatePlayer();
			            setColor(34, 139, 34);
			            fillRect(oldPlayerX - PLAYER_SIZE - 2, oldPlayerY - PLAYER_SIZE - 2,
			                    oldPlayerX + PLAYER_SIZE + 2, oldPlayerY + PLAYER_SIZE + 2);
			            // White boundary
			            setColor(255, 255, 255);
			            drawHLine(FIELD_LEFT, 160, FIELD_WIDTH);
			            drawHLine(FIELD_LEFT, FIELD_TOP, FIELD_WIDTH);
			            drawHLine(FIELD_LEFT, FIELD_BOTTOM, FIELD_WIDTH);
			            drawVLine(FIELD_LEFT, FIELD_TOP, FIELD_HEIGHT);
			            drawVLine(FIELD_RIGHT, FIELD_TOP, FIELD_HEIGHT);
			            // Center circle
			            setColor(255, 255, 255);
			            for (int i = 0; i < 360; i += 10) {
			                int x = 120 + 30 * cosf(i * 3.14159f / 180.0f);
			                int y = 160 + 30 * sinf(i * 3.14159f / 180.0f);
			                fillRect(x, y, x+1, y+1);
			            }
			            checkBallPlayerCollision();
			            updateBall();
			            drawWalls();
			            drawBall();
			            drawPlayerAtAngle(player_football.x, player_football.y, player_football.angle, PLAYER_SIZE);
			            break;
					}
				} 
 
				int transit = checkGameStatus(game_no); 
 
				if (transit == 1){ 
					return Q_TRAN(&Result); 
				}else{ 
					return Q_HANDLED(); 
				} 
			} 
			case ENCODER_CLICK:  { 
				game_win = 0; 
			    return Q_TRAN(&Result); 
			} 
		} 
		return Q_SUPER(&QHsm_top); 
} 
 
QState Result(Game *me) { 
		switch (Q_SIG(me)) { 
			case Q_ENTRY_SIG: { 
				running = 0; 
				custom_tick = 0; 
				ResultPrint(); 
				counter = 0; 
			} 
			case CUSTOM_TIMEOUT:{ 
				running = 2;	// To Reset the Counter 
				return Q_TRAN(&Title_Page); 
			} 
		} 
		return Q_SUPER(&QHsm_top); 
} 
 
void initVars(void){ 
    counter = 0; 
    running = 0; 
	lvl_no = 0; 
	game_no = 0; 
	buttonPressed = 0; 
	speed = 1000;		// 500 ms system clock 
	gameDistance = 0;
	gameDifficulty = 0;
    score = 0;
    gameCounter = 0;
} 
 
int checkGameStatus(int game_no){ 
	int transit=0; 
	switch (game_no){ 
		case 1: { 
			if (opponent.gridX == player.gridX && opponent.gridY == player.gridY){ 
				transit = 1; 
				game_win = 0; 
			} 
			if (player.gridX == GRID_COLS-1 && player.gridY == GRID_ROWS-2){ 
				game_win = 1; 
				transit = 1; 
			} 
			break;
		} 
		case 2: { 
			if(checkCollisionCave()) transit = 1;
			break;
		} 
		case 3: { 
			if(checkCollisionDino()) transit = 1;
			break;
		} 
		case 4: { 
			if(checkGoalScored()){
				transit = 1;
				game_win = 1;
			}
			break;
		} 
	} 
	return transit; 
} 
 
void ResultPrint(void){ 
	switch (game_no){ 
		case 1: { 
			if (game_win == 0){ 
				setFont(BigFont); 
			    setColor(238, 64, 0); 
			    lcdPrint("You Loose", 30, 100); 
				setColor(238, 64, 0); 
				setFont(BigFont); 
			    lcdPrint("Game Time(s):", 20, 150); 
			    setFont(SevenSegNumFont); 
			    snprintf(buf, sizeof(buf), "%d", counter/1000); 
			    lcdPrint(buf, 80, 175); 
			} 
			if (game_win==1){ 
				setFont(BigFont); 
				setColor(238, 64, 0); 
				lcdPrint("You Win", 40, 100); 
				setColor(238, 64, 0); 
				setFont(BigFont); 
			    lcdPrint("Game Time(s):", 20, 150); 
			    setFont(SevenSegNumFont); 
			    snprintf(buf, sizeof(buf), "%d", counter/1000); 
			    lcdPrint(buf, 80, 175); 
			} 
			break;
		} 
		case 2: { 
			drawGameOverCave(gameDistance);
			setColor(238, 64, 0);
			setFont(BigFont);
		    lcdPrint("Game Time(s):", 20, 10);
		    setFont(SevenSegNumFont);
		    snprintf(buf, sizeof(buf), "%d", counter/1000);
		    lcdPrint(buf, 80, 35);
		    break;
		} 
		case 3: { 
			drawGameOverDino(score);
			setColor(238, 64, 0);
			setFont(BigFont);
		    lcdPrint("Game Time(s):", 20, 10);
		    setFont(SevenSegNumFont);
		    snprintf(buf, sizeof(buf), "%d", counter/1000);
		    lcdPrint(buf, 80, 35);
		    break;
		} 
		case 4: { 
            setColor(255, 255, 255);
            setFont(BigFont);
            lcdPrint("GAME OVER!", 45, 110);
            if (game_win) {
                lcdPrint("GOAL!", 80, 140);
            } else {
                lcdPrint("Loose!", 80, 140);
            }
			setColor(238, 64, 0);
			setFont(BigFont);
		    lcdPrint("Game Time(s):", 20, 10);
		    setFont(SevenSegNumFont);
		    snprintf(buf, sizeof(buf), "%d", counter/1000);
		    lcdPrint(buf, 80, 35);
		    break;
		} 
	} 
} 

// Spawn a new obstacle
void spawnObstacle(void) {
    // Find inactive obstacle slot
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (!obstacles[i].active) {
            obstacles[i].active = 1;
            obstacles[i].x = 240;  // Start off-screen right

            // Randomly choose ground or flying obstacle
            int obsType = (gameCounter % 3 == 0) ? OBS_FLYING : OBS_GROUND;
            obstacles[i].type = obsType;

            if (obsType == OBS_GROUND) {
                // Ground cactus
                obstacles[i].width = 15;
                obstacles[i].height = 30;
                obstacles[i].y = GROUND_Y - obstacles[i].height;
            } else {
                // Flying bird
                obstacles[i].width = 20;
                obstacles[i].height = 15;
                // Random height for flying obstacles
                int heights[] = {160, 175, 190};
                obstacles[i].y = heights[gameCounter % 3];
            }
            break;
        }
    }
}

// Update all obstacles
void updateObstacles(void) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacles[i].active) {
            // Clear old position
            clearObstacle(&obstacles[i]);

            // Move left
            obstacles[i].x -= 4*lvl_no;

            // Deactivate if off screen
            if (obstacles[i].x + obstacles[i].width < 0) {
                obstacles[i].active = 0;
            } else {
                // Draw at new position
                drawObstacle(&obstacles[i]);
            }
        }
    }
}
// Update score display
void updateScore(void) {
	char buf[12];
    setColor(135, 206, 235);
    fillRect(150, 10, 230, 30);

    setFont(SmallFont);
    setColor(255, 255, 255);
    snprintf(buf, sizeof(buf), "Score:%d", score);
    lcdPrint(buf, 155, 15);
}
/*
void checkBallPlayerCollision(void) {
    float dx = ball.x - player_football.x;
    float dy = ball.y - player_football.y;
    float dist = sqrtf(dx*dx + dy*dy);

    if (dist < PLAYER_SIZE/2 + BALL_RADIUS) {
        // Collision detected - push ball away
        float angle = atan2f(dy, dx);
        ball.vx += PLAYER_PUSH_STRENGTH * cosf(angle);
        ball.vy += PLAYER_PUSH_STRENGTH * sinf(angle);

        // Separate ball from player
        float overlap = (PLAYER_SIZE/2 + BALL_RADIUS) - dist;
        ball.x += overlap * cosf(angle);
        ball.y += overlap * sinf(angle);
    }
}
*/

void checkBallPlayerCollision(void) {
    float dx = ball.x - player_football.x;
    float dy = ball.y - player_football.y;
    float dist = sqrtf(dx*dx + dy*dy);

    // Collision threshold - sum of player radius and ball radius
    float collisionDist = PLAYER_SIZE/2 + BALL_RADIUS;

    if (dist < collisionDist && dist > 0.1f) {  // Avoid division by zero
        // Collision detected - push ball away
        float angle = atan2f(dy, dx);

        // Add velocity based on player's current angle and speed
        // This makes the ball move in the direction the player is facing
        ball.vx += PLAYER_PUSH_STRENGTH * cosf(player_football.angle);
        ball.vy += PLAYER_PUSH_STRENGTH * sinf(player_football.angle);

        // Also add some push based on collision angle
        ball.vx += (PLAYER_PUSH_STRENGTH * 0.5f) * cosf(angle);
        ball.vy += (PLAYER_PUSH_STRENGTH * 0.5f) * sinf(angle);

        // Separate ball from player to prevent sticking
        float overlap = collisionDist - dist;
        ball.x += (overlap + 1) * cosf(angle);
        ball.y += (overlap + 1) * sinf(angle);

        // Ensure ball stays within field after push
        if (ball.x - BALL_RADIUS < FIELD_LEFT) ball.x = FIELD_LEFT + BALL_RADIUS;
        if (ball.x + BALL_RADIUS > FIELD_RIGHT) ball.x = FIELD_RIGHT - BALL_RADIUS;
        if (ball.y - BALL_RADIUS < FIELD_TOP) ball.y = FIELD_TOP + BALL_RADIUS;
        if (ball.y + BALL_RADIUS > FIELD_BOTTOM) ball.y = FIELD_BOTTOM - BALL_RADIUS;
    }
}

void initGame(void) {
    // Initialize player
	player_football.x = 120;
	player_football.y = 270;
	player_football.angle = -PI / 2.0f;  // -90 degrees = pointing up
	player_football.speed = PLAYER_SPEED_NORMAL;

    // Initialize ball
    ball.x = INITIAL_BALL_X;
    ball.y = INITIAL_BALL_Y;
    ball.vx = 0;
    ball.vy = 0;

    // Initialize walls (central obstacles)
    walls[0].x1 = 80;  walls[0].y1 = 120; walls[0].x2 = 100; walls[0].y2 = 140;
    walls[1].x1 = 140; walls[1].y1 = 120; walls[1].x2 = 160; walls[1].y2 = 140;
    walls[2].x1 = 110; walls[2].y1 = 210; walls[2].x2 = 130; walls[2].y2 = 230;
}

void rotatePlayerClockwise(void) {
	player_football.angle += ROTATION_SPEED;
    // Normalize angle to -PI to PI range
    while (player_football.angle > PI) player_football.angle -= 2.0f * PI;
}

void rotatePlayerAntiClockwise(void) {
	player_football.angle -= ROTATION_SPEED;
    // Normalize angle to -PI to PI range
    while (player_football.angle < -PI) player_football.angle += 2.0f * PI;
}
/*
void updatePlayer(void) {
    // Calculate velocity based on current angle
    float dx = player_football.speed * cosf(player_football.angle);
    float dy = player_football.speed * sinf(player_football.angle);

    float newX = player_football.x + dx;
    float newY = player_football.y + dy;

    // Boundary checking
    if (newX - PLAYER_SIZE/2 >= FIELD_LEFT && newX + PLAYER_SIZE/2 <= FIELD_RIGHT) {
    	player_football.x = newX;
    }
    if (newY - PLAYER_SIZE/2 >= FIELD_TOP && newY + PLAYER_SIZE/2 <= FIELD_BOTTOM) {
    	player_football.y = newY;
    }
}
*/

void updatePlayer(void) {
    // Calculate velocity based on current angle
    float dx = player_football.speed * cosf(player_football.angle);
    float dy = player_football.speed * sinf(player_football.angle);

    float newX = player_football.x + dx;
    float newY = player_football.y + dy;

    // Check collision with field boundaries
    int canMoveX = 1;
    int canMoveY = 1;

    // Boundary checking - treat player as a circle with radius PLAYER_SIZE/2
    if (newX - PLAYER_SIZE/2 < FIELD_LEFT || newX + PLAYER_SIZE/2 > FIELD_RIGHT) {
        canMoveX = 0;
    }
    if (newY - PLAYER_SIZE/2 < FIELD_TOP || newY + PLAYER_SIZE/2 > FIELD_BOTTOM) {
        canMoveY = 0;
    }

    // Check collision with walls (obstacles)
    for (int i = 0; i < NUM_WALLS; i++) {
        Wall* w = &walls[i];

        // Check if new X position would collide (keeping Y the same)
        if (canMoveX) {
            if (newX + PLAYER_SIZE/2 > w->x1 && newX - PLAYER_SIZE/2 < w->x2 &&
            		player_football.y + PLAYER_SIZE/2 > w->y1 && player_football.y - PLAYER_SIZE/2 < w->y2) {
                canMoveX = 0;
            }
        }

        // Check if new Y position would collide (keeping X the same)
        if (canMoveY) {
            if (player_football.x + PLAYER_SIZE/2 > w->x1 && player_football.x - PLAYER_SIZE/2 < w->x2 &&
                newY + PLAYER_SIZE/2 > w->y1 && newY - PLAYER_SIZE/2 < w->y2) {
                canMoveY = 0;
            }
        }

        // Check if moving both X and Y would collide
        if (canMoveX && canMoveY) {
            if (newX + PLAYER_SIZE/2 > w->x1 && newX - PLAYER_SIZE/2 < w->x2 &&
                newY + PLAYER_SIZE/2 > w->y1 && newY - PLAYER_SIZE/2 < w->y2) {
                canMoveX = 0;
                canMoveY = 0;
            }
        }
    }

    // Apply movement only if no collision
    if (canMoveX) {
    	player_football.x = newX;
    }
    if (canMoveY) {
    	player_football.y = newY;
    }
}
