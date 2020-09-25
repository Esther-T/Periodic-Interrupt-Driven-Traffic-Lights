//Name: Esther Tan
//Project Description: Periodic Interrupt-Driven Traffic Lights

//C driver for Traffic Lights

//initialize ports B
void TLights_init(void)
{
	//activate the clock on Ports B
	SYSCTL->RCGCGPIO |= 0x02;   // enable clock to GPIOB
	
	while ((SYSCTL -> PRGPIO & 0x02) != 0x02) {}; // ready?

	//PortB 5-0 LEDs
    GPIOB->AMSEL &= ~0x3F;      // turn off analog of PORTB 5-0
    GPIOB->PCTL &= ~0x00000000; 
    GPIOB->DIR |= 0x3F;         // PORTB 5-0 output high
	GPIOD->AFSEL &= ~0x3F; 
    GPIOB->DEN |= 0x3F;         // PORTB 3-1 as digital pins
    GPIOB->DATA |= 0x3F;        // set pins PORTB 5-0
}

void Write_LED(unsigned char i) {
GPIOB->DATA = i; //write i to ports B to display onto the lights
}

//function for PLL
void PLL_Init(void) {
	SYSCTL->RCC2 |= 0x80000000; // 0) Use RCC2
	SYSCTL->RCC2 |= 0x00000800; // 1) bypass PLL while initializing
	SYSCTL->RCC = (SYSCTL->RCC&~0x000007C0)+0x00000540; //2)16 MHz
	SYSCTL->RCC2 &= ~0x00000070; // configure for main oscillator source
	SYSCTL->RCC2 &= ~0x00002000; // 3) activate PLL by clearing PWRDN
	SYSCTL->RCC2 |= 0x40000000; // 4) use 400 MHz PLL
	SYSCTL->RCC2 = (SYSCTL->RCC2&~0x1FC00000) + (39<<22); //10MHz
	while((SYSCTL->RIS&0x00000040) == 0) {}; // 5) wait for the PLL to lock
	SYSCTL->RCC2 &= ~0x00000800; // 6) enable PLL by clearing BYPASS
}


//using Periodic Interrupt
int main(void) {
	
LED_Init(); // initialize bar graph LEDs
SysTick_Init(1000000); // Initialize SysTick and set 100ms-period, Assume 10MHz bus clock
while(1);
}

//In SysTick.c

int ix=0; // index of digits
int state = 0; //counts the states
int count = 100; //count the number of interrupts
const char LEDtbl[]= {
0x0C, 0x0A, 0x09, 0x21,
0x11, 0x09
};

void SysTick_Init(uint32_t period){
SysTick->CTRL = 0; // 1) disable SysTick during setup
SysTick->LOAD = period - 1; // 2) reload value
SysTick->VAL = 0; // 3) any write to CURRENT clears it
SysTick->CTRL = 0x00000007; // 4) enable SysTick with core clock and //interrupts (local) //question
__enable_irq(); // enable interrupts (global)
}

void SysTick_Handler(void){ // SysTick interrupt handler
if(ix == 6)
	ix = 0;
count--;
if(count == 0){ // time for outputting a LED pattern
	Write_LED(LEDtbl[ix]);
	ix++;
	
	if((state == 0) && (state == 3))
	{
		count = 100; // reset repetition count //delay for 10s
		state ++;
	}
	else if((state == 1) && (state == 4))
	{
		count = 20; //delay for 2s
		state ++;
	}
	else if(state == 2)
	{ 
		count = 10; //delay for 1s
		state ++;
	}
	else if(state == 5)
	{
		count = 10;
		state = 0; //resets to state 0
	}
  }
}
