#include "reg51.h"

char txBuffer[16];
static int in = 0;
char rxBuffer[16];
static int out = 0;

static int indexTx =0;

bit recebeuString = 0;

void configSerial(){
	SCON  = 0x50;		// Seleciona o Serial no modo 1
	TMOD 	= 0x20;   // Define modo1 do Timer         
	TH1   = 0xF0;   // Define boudrate
	TR1   = 1;   		// Inicia o Timer1 
	IE 		= 0x90;		// Ativa interrupção
}
	
// Funcao para escrever na UART
void writeSerial(unsigned char letra){
	SBUF = letra;			
	while(!TI);						// Aguarda transmissão
	TI = 0;								// Sinaliza final da transmissão
	out++;
}
	
void intSerial (void) interrupt 4{	
	unsigned char inputSerial;
	inputSerial = SBUF;
	IE = 0x00;						
	if(!(in+1)%16 == out){
		rxBuffer[in] = inputSerial;
		if(inputSerial == '$'){
			recebeuString = 1;
		}
		if(in < 14){
			in++;
		}else{
			in = 0;
		}
	}
	
	RI = 0;														// Sinaliza final de recepção
	IE = 0x90;												// Ativa interrupçao novamente
}

void sendChar(char c){
	if(indexTx < 16){
		txBuffer[indexTx] = c;
		TI = 1;
		writeSerial(txBuffer[indexTx]);
		indexTx++;
	} 
}

void sendString(char *s){
	int i = 0;
	while(s[i] != '$'){
		sendChar(s[i]);
		i++;
	}
	out = 0;
}

char rxBufferVazio(){
	char vazio;
	if(in == out){
		vazio = 1;
	}else{
		vazio = 0;
	}
	return vazio;
}

char receiveChar(int i){	
	return rxBuffer[i];
}

void receiveString(char *s){
	int i = 0;
	while(rxBuffer[i] != '$'){
		s[i] = receiveChar(i);
		i++;
	}
}

int main()
{
	configSerial();
	while(1){
		char string[];
		if(recebeuString){
			receiveString(string);
			sendString(rxBuffer);
		}
	}
}