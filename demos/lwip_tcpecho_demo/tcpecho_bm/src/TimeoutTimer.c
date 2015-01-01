#include "TimeoutTimer.h"

void TO_Timer_init(uint8_t timeout_ms){
	
	pit_user_config_t config = {
		.isInterruptEnabled = true,
		.isTimerChained = false,
		.periodUs = 1000*timeout_ms,
	};
	
	PIT_DRV_Init ( PIT_INSTANCE, true );
	PIT_DRV_InitChannel ( PIT_INSTANCE, 0, &config );
	PIT_DRV_InstallCallback ( PIT_INSTANCE, 0, TO_Timer_callbackIRQ );
	
}
void TO_Timer_reset(void){
	
	PIT_DRV_StartTimer ( PIT_INSTANCE, 0 );
	
}
void TO_Timer_stop(void){
	
	PIT_DRV_StopTimer ( PIT_INSTANCE, 0 );
	
}

void TO_Timer_callbackIRQ(void){
	
	//jezeli bufor in nie jest pusty to odpalic dma
	if(HalfBuffTx.write_size>0){
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

			//ustawiac dma src addres na HalfBuffTx.read_ptr i pamietac ze w DMA_END_IRQ tcp_write tyle bajtow ile jest w HalfBuffTx.read_size
			DMA_startTX();
			
			tx_flag=1;
		}
	else
		TO_Timer_stop();
		
}
