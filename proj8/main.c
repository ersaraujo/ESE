#include <REG517A.H>

char valueSerial;

unsigned char luTable[] = 
{
  0xFF, 0xE0, 0xD0, 0xC0, 0xB0, 0xA0, 0x90, 0x60, 0x30, 0x00
};


void initializeSerial(){	// Using UART 1
  S0RELH = 0x03;
  S0RELL = 0xD9;
  BD = 1;
	S0CON |= 0x50;
	TMOD  = (TMOD&0x0F) | 0x20;
	ES0 = 1;	
}

void initializeCTimer(){
  CTCON = CTCON&0xF8;
  CTRELH = 0x0FF;
	CTRELL = 0x00;
  CMEN = 0xFF;
	CMSEL = 0xFF;
  CMH0 = CMH1 = CMH2 = CMH3 = CMH4 = CMH5 = CMH6 = CMH7 = 0xFF;
  CML0 = CML1 = CML2 = CML3 = CML4 = CML5 = CML6 = CML7 = 0x00;
}

void interruptSerial (void) interrupt 4{
	valueSerial = S0BUF - '0';
	RI0 = 0;
  CML0 = CML1 = CML2 = CML3 = CML4 = CML5 = CML6 = CML7 = luTable[valueSerial];
}

int main(){
	initializeSerial();
  initializeCTimer();
	EAL = 1;
	while(1){
	}
}