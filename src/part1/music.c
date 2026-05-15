// File name: music.c
// Course number: CECS 447
// Term: Spring 2026
// Project number: Project 1 â€“ Part 1
// Project description: System P1 (Digital Output): outputs a square wave to drive the speaker
// Team #: 7
// Team members: Adrian Gustilo, Alfredo Regla, Gerald Calero, Paris Talebi
//
// Description:
// This file implements music playback functionality including
// automatic song playback, piano mode behavior, timing delays,
// and speaker initialization. The module uses SysTick interrupts
// to generate square wave signals for sound output.


#include "tm4c123gh6pm.h"
#include "SysTick.h"
#include "music.h"
#include <stdint.h>

extern volatile uint8_t current_song;  // Selected song index
extern volatile uint8_t mode;          // Current system mode

const uint32_t Tone_Tab[] = {
     76335,  68028,  60605,  57308,  51020,  45455,  40485,  // C4 (indices 0-6)
     38168,  34014,  30303,  28654,  25510,  22728,  20243,  // C5 (indices 7-13)
     19084,  17007,  15152,  14327,  12755,  11364,  10121,  // C6 (indices 14-20)
      9542,   8504,   7576                                    // C7 (indices 21-23)
};

NTyp mysong[][255] = {
 
// Mary Had a Little Lamb
    {E4,4,D4,4,C4,4,D4,4,E4,3,PAUSE,1,E4,3,PAUSE,1,E4,8,
     D4,3,PAUSE,1,D4,3,PAUSE,1,D4,8,E4,4,G4,4,PAUSE,1,G4,8,
     E4,4,D4,4,C4,4,D4,4,E4,3,PAUSE,1,E4,3,PAUSE,1,E4,8,
     D4,3,PAUSE,1,D4,4,E4,4,D4,4,C4,8,0,0},
	
    // Happy Birthday melody 
    {C4,1,PAUSE,1,C4,2,D4,4,C4,4,F4,4,E4,8,
     PAUSE,4,
     C4,1,PAUSE,1,C4,2,D4,4,C4,4,G4,4,F4,8,
     PAUSE,4,
     C4,1,PAUSE,1,C4,2,C5,4,A4,4,F4,4,E4,4,D4,8,
     PAUSE,4,
     B4,1,PAUSE,1,B4,2,A4,4,F4,4,G4,4,F4,12,0,0},
   
    // Twinkle Twinkle Little Star
    {C4,3,PAUSE,1,C4,4,G4,3,PAUSE,1,G4,4,A4,3,PAUSE,1,A4,4,G4,8,
     F4,3,PAUSE,1,F4,4,E4,3,PAUSE,1,E4,4,D4,3,PAUSE,1,D4,4,C4,8,
     G4,3,PAUSE,1,G4,4,F4,3,PAUSE,1,F4,4,E4,3,PAUSE,1,E4,4,D4,8,
     G4,3,PAUSE,1,G4,4,F4,3,PAUSE,1,F4,4,E4,3,PAUSE,1,E4,4,D4,8,
     C4,3,PAUSE,1,C4,4,G4,3,PAUSE,1,G4,4,A4,3,PAUSE,1,A4,4,G4,8,
     F4,3,PAUSE,1,F4,4,E4,3,PAUSE,1,E4,4,D4,3,PAUSE,1,D4,4,C4,8,0,0},

// Song 4: 
{E5,1,E5,2,E5,2,C5,1,E5,2,
 G5,4,G4,4,
 C5,3,G4,3,E4,3,
 A4,2,B4,2,A4,1,A4,2,
 G4,1,E5,1,G5,1,A5,2,F5,1,G5,2,
 E5,2,C5,1,D5,1,B4,3,
 C5,3,G4,3,E4,3,
 A4,2,B4,2,A4,1,A4,2,
 G4,1,E5,1,G5,1,A5,2,F5,1,G5,2,
 E5,2,C5,1,D5,1,B4,3,
 PAUSE,4,
 G5,2,G5,2,F5,2,E5,2,E5,1,
 A4,1,A4,1,C5,2,A4,1,C5,1,D5,3,
 G5,2,G5,2,F5,2,E5,2,E5,1,
 C6,2,C6,2,C6,4,
 G5,2,G5,2,F5,2,E5,2,E5,1,
 A4,1,A4,1,C5,2,A4,1,C5,1,D5,3,
 E5,3,D5,3,C5,4,
 PAUSE,4,0,0}};



void autoplay(NTyp scoretab[])
{
	uint8_t i=0, j;
	uint8_t song_at_start = current_song;
	uint8_t octave_offset = current_octave * 7;
	
	while (scoretab[i].delay) {                                      // Exit if song changed or mode changed
		if (song_at_start != current_song || mode != AUTOPLAY) {
			return;
		}
		
if (scoretab[i].tone_index==PAUSE)
    SysTick_stop();
else {
	// this one line is a edge case, because for the mario song the highest octave won't play the note 
    uint8_t offset = (current_song == 3) ? ((octave_offset > 7) ? 7 : octave_offset) : octave_offset;
    SysTick_Set_Current_Note(Tone_Tab[scoretab[i].tone_index + offset]);
    SysTick_start();
}
		for (j=0;j<scoretab[i].delay;j++) // Hold note duration
			Delay();
		SysTick_stop();
		i++;
	}
	for (j=0;j<15;j++) // Small delay after song finishes
		Delay();
}

void piano_mode(void)
{
	while(mode == PIANO) { // If no keys pressed (PE0â€“PE3 all HIGH)
		if((GPIO_PORTE_DATA_R & 0x0F) == 0x0F) {
			SysTick_stop();
			GPIO_PORTF_DATA_R &= ~0x08; // Turn off green LED
		}
	}
}

void Delay(void){
	uint32_t volatile time;
	time = 727240*20/91*2.5; // Calibrated timing constant
	while(time){
		time--;
	}
}

void Speaker_Init(void){ 
	volatile uint32_t delay;
	SYSCTL_RCGC2_R |= 0x01;            // Enable clock for Port A
	delay = SYSCTL_RCGC2_R;            // Allow time for clock to stabilize
	GPIO_PORTA_PCTL_R &= ~0x00000F00;  // Clear alternate function
	GPIO_PORTA_AMSEL_R &= ~0x04;       // Disable analog
	GPIO_PORTA_DIR_R |= 0x04;          // Set PA2 as output
	GPIO_PORTA_AFSEL_R &= ~0x04;       // Disable alternate function
	GPIO_PORTA_DEN_R |= 0x04;          // Enable digital
	GPIO_PORTA_DR8R_R |= 0x04;         // 8mA drive strength
}
