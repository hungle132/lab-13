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
// Pins on PORTA are used as input for A2D conversion
	//    The default channel is 0 (PA0)
	// The value of pinNum determines the pin on PORTA
	//    used for A2D conversion
	// Valid values range between 0 and 7, where the value
	//    represents the desired pin for A2D conversion
void Set_A2D_Pin(unsigned char pinNum) {
	ADMUX = (pinNum <= 0x07) ? pinNum : ADMUX;
	// Allow channel to stabilize
	static unsigned char i = 0;
	for ( i=0; i<15; i++ ) { asm("nop"); }
}



//right max around 750-770
//neutral is around 400-500
enum joystick{init,start,left,right}state;
enum updown {check,up,down,reset}ad;
enum display{show}led;
unsigned char pattern = 0x80;
unsigned char row = 0xFE;
unsigned short move = 0x00;
unsigned short val = 0x00;
unsigned char time = 0x00;
void joy(){
	Set_A2D_Pin(0x00);
	move = ADC;
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
				time = 250;
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
				state = left;
			else
				pattern = pattern << 1 ;
			break;
		case right:
			if(pattern == 0x01)
				state = right;
			else
				pattern = pattern >> 1 ;
			break;
	}
	//time = 100;
	//TimerSet(1);

}
unsigned char flag = 0x00;
void ud(){
	Set_A2D_Pin(0x01);
	val = ADC;
	switch(ad){
		case check:
			if ( val < 150 && flag != 4){
			flag++;
			ad = up;
			TimerSet(100);
			}
			else if (val > 700 && flag != 0){
			flag--;
			ad = down;
			TimerSet(100);
			}
			else{
			ad = check;
			}
			break;

		case up:
			 if (val < 150 && flag != 4){
			flag++;
			TimerSet(100);
			ad = up;
			}
			 else if ( 150 > val && val < 320 && flag != 4){
			flag++;
			TimerSet(250);
			ad = up;
			 }
			else{
			ad = check;
			}
			break;
		case down:
			if (flag == 1){
			ad = reset;
			}
			else if (val > 700 && flag != 0){
			flag--;
			TimerSet(100);
			ad = down;
			}
			else{
			ad = check;
			}
			break;
		case reset:
			row = 0xFE;
			flag = 0x00;
			PORTC = pattern;
			PORTD = row;
			ad = check;
			break;
		default:
			ad = check;
	}
	switch(ad){
		case check:
			break;
		case up:
			row = row << 1 | 0x01;
			break;
		case down:
			row = row >> 1 | 0x01;
			break;
		case reset:
			break;
	}
}
void dis(){
	switch(led){
	case show:
		PORTC = pattern;
		PORTD = row;
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
TimerOn();
TimerSet(100);
    while (1) {
	    joy();
	    ud();
	    dis();
	    while(!TimerFlag);
	    TimerFlag = 0;
    }
    return 1;
}
