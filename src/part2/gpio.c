// File name: GPIO.c
// Course number: CECS 447
// Term: Spring 2026
// Project number: Project 1 – Part 2
// Project description: System P2 (Analog Output): outputs a sine wave using a 6-bit R/2R DAC
// Team #: 7
// Team members: Adrian Gustilo, Alfredo Regla, Gerald Calero, Paris Talebi
//
// Description:
// This file configures GPIO Port E and Port F and implements their
// interrupt service routines. GPIO interrupts are used to detect
// button presses for piano input, octave changes, mode switching,
// and song selection while controlling sound output via SysTick and DAC.

// Register definitions for TM4C123, music note tables, GPIO prototypes, and SysTick
#include "tm4c123gh6pm.h"
#include "music.h"
#include "GPIO.h"
#include "SysTick.h"

// Tracks which song is selected in song playback mode
volatile uint8_t current_song = 0;

// Owned by main.c — mode toggles between PIANO and SONG, current_octave shifts note lookup
extern volatile uint8_t mode;
extern volatile uint8_t current_octave;

void Delay_10ms(void);

void PortF_Init(void)
{
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;
	while((SYSCTL_RCGC2_R & SYSCTL_RCGC2_GPIOF) == 0){}
	GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
	GPIO_PORTF_CR_R |= 0x1F;
	GPIO_PORTF_DIR_R = (GPIO_PORTF_DIR_R & ~0x11) | 0x0E;
	GPIO_PORTF_AFSEL_R &= ~0x1F;
	GPIO_PORTF_DEN_R |= 0x1F;
	GPIO_PORTF_PCTL_R &= ~0x000FFFFF;
	GPIO_PORTF_AMSEL_R &= ~0x1F;
	GPIO_PORTF_PUR_R |= 0x11;       // pull-ups on PF0/PF4 so buttons read high at rest (active-low)
	GPIO_PORTF_IS_R &= ~0x11;
	GPIO_PORTF_IBE_R &= ~0x11;
	GPIO_PORTF_IEV_R &= ~0x11;      // falling edge only — interrupt fires on press, not release
	GPIO_PORTF_ICR_R = 0x11;
	GPIO_PORTF_IM_R |= 0x11;
	NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF1FFFFF) | 0x00400000;
	NVIC_EN0_R |= 0x40000000;
	GPIO_PORTF_DATA_R |= 0x04;      // blue LED on at startup to indicate default SONG mode
}

void PortE_Init(void)
{
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;
	while((SYSCTL_RCGC2_R & SYSCTL_RCGC2_GPIOE) == 0){}
	GPIO_PORTE_LOCK_R = GPIO_LOCK_KEY;
	GPIO_PORTE_CR_R |= 0x0F;
	GPIO_PORTE_DIR_R &= ~0x0F;
	GPIO_PORTE_AFSEL_R &= ~0x0F;
	GPIO_PORTE_DEN_R |= 0x0F;
	GPIO_PORTE_PCTL_R &= ~0x0000FFFF;
	GPIO_PORTE_AMSEL_R &= ~0x0F;
	GPIO_PORTE_IS_R &= ~0x0F;
	GPIO_PORTE_IBE_R |= 0x0F;       // both edges — ISR fires on key press AND release
	GPIO_PORTE_ICR_R = 0x0F;
	GPIO_PORTE_IM_R |= 0x0F;
	NVIC_PRI1_R = (NVIC_PRI1_R & 0xFFFFFF1F) | 0x00000040;
	NVIC_EN0_R |= 0x00000010;
	                                 // no pull-ups — hardware uses external pull-downs or direct drive
}

void GPIOPortF_Handler(void)
{
	// SW1 (PF4): toggle between PIANO and SONG mode
	if(GPIO_PORTF_RIS_R & 0x10) {
		GPIO_PORTF_ICR_R = 0x10;        // clear interrupt flag
		Delay_10ms();                    // debounce — wait for signal to settle
		if((GPIO_PORTF_DATA_R & 0x10) == 0) {  // re-check pin; if still low, press was real
			mode = !mode;                // toggle mode
			Sound_Stop();                // stop any active sound before switching context
			if(mode == PIANO) {
				GPIO_PORTF_DATA_R |= 0x02;   // red LED = PIANO mode
				GPIO_PORTF_DATA_R &= ~0x04;
			} else {
				GPIO_PORTF_DATA_R |= 0x04;   // blue LED = SONG mode
				GPIO_PORTF_DATA_R &= ~0x02;
			}
		}
	}

	// SW2 (PF0): cycle octave in PIANO mode, or advance to next song in SONG mode
	if(GPIO_PORTF_RIS_R & 0x01) {
		GPIO_PORTF_ICR_R = 0x01;        // clear interrupt flag
		Delay_10ms();                    // debounce
		if((GPIO_PORTF_DATA_R & 0x01) == 0) {  // confirm real press
			if(mode == PIANO) {
				current_octave = (current_octave + 1) % 3;  // wraps 0->1->2->0
			} else {
				current_song = (current_song + 1) % NUM_SONGS;  // advance song, wrap at end
				Sound_Stop();
			}
		}
	}
}

void GPIOPortE_Handler(void)
{
	// Each octave shifts 7 positions in the tone table (one full diatonic set)
	uint8_t note_offset = current_octave * 7;

	// PE0: key C — both edges handled; rising = play, falling = stop
	if(GPIO_PORTE_RIS_R & 0x01)
	{
		GPIO_PORTE_ICR_R = 0x01;
		if(GPIO_PORTE_DATA_R & 0x01)         // rising edge, key pressed
		{
			GPIO_PORTF_DATA_R |= 0x08;        // green LED on while key held
			Sound_Start(Tone_Tab[C4 + note_offset] / 64);  // /64 converts table value to SysTick reload period
		}
		else                                  // falling edge, key released
		{
			GPIO_PORTF_DATA_R &= ~0x08;       // green LED off
			Sound_Stop();
		}
	}

	// PE1: key D
	if(GPIO_PORTE_RIS_R & 0x02)
	{
		GPIO_PORTE_ICR_R = 0x02;
		if(GPIO_PORTE_DATA_R & 0x02)
		{
			GPIO_PORTF_DATA_R |= 0x08;
			Sound_Start(Tone_Tab[D4 + note_offset] / 64);
		}
		else
		{
			GPIO_PORTF_DATA_R &= ~0x08;
			Sound_Stop();
		}
	}

	// PE2: key E
	if(GPIO_PORTE_RIS_R & 0x04)
	{
		GPIO_PORTE_ICR_R = 0x04;
		if(GPIO_PORTE_DATA_R & 0x04)
		{
			GPIO_PORTF_DATA_R |= 0x08;
			Sound_Start(Tone_Tab[E4 + note_offset] / 64);
		}
		else
		{
			GPIO_PORTF_DATA_R &= ~0x08;
			Sound_Stop();
		}
	}

	// PE3: key F
	if(GPIO_PORTE_RIS_R & 0x08)
	{
		GPIO_PORTE_ICR_R = 0x08;
		if(GPIO_PORTE_DATA_R & 0x08)
		{
			GPIO_PORTF_DATA_R |= 0x08;
			Sound_Start(Tone_Tab[F4 + note_offset] / 64);
		}
		else
		{
			GPIO_PORTF_DATA_R &= ~0x08;
			Sound_Stop();
		}
	}
}

// ~10ms busy-wait used for button debouncing; 133333 iterations ≈ 10ms at 80MHz
void Delay_10ms(void)
{
    uint32_t volatile time = 133333;
    while(time) { time--; }
}
