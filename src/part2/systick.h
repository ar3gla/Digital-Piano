// File name: SysTick.h
// Course number: CECS 447
// Term: Spring 2026
// Project number: Project 1 – Part 2
// Project description: System P2 (Analog Output): outputs a sine wave using a 6-bit R/2R DAC
// Team #: 7
// Team members: Adrian Gustilo, Alfredo Regla, Gerald Calero, Paris Talebi
//
// Description:
// This header file declares functions for configuring and controlling
// the SysTick timer and 6-bit DAC. SysTick generates periodic interrupts
// that step through a 64-sample sine wave lookup table, outputting analog
// values to the DAC to produce musical tones.

#ifndef SYSTICK_H
#define SYSTICK_H

// Initialize 6-bit DAC on Port B (PB5-0)
void DAC_Init(void);

// Initialize SysTick timer with interrupts enabled
void Sound_Init(void);

// Start SysTick timer with specified period
// Input: period in bus clock cycles (12.5ns units at 80MHz)
void Sound_Start(unsigned long period);

// Stop SysTick timer and set DAC to middle value
void Sound_Stop(void);

#endif
