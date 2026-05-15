// File name: SysTick.c
// Course number: CECS 447
// Term: Spring 2026
// Project number: Project 1 – Part 2
// Project description: System P2 (Analog Output): outputs a sine wave using a 6-bit R/2R DAC
// Team #: 7
// Team members: Adrian Gustilo, Alfredo Regla, Gerald Calero, Paris Talebi
//
// Description:
// This file initializes the 6-bit R/2R DAC on Port B and configures
// SysTick periodic interrupts for sine wave output. The SysTick handler
// steps through a 64-sample sine wave lookup table and writes each
// sample to the DAC to produce analog audio output.

#include "tm4c123gh6pm.h"

// Bit-banded address for PB5–PB0; writing a 6-bit value here drives the DAC directly
#define DAC (*((volatile unsigned long *)0x400050FC))

// Current position in the sine wave table; incremented each SysTick interrupt
unsigned char Index;

// 64-sample sine wave quantized to 6 bits (0–63).
// One full cycle is stored; the SysTick period determines the output frequency.
const unsigned char SineWave[64] = {
  32, 35, 38, 41, 44, 47, 49, 52, 54, 56, 58, 59, 61, 62, 62, 63,
  63, 63, 62, 62, 61, 59, 58, 56, 54, 52, 49, 47, 44, 41, 38, 35,
  32, 29, 26, 23, 20, 17, 15, 12, 10,  8,  6,  5,  3,  2,  2,  1,
   1,  1,  2,  2,  3,  5,  6,  8, 10, 12, 15, 17, 20, 23, 26, 29
};

// Initializes PB5–PB0 as digital outputs driving the 6-bit R/2R DAC.
// 8 mA drive strength is enabled to ensure the DAC resistor network
// gets enough current for accurate voltage levels.
void DAC_Init(void) {
    unsigned long volatile delay;
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;
    delay = SYSCTL_RCGC2_R;           // dummy read to let clock stabilize
    GPIO_PORTB_AMSEL_R &= ~0x3F;      // disable analog mode on PB5-0
    GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; // use pins as GPIO, no peripheral
    GPIO_PORTB_DIR_R |= 0x3F;         // PB5-0 as outputs
    GPIO_PORTB_AFSEL_R &= ~0x3F;      // disable alternate functions
    GPIO_PORTB_DEN_R |= 0x3F;         // enable digital I/O
    GPIO_PORTB_DR8R_R |= 0x3F;        // 8 mA drive for DAC resistor network
}

// Configures SysTick for periodic interrupts used to clock sine wave output.
// Does not start the timer — Sound_Start() enables it with a specific period.
// Priority is set to 1 so it can interrupt lower-priority GPIO handlers.
void Sound_Init(void) {
    Index = 0;
    NVIC_ST_CTRL_R = 0;          // disable SysTick before configuring
    NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x20000000;  // priority 1
    NVIC_ST_CTRL_R |= NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_INTEN;    // use core clock, enable interrupt (timer still off)
}

// Stops sine wave output by disabling the SysTick counter.
// DAC is left at its last value; uncommenting the middle line would
// set it to midscale (silence) to avoid a DC click on the output.
void Sound_Stop(void) {
    NVIC_ST_CTRL_R &= ~NVIC_ST_CTRL_ENABLE;
    // DAC = 32; // optional: return DAC to midpoint to avoid DC offset
}

// Starts sine wave output at the frequency defined by period.
// period = (Tone_Tab[note] / 64) — the per-sample SysTick interval in clock cycles.
// Writing 0 to CURRENT clears it and forces an immediate reload.
void Sound_Start(unsigned long period) {
    NVIC_ST_RELOAD_R = period - 1;  // reload value (period - 1 per TM4C datasheet)
    NVIC_ST_CURRENT_R = 0;          // clear and restart counter
    NVIC_ST_CTRL_R |= NVIC_ST_CTRL_ENABLE;  // start SysTick
}

// SysTick ISR — fires once per sample period.
// Advances the sine table index (wraps at 64 using bitwise AND),
// then writes the next sample to the DAC to reconstruct the waveform.
// PF3 toggle is used for oscilloscope debugging to verify interrupt rate.
void SysTick_Handler(void) {
    GPIO_PORTF_DATA_R ^= 0x08;      // toggle green LED (PF3) for debug
    Index = (Index + 1) & 0x3F;     // advance index, wrap 0–63
    DAC = SineWave[Index];           // output next sine sample to DAC
}
