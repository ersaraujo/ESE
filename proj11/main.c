#include <REG517A.H>

#define DEBOUNCING_TIME 4


static unsigned int value = 0x00;
unsigned lerTeclas(void){
  return value;
}

void varreduraDebounce(){

  unsigned char i;  
  
  P5 |= 0x0F;
  
  for(i = 0; i < 4; i++){
     P5.i = 0; 
 
  }
    
}




sbit varredura = 0;



void interrupcaoT0() interrupt 1 
{
  if(-timerVarredura == 0)
  {
    timerVarredura = ; // Reinicializa temporizador
    varredura = 1;						  // Sinaliza que terminou a contagem
  }
}



int main(){

  while(1){
    if(varredura){
      varredura = 0;
    }
  }
}

  
