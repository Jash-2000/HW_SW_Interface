/*****************************************************************************
 * Rocket League Soccer - game.h
 * Game state machine header
 *****************************************************************************/

#ifndef GAME_H
#define GAME_H

#include "qpn_port.h"

// Event signals for the state machine
enum GameSignals {
    CUSTOM_TIMEOUT = Q_USER_SIG,  // Regular game update tick (from timer)
    BUTTON,                        // Button press event
    ENCODER_TWIST,                 // Encoder rotation event
    ENCODER_CLICK,                 // Encoder click event
    MAX_SIG
};

// External declarations
extern volatile int buttonPressed;
extern volatile int encoder_twist;
extern volatile int encoder_click;
extern volatile int game_timer;
extern volatile int running;
extern int counter;
extern int custom_tick;

// State machine constructor
void Lab2A_ctor(void);

// Active object declaration
extern struct Lab2ATag AO_Lab2A;

#endif // GAME_H
