/*
 *
 * Enrico Gunawan eguna001@ucr.edu
 * Lab section: B21
 * Assignment: Final Project
 *
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */

 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include "io.c"
 #include <time.h>
 #include <stdlib.h>
 
 volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

 // Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
 unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
 unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

 unsigned char tmpA;
 unsigned char tmpB;

 unsigned char cntLED = 0;
 unsigned char cntUser = 0;
 unsigned char cntSeq = 0;
 
 unsigned char correctSeq[9];
 unsigned char userInput[9];

 unsigned char currentLives = 3;
 unsigned char press;
 unsigned char lvl = 2;
 unsigned char score = 0;
 unsigned char highScore = 0;
 unsigned char tens = 0;
 unsigned char highTens = 0;

void LCD_DisplayString2( unsigned char column, const unsigned char* string) {
	unsigned char c = column;
	while(*string) {
		LCD_Cursor(c++);
		LCD_WriteData(*string++);
	}
}

void generateSeq()
{
	unsigned char k;
	unsigned char array[] = {1,2,4,8,16};

	for (unsigned char i = 0; i < 9; i++)
	{
		k = rand()%5;
		correctSeq[i] = array[k];
	}
}

 void set_PWM(double frequency) {
	 // Keeps track of the currently set frequency
	 // Will only update the registers when the frequency
	 // changes, plays music uninterrupted.
	 static double current_frequency;
	 if (frequency != current_frequency) {

		 if (!frequency) TCCR3B &= 0x08; //stops timer/counter
		 else TCCR3B |= 0x03; // resumes/continues timer/counter
		 
		 // prevents OCR3A from overflowing, using prescaler 64
		 // 0.954 is smallest frequency that will not result in overflow
		 if (frequency < 0.954) OCR3A = 0xFFFF;
		 
		 // prevents OCR3A from underflowing, using prescaler 64					// 31250 is largest frequency that will not result in underflow
		 else if (frequency > 31250) OCR3A = 0x0000;
		 
		 // set OCR3A based on desired frequency
		 else OCR3A = (short)(8000000 / (128 * frequency)) - 1;

		 TCNT3 = 0; // resets counter
		 current_frequency = frequency;
	 }
 }
 void PWM_on() {
	 TCCR3A = (1 << COM3A0);
	 // COM3A0: Toggle PB6 on compare match between counter and OCR3A
	 TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	 // WGM32: When counter (TCNT3) matches OCR3A, reset counter
	 // CS31 & CS30: Set a prescaler of 64
	 set_PWM(0);
 }
 void PWM_off() {
	 TCCR3A = 0x00;
	 TCCR3B = 0x00;
 }

 
 void TimerOn() {
	 // AVR timer/counter controller register TCCR1
	 TCCR1B = 0x0B;// bit3 = 0: CTC mode (clear timer on compare)
	 // bit2bit1bit0=011: pre-scaler /64
	 // 00001011: 0x0B
	 // SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
	 // Thus, TCNT1 register will count at 125,000 ticks/s

	 // AVR output compare register OCR1A.
	 OCR1A = 125;	// Timer interrupt will be generated when TCNT1==OCR1A
	 // We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
	 // So when TCNT1 register equals 125,
	 // 1 ms has passed. Thus, we compare to 125.
	 // AVR timer interrupt mask register
	 TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

	 //Initialize avr counter
	 TCNT1=0;

	 _avr_timer_cntcurr = _avr_timer_M;
	 // TimerISR will be called every _avr_timer_cntcurr milliseconds

	 //Enable global interrupts
	 SREG |= 0x80; // 0x80: 1000000
 }

 void TimerOff() {
	 TCCR1B = 0x00; // bit3bit1bit0=000: timer off
 }

 void TimerISR() {
	 TimerFlag = 1;
 }

 // In our approach, the C programmer does not touch this ISR, but rather TimerISR()
 ISR(TIMER1_COMPA_vect) {
	 // CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	 _avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	 if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		 TimerISR(); // Call the ISR that the user uses
		 _avr_timer_cntcurr = _avr_timer_M;
	 }
 }
 // Set TimerISR() to tick every M ms
 void TimerSet(unsigned long M) {
	 _avr_timer_M = M;
	 _avr_timer_cntcurr = _avr_timer_M;
 }
 
 enum SM_LED {Init, On, Off, Init_2, buttonPressed, ledOn, check, lives, winState} state;

 void tick(){

	switch(state){
		
		case Init:
			if(tmpA)
				state = On;
		break;

		case On:
			if(cntLED < lvl)
				state = Off;
			else
				state = Init_2;
		break;

		case Off:
			state = On;
		break;

		case Init_2:
			state = buttonPressed;

		break;

		case buttonPressed:
			if(tmpA)
			{
				state = ledOn;	
			}
			else
			{
				state = buttonPressed;
			}
		break;
		
		case ledOn:

			if(tmpA)
			{
				state = ledOn;
			}
			else
			{
				state = check;
			}
		break;

		case check:
			if(press == correctSeq[cntSeq] && cntUser < lvl)
				{
					state = buttonPressed;
					score++;
					if(score >= 10)
					{
						score -= 10;
						tens++;
					}
					cntUser++;
					cntSeq++;
				}
			else if(press == correctSeq[cntSeq] && cntUser >= lvl)
			{
				state = On;
				if (lvl == 8)
					{
						state = winState;
					}
				score++;
				if(score >= 10)
				{
					score -= 10;
					tens++;
				}
				lvl++;
				cntSeq = 0;
				cntUser = 0;
				cntLED = 0;
			}

			else
			{
				state = lives;
			}
		break;

		case lives:
			if(currentLives > 0)
			{
				 state = On;
				 cntSeq = 0;
				 cntUser = 0;
				 cntLED = 0;
			} 
			else
			{	
				state = Init;
				LCD_DisplayString(1, "You Lose!");
				LCD_DisplayString2(17, "High Score: ");
				if(tens > highTens)
				{
					highTens = tens;
				}
				if (tens == highTens && score > highScore)
				{
					highScore = score;
				}		
				if (highTens > 0)
				{
					LCD_WriteData(highTens + '0');
				}
				
				LCD_WriteData(highScore + '0');

				currentLives = 3;
				lvl = 2;
				score = 0;
				tens = 0;
				cntSeq = 0;
				cntUser = 0;
				cntLED = 0;
			}
		break;

		case winState:
			if (tmpA)
			{
				state = Init;
			}		
		break;

		default:
			state = Init;
		break;
	}
	 
	switch(state){
		case Init:	
			{			
				generateSeq();
			}
		break;

		case On:
		{
			TimerSet(500);
			LCD_DisplayString(1, "Lvl ");
			LCD_WriteData((lvl - 1) + '0');
			LCD_DisplayString2(7, "Score: " );
			if (tens > 0)
			{
				LCD_WriteData(tens + '0' );
			}
			
			LCD_WriteData((score) + '0');
			LCD_DisplayString2(17, "Lives: ");
			LCD_WriteData(currentLives + '0');
				
			PORTB = correctSeq[cntLED];
			
			if (correctSeq[cntLED] == 0x01)
			{
				set_PWM(523.25);
			} 
			else if (correctSeq[cntLED] == 0x02)
			{
				set_PWM(587.33);
			}
			else if(correctSeq[cntLED] == 0x04)
			{
				set_PWM(659.23);
			}
			else if(correctSeq[cntLED] == 0x08)
			{
				set_PWM(698.46);
			}
			else if (correctSeq[cntLED] == 0x10)
			{
				set_PWM(783.99);
			}
		}
		break;

		case Off:
			PORTB = 0;
			set_PWM(0);
			cntLED++;
		break;

		case Init_2:
			TimerSet(100);
		break;
		
		case buttonPressed:
			PORTB = 0;
			set_PWM(0);
		break;

		case ledOn:
			PORTB = tmpA;
			
			if (tmpA == 0x01)
			{
				set_PWM(523.25);
			}
			else if (tmpA == 0x02)
			{
				set_PWM(587.33);
			}
			else if(tmpA == 0x04)
			{
				set_PWM(659.23);
			}
			else if(tmpA == 0x08)
			{
				set_PWM(698.46);
			}
			else if (tmpA == 0x10)
			{
				set_PWM(783.99);
			}
			press = tmpA;
		break;

		case check:
			PORTB = 0;
			set_PWM(0);
		break;

		case lives:
			PORTB = 0;
			set_PWM(0);
			currentLives--;
		break;

		case winState:
			if(tens > highTens)
			{
				highTens = tens;
			}
			if (tens == highTens && score > highScore)
			{
				highScore = score;
			}
			LCD_DisplayString(1, "You Win! Press a button to play again.");
			currentLives = 3;
			lvl = 2;
			cntSeq = 0;
			cntUser = 0;
			cntLED = 0;
			tens = 0;
			score = 0;
		break;

		default:
		break;
	}
 }

 int main(void)
 {
	srand(time(NULL));

	 unsigned long timerPeriod = 100;

	 DDRA = 0x00; PORTA = 0xFF;
	 DDRB = 0xFF; PORTB = 0x00;
	 DDRC = 0xFF; PORTC = 0x00; // LCD data lines
	 DDRD = 0xFF; PORTD = 0x00; // LCD control lines

	 TimerSet(timerPeriod);
	 TimerOn();
	 LCD_init();
	 PWM_on();

	 state = Init;
	 LCD_DisplayString(4, "SIMON GAME");
	 LCD_DisplayString2(17, "press any to start");

	 while(1)
	 {
		 tmpA = ~PINA & 0x3F;
		 
		 tick();
		
		 while (!TimerFlag);
		 TimerFlag = 0;
	 }
 }
