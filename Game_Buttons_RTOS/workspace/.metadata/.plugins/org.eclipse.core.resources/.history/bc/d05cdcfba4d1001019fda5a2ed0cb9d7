#ifndef game_h 
#define game_h 
 
enum GameSignals { 
	ENCODER_TWIST = Q_USER_SIG,	// Used to register the Encoder Rotation Effect 
	ENCODER_CLICK,				// Used to register the Encoder Click Effect 
	BUTTON,						// Used to register the Buttons Press Effect 
	CUSTOM_TIMEOUT,				// Used to register the Custom Timeout of 2secs 
	GAME_OVER,					// Used to register that the game ended 
	GAME_TICK					// Used to provide regular ticks for the game 
}; 
 
extern struct GameTag AO_Game; 
 
extern volatile int custom_tick;	// Used for custom timeout of 2 seconds 
extern volatile int buttonPressed;	// Used to register which button was pressed 
extern volatile int running;		// Flag used to register if the game status 
extern volatile int counter;		// Used to display the counter on the seven segment display 
extern volatile int encoder_twist;	// Used to register which direction the encoder twisted 
extern int lvl_no; 
extern int game_win; 
extern int speed; 
 
void initVars(void); 
void Game_ctor(void); 
void GpioHandler(void *CallbackRef); 
void TwistHandler(void *CallbackRef); 
void updateEnemyPosition(int lvl_no); 
int checkGameStatus(int game_no); 
 
#endif 
