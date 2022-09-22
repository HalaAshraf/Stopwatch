/*
 * stopwatch.c
 *
 *  Created on: 17 Sept 2022
 *      Author: Hala Ashraf Kamel
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned char sec1 = 0;
unsigned char sec2 = 0;
unsigned char min1 = 0;
unsigned char min2 = 0;
unsigned char hour1 = 0;
unsigned char hour2 = 0;

void INT0_Init(void){
		DDRD  &= (~(1<<PD2));               // Configure INT0/PD2 as input pin
		PORTD |= (1 << PD2);				// Pull up
		MCUCR |= (1<<ISC01);
		MCUCR &= ~(ISC00);  				// Trigger INT0 with the falling edge
		GICR  |= (1<<INT0);                 // Enable external interrupt pin INT0
		SREG  |= (1<<7);					// Enable I-bit
}

ISR(INT0_vect){
	// RESET
	sec1 = 0;
	sec2 = 0;
	min1 = 0;
	min2 = 0;
	hour1 = 0;
	hour2 = 0;
}

void INT1_Init(void){

		DDRD  &= (~(1<<PD3));  				// Configure INT1/PD3 as input pin
		MCUCR |= (1<<ISC11) | (1<<ISC10);	// Trigger INT1 with the rising edge
		GICR  |= (1<<INT1);    				// Enable external interrupt pin INT1
		SREG  |= (1<<7);
}

ISR(INT1_vect){
	// Pause clock
	TCCR1B &=0xF8;
}

void INT2_Init(void){

	DDRD &=(~(1<<PB2));
	PORTB |= (1 << PB2);	// Pull up
	MCUCSR &= ~(1<<ISC2); 	// Trigger INT2 with the falling edge
	GICR  |= (1<<INT2);    				// Enable external interrupt pin INT2
	SREG  |= (1<<7);
}

ISR(INT2_vect){
	// Resume Clock
	TCCR1B |= (1<<CS12) | (1<<CS10);
}

void Timer1_Init_CTC(void){

	TCNT1 =0;
	TCCR1B |= (1<<WGM12) | (1<<CS12) | (1<<CS10);	// CTC, Prescaler = 1024
	OCR1A = 976;
	TIMSK |= (1<<OCIE1A);
	SREG |=(1<<7);
}

ISR(TIMER1_COMPA_vect){
	sec1++;
	if(sec1==10){
		sec1=0;
		sec2++;
	}
	else if(sec2 == 6){
		sec2 =0;
		min1++;
	}
	else if(min1 == 10){
		min1 =0;
		min2++;
	}
	else if(min2 == 6){
		min2 =0;
		hour1++;
	}
	else if(hour1 == 10){
		hour1 = 0;
		hour2++;
	}
	else if((hour2 == 3) && (hour1 == 5)){
		hour2 = 0;
	}
}

void display_time(void){

	PORTA = (PORTA & 0XC0) | (1<<PA0); 		// Enable sec1, Disable the rest
	PORTC = (PORTC & 0xF0) | (sec1 & 0x0F);	// Display sec1
	_delay_ms(1);

	PORTA = (PORTA & 0XC0) | (1<<PA1);
	PORTC = (PORTC & 0xF0) | (sec2 & 0x0F);
	_delay_ms(1);

	PORTA = (PORTA & 0XC0) | (1<<PA2);
	PORTC = (PORTC & 0xF0) | (min1 & 0x0F);
	_delay_ms(1);

	PORTA = (PORTA & 0XC0) | (1<<PA3);
	PORTC = (PORTC & 0xF0) | (min2 & 0x0F);
	_delay_ms(1);

	PORTA = (PORTA & 0XC0) | (1<<PA4);
	PORTC = (PORTC & 0xF0) | (hour1 & 0x0F);
	_delay_ms(1);

	PORTA = (PORTA & 0XC0) | (1<<PA5);
	PORTC = (PORTC & 0xF0) | (hour2 & 0x0F);
	_delay_ms(1);
}


int main(void){

	DDRC |=0X0F; // Set first 4 pins of PORTC as outputs ( 7447 decoder)
	PORTC &=0XF0; // Initialise 7-segments to 0

	DDRA |= 0X3F; // Set first 6 pins of PORTA as outputs (enable pins for seven segments)
	PORTA |= 0X3F;

	Timer1_Init_CTC();
	INT0_Init();
	INT1_Init();
	INT2_Init();

	while(1){

		display_time();
	}

}


