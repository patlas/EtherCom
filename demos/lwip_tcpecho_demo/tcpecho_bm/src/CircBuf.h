#ifndef _CIRCBUF_H_
#define _CIRCBUF_H_

#include <stdint.h>
#include <string.h>
#include "TimeoutTimer.h"



#define TX_BUFFER_SIZE 10
#define TX_TIMEOUT 10 //timeout incoming data in ms 
#define ABSIZE 10 //A and B buffer size

#define UART_READ_DATA_REG UART0->D
#define CLEAR_TIMEOUT_TIMER 

extern uint8_t buffA[ABSIZE];
extern uint8_t buffB[ABSIZE];

extern uint8_t tx_buffer[TX_BUFFER_SIZE]; //bd 100

typedef struct {
	uint8_t *write_ptr;
	uint8_t *read_ptr;
	uint8_t write_size;
	uint8_t read_size;
	uint8_t AorB;
} HBuffer;

extern HBuffer HalfBuffTx;
extern HBuffer HalfBuffRx;


void DMA_startTX(void);

void ReadUARTdata(void);
void HalfBuffInit(void);





#endif
