#include <REG517A.H>

#define ROW_SIZE        4
#define COL_SIZE        3
#define CORRECAO				0
#define BOUCING_TIME    4

#define FREQ_TIMER 			100
#define FREQ_CLK 				12000000

#define TH0_INICIAL ((65536-(FREQ_CLK/(12*FREQ_TIMER)) + CORRECAO)>>8)
#define TL0_INICIAL	((65536-(FREQ_CLK/(12*FREQ_TIMER)) + CORRECAO)&0xFF)

typedef enum{
	NTTA = 0,
	WAITING_1,
	TTA,
  WAITING_2,
}estados;

const unsigned char keys[ROW_SIZE][COL_SIZE] = {
	{'1','2','3'},
	{'4','5','6'},
	{'7','8','9'},
	{'*','0','#'}
};

static unsigned char ln, cl;
static estados estado = 0;
static unsigned char contador_debounce = 0;
static unsigned char bit_de_varredura = 0;
static unsigned char leitura = 0;
bit teclaApertada = 0;

void initializeSerial(){	// Using UART 1
  S0RELH = 0x03;
  S0RELL = 0xD9;
	PCON |= 0x80;
  BD = 1;
	S0CON |= 0x50;
	TMOD  = (TMOD&0x0F) | 0x20;
}
void timerInit(){
	TR0 = 0;																// Para o timer
	TMOD = (TMOD & 0xF0)|0x01;							// Define modo
	TH0 = TH0_INICIAL;											// Define valores iniciais para os registradores TH0 e TL0
	TL0 = TL0_INICIAL;
	ET0 = 1; 																// Habilita interrupcao do timer 0
	TR0 = 1;																// Ativa o timer
}

void intSerial (void) interrupt 4
{
	if(TI0){
		TI0=0;
	}
}

void isr_timer0 (void) interrupt 1 {
	TR0 = 0;
	TH0 = TH0_INICIAL + (unsigned char) CY;
	TL0 = TL0_INICIAL;
	TR0 = 1;
	
	switch(estado){
		case NTTA: 
        P5 = ~(1 << (bit_de_varredura & 0x03));
				if(!(P5&(1<<leitura+4))){									// Verifica os bits de leitura de P5 (P5.5, P5.6, P5.7)
					ln = bit_de_varredura;									
					cl = leitura;
					teclaApertada = 1;
					S0BUF = keys[ln][cl];
          estado = WAITING_1;
        }else{
          bit_de_varredura++;
          leitura++;   
          if(bit_de_varredura > 3){
						bit_de_varredura = 0;
          }
          if(leitura > 2){
						leitura = 0;
          }
        }
				break;
		case WAITING_1:
				teclaApertada = 0;
        if (contador_debounce++ > 3){
					contador_debounce = 0;
					estado = TTA;
				}
				break;
		case TTA: 
        P5 = 0xF0;
				if (P5 >= 0xF0){
					estado = WAITING_2;
        }
        break;
		case WAITING_2:
				if (contador_debounce++ > 3){
					contador_debounce = 0;
					estado = NTTA;
        }
				break;
		default:
				estado = NTTA;
        break;
    }
}


int main(){
    timerInit();
		initializeSerial();
		EAL = 1;
    while(1);
}
