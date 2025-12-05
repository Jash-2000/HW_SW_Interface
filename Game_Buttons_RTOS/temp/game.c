/***************************************************************************** 
Created by: Jash Shah 
Date Created: 25 Nov, 2025 
Last Modified: 26 Nov, 2025 
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
 
void initVars(); 
void rotateClockwise(); 
void rotateAntiClockwise(); 
void moveForward(int lvl_no); 
void moveBackward(int lvl_no); 
 
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
int lvl_no = 0; 
int game_no = 0; 
int game_win = 0; 
char buf[12]; // To Print "Level " + number 
int speed = 1000; 
 
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
			game_no = buttonPressed; 
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
					case 1: drawMazeMenu(); 
					case 2: drawCaveMenu(); 
					case 3: drawDinoMenu(); 
					case 4: drawGoalMenu(); 
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
				switch (game_no){ 
					case 1: {initMaze(lvl_no); speed = 1000;} 
					case 2: {initCave(lvl_no); speed = 1000;} 
					case 3: {initDino(lvl_no); speed = 1000;} 
					case 4: {initGoal(lvl_no); speed = 1000;} 
				} 
				return Q_HANDLED(); 
			} 
			case ENCODER_TWIST: { 
				switch (game_no){ 
					case 1: { 
						if(encoder_twist) speed = speed/2; 
						else speed = speed*2; 
					} 
					case 2: {} 
					case 3: {} 
					case 4: {} 
				} 
				return Q_HANDLED(); 
			} 
			case BUTTON: { 
				if(buttonPressed == 1) 
				{ 
					switch (game_no){ 
						case 1: {player.direction = DIR_UP; moveForward(lvl_no);} 
						case 2: {} 
						case 3: {} 
						case 4: {} 
					} 
				} else if(buttonPressed == 2) 
				{ 
					switch (game_no){ 
						case 1: {player.direction = DIR_RIGHT; moveForward(lvl_no);} 
						case 2: {} 
						case 3: {} 
						case 4: {} 
					} 
				} else if(buttonPressed == 3) 
				{ 
					switch (game_no){ 
						case 1: {player.direction = DIR_DOWN; moveForward(lvl_no);} 
						case 2: {} 
						case 3: {} 
						case 4: {} 
					} 
				} else if(buttonPressed == 4) 
				{ 
					switch (game_no){ 
						case 1: {player.direction = DIR_LEFT; moveForward(lvl_no);} 
						case 2: {} 
						case 3: {} 
						case 4: {} 
					} 
				} 
				return Q_HANDLED(); 
			} 
			case GAME_TICK: { 
				switch (game_no){ 
					case 1: {updateEnemyPosition(lvl_no);} 
					case 2: {} 
					case 3: {} 
					case 4: {} 
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
		} 
		case 2: { 
 
		} 
		case 3: { 
 
		} 
		case 4: { 
 
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
		} 
		case 2: { 
 
		} 
		case 3: { 
 
		} 
		case 4: { 
 
		} 
	} 
} 
