#include "CircBuf.h"
#include "MK64F12.h"



void DMA_startTX(void){
	//memcpy(tx_buffer, HalfBuffTx.read_ptr, size); //zastapic to funkcja do dma

	for (unsigned int x = 0; x < ABSIZE; x++)
		tx_buffer[x] = *(HalfBuffTx.read_ptr + x);
}


void HalfBuffInit(void){

	HalfBuffTx.AorB = 'A';
	HalfBuffTx.write_ptr = buffA;
	HalfBuffTx.read_ptr = NULL;
	HalfBuffTx.read_size = 0;
	HalfBuffTx.write_size = 0;

}

void ReadUARTdata(void){

	TO_Timer_reset();
	if (HalfBuffTx.write_size < ABSIZE/*-1*/){ // chyba ten if nie jest konieczne bo zawsze musi zajsc (dba o to nastepna funkcja)

		*(HalfBuffTx.write_ptr++) = UART_READ_DATA_REG; //rejestr do odczytu w przerwaniu z uarta
		HalfBuffTx.write_size++;
		CLEAR_TIMEOUT_TIMER;
	}
	
	if (HalfBuffTx.write_size == ABSIZE){

		if (HalfBuffTx.AorB == 'A'){
			HalfBuffTx.write_ptr = &buffB[0];
			HalfBuffTx.read_ptr = &buffA[0];
			HalfBuffTx.AorB = 'B';
		}
		else{
			HalfBuffTx.write_ptr = &buffA[0];
			HalfBuffTx.read_ptr = &buffB[0];
			HalfBuffTx.AorB = 'A';
		}
		HalfBuffTx.read_size = HalfBuffTx.write_size;
		HalfBuffTx.write_size = 0;

		//ustawiać dma src addres na HalfBuffTx.read_ptr
		DMA_startTX();
		tx_flag = 1; //tymczasowo, zalatwi to dma
	}




}
