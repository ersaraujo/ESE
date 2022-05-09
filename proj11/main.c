#include <REG517A.H>

#define ROW_SIZE        4
#define COL_SIZE        3
#define CORRECAO				0
#define BOUCING_TIME    4

#define FREQ_TIMER 			100
#define FREQ_CLK 				12000000

#define TH0_INICIAL ((65536-(FREQ_CLK/(12*FREQ_TIMER)) + CORRECAO)>>8)
#define TL0_INICIAL	((65536-(FREQ_CLK/(12*FREQ_TIMER)) + CORRECAO)&0xFF)

bit  teclado_TeclaApertada = 0;

static unsigned char timerBoucing = 0;

typedef enum{
	INIT,
	CHECKBIT,
	BOUCING,
	RESET,
}estados;

const char row_value[ROW_SIZE] = {0x0E, 0x0D, 0x0B, 0x7};
unsigned char stable[ROW_SIZE][COL_SIZE];
unsigned char unstable[ROW_SIZE][COL_SIZE];

const char keys[ROW_SIZE][COL_SIZE] = {
	{'1','2','3'},
	{'4','5','6'},
	{'7','8','9'},
	{'*','0','#'}
};

void timerInit(){
	TR0 = 0;															
	TMOD = (TMOD & 0xF0)|0x01;
	TH0 = TH0_INICIAL;					
	TL0 = TL0_INICIAL;
	ET0 = 1; 
	TR0 = 1;						
}

void initializeSerial(){
  S0RELH = 0x03;
  S0RELL = 0xD9;
  BD = 1;
	S0CON |= 0x50;
	TMOD  = (TMOD&0x0F) | 0x20;
	ES0 = 1;	
}

void timerInterruption (void) interrupt 1 using 2{
	TR0 = 0;																
	TL0 += TL0_INICIAL;
	TH0 += TH0_INICIAL + (unsigned char) CY;
	TR0 = 1; 																											
	timerBoucing++;											
}

void verificaTecla(int i){
	static estados stateFunction;
	
	switch(stateFunction){
		case BOUCING:													
			if(timerBoucing < BOUCING_TIME){
				stateFunction = BOUCING;
			}
			stateFunction = CHECKBIT;
			timerBoucing = 0;
			break;
		case CHECKBIT:
			if(P5^i){
				teclado_TeclaApertada = 1;
			}else{
				teclado_TeclaApertada = 0;
			}
			stateFunction = RESET;
			break;
		default:		
			stateFunction = BOUCING;					
			break;
	}
}

void scan(){
  int i, j;
	P5 |= 0x0F;
	for(i = 0; i < ROW_SIZE; ++i){
		P5 |= row_value[i-1];
		for(j = 0; j < COL_SIZE; ++j){
			if(P5^(j+4) == 1){
				verificaTecla(j+4);			
				if(teclado_TeclaApertada){
					stable[i-1][j-1] = 1;
				}else{
					stable[i-1][j-1] = 0;
				}
			}
			teclado_TeclaApertada = 0;
		}
	}
}

void printSerial(){
	int i, j;
	for(i = 0; i < ROW_SIZE; i++){
		for(j = 0; j < COL_SIZE; j++){
			if(stable[i][j]){
				S0BUF = keys[i-1][j-1];
				while(!TI0);
				TI0 = 0;
			}
		}
	}	
}

int main(){
	timerInit();
	initializeSerial();
	EAL = 1;
	while(1){
		scan();
		printSerial();
	}
}