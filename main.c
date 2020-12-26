#include "msp.h"
#include <stdint.h>
#include <stdbool.h>

//Macros
#define DELAY 1500
#define STATE1 0
#define STATE2 1
#define STATE3 2
#define STATE4 3

//Function Headers
void PORT1_IRQHandler(void);
void configurePort1(void);
void configurePort2(void);
void configureState(int state);
int nextStateButtonAction(int state);
int previousStateButtonAction(int state);
void handleState(bool next);
void EUSCIA0_IRQHandler(void);
void configureUART(void);
	
int main(void){
	
	//Disable Watchdog Timer
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; 
	
	configureUART();
	
	configurePort1();
	configurePort2();
	
	configureState(STATE1);
	
	/* Main Loop */
	for (;;) {
		__WFI();
	}
  
	return 0;
}

void configurePort1(void){

    //Select Values
    P1SEL0 &= (uint8_t)~(1 << 0 | 1 << 1 | 1 << 4);
    P1SEL1 &= (uint8_t)~(1 << 0 | 1 << 1 | 1 << 4);

    //Direction Values
    P1DIR &= (uint8_t)~(1 << 1 | 1 << 4);
    P1DIR |= (uint8_t)(1 << 0);

    //Resistor Values
    P1REN |= (uint8_t)(1 << 1 | 1 << 4);

    //Output Values
    P1OUT |= (uint8_t)(1 << 1 | 1 << 4);
    P1OUT &= (uint8_t)~(1 << 0);

    //Interrupts
    P1IE |= (uint8_t)(1 << 1 | 1 << 4);
    P1IES |= (uint8_t)(1 << 1 | 1 << 4);
    P1IFG &= (uint8_t)~(1 << 1 | 1 << 4);
    P1IE &= (uint8_t)~(1 << 0);

    //NVIC Interrupts
    NVIC_SetPriority(PORT1_IRQn,2);
    NVIC_GetPendingIRQ(PORT1_IRQn);
    NVIC_EnableIRQ(PORT1_IRQn);

}

void configurePort2(void){

    //Select Pins
    P2SEL0 &= (uint8_t)~(1 << 0);
    P2SEL1 &= (uint8_t)~(1 << 0);

    //Direction Pins
    P2DIR |= (uint8_t)(1 << 0);

    //Output Pins
    P2OUT &= (uint8_t)~(1 << 0);

    //Interrupts
    P2IE &= (uint8_t)~(1 << 0);

}

void PORT1_IRQHandler(void){
	
	if (P1->IFG & (1<<1)) {
		
		// Added Debouncing
		int i = 0;
    for(; i < DELAY;i++);
		
		// Next State
		if (!(P1->IN & (1<<1))) {
			handleState(true);
						
			}
		P1->IFG &= ~(1<<1);
	}
	
	if (P1->IFG & (1<<4)) {
		
		// Added Debouncing
    int i = 0;
    for(; i < DELAY;i++);
		
		// Previous state
    if (!(P1->IN & (1<<4))) {
			handleState(false);
			}					
		}
		P1->IFG &= ~(1<<4);
	}

void configureState(int state){
	switch(state){
		case STATE1:
			P1->OUT &= ~(uint8_t)(1 << 0);
			P2->OUT &= ~(uint8_t)(1 << 0);
			//Send STATE 1 Serial
			EUSCI_A0->TXBUF = '1';
			break;
						
		case STATE2:
			P1->OUT &= ~(uint8_t)(1 << 0);
			P2->OUT |= (uint8_t)(1 << 0);
		  //Send STATE 2 Serial
			EUSCI_A0->TXBUF = '2';
			break;
						
		case STATE3:
			P1->OUT |= (uint8_t)(1 << 0);
			P2->OUT &= ~(uint8_t)(1 << 0);
			//Send STATE 3 Serial
			EUSCI_A0->TXBUF = '3';
			break;
						
		case STATE4:
			P1->OUT |= (uint8_t)(1 << 0);
			P2->OUT |= (uint8_t)(1 << 0);
			//Send STATE 4 Serial
			EUSCI_A0->TXBUF = '4';
			break;
	}
}

void EUSCIA0_IRQHandler(void){
	if (EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG){
		
			if(EUSCI_A0->RXBUF == 'P'){
				handleState(false);
			} else if(EUSCI_A0->RXBUF == 'N'){
				handleState(true);
			}
			EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;    // Clear eUSCI RX interrupt flag
		}
}

int nextStateButtonAction(int state){
	switch(state){
				case STATE1:
					configureState(STATE2);
					state = STATE2;
					break;
						
				case STATE2:
					configureState(STATE3);
					state = STATE3;
					break;
						
				case STATE3:
					configureState(STATE4);
					state = STATE4;
					break;
						
				case STATE4:
					configureState(STATE1);
					state = STATE1;
					break;
			}
	return state;
}

int previousStateButtonAction(int state){
	switch(state){
				case STATE1:
					configureState(STATE4);
					state = STATE4;
					break;
						
				case STATE2:
					configureState(STATE1);
					state = STATE1;
					break;
						
				case STATE3:
					configureState(STATE2);
					state = STATE2;
					break;
						
				case STATE4:
					configureState(STATE3);
					state = STATE3;
					break;
			}
	return state;
}

void handleState(bool next){
	static int currentState = STATE1;
	if(next){
		currentState = nextStateButtonAction(currentState);
	} else {
		currentState = previousStateButtonAction(currentState);
	}
}

void configureUART(void){
	
	// Configure UART pins
  P1->SEL0 |= (uint8_t)(1 << 2 | 1 << 3);	// set 2-UART pin as secondary function
	P2->SEL1 &= (uint8_t)~(1 << 2 | 1 << 3);               
	
	// Setting clock to 12 MHz
	CS->KEY = CS_KEY_VAL;                   // Unlock CS module for register access
  CS->CTL0 = 0;                           // Reset tuning parameters
  CS->CTL0 = CS_CTL0_DCORSEL_3;           // Set DCO to 12MHz (nominal, center of 8-16MHz range)
  CS->CTL1 = CS_CTL1_SELA_2 |             // Select ACLK = REFO
          CS_CTL1_SELS_3 |                // SMCLK = DCO
          CS_CTL1_SELM_3;                 // MCLK = DCO
  CS->KEY = 0;                            // Lock CS module from unintended accesses
	
	// Configure UART
  EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
  EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
  EUSCI_B_CTLW0_SSEL__SMCLK;      				// Configure eUSCI clock source for SMCLK
	
	//Configure Control Registers
	UCA0CTLW0 |= (uint16_t)(1 << 0); //Reset UART state
	UCA0CTLW0 &= (uint16_t)~(BITD); //Set first bit to LSB
	UCA0CTLW0 &= (uint16_t)~(BITC); //Configure 8 data bits
	UCA0CTLW0 |= (uint16_t)(BITB);   //Configure 2 data bits
	UCA0CTLW0 &= (uint16_t)~(BITF);  //Disable parity bit
	UCA0CTLW0 &= (uint16_t)~(BITA | 1 << 9); //Set UART mode
	UCA0CTLW0 &= (uint16_t)~(1 << 8); //Enables asynchronous mode
	UCA0IRCTL &= (uint16_t)~(1 << 0); //Disable IrDA encoder
	
	//Configure Baud rate
	UCA0CTLW0 |= (uint16_t)(1 << 7);
	UCA0CTLW0 |= (uint16_t)(1 << 6);
	UCA0MCTLW |= (uint16_t)(1 << 0);
	UCA0BRW = (uint16_t)(78);
	UCA0MCTLW &= (uint16_t)~(1 << 7|1 << 6|1 << 4);
	UCA0MCTLW |= (uint16_t)(1 << 5);
	UCA0MCTLW &= (uint16_t)~(1 << 8|BITA|BITD);
	UCA0MCTLW &= (uint16_t)~(1 << 9|BITB|BITC|BITC|BITE|BITF);
	
	//Set UART state
	UCA0CTLW0 &= (uint16_t)~(1 << 0);
	
	//Configure Interupts
	EUSCI_A0->IE |= (uint8_t)(1 << 1); //Transmit IE
	EUSCI_A0->IE |= (uint8_t)(1 << 0); //Recieve IE
	UCA0IFG &= (uint16_t)~(1 << 1); //Transmit Flag Cleared
	UCA0IFG &= (uint16_t)~(1 << 0); //Recieve Flag Cleared
	
	//Configure NVIC
	NVIC_SetPriority(EUSCIA0_IRQn,2);
	NVIC_ClearPendingIRQ(EUSCIA0_IRQn);
	NVIC_EnableIRQ(EUSCIA0_IRQn);
}
