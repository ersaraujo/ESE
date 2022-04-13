#include <REG51F.H>

#define TIME_LIMIT 100
#define CORRECAO 0
#define FREQ_TIMER 100
#define FREQ_CLK 12000000

#define TH0_INICIAL ((65536-(FREQ_CLK/(12*FREQ_TIMER)) + CORRECAO)>>8)
#define TL0_INICIAL	((65536-(FREQ_CLK/(12*FREQ_TIMER)) + CORRECAO)&0xFF)

static unsigned int timerFuncBit0 = 0;
static unsigned int timerFuncBit1 = 0;

sbit bit0P2 = P2 ^ 0;
sbit bit1P2 = P2 ^ 1;

typedef enum{
	INIT,
	CHECKBIT,
	COUNTER,
	RESET,
}estados;

//////////////////////////////////////////////////////////////////////////////////////
void timerInit(){
	
	TR0 = 0;																// Para o timer
	TMOD = (TMOD & 0xF0)|0x01;							//Define modo
	
	TH0 = TH0_INICIAL;											// Define valores iniciais para os registradores TH0 e TL0
	TL0 = TL0_INICIAL;

	ET0 = 1; 																// Habilita interrupcao do timer 0
	TR0 = 1;																// Ativa o timer
}

void timerInterruption (void) interrupt 1 using 2{

	TR0 = 0;																// Para o timer
	
	TL0 += TL0_INICIAL;											// Atribui valor incial para os registradores TH0 E TL0
	TH0 += TH0_INICIAL + (unsigned char) CY;
	
	TR0 = 1; 																// Ativa o timer
																	// Verifica a flag de overflow do timer
	timerFuncBit0++;											// Incrementa contador 1
	timerFuncBit1++;											// Incrementa contador 2
}

//////////////////////////////////////////////////////////////////////////////////////

void checkBit0(){													// Funcao que verifica o bit0 de P2
	
	static int countTime;
	static estados stateFunction;
	
	switch(stateFunction){
		case CHECKBIT:												// Estado que verifica a transi??o 0->1 do bit0
			if(bit0P2){													// Se o bit0 de P2 for 1
				P1 = (P1 & 0xF0) | (P0 & 0x0F);		// Os 4bits menos significativos de P1 recebem os 4bits menos significativos de P0
				stateFunction = COUNTER;					// Passamos para o proximo estado
				countTime = 0;										// Contador ? zerado
				timerFuncBit0 = 0;
			}	
			break;
			
		case COUNTER:													// Estado do contador, conta 1s	
			if(timerFuncBit0 >= TIME_LIMIT){		// Verificar se o contador atingiu o limite(1s)	// Zera o contador
				timerFuncBit0 = 0; 								// Zera o contador da função
				P1 = P1 & 0xF0;										// Mantem P1 com os 4bits mais significativos
				stateFunction = RESET;						// Passamos para o proximo estado
			}
			break;
			
		case RESET:														// Estado que reseta os 4bits menos significativos de P1;
			if(!bit0P2){												// Verifica se o bit0 == 0
				stateFunction = CHECKBIT;					// Se o bit0 de P2 == 0, passamos para o estado CHECKBIT
			}
			break;
			
		default:															// Executada uma vez	
			if(!bit0P2){												// Verifica se o bit0 de P2 == 0
				stateFunction = CHECKBIT;					// Caso seja, passamos para o estado CHECKBIT
			}
			break;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void checkBit1(){													// Fun??o que verifica o bit1 de P2
	
	static int countTime;
	static estados stateFunction;
	
	switch(stateFunction){
		case CHECKBIT:												// Estado que verifica a transi??o 0->1 do bit0
			if(bit1P2){													// Se o bit0 de P2 for 1
				P1 = (P1 & 0x0F) | (P0 & 0xF0);		// Os 4bits mais significativos de P1 recebem os 4bits mais significativos de P0
				stateFunction = COUNTER;					// Passamos para o proximo estado
				countTime = 0;										// Contador ? zerado
				timerFuncBit1 = 0;
			}
			break;
			
		case COUNTER:													// Estado do contador, conta 1s	
			if(timerFuncBit1 >= TIME_LIMIT){		// Verificar se o contador atingiu o limite(1s)	// Zera o contador
				timerFuncBit1 = 0; 								// Zera o contador da funcao
				P1 = P1 & 0x0F;										// Mantem P1 com os 4bits menos significativos
				stateFunction = RESET;						// Passamos para o proximo estado
			}
			break;
			
		case RESET:														// Estado que reseta os 4bits menos significativos de P1;
			if(!bit1P2){												// Verifica se o bit1 == 0
				stateFunction = CHECKBIT;					// Se o bit1 de P2 == 0, passamos para o estado CHECKBIT
			}
			break;
			
		default:															// Executada uma vez	
			if(!bit1P2){												// Verifica se o bit1 de P2 == 0
				stateFunction = CHECKBIT;					// Caso seja, passamos para o estado CHECKBIT
			}
			break;
	}
}

void main(){
	
	timerInit();				// Inicializa o timer
	EA = 1; 						// Habilita o tratamento de interrupções
	while(1){
		checkBit0();
		checkBit1();
	}
}

