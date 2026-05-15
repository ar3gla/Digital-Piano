
```// File name: SysTick.c
// Course number: CECS 447
// Term: Spring 2026
// Project number: Project 1 – Part 1
// Project description: System P1 (Digital Output): outputs a square wave to drive the speaker
// Team #: 7
// Team members: Adrian Gustilo, Alfredo Regla, Gerald Calero, Paris Talebi
//
// Description:
// This file implements the SysTick timer driver. The SysTick interrupt
// service routine toggles the speaker output pin at a frequency
// determined by the selected note, generating a square wave signal.


#include "tm4c123gh6pm.h"
#include "Systick.h"

#define SPEAKER (*((volatile unsigned long *)0x40004010)) // define SPEAKER connects to PA2: 100

// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // priority 2
    NVIC_ST_CTRL_R |= NVIC_ST_CTRL_CLK_SRC+NVIC_ST_CTRL_INTEN;   // Enable: System clock as source and SysTick interrupt
}

void SysTick_start(void)
{
    NVIC_ST_CTRL_R |= NVIC_ST_CTRL_ENABLE;
}
void SysTick_stop(void)
{
    NVIC_ST_CTRL_R &= ~NVIC_ST_CTRL_ENABLE;
}

void SysTick_Set_Current_Note(unsigned long n_value)
{
  NVIC_ST_RELOAD_R = n_value-1;// update reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it
}

// Interrupt service routine, 
// frequency is determined by current tone being played
void SysTick_Handler(void){
    NVIC_ST_CTRL_R &= ~NVIC_ST_CTRL_ENABLE; //clr bit 0
    SPEAKER ^= 0x00000004; // inverse bit 2    
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
    NVIC_ST_CTRL_R |= NVIC_ST_CTRL_ENABLE;  // set bit 0
}```
