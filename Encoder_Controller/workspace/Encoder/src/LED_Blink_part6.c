/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */


/*
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xil_exception.h"
#include "xintc.h"      // MicroBlaze interrupt controller
#include "xgpio.h"      // AXI GPIO
#include "xtmrctr.h"    // AXI Timer

XGpio Gpio_LED;	// LED Driver Pin initialization

int main()
{
    init_platform();

    //INITIALIZATION FOR AXI GPIO
    XGpio_Initialize(&Gpio_LED, XPAR_AXI_GPIO_LED_DEVICE_ID);

    while(1){
    	XGpio_DiscreteWrite(&Gpio_LED, 1, 0X1<<0u); //Turns on LED on Channel 1
    	usleep(500000); // Delay for 500,000 microseconds (0.5 seconds)
    	XGpio_DiscreteWrite(&Gpio_LED, 1, 0X1<<1u); //Turns on LED on Channel 1
    	usleep(500000); // Delay for 500,000 microseconds (0.5 seconds)
    	XGpio_DiscreteWrite(&Gpio_LED, 1, 0X1<<2u); //Turns on LED on Channel 1
    	usleep(500000); // Delay for 500,000 microseconds (0.5 seconds)
    	XGpio_DiscreteWrite(&Gpio_LED, 1, 0X1<<3u); //Turns on LED on Channel 1
    	usleep(500000); // Delay for 500,000 microseconds (0.5 seconds)
    	XGpio_DiscreteWrite(&Gpio_LED, 1, 0X1<<4u); //Turns on LED on Channel 1
    	usleep(500000); // Delay for 500,000 microseconds (0.5 seconds)
    	XGpio_DiscreteWrite(&Gpio_LED, 1, 0X1<<5u); //Turns on LED on Channel 1
    	usleep(500000); // Delay for 500,000 microseconds (0.5 seconds)
    	XGpio_DiscreteWrite(&Gpio_LED, 1, 0X1<<6u); //Turns on LED on Channel 1
    	usleep(500000); // Delay for 500,000 microseconds (0.5 seconds)
    	XGpio_DiscreteWrite(&Gpio_LED, 1, 0X1<<7u); //Turns on LED on Channel 1
    	usleep(500000); // Delay for 500,000 microseconds (0.5 seconds)
    	XGpio_DiscreteWrite(&Gpio_LED, 1, 0X1<<8u); //Turns on LED on Channel 1
    	usleep(500000); // Delay for 500,000 microseconds (0.5 seconds)
    	XGpio_DiscreteWrite(&Gpio_LED, 1, 0X1<<9u); //Turns on LED on Channel 1
    	usleep(500000); // Delay for 500,000 microseconds (0.5 seconds)
    	XGpio_DiscreteWrite(&Gpio_LED, 1, 0X1<<10u); //Turns on LED on Channel 1
    	usleep(500000); // Delay for 500,000 microseconds (0.5 seconds)
    	XGpio_DiscreteWrite(&Gpio_LED, 1, 0X1<<11u); //Turns on LED on Channel 1
    	usleep(500000); // Delay for 500,000 microseconds (0.5 seconds)
    	XGpio_DiscreteWrite(&Gpio_LED, 1, 0X1<<12u); //Turns off one LED
    	usleep(500000); // Delay for 500,000 microseconds (0.5 seconds)
    	XGpio_DiscreteWrite(&Gpio_LED, 1, 0X1<<13u); //Turns on LED on Channel 1
    	usleep(500000); // Delay for 500,000 microseconds (0.5 seconds)
    	XGpio_DiscreteWrite(&Gpio_LED, 1, 0X1<<14u); //Turns on LED on Channel 1
    	usleep(500000); // Delay for 500,000 microseconds (0.5 seconds)
    	XGpio_DiscreteWrite(&Gpio_LED, 1, 0X1<<15u); //Turns on LED on Channel 1
    	sleep(2); // Delay for 2 seconds
    }

    cleanup_platform();
    return 0;
}
*/
