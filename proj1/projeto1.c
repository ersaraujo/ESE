#include <stdio.h>

#define TIME_LIMIT 2288

int bit0P2 = 0;
int p1 = 0;

typedef enum{
	CHECKBIT,
	COUNTER,
	RESET,
}estados;

void checkBit0(){
	
	int countTime_1;
	static estados state;
	
	switch(state){
		case CHECKBIT:
			if(!bit0P2){
				printf("Inicio\n");
				p1 = 1;
				state = COUNTER;
				countTime_1 = 0;
			}
			break;
		case COUNTER:
			if(countTime_1++ >= TIME_LIMIT){
				printf("Limite atingido\n");
				countTime_1 = 0;
				state = RESET;
			}
			break;
		case RESET:
			printf("Reset atingido\n");
			p1 = 0;
			state = CHECKBIT;
			break;
		default:
			state = CHECKBIT;
	}
}

void main(){
	while(1){
		checkBit0();
	}
}