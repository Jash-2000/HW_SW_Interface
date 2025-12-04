/*****************************************************************************
Cave Flyer - Game Header
*****************************************************************************/

#ifndef game_h
#define game_h

// QP-nano Signals for Dino Runner
enum Lab2ASignals {
    ENCODER_UP_SIG = Q_USER_SIG,
    ENCODER_DOWN_SIG,
    ENCODER_CLICK_SIG,
    BUTTON_SIG,
    GAME_TICK_SIG,          // New signal for game updates
    TIMEOUT_SIG
};

// Signal aliases
#define ENCODER_UP      ENCODER_UP_SIG
#define ENCODER_DOWN    ENCODER_DOWN_SIG
#define ENCODER_CLICK   ENCODER_CLICK_SIG
#define BUTTON          BUTTON_SIG
#define GAME_TICK       GAME_TICK_SIG     // Used for game loop updates

static int tickCounter;
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
