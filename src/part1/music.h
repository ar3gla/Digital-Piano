// File name: music.h
// Course number: CECS 447
// Term: Spring 2026
// Project number: Project 1 â€“ Part 1
// Project description: System P1 (Digital Output): outputs a square wave to drive the speaker
// Team #: 7
// Team members: Adrian Gustilo, Alfredo Regla, Gerald Calero, Paris Talebi
//
// Description:
// This header file defines musical data structures, note constants,
// tone lookup tables, and function prototypes used for sound generation.
// It supports both manual piano input and automatic song playback
// by interfacing with the SysTick timer and GPIO modules.


#ifndef MUSIC_H
#define MUSIC_H
#include <stdint.h>

struct Note {
  uint8_t tone_index;
  uint8_t delay;
};
typedef const struct Note NTyp;

#define NUM_SONGS 4
#define PAUSE 255

#define PIANO 0
#define AUTOPLAY 1

extern volatile uint8_t mode;
extern volatile uint8_t current_octave;
extern const uint32_t Tone_Tab[];
extern NTyp mysong[][255];

void autoplay(NTyp scoretab[]);
void piano_mode(void);
void Speaker_Init(void);
void Delay(void);

#define C4 0
#define D4 1
#define E4 2
#define F4 3 
#define G4 4
#define A4 5
#define B4 6
#define C5 0+7
#define D5 1+7
#define E5 2+7
#define F5 3+7
#define G5 4+7
#define A5 5+7
#define B5 6+7
#define C6 0+2*7
#define D6 1+2*7
#define E6 2+2*7
#define F6 3+2*7
#define G6 4+2*7
#define A6 5+2*7
#define B6 6+2*7
#define C7 0+3*7
#define Bb5 (B5-1)
#define Gb6 (G6-1)
#define Eb6 (E6-1)
#define Ab5 (A5-1)

#endif
