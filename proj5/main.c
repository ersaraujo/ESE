#include <REG51F.H>

char txBuffer[16];
char rxBuffer[16];
static int in = 0;
static int out = 0;
static int indexTx = 0;

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
	
	char inputSerial;
	inputSerial = SBUF;
	
	IE = 0x00;	
	
	if(in == out || (!(in+1 < out))){	// Verifica se é possivel escrever no buffer
		rxBuffer[in] = inputSerial;
		if(inputSerial == '$'){
			recebeuString = 1;
		}
		if(in < 15){	
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
		writeSerial(txBuffer[out]);
		indexTx++;
	}
}

void resetVar(){
	out = 0;
	in = 0;
	indexTx = 0;
	recebeuString = 0;
}

void sendString(char *s){
	int i = 0;
	IE = 0x00;
	while(s[i] != '$'){
		sendChar(s[i]);
		i++;
	}
	IE = 0x90;
	resetVar();
}

char receiveChar(int i){	
	return rxBuffer[i];
}

void receiveString(char *s){
	int i = 0;
	while(receiveChar(i) != '$'){
		s[i] = receiveChar(i);
		i++;
	}
	s[i] = receiveChar(i);
}

int main()
{
	configSerial();
	while(1){
		char string[16];
		if(recebeuString){
			receiveString(&string);
			sendString(&string);
		}
	}
}