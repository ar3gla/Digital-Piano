```// File name: SysTick.h
// Course number: CECS 447
// Term: Spring 2026
// Project number: Project 1 – Part 1
// Project description: System P1 (Digital Output): outputs a square wave to drive the speaker
// Team #: 7
// Team members: Adrian Gustilo, Alfredo Regla, Gerald Calero, Paris Talebi
//
// Description:
// This header file declares functions for configuring and controlling
// the SysTick timer. SysTick is used to generate periodic interrupts
// that toggle a GPIO pin, producing a square wave to drive the speaker.

#ifndef SYSTICK_H
#define SYSTICK_H

// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void);
void SysTick_start(void);
void SysTick_stop(void);
void SysTick_Set_Current_Note(unsigned long n_value);
#endif

```
