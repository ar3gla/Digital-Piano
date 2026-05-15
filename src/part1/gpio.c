// File name: GPIO.c
// Course number: CECS 447
// Term: Spring 2026
// Project number: Project 1 – Part 1
// Project description: System P1 (Digital Output): outputs a square wave to drive the speaker
// Team #: 7
// Team members: Adrian Gustilo, Alfredo Regla, Gerald Calero, Paris Talebi
//
// Description:
// This file configures GPIO Port E and Port F and implements their
// interrupt service routines. GPIO interrupts are used to detect
// button presses for piano input, octave changes, mode switching,
// and song selection while controlling sound output via SysTick.
#include "tm4c123gh6pm.h"
#include "music.h"
#include "GPIO.h"
#include "SysTick.h"
volatile uint8_t current_song = 0; // Stores currently selected song
extern volatile uint8_t mode; // Current system mode (PIANO or SONG)
extern volatile uint8_t current_octave; // Current octave (0,1,2)
void Delay_10ms(void); // Forward declaration


void PortF_Init(void)
{
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF; 
	while((SYSCTL_RCGC2_R & SYSCTL_RCGC2_GPIOF) == 0){}   //  Enable clock for Port F
	GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
	GPIO_PORTF_CR_R |= 0x1F;                              //  Unlock PF0 (special locked pin)
	GPIO_PORTF_DIR_R = (GPIO_PORTF_DIR_R & ~0x11) | 0x0E; //  Set directions: PF4, PF0 = input (buttons) // PF1, PF2, PF3 = output (LEDs)
	GPIO_PORTF_AFSEL_R &= ~0x1F;                          //  Disable alternate functions
	GPIO_PORTF_DEN_R |= 0x1F;                             //  Enable digital function
	GPIO_PORTF_PCTL_R &= ~0x000FFFFF;                     //  Clear port control register
	GPIO_PORTF_AMSEL_R &= ~0x1F;                          //  Disable analog mode
	GPIO_PORTF_PUR_R |= 0x11;                             //  Enable internal pull-up resistors on PF4 and PF0
	GPIO_PORTF_IS_R &= ~0x11;                             //  Edge-sensitive
	GPIO_PORTF_IBE_R &= ~0x11;                            // Disable both-edge triggering
	GPIO_PORTF_IEV_R &= ~0x11;                            // Falling edge trigger (button press)
	GPIO_PORTF_ICR_R = 0x11;                              // Clear any prior interrupt flags
	GPIO_PORTF_IM_R |= 0x11;                              // Arm interrupts
	NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF1FFFFF) | 0x00400000; // Set interrupt priority
	NVIC_EN0_R |= 0x40000000;                              // Enable interrupt 30 in NVIC
	GPIO_PORTF_DATA_R |= 0x02;                             // Turn ON Red LED initially (default state)
}
void PortE_Init(void)
{
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;
	while((SYSCTL_RCGC2_R & SYSCTL_RCGC2_GPIOE) == 0){} // Enable clock for Port E
	GPIO_PORTE_LOCK_R = GPIO_LOCK_KEY;
	GPIO_PORTE_CR_R |= 0x0F;                            // Unlock and allow changes
	GPIO_PORTE_DIR_R &= ~0x0F;                          // Set PE0–PE3 as inputs
	GPIO_PORTE_AFSEL_R &= ~0x0F;                        // Disable alternate function
	GPIO_PORTE_DEN_R |= 0x0F;                           // Enable digital I/O
	GPIO_PORTE_PCTL_R &= ~0x0000FFFF;                   // Clear port control
	GPIO_PORTE_AMSEL_R &= ~0x0F;                        //  Disable analog
	GPIO_PORTE_IS_R &= ~0x0F;                           // Edge-sensitive
	GPIO_PORTE_IBE_R |= 0x0F;                           // Enable both-edge triggering (press and release)
	GPIO_PORTE_ICR_R = 0x0F;                            // Clear interrupt flags
	GPIO_PORTE_IM_R |= 0x0F;                            // Arm interrupts
	NVIC_PRI1_R = (NVIC_PRI1_R & 0xFFFFFF1F) | 0x00000040; // Set interrupt priority
	NVIC_EN0_R |= 0x00000010;                           // Enable interrupt 4 in NVIC
}
void GPIOPortF_Handler(void)
{
	if(GPIO_PORTF_RIS_R & 0x10) {
		GPIO_PORTF_ICR_R = 0x10; // Clear interrupt
		Delay_10ms();            // Debounce delay
		if((GPIO_PORTF_DATA_R & 0x10) == 0) {
			mode = !mode;       // Toggle mode
			SysTick_stop();     // Stop sound
			if(mode == PIANO) {         // Red LED ON, Blue OFF
				GPIO_PORTF_DATA_R |= 0x02;
				GPIO_PORTF_DATA_R &= ~0x04;
			} else {            // Blue LED ON, Red OFF

				GPIO_PORTF_DATA_R |= 0x04;
				GPIO_PORTF_DATA_R &= ~0x02;
			}
		}
	}
	
	if(GPIO_PORTF_RIS_R & 0x01) {   // Clear interrupt
		GPIO_PORTF_ICR_R = 0x01;    // Debounce
		Delay_10ms();
		if((GPIO_PORTF_DATA_R & 0x01) == 0) {
			if(mode == PIANO) {    // Cycle octave (0 → 1 → 2 → 0)
				current_octave = (current_octave + 1) % 3;
			} else {               // Cycle through songs
				current_song = (current_song + 1) % NUM_SONGS;
				SysTick_stop();
			}
		}
	}
}
void GPIOPortE_Handler(void)
{
	uint8_t note_offset = current_octave * 7;
	if(GPIO_PORTE_RIS_R & 0x01)
	{
		GPIO_PORTE_ICR_R = 0x01;
		if(GPIO_PORTE_DATA_R & 0x01)
		{
			GPIO_PORTF_DATA_R |= 0x08; // Green LED ON
			SysTick_Set_Current_Note(Tone_Tab[C4 + note_offset]);
			SysTick_start();
		}
		else            
		{
			GPIO_PORTF_DATA_R &= ~0x08;   // Green LED OFF
			SysTick_stop();
		}
	}
	if(GPIO_PORTE_RIS_R & 0x02)          //PE1  D NOTE
	{
		GPIO_PORTE_ICR_R = 0x02;
		if(GPIO_PORTE_DATA_R & 0x02)
		{
			GPIO_PORTF_DATA_R |= 0x08;
			SysTick_Set_Current_Note(Tone_Tab[D4 + note_offset]);
			SysTick_start();
		}
		else
		{
			GPIO_PORTF_DATA_R &= ~0x08;
			SysTick_stop();
		}
	}
	if(GPIO_PORTE_RIS_R & 0x04)  // PE2  E NOTE
	{
		GPIO_PORTE_ICR_R = 0x04;
		if(GPIO_PORTE_DATA_R & 0x04)
		{
			GPIO_PORTF_DATA_R |= 0x08;
			SysTick_Set_Current_Note(Tone_Tab[E4 + note_offset]);
			SysTick_start();
		}
		else
		{
			GPIO_PORTF_DATA_R &= ~0x08;
			SysTick_stop();
		}
	}
	if(GPIO_PORTE_RIS_R & 0x08) //PE3  F NOTE
	{
		GPIO_PORTE_ICR_R = 0x08;
		if(GPIO_PORTE_DATA_R & 0x08)
		{
			GPIO_PORTF_DATA_R |= 0x08;
			SysTick_Set_Current_Note(Tone_Tab[F4 + note_offset]);
			SysTick_start();
		}
		else
		{
			GPIO_PORTF_DATA_R &= ~0x08;
			SysTick_stop();
		}
	}
}

void Delay_10ms(void)
{
    uint32_t volatile time = 133333; // ~10ms at 40MHz
    while(time) { time--; }
}
