/*****************************************************************************
Cave Flyer - Game Header
*****************************************************************************/

#ifndef game_h
#define game_h

// Game signals
enum GameSignals {
    ENCODER_CLICK = Q_USER_SIG,
    ENCODER_UP,
    ENCODER_DOWN,
    ENCODER_TWIST,
    BUTTON,
    GAME_TICK,
    CUSTOM_TIMEOUT
};

// Global variables
extern volatile int custom_tick;
extern volatile int buttonPressed;
extern volatile int running;
extern volatile int counter;
extern volatile int encoder_twist;

// Functions
void Lab2A_ctor(void);
void printDebugLog(void);

// Active object
extern struct Lab2ATag AO_Lab2A;

#endif /* game_h */
