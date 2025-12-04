// lcd.h - Header file for Rocket League Soccer Game
#ifndef LCD_H
#define LCD_H

#include "xparameters.h"
#include "xil_io.h"
#include "xil_types.h"
#include "xspi_l.h"
#include "xil_printf.h"
#include "xstatus.h"
#include "unistd.h"
#include <stdlib.h>

// Font definitions
extern u8 SmallFont[];
extern u8 BigFont[];
extern u8 SevenSegNumFont[];


#define SPI_DC          XPAR_SPI_DC_BASEADDR
#define B_RS            0x00000001

#define SPI_DTR         XPAR_SPI_BASEADDR + XSP_DTR_OFFSET
#define SPI_DRR         XPAR_SPI_BASEADDR + XSP_DRR_OFFSET
#define SPI_IISR        XPAR_SPI_BASEADDR + XSP_IISR_OFFSET
#define SPI_SR          XPAR_SPI_BASEADDR + XSP_SR_OFFSET

#define cbi(reg, bitmask)       Xil_Out32(reg, Xil_In32(reg) & ~(u32)bitmask)
#define sbi(reg, bitmask)       Xil_Out32(reg, Xil_In32(reg) |= (u32)bitmask)
#define swap(type, i, j)        {type t = i; i = j; j = t;}

// Display dimensions
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 320
#define DISP_X_SIZE 239
#define DISP_Y_SIZE 319

// Ship configuration
#define SHIP_SIZE 16
#define SHIP_SPEED_SLOW   5
#define SHIP_SPEED_NORMAL 10
#define SHIP_SPEED_FAST   20
#define MAX_ANGLE 30

// Cave configuration
#define CAVE_SEGMENTS 30
#define SEGMENT_WIDTH (SCREEN_WIDTH / CAVE_SEGMENTS)
#define CAVE_WIDTH_INITIAL 120
#define CAVE_WIDTH_MIN 60

// Macro for swapping
#define swap(type, i, j) {type t = i; i = j; j = t;}

// Font structure
struct _current_font {
    u8* font;
    u8 x_size;
    u8 y_size;
    u8 offset;
    u8 numchars;
};

// Ship structure
typedef struct {
    int x;          // X position (fixed horizontally in gameplay)
    int y;          // Y position (vertical movement)
    int angle;      // Angle of ship (-MAX_ANGLE to +MAX_ANGLE)
    int speed;      // Current speed
} Ship;

// Global variables
extern int fch, fcl, bch, bcl;
extern struct _current_font cfont;
extern Ship ship;
extern int leftWall[CAVE_SEGMENTS];
extern int rightWall[CAVE_SEGMENTS];

// LCD Functions
void LCD_Write_COM(char VL);
void LCD_Write_DATA(char VL);
void initLCD(void);
void setXY(int x1, int y1, int x2, int y2);
void clrXY(void);
void setColor(u8 r, u8 g, u8 b);
void setColorBg(u8 r, u8 g, u8 b);
void clrScr(void);
void drawVLine(int x, int y, int l);
void drawHLine(int x, int y, int l);
void fillRect(int x1, int y1, int x2, int y2);
void setFont(u8* font);
void printChar(u8 c, int x, int y);
void lcdPrint(char *st, int x, int y);

// Cave Flyer specific functions
void drawTitleScreen(void);
void initCave(void);
void scrollCave(int difficulty);
void drawCave(void);
void drawShip(void);
int checkCollision(void);
void updateShip(void);
void drawGameOver(int distance);
void drawHUD(int distance);

#endif /* LCD_H_ */
