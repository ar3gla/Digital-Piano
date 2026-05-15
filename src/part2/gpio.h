// File name: GPIO.h
// Course number: CECS 447
// Term: Spring 2026
// Project number: Project 1 – Part 2
// Project description: System P2 (Analog Output): outputs a sine wave using a 6-bit R/2R DAC
// Team #: 7
// Team members: Adrian Gustilo, Alfredo Regla, Gerald Calero, Paris Talebi
//
// Description:
// This header file declares GPIO initialization and interrupt handler
// functions for Port E and Port F. Port E is used for piano key inputs,
// while Port F controls mode selection, song changes, and LED indicators.

#include <stdint.h>

#ifndef SWITCH_H
#define SWITCH_H

void PortF_Init(void);
void PortE_Init(void);
void GPIOPortF_Handler(void);
void GPIOPortE_Handler(void);

#endif
