#include <REG517A.H>

char valueSerial;

char luTable[] = 
{
  0x00, 0x30, 0x60, 0x90, 0xB0, 0xD0, 0xE0, 0xF0, 0xFF 
};


void initialize(){	// Using UART 1
	S0CON |= 0x50;
	TMOD  = 0x20;
	IEN0  = 0x98;	
}

void intSerial (void) interrupt 4{
	IEN0 = 0x00;
	valueSerial = S0BUF;
	IEN0 = 0x98;
}

void setupInterrupt(){
	IEN2 |= 0x98;
}

void setupCTimer(){
	CTCON |= 0x08;
	CTRELH = 0x0FF;
	CTRELL = 0x00;
	CMSEL |= 0x01;
}

void interruptCompare (void) interrupt 19{
	CM0 = luTable[valueSerial];
}
	

int main(){
	
	initialize();
	setupInterrupt();
	setupCTimer();
	
	while(1){
	}
	
}