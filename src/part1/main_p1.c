```// main_p1.c
// Course number: CECS 447
// Term: Spring 2026
// Project number: Project 1
// Project description: Music programming
// Team #: 7
// Team members: Adrian Gustilo, Alfredo Regla, Gerald Calero, Paris Talebi

#include "tm4c123gh6pm.h"
#include "GPIO.h"
#include "SysTick.h"
#include "music.h"
#include "PLL.h"
#include <stdint.h>
extern volatile uint8_t current_song; // Current song index (from music.c)
volatile uint8_t mode = PIANO;        // Default mode at startup = PIANO
volatile uint8_t current_octave = 0;  // Octave range: 0,1,2 (used in piano mode)
//defintions 

// function prototypes
void System_Init(void);
extern void EnableInterrupts(void);  
extern void DisableInterrupts(void);


int main(void)
{    
  System_Init(); // Configure all hardware modules
  while(1)
  {
    if(mode == AUTOPLAY) {  // If in AUTOPLAY mode, play selected song automatically
      autoplay(mysong[current_song]);
    } else {               // If in PIANO mode, wait for key presses (interrupt driven)
      piano_mode();
    }
  }
}

void System_Init(void) 
{
    SysTick_Init();  // Configure SysTick timer for tone generation
    PortF_Init();    // Configure mode buttons + LEDs
    PortE_Init();   // Configure piano keys (interrupt driven)
    Speaker_Init(); // Configure PA2 as speaker output
    PLL_Init();     // Set system clock (typically 40 MHz)
    EnableInterrupts(); // Enable global interrupt processing
}
```

