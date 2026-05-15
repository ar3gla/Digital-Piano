// File name: music.c
// Course number: CECS 447
// Term: Spring 2026
// Project number: Project 1 – Part 2
// Project description: System P2 (Analog Output): outputs a sine wave using a 6-bit R/2R DAC
// Team #: 7
// Team members: Adrian Gustilo, Alfredo Regla, Gerald Calero, Paris Talebi
//
// Description:
// This file implements music playback functionality including
// automatic song playback, piano mode behavior, and timing delays.
// The module uses SysTick interrupts to step through a 64-sample
// sine wave lookup table, outputting analog values to the 6-bit DAC.

#include "tm4c123gh6pm.h"
#include "SysTick.h"
#include "music.h"
#include <stdint.h>

// Owned by GPIO.c — which song is selected and current operating mode
extern volatile uint8_t current_song;
extern volatile uint8_t mode;

// Tone table: SysTick reload values for each note across octaves C4–C7.
// Each value represents the period needed to step through the 64-sample sine table
// at the correct frequency. Dividing by 64 in the caller gives the per-sample interval.
const uint32_t Tone_Tab[] = {
    190838, 170070, 151513, 143270, 127550, 113638, 101213,  // C4–B4 (indices 0-6)
     95419,  85035,  75758,  71635,  63775,  56820,  50608,  // C5–B5 (indices 7-13)
     47710,  42518,  37879,  35818,  31888,  28410,  25303,  // C6–B6 (indices 14-20)
     23855,  21259,  18939,                                   // C7–D7 (indices 21-23)
};

// Song table: each song is an array of NTyp structs {tone_index, delay}.
// A tone_index of PAUSE silences output; delay is in units of one Delay() call.
// Each song is terminated by a {0, 0} sentinel entry.
NTyp mysong[][255] = {

    // Song 0: Mary Had a Little Lamb
    {E4,4,D4,4,C4,4,D4,4,E4,3,PAUSE,1,E4,3,PAUSE,1,E4,8,
     D4,3,PAUSE,1,D4,3,PAUSE,1,D4,8,E4,4,G4,8,
     E4,4,D4,4,C4,4,D4,4,E4,3,PAUSE,1,E4,3,PAUSE,1,E4,8,
     D4,3,PAUSE,1,D4,4,E4,4,D4,4,C4,8,0,0},

    // Song 1: Happy Birthday
    {C4,1,PAUSE,1,C4,2,D4,4,C4,4,F4,4,E4,8,
     PAUSE,4,
     C4,1,PAUSE,1,C4,2,D4,4,C4,4,G4,4,F4,8,
     PAUSE,4,
     C4,1,PAUSE,1,C4,2,C5,4,A4,4,F4,4,E4,4,D4,8,
     PAUSE,4,
     B4,1,PAUSE,1,B4,2,A4,4,F4,4,G4,4,F4,12,0,0},

    // Song 2: Twinkle Twinkle Little Star
    {C4,3,PAUSE,1,C4,4,G4,3,PAUSE,1,G4,4,A4,3,PAUSE,1,A4,4,G4,8,
     F4,3,PAUSE,1,F4,4,E4,3,PAUSE,1,E4,4,D4,3,PAUSE,1,D4,4,C4,8,
     G4,3,PAUSE,1,G4,4,F4,3,PAUSE,1,F4,4,E4,3,PAUSE,1,E4,4,D4,8,
     G4,3,PAUSE,1,G4,4,F4,3,PAUSE,1,F4,4,E4,3,PAUSE,1,E4,4,D4,8,
     C4,3,PAUSE,1,C4,4,G4,3,PAUSE,1,G4,4,A4,3,PAUSE,1,A4,4,G4,8,
     F4,3,PAUSE,1,F4,4,E4,3,PAUSE,1,E4,4,D4,3,PAUSE,1,D4,4,C4,8,0,0},

    // Song 3: Super Mario Bros. Theme (main melody)
    {E6,1,PAUSE,1,E6,1,PAUSE,1,E6,2,C6,1,E6,2,
     G6,4,PAUSE,4,
     G5,4,PAUSE,4,
     C6,3,G5,3,E5,3,
     A5,2,B5,2,Bb5,1,A5,2,
     G5,1,E6,1,G6,1,A6,2,F6,1,G6,2,
     E6,2,C6,1,D6,1,B5,3,
     C6,3,G5,3,E5,3,
     A5,2,B5,2,Bb5,1,A5,2,
     G5,1,E6,1,G6,1,A6,2,F6,1,G6,2,
     E6,2,C6,1,D6,1,B5,3,
     PAUSE,4,
     G6,1,PAUSE,1,Gb6,2,F6,2,Eb6,2,E6,1,
     Ab5,1,A5,1,C6,2,A5,1,C6,1,D6,3,
     G6,1,PAUSE,1,Gb6,2,F6,2,Eb6,2,E6,1,
     C7,1,PAUSE,1,C7,1,PAUSE,1,C7,4,
     G6,1,PAUSE,1,Gb6,2,F6,2,Eb6,2,E6,1,
     Ab5,1,A5,1,C6,2,A5,1,C6,1,D6,3,
     Eb6,3,D6,3,C6,4,
     PAUSE,4,
     0,0}
};

// Plays through a song array one note at a time.
// Captures current_song and mode at entry so it can bail out early
// if the user switches songs or changes mode mid-playback.
void autoplay(NTyp scoretab[])
{
    uint8_t i = 0, j;
    uint8_t song_at_start = current_song;   // snapshot to detect song change
    uint8_t octave_offset = current_octave * 7; // shift tone table index by octave

    while (scoretab[i].delay) {
        // Exit immediately if user changed song or left AUTOPLAY mode
        if (song_at_start != current_song || mode != AUTOPLAY) {
            return;
        }

        if (scoretab[i].tone_index == PAUSE)
            Sound_Stop();                   // silent beat
        else
            Sound_Start(Tone_Tab[scoretab[i].tone_index + octave_offset] / 64);

        // Hold the note for its specified duration (each unit = one Delay() call)
        for (j = 0; j < scoretab[i].delay; j++)
            Delay();

        Sound_Stop();   // brief gap between notes prevents them from blurring together
        i++;
    }

    // Short silence at end of song before looping or returning
    for (j = 0; j < 15; j++)
        Delay();
}

// Keeps the system alive in PIANO mode.
// Waits in a loop until mode changes; if all piano keys are released,
// stops sound and turns off the green LED (PF3).
void piano_mode(void)
{
    while (mode == PIANO) {
        if ((GPIO_PORTE_DATA_R & 0x0F) == 0x0F) {  // all four keys released (active-high)
            Sound_Stop();
            GPIO_PORTF_DATA_R &= ~0x08;             // green LED off
        }
    }
}

// Busy-wait delay — one call ≈ one note duration unit used by autoplay().
// The constant (727240 * 20 / 91 * 2.5) is tuned to produce the desired
// tempo at 80 MHz system clock.
void Delay(void)
{
    uint32_t volatile time;
    time = 727240 * 20 / 91 * 2.5;
    while (time) {
        time--;
    }
}
