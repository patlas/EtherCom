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

	if (HalfBuffTx.write_size < ABSIZE-1){

		*(HalfBuffTx.write_ptr++) = UART_READ_DATA_REG; //rejestr do odczytu w przerwaniu z uarta
		HalfBuffTx.write_size++;
		CLEAR_TIMEOUT_TIMER;
	}
	
	if (HalfBuffTx.write_size == ABSIZE-1){

		if (HalfBuffTx.AorB == 'A'){
			HalfBuffTx.write_ptr = buffB;
			HalfBuffTx.read_ptr = buffA;
			HalfBuffTx.AorB = 'B';
		}
		else{
			HalfBuffTx.write_ptr = buffA;
			HalfBuffTx.read_ptr = buffB;
			HalfBuffTx.AorB = 'A';
		}
		HalfBuffTx.read_size = HalfBuffTx.write_size;
		HalfBuffTx.write_size = 0;

		//ustawiać dma src addres na HalfBuffTx.read_ptr
		DMA_startTX();
	}




}
