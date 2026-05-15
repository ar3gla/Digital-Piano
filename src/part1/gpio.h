// File name: GPIO.h
// Course number: CECS 447
// Term: Spring 2026
// Project number: Project 1 – Part 1
// Project description: System P1 (Digital Output): outputs a square wave to drive the speaker
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

void PortF_Handler(void);
void PortE_Handler(void);


#endif

