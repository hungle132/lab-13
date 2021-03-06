/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif


void ADC_init() {
ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){

TCCR1B = 0x0B;

OCR1A = 125;

TIMSK1 = 0x02;

TCNT1 = 0;

_avr_timer_cntcurr = _avr_timer_M;

SREG |= 0x80;

}

void TimerOff(){
	TCCR1B = 0x00;
}

void TimerISR(){
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}


//right max around 750-770
//neutral is around 400-500
enum joystick{init,start,left,right}state;
enum display{show}led;
unsigned char pattern = 0x80;
unsigned short move = 0x00;
unsigned char time = 0x00;
void joy(){

	//move = ADC;
	switch(state){
		case init:
			state = start;
			break;

		case start:
			if (move  < 150){
				TimerSet(100);
				state = left;
			}
			else if (move > 150 && move < 320 ){
				//time = 250;
				TimerSet(250);
				state = left;
			}
			else if (move > 320 && move < 370){
				TimerSet(500);
				state = left;
			}
			else if (move > 350 && move < 420){
				TimerSet(1000);
				state= left;
			}
			else if (move < 650 && move > 600){
				TimerSet(1000);
				state = right;
			}
			else if (move < 700 && move > 650){
				TimerSet(500);
				state = right;
			}
			else if (move < 750 && move > 700){
				TimerSet(250);
				state = right;
			}	
			else if (move > 750){
				TimerSet(100);
				state = right;
			}
			else{
			state = start;
			}
			break;
		case left:
			if (move < 150){
				TimerSet(100);
				state = left;
			}
			else if (move > 150 && move < 320){
				TimerSet(250);
				state = left;
			}
			else if (move > 320 && move < 370){
				TimerSet(500);
				state = left;
			}
			else if (move > 350 && move < 420){
				TimerSet(1000);
				state = left;
			}
			else{
				state = init;
			}
			break;

			
		case right:
			if (move > 750){
				TimerSet(100);
				state = right;
			}
			else if (move < 750 && move > 700){
				TimerSet(250);
				state = right;
			}
			else if (move < 700 && move > 650){
				TimerSet(500);
				state = right;
			}
			else if (move < 650 && move > 600){
				TimerSet(1000);
				state = right;
			}
			else{
				state = start;
			}
			break;
			
		default:
		state = init;
		break;
	}
	switch(state){
		case init:
			break;
		case start:
			break;
		case left:
			if(pattern == 0x80)
				pattern = 0x01;
			else
				pattern = pattern << 1;
			break;
		case right:
			if(pattern == 0x01)
				pattern = 0x80;
			else
				pattern = pattern >> 1;
			break;
	}
	//time = 100;
	//TimerSet(time);

}
void dis(){
	switch(led){
	case show:
		PORTC = pattern;
		PORTD = 0xFE;
		break;

	default:
		led = show;
		break;
	}
}

//unsigned short input = ADC;
int main(void) {
    /* Insert DDR and PORT initializations */
DDRA = 0x00; PORTA = 0xFF;
DDRB = 0xFF; PORTB = 0x00;
DDRC = 0xFF; PORTC = 0x00;
DDRD = 0xFF; PORTD = 0x00;
    /* Insert your solution below */
ADC_init();
//TimerSet(100);
TimerOn();
//move = ADC;
    while (1) {
	    move = ADC;
	    dis();
	    joy();
	    while(!TimerFlag);
	    TimerFlag = 0;
    }
    return 1;
}
