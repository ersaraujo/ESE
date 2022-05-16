#include <REG517A.H>

char txBuffer[16];
char rxBuffer[16];		
static int in = 0;
static int out = 0;
static int indexTx = 0;

bit recebeuString = 0;

void configSerial(){
	S0CON  = 0x50;			// Seleciona o Serial no modo 1
	TMOD 	= 0x20;   	// Define modo1 do Timer         
	TH1   = 0x2580;   // Define boudrate
	TR1   = 1;   			// Inicia o Timer1 
	IEN0 		= 0x90;			// Ativa interrup��o
}
	
// Funcao para escrever na UART
void writeSerial(unsigned char letra){
	S0BUF = letra;			
	while(!TI0);						// Aguarda transmiss�o
	TI0 = 0;								// Sinaliza final da transmiss�o
	out++;
}
	
void intSerial (void) interrupt 4{
	
	char inputSerial;
	inputSerial = S0BUF;
	
	IEN0 = 0x00;												// Desativa a interrup��o
	
	if(in == out || (!(in+1 < out))){	// Verifica se � possivel escrever no buffer
		rxBuffer[in] = inputSerial;			// Salva a entrada no rxBuffer
		if(inputSerial == '$'){					// Verifica se � o fim da string
			recebeuString = 1;
		}
		if(in < 15){										// Verifica o espa�o no rxBuffer
			in++;														
		}else{
			in = 0;
		}
	}
	
	RI0 = 0;														// Sinaliza final de recep��o
	IEN0 = 0x90;												// Ativa interrup�ao novamente

}
/* Mudar verifica��o para ver se in < out, caso seja, uma nova escrita foi feita */
void sendChar(char c){
	if(indexTx < 16){										// Verifica a escrita na UART
		txBuffer[indexTx] = c;						// Coloca c em txBuffer
		writeSerial(txBuffer[out]);				// Chama rotina de print
		indexTx++;
	}
}

void resetVar(){			// Fun��o que reseta variaveis utilizadas depois da rotina de print
	out = 0;
	in = 0;
	indexTx = 0;
	recebeuString = 0;
}

void sendString(char *s){		// Envia string para ser printada
	int i = 0;		
	IEN0 = 0x00;								// Desativa a interrup��o
	while(s[i] != '$'){				// Verifica se � o final da string
		sendChar(s[i]);					// Chama rotina para printar o caractere
		i++;
	}
	IEN0 = 0x90;								// Ativa a interrup��o no final do print da string
	resetVar();								// Rotina para reset das variaveis
}

char receiveChar(int i){		// Salva caractere em rxBuffer
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