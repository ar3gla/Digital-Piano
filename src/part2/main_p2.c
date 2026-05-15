// main_p2.c
// Course number: CECS 447
// Term: Spring 2026
// Project number: Project 1 Part 2
// Project description: Music programming with 6-bit DAC sine wave output
// Team #: 7
// Team members: Adrian Gustilo, Alfredo Regla, Gerald Calero, Paris Talebi

#include "tm4c123gh6pm.h"
#include "GPIO.h"
#include "SysTick.h"
#include "music.h"
#include "PLL.h"
#include <stdint.h>

extern volatile uint8_t current_song;
volatile uint8_t mode = AUTOPLAY;
volatile uint8_t current_octave = 0;

// function prototypes
void System_Init(void);
extern void EnableInterrupts(void);  
extern void DisableInterrupts(void);

int main(void)
{    
  System_Init();
  while(1)
  {
    if(mode == AUTOPLAY) {
      autoplay(mysong[current_song]);
    } else {
      piano_mode();
    }
  }
}

void System_Init(void) 
{
    PLL_Init();          // Initialize PLL first 
    DAC_Init();          // Initialize 6-bit DAC
    Sound_Init();        // nitialize SysTick 
    PortF_Init();        // Initialize Port F (switches and LEDs)
    PortE_Init();        // Initialize Port E (piano keys)
    EnableInterrupts();  // Enable global interrupts
}
