/*****************************************************************************
    Dino Runner - Game State Machine
    Endless runner with jump and duck mechanics
*****************************************************************************/
#define AO_LAB2A
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

// Function prototypes
void updateDinoPhysics(void);
void updateObstacles(void);
void spawnObstacle(void);
int checkCollision(void);

typedef struct Lab2ATag {
    QActive super;
} Lab2A;

// State machine states
static QState Lab2A_initial(Lab2A *me);
static QState Menu(Lab2A *me);
static QState Running(Lab2A *me);
static QState GameOver(Lab2A *me);

Lab2A AO_Lab2A;

// Game variables
volatile int buttonPressed = 0;
volatile int running = 0;
volatile int gameCounter = 0;
volatile int spawnTimer = 0;
extern DinoPlayer dino;
extern Obstacle obstacles[MAX_OBSTACLES];
extern int score;
extern int gameSpeed;
static int tickCounter = 0;

void Lab2A_ctor(void) {
    Lab2A *me = &AO_Lab2A;
    QActive_ctor(&me->super, (QStateHandler)&Lab2A_initial);
}

QState Lab2A_initial(Lab2A *me) {
    initLCD();
    drawMenuScreen();
    return Q_TRAN(&Menu);
}

QState Menu(Lab2A *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            running = 0;
            score = 0;
            gameCounter = 0;
            gameSpeed = 4;
            dino.y = 200;
            dino.jumping = 0;
            dino.ducking = 0;
            dino.jumpVelocity = 0;
            return Q_HANDLED();
        }

        case ENCODER_CLICK: {
            initGameScreen();
            dino.y = 200;
            dino.jumping = 0;
            dino.ducking = 0;
            dino.jumpVelocity = 0;
            drawDino();
            updateScore();
            return Q_TRAN(&Running);
        }
    }
    return Q_SUPER(&QHsm_top);
}

QState Running(Lab2A *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            running = 1;
            gameCounter = 0;
            tickCounter = 0;
            spawnTimer = 0;
            // Clear obstacles
            for (int i = 0; i < MAX_OBSTACLES; i++) {
                obstacles[i].active = 0;
            }
            return Q_HANDLED();
        }

        case GAME_TICK: {
            gameCounter++;
            spawnTimer++;

            // Update physics
            updateDinoPhysics();

            // Update obstacles
            updateObstacles();

            // Spawn new obstacles periodically
            if (spawnTimer > 60 - (gameSpeed * 5)) {
                spawnObstacle();
                spawnTimer = 0;
            }

            // Increase score
            if (gameCounter % 10 == 0) {
                score++;
                updateScore();

                // Increase difficulty
                if (score % 100 == 0 && gameSpeed < 8) {
                    gameSpeed++;
                }
            }

            // Check collision
            if (checkCollision()) {
                return Q_TRAN(&GameOver);
            }

            return Q_HANDLED();
        }

        case BUTTON: {
            if (buttonPressed == 1) {  // Jump button
                if (!dino.jumping && !dino.ducking) {
                    dino.jumping = 1;
                    dino.jumpVelocity = -12;  // Initial jump velocity
                }
            } else if (buttonPressed == 2) {  // Duck button
                if (!dino.jumping) {
                    int oldY = dino.y;
                    int wasDucking = dino.ducking;
                    dino.ducking = 1;
                    if (!wasDucking) {
                        clearDino(oldY, 0);
                        drawDino();
                    }
                }
            } else if (buttonPressed == 0) {  // Button released
                if (dino.ducking && !dino.jumping) {
                    int oldY = dino.y;
                    dino.ducking = 0;
                    clearDino(oldY, 1);
                    drawDino();
                }
            }
            return Q_HANDLED();
        }

        case ENCODER_CLICK: {
            return Q_TRAN(&GameOver);
        }
    }
    return Q_SUPER(&QHsm_top);
}

QState GameOver(Lab2A *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            running = 0;
            drawGameOver(score);
            return Q_HANDLED();
        }

        case ENCODER_CLICK: {
            drawMenuScreen();
            return Q_TRAN(&Menu);
        }
    }
    return Q_SUPER(&QHsm_top);
}

// Update dino physics (jumping/falling)
void updateDinoPhysics(void) {
    if (dino.jumping) {
        int oldY = dino.y;

        // Apply gravity
        dino.jumpVelocity += 1;  // Gravity acceleration
        dino.y += dino.jumpVelocity;

        // Check if landed
        if (dino.y >= 200) {
            dino.y = 200;
            dino.jumping = 0;
            dino.jumpVelocity = 0;
        }

        // Redraw dino at new position
        clearDino(oldY, 0);
        drawDino();
    }
}

// Update all obstacles
void updateObstacles(void) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacles[i].active) {
            // Clear old position
            clearObstacle(&obstacles[i]);

            // Move left
            obstacles[i].x -= gameSpeed;

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

// Check collision between dino and obstacles
int checkCollision(void) {
    int dinoLeft = dino.x + 2;
    int dinoRight = dino.x + 18;
    int dinoTop, dinoBottom;

    if (dino.ducking) {
        dinoTop = dino.y + 12;
        dinoBottom = dino.y + 33;
    } else {
        dinoTop = dino.y - 6;
        dinoBottom = dino.y + 28;
    }

    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacles[i].active) {
            int obsLeft = obstacles[i].x;
            int obsRight = obstacles[i].x + obstacles[i].width;
            int obsTop = obstacles[i].y;
            int obsBottom = obstacles[i].y + obstacles[i].height;

            // Check AABB collision
            if (dinoRight > obsLeft && dinoLeft < obsRight &&
                dinoBottom > obsTop && dinoTop < obsBottom) {
                return 1;  // Collision detected
            }
        }
    }
    return 0;  // No collision
}
