#define  F_CPU 8000000
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>

#define TRIGGER 7
#define ECHO 6 
#define MAX_DISTANCE 20
#define DELAY 60

#define MOTOR_DDR DDRC
#define MOTOR_PORT PORTC

#define LCD_DPRT PORTA
#define LCD_DDDR DDRA
#define LCD_DPIN PINA

#define LCD_CPRT PORTB
#define LCD_CDDR DDRB
#define LCD_CPIN PINB
#define LCD_RS 0
#define LCD_RW 1
#define LCD_EN 2

#define KEY_PRT PORTA
#define KEY_DDR	DDRA
#define KEY_PIN	PINA

#define CMD_CLEAR 0x01
#define CMD_LTOR 0x06
#define CMD_DP_ON_CR_ON 0x0E
#define CMD_DP_ON_CR_BL 0x0F

int t1;
int t2;
long int distance;
int state=0;
int motor_state = 0;

void show_distance() {
	if (distance < 0)
	{
	 	distance += 141;
	}
	char str[17];
	sprintf(str, " Distance: %dcm", distance);
	lcd_command(CMD_CLEAR);
	lcd_print(str);	
}

// LCD
void lcd_command(unsigned char cmd) {
	LCD_DPRT = cmd;
	LCD_CPRT &= ~(1 << LCD_RS);
	LCD_CPRT &= ~(1 << LCD_RW);
	LCD_CPRT |= (1 << LCD_EN);
	_delay_us(1);
	LCD_CPRT &= ~(1 << LCD_EN);
	_delay_us(100);
}

void lcd_data(unsigned char data) {
	LCD_DPRT = data;
	LCD_CPRT |= (1 << LCD_RS);
	LCD_CPRT &= ~(1 << LCD_RW);
	LCD_CPRT |= (1 << LCD_EN);
	_delay_us(1);
	LCD_CPRT &= ~(1 << LCD_EN);
	_delay_us(100);
}

void lcd_goto(unsigned char x) {
	unsigned char firstCharAddr = 0x80;
	lcd_command(firstCharAddr + x - 1);
	_delay_us(100);
}

void lcd_print(char *str) {
	unsigned char i = 0;
	while (str[i] != 0) {
		lcd_data(str[i]);
		i++;
	}
}

void lcd_print_digit(unsigned char ch) {
	lcd_data(ch);
}

void lcd_init() {
	LCD_DDDR = 0xFF;
	LCD_CDDR = 0xFF;
	
	LCD_CPRT &= ~(1 << LCD_EN);
	
	_delay_us(2000);
	
	lcd_command(CMD_DP_ON_CR_ON);
	lcd_command(CMD_CLEAR);
	
	_delay_us(2000);
	
	lcd_command(CMD_LTOR);
}

void cap_init(){
	TCCR1A =0x00;
	TCCR1B = 0x41;
}


ISR (TIMER1_CAPT_vect){
	if (state==0)
	{
		t1=ICR1;
		TCCR1A =0x00;
		TCCR1B=0x02;
		state=1;
	}
	else
	{
		t2=ICR1;
		distance= t2-t1;
		distance /= 58;
		show_distance();
		if (distance < MAX_DISTANCE)
			motor_routine_90();
		//else { 
			//motor_routine_90();
			//motor_routine_90();
			//motor_routine_90();
			//motor_routine_90();
		//}
		_delay_ms(1000);
		TCCR1A =0x00;
		TCCR1B=0x42;
		state=0;
		trigger_for_ultrasound();
		//PORTC = distance;
	}
}

void trigger_for_ultrasound(){
	PORTD =0xff;
	_delay_ms(DELAY);
	PORTD= 0x7f;
}

void motor_init() {
	MOTOR_DDR = 0xFF;	
}

void ultrasound_init() {
    DDRD |= (1<<TRIGGER);
	DDRC = 0xff;
}

void motor_routine_90() {
	switch (motor_state % 4) {
		case 0:
			MOTOR_PORT = 0x99;
			motor_state = 1;
			break;
		case 1: 
			MOTOR_PORT = 0xcc;
			motor_state = 2;
			break;
		case 2:
			MOTOR_PORT = 0x66;
			motor_state = 3;
			break;
		case 3:
			MOTOR_PORT = 0x33;
			motor_state = 0;
			break;
	}
}

int main(void)
{
	motor_init();
	lcd_init();
	ultrasound_init();
	cap_init();
	trigger_for_ultrasound();
	TIMSK = (1<<TICIE1);
	sei();
    while (1) 
    {
		
    }
}

