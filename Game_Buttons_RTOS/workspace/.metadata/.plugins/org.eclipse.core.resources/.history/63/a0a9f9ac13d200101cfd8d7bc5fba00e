/***************************************************************************** 
Created by: Jash Shah 
Date Created: 25 Nov, 2025 
Last Modified: 25 Nov, 2025 
*****************************************************************************/ 
 
#include "qpn_port.h"                                       /* QP-nano port */ 
#include "bsp.h"                             /* Board Support Package (BSP) */ 
#include "game.h"                               /* application interface */ 
#include "xil_cache.h"		                /* Cache Drivers */ 
 
 
static QEvent l_GameQueue[30]; 
 
QActiveCB const Q_ROM Q_ROM_VAR QF_active[] = { 
	{ (QActive *)0,            (QEvent *)0,          0                    }, 
	{ (QActive *)&AO_Game,    l_GameQueue,         Q_DIM(l_GameQueue)  } 
}; 
 
Q_ASSERT_COMPILE(QF_MAX_ACTIVE == Q_DIM(QF_active) - 1); 
 
int main(void) { 
 
	Xil_ICacheInvalidate(); 
	Xil_ICacheEnable(); 
	Xil_DCacheInvalidate(); 
	Xil_DCacheEnable(); 
 
	xil_printf("\n\n Hello Pratyush the King!! \n"); 
	Game_ctor(); 
	BSP_init(); 
	QF_run(); 
	return 0; 
} 
 
void printDebugLog() { 
	int size = Q_DIM(QF_active); 
	xil_printf("Number of HSMs: %i\r\n", size); 
	for (int i = 0; i < size; i++) { 
		xil_printf("\r\n"); 
		if (QF_active[i].act == 0 || QF_active[i].act->prio != i) { 
			xil_printf("HSM %i: not initialized.\r\n", i); 
			continue; 
		} 
		const QActiveCB *block = &(QF_active[i]); 
		QActive *act = block->act; 
		xil_printf("HSM %i: initialized\r\n", i); 
		xil_printf("Queue: %i/%i\r\n", act->nUsed, block->end); 
		int ind = act->tail; 
		for (int j = 0; j < act->nUsed; j++) { 
			QSignal sig = block->queue[ind].sig; 
			xil_printf("\tEvent %i: %i\r\n", j, sig); 
			ind++; 
			if (ind >= block->end) 
				ind -= block->end; 
		} 
	} 
} 
