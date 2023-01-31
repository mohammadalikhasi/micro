/*
 * GccApplication2.c
 *
 * Created: 1/31/2023 6:13:02 PM
 * Author : Samen
 */ 
#define trigger 7
#define echo 6 
int t1;
int t2;
long int distance;
int state=0;
#define  F_CPU 8000000
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>



void cap_init(){
	TCCR1A =0x00;
	TCCR1B = 0x41;
}


ISR (TIMER1_CAPT_vect){
	if (state==0)
	{
		t1=ICR1;
		TCCR1A =0x00;
		TCCR1B=0x01;
		state=1;
	}
	else
	{
		t2=ICR1;
		distance= t2-t1;
		distance /= 464;
		if (distance > 70)
		{
			distance = distance - 115;
		}
		PORTC = distance;
		//_delay_ms(1000);
		TCCR1A =0x00;
		TCCR1B=0x41;
		state=0;
		trigger_for_ultrasound();
		//PORTC = distance;
	}
	
}

void trigger_for_ultrasound(){
	PORTD =0xff;
	_delay_ms(60);
	PORTD= 0x7f;
}

int main(void)
{
    DDRD |= (1<<trigger);
	DDRC = 0xff;
	cap_init();
	trigger_for_ultrasound();
	TIMSK = (1<<TICIE1);
	sei();
    while (1) 
    {
		
    }
}

