# Project 1: Digital Piano & Music Box

**Group #7**
California State University Long Beach, College of Engineering
Alfredo Regla
CECS 447 - Embedded Systems III
Instructor: Dr. Min He
February 18, 2026

---

## Table of Contents

1. [Introduction](#introduction)
2. [System Design and Architecture](#system-design-and-architecture)
3. [Hardware-Software Interaction](#hardware-software-interaction)
4. [Functional Description of Each Module](#functional-description-of-each-module)
5. [Development Steps and Task Assignments](#development-steps-and-task-assignments)
6. [Hardware Design](#hardware-design)
7. [Software Design](#software-design)
8. [Demonstration](#demonstration)
9. [Conclusion](#conclusion)
10. [References](#references)

---

## Introduction

This project implements a Digital Piano and Music Box embedded system using the TM4C123 LaunchPad microcontroller, configured to run at 40 MHz via the on-chip PLL. The project is divided into two systems that share the same user-facing behavior but differ in how audio is generated.

System P1 generates audio by toggling a GPIO output pin (PA2) at the desired note frequency using SysTick timer interrupts, producing a square wave that is amplified by an LM386 audio amplifier and output to a speaker. System P2 generates a higher-quality analog sine wave by stepping through a 64-sample sinusoid lookup table and writing successive 6-bit values to a resistor-ladder DAC built on Port B, with each sample output triggered by a SysTick interrupt.

Both systems support two operating modes controlled by the onboard switches. In Piano Mode (default on reset), the user presses and holds one of four external push buttons connected to Port E to play the assigned note (C, D, E, or F) continuously; releasing the button stops the sound immediately. SW2 cycles through three octaves (Lower C, Middle C, Upper C) in round-robin order. In Auto-Play Mode, the system automatically plays one of four pre-programmed songs: Mary Had a Little Lamb, Happy Birthday, Twinkle Twinkle Little Star, and a video game song (Mario theme) as extra credit. SW1 toggles between modes, resets the song to the beginning, and SW2 advances song selection during Auto-Play.

---

## System Design and Architecture

Both systems share a common modular architecture built around the TM4C123 LaunchPad. The major subsystems are:

**Input Subsystem (GPIO Push Buttons and Onboard Switches):** Four external push buttons on Port E (PE0-PE3) serve as piano keys. The onboard SW1 (PF4) toggles between Piano and Auto-Play modes; SW2 (PF0) cycles octaves in Piano Mode or advances the song in Auto-Play Mode. All inputs are interrupt-driven.

**Audio Output - System P1 (Square Wave):** A single GPIO output pin PA2 is toggled by the SysTick ISR at each half-period of the target note, producing a square wave signal sent through an LM386 amplifier to the speaker.

**Audio Output - System P2 (Sine Wave via 6-bit R-2R DAC):** Port B pins PB5-PB0 drive a 6-bit R-2R resistor ladder DAC. The SysTick ISR advances a 64-sample sine wavetable index on each interrupt and writes the current sample value to Port B, producing a staircase approximation of a sine wave. The interrupt period is set to 1/64 of the desired note period so that 64 interrupts complete one full sine wave cycle at the target frequency.

**Timer and Interrupt Control:** SysTick drives all audio output in both systems. GPIO interrupts on Port E and Port F handle all user input. All time-critical operations use interrupt-driven architecture.

**Music Playback Engine:** A shared software module (music.c) stores note-duration sequences for four songs as arrays of NTyp structures. The autoplay() function sequences notes by configuring SysTick for each note and using a calibrated software delay loop for note duration. Song and mode changes are communicated via volatile shared variables and take effect immediately.

---

## Hardware-Software Interaction

The TM4C123 interfaces with all hardware peripherals through GPIO registers, SysTick timer interrupts, and the R-2R DAC as described below.

### System P1 Signal Flow

When the user presses a piano key on Port E, a GPIO interrupt fires and the GPIOPortE_Handler() ISR identifies the note. It calls SysTick_Set_Current_Note() to load the half-period reload value from Tone_Tab[] into NVIC_ST_RELOAD_R, then enables SysTick. On every SysTick interrupt, SysTick_Handler() toggles PA2, producing a square wave at the correct note frequency. Releasing the key disables SysTick and stops the tone. The LM386 amplifier receives the PA2 signal and drives the 8-ohm speaker.

### System P2 Signal Flow

When the user presses a piano key, GPIOPortE_Handler() calls Sound_Start() with a period value equal to Tone_Tab[note_index] / 64. This divides the full note period by 64 so that 64 SysTick interrupts complete exactly one sine wave cycle at the target frequency. On every SysTick interrupt, SysTick_Handler() increments the wavetable index (modulo 64) and writes SineWave[Index] to the DAC register (Port B bits 5-0). The R-2R resistor ladder converts the 6-bit digital value to an analog voltage step. The LM386 amplifier smooths and amplifies this staircase waveform to drive the speaker. When the key is released, Sound_Stop() disables SysTick and the DAC output holds its last value.

### Mode and Song Control

SW1 (PF4) triggers GPIOPortF_Handler() which toggles the volatile mode variable between PIANO and AUTOPLAY, calls Sound_Stop() to silence audio immediately, resets current_song to 0 so the song restarts from the beginning on re-entry, and updates the LED color (red for Piano Mode, blue for Auto-Play Mode). SW2 (PF0) cycles current_octave in Piano Mode or increments current_song in Auto-Play Mode. In Auto-Play Mode the main loop calls autoplay() which checks both current_song and mode on every note iteration, returning immediately if either changes.

---

## Functional Description of Each Module

### 1. SysTick / Sound Module (SysTick.c)

This file serves different roles in P1 and P2 but is built on the same SysTick peripheral.

In System P1, SysTick_Init() disables SysTick, sets interrupt priority to 2, and enables the system clock source and interrupt enable bits. SysTick_Set_Current_Note() loads the half-period reload value and clears the current count. SysTick_Handler() toggles PA2 on every interrupt to produce the square wave.

In System P2, Sound_Init() similarly configures SysTick but sets priority to 1. DAC_Init() enables Port B, configures PB5-PB0 as digital outputs with 8 mA drive strength, and disables analog and alternate functions. Sound_Start(period) sets NVIC_ST_RELOAD_R to the per-sample period (Tone_Tab[note] / 64) and enables SysTick. Sound_Stop() clears the enable bit. SysTick_Handler() increments the 64-entry wavetable index using a bitwise AND mask (Index = (Index+1) & 0x3F) and writes SineWave[Index] to the DAC port register. A debug toggle of PF3 is also performed in the ISR.

### 2. DAC Module (SysTick.c - DAC_Init, System P2 only)

The 6-bit R-2R ladder DAC is implemented in hardware using resistors on a breadboard, with the six output bits connected to PB5-PB0. DAC_Init() configures Port B: enables the GPIO clock, disables analog mode and alternate functions on PB5-PB0, sets all six pins as outputs, and enables 8 mA drive strength to ensure the pins can source the current required by the resistor ladder without voltage droop. The DAC macro is defined as a bit-banded address covering all six Port B data bits:

```c
#define DAC (*((volatile unsigned long *)0x400050FC))
```

### 3. Sine Wave Lookup Table (SysTick.c)

The 64-sample sinusoid is stored as a constant array of unsigned chars with values ranging from 1 to 63 (6-bit range). The waveform is centered at 32 (midpoint of 0-63) and uses the full dynamic range of the DAC. The table is computed to represent one complete cycle of a sine wave. Using a lookup table avoids runtime trigonometric computation in the ISR, ensuring the interrupt handler completes quickly and consistently.

### 4. GPIO Module (GPIO.c)

PortF_Init() configures PF0 and PF4 as inputs with internal pull-up resistors and falling-edge interrupts at NVIC priority 2. PF1, PF2, and PF3 are configured as outputs for the onboard RGB LED. In P2, the initial LED state is set to blue (PF2); in P1 it is red (PF1). GPIOPortF_Handler() debounces SW1 and SW2 using a 10 ms software delay loop calibrated for 40 MHz (133,333 iterations), then reads the confirmed pin state before acting.

PortE_Init() configures PE0-PE3 as inputs with both-edge interrupts at NVIC priority 1, so each key generates an interrupt on both press and release. GPIOPortE_Handler() checks the raw interrupt status register (GPIO_PORTE_RIS_R) to identify which pin changed, clears the interrupt flag, and reads the data register to determine press or release. On press, it calls Sound_Start() (P2) or SysTick_Set_Current_Note()+SysTick_start() (P1) with the Tone_Tab value for the corresponding note at the current octave offset. On release, it calls Sound_Stop() or SysTick_stop().

### 5. Music Module (music.c)

Tone_Tab[] stores the SysTick reload values for all notes across four octaves. For System P1 these are half-period values (clock / (2 × frequency)); for System P2 they are full-period values (clock / frequency) which are then divided by 64 when passed to Sound_Start() to produce the per-sample interrupt period. Both sets are calibrated for a 40 MHz system clock.

mysong[][] stores four song tables as NTyp arrays, where each entry holds a tone index and a duration multiplier. The songs are: Mary Had a Little Lamb, Happy Birthday, Twinkle Twinkle Little Star, and a Mario theme (extra credit). The autoplay() function iterates through the active song, configures audio for each note, waits using a Delay() loop scaled by the duration value, and stops audio between notes. Song switching and mode changes are detected each iteration via volatile variables.

### 6. PLL Module (PLL.c)

PLL_Init() configures the TM4C123 to run at 40 MHz by setting SYSDIV2 to 4 in the RCC2 register, producing a 400 MHz VCO output divided by 2 then divided by 5. All timing constants in Tone_Tab[] and Delay() are calibrated to this 40 MHz clock. PLL_Init() is called first in System_Init() before any peripheral that depends on the system clock frequency.

---

## Development Steps and Task Assignments

The project was developed in the following sequential steps:

1. Hardware schematic design and component selection, including R-2R DAC resistor values and LM386 amplifier circuit for both systems.
2. Individual module development: GPIO configuration (Port E and Port F), SysTick/Sound driver for P1 square wave output, DAC initialization and SysTick sine wave driver for P2, and the shared music playback engine.
3. Unit testing of each module: verifying GPIO interrupt triggering on both edges, measuring SysTick toggle frequency on an oscilloscope against expected note frequencies, and verifying DAC output waveform shape for P2.
4. Hardware integration: connecting push buttons to PE0-PE3, wiring the R-2R DAC from PB5-PB0 to the resistor network, connecting the LM386 amplifier, and verifying the speaker output.
5. Full system testing: validating all four piano keys across three octaves, mode switching, song auto-play with correct note sequences and durations, and mid-song song switching.
6. Performance optimization: recalibrating Tone_Tab[] reload values for 40 MHz, adjusting the Delay() loop constant for correct note durations, and verifying no frequency drift between notes.

### Task Assignment Table

| Team Member | Responsibility | Assigned Tasks |
|---|---|---|
| Alfredo Regla | Software, Hardware & Testing | Music playback engine, Tone_Tab calibration, GPIO interrupts, DAC circuit, System_Init() integration, audio verification |

---

## Hardware Design

### Schematic Diagram
<img width="558" height="554" alt="image" src="https://github.com/user-attachments/assets/62318cef-dda6-418b-acfd-cd07a3689874" />



### 6-bit R-2R DAC Description (System P2)

The 6-bit R-2R ladder DAC is constructed on a breadboard using 1k ohm (R) and 2k ohm (2R) resistors. The six DAC output bits are connected to PB5 (MSB) through PB0 (LSB). The R-2R ladder network produces an output voltage proportional to the binary value on the port pins:

> Vout = Vref × (D / 64)

where D is the 6-bit digital value (0-63) and Vref is the 3.3V GPIO output high voltage. The analog output of the DAC feeds into the LM386 audio amplifier input. All Port B output pins driving the DAC are configured with 8 mA drive strength to minimize output impedance effects on the ladder network.

### Bill of Materials (BOM)

| # | Item | Qty | Part Number / Example | Key Specs | Notes |
|---|---|---|---|---|---|
| 1 | TM4C123 LaunchPad | 1 | EK-TM4C123GXL | ARM Cortex-M4F, 40 MHz (PLL), 3.3V I/O | Onboard SW1 (PF4), SW2 (PF0), RGB LED (PF1-PF3) |
| 2 | Push Buttons (Piano Keys) | 4 | Standard Tactile Switch | Active high, 3.3V logic | Connected to PE0-PE3. Optional 10k pull-down resistors |
| 3 | 1k ohm Resistors (R) | 6 | 1/4W through-hole resistor | 1k ohm, 5% | Series resistors in R-2R DAC ladder for System P2 |
| 4 | 2k ohm Resistors (2R) | 7 | 1/4W through-hole resistor | 2k ohm, 5% | Shunt resistors in R-2R DAC ladder for System P2 (includes termination) |
| 5 | Audio Amplifier Module | 1 | LM386-based audio board | 5V supply | Amplifies DAC output (P2) or square wave (P1) to drive the speaker |
| 6 | Speaker | 1 | 8 ohm small speaker | Audio output | Driven by LM386 output |
| 7 | Breadboard / Jumper Wires | 1 set | Half-size breadboard + Dupont jumpers | Wiring | Used to assemble R-2R DAC and connect all peripherals |
| 8 | USB Cable | 1 | Micro-B (for EK-TM4C123GXL) | Data + 5V power | Programs and powers the LaunchPad |

---

## Software Design

### Flowcharts

<img width="620" height="720" alt="image" src="https://github.com/user-attachments/assets/ce8acf30-4073-43e5-9126-dd587824d7e9" />


### Software Architecture Overview

The firmware is organized into the following source files, shared between P1 and P2 with differences noted:

**main_p1.c / main_p2.c:** Entry points for each system. Both call System_Init() and enter the same main loop checking mode to call either autoplay() or piano_mode(). P2's System_Init() additionally calls DAC_Init() and uses Sound_Init() instead of SysTick_Init().

**GPIO.c:** Identical structure between P1 and P2. Configures Port E (both-edge interrupts on PE0-PE3 for piano keys) and Port F (falling-edge interrupts on PF0 and PF4 for switches). P2's GPIOPortE_Handler() calls Sound_Start(Tone_Tab[note] / 64) on key press and Sound_Stop() on release. P1 calls SysTick_Set_Current_Note() and SysTick_start()/stop() instead.

**SysTick.c (P1):** Implements SysTick_Init(), SysTick_start(), SysTick_stop(), SysTick_Set_Current_Note(), and SysTick_Handler() which toggles PA2. Speaker_Init() configures PA2 as a digital output.

**SysTick.c (P2):** Implements DAC_Init(), Sound_Init(), Sound_Start(), Sound_Stop(), and SysTick_Handler() which advances the sine table index and writes to the DAC. Defines the 64-sample SineWave[] lookup table and the DAC bit-band address macro.

**music.c:** Shared between both systems. Contains Tone_Tab[] (different values for P1 vs P2, both at 40 MHz), mysong[][] with four song tables, autoplay(), piano_mode(), and Delay().

**PLL.c:** Configures the system clock to 40 MHz. Called first in System_Init().

### Tone_Tab Reload Value Formulas

For System P1 (square wave), the reload value is the half-period of the note:

```
P1 Reload = 40,000,000 / (2 × NoteFrequency)
```

For System P2 (sine wave), Tone_Tab[] stores the full note period, divided by 64 in Sound_Start():

```
P2 Tone_Tab  = 40,000,000 / NoteFrequency
P2 Period    = Tone_Tab[note] / 64  =  40,000,000 / (64 × NoteFrequency)
```

Example at Middle C (523 Hz):
- P1: 40,000,000 / (2 × 523) = **38,240**
- P2: 40,000,000 / 523 = 76,481 → Sound_Start period = 76,481 / 64 = **1,195**

### Sine Wave Generation Detail

The 64-sample sine table (SineWave[]) contains unsigned 6-bit values from 1 to 63, centered at 32. In SysTick_Handler(), the index is advanced using a bitwise mask:

```c
Index = (Index + 1) & 0x3F;
```

This efficiently wraps from 63 back to 0 without a branch. The DAC is written directly via a bit-banded port register macro covering PB5-PB0. At a system clock of 40 MHz, the SysTick ISR fires 64 times per note period, producing a smooth staircase approximation of the sine wave across the DAC output.

### Note Frequency Table

| Note | Lower C (Hz) | Middle C (Hz) | Upper C (Hz) |
|---|---|---|---|
| C | 262 | 523 | 1046 |
| D | 294 | 587 | 1174 |
| E | 330 | 659 | 1318 |
| F | 349 | 698 | 1396 |

---

## Demonstration

The system was demonstrated with the following verified behaviors for both P1 and P2:

- **Piano Mode - Note Playback:** Each of the four push buttons (PE0-PE3) played the assigned note (C, D, E, F) continuously at the selected octave while held, and stopped immediately on release. P1 produced a clean square wave tone; P2 produced a noticeably smoother sine wave tone through the speaker.
- **Octave Switching:** SW2 cycled through Lower C, Middle C, and Upper C in round-robin order in Piano Mode. The selected octave carried over when entering Auto-Play Mode.
- **Mode Toggle:** SW1 switched between Piano Mode (red LED on PF1) and Auto-Play Mode (blue LED on PF2), with audio stopping immediately and the song resetting to the beginning on each transition.
- **Auto-Play - Mary Had a Little Lamb:** Correct note sequence and durations, including the held notes at the end of each phrase.
- **Auto-Play - Happy Birthday:** Correct melody including the short-long rhythm pattern at the start of each phrase.
- **Auto-Play - Twinkle Twinkle Little Star:** Full two-verse sequence played correctly.
- **Auto-Play - Mario Theme (Extra Credit):** The Mario overworld theme played using notes up to C7, with correct rhythm and phrasing. Mid-song song switching immediately restarted the newly selected song.
- **Song Switching:** Pressing SW2 during Auto-Play immediately halted the current song and began the next song from its beginning.

Video Link for System P1: https://www.youtube.com/shorts/dqJ20yZ9qxA

Video Link for System P2: https://www.youtube.com/shorts/gTUu-QBb1-s

---

## Conclusion

This project successfully implemented a Digital Piano and Music Box on the TM4C123 LaunchPad in two audio output configurations. System P1 demonstrated square wave audio generation through direct GPIO toggling under SysTick interrupt control. System P2 demonstrated higher-quality analog audio generation using a 6-bit R-2R ladder DAC driven by SysTick-timed samples from a 64-entry sine wave lookup table.

Both systems operate at 40 MHz. In P1, SysTick reload values in Tone_Tab[] are half-period values at 40 MHz. In P2, Tone_Tab[] stores full-period values which are divided by 64 when loading Sound_Start(), ensuring each of the 64 sine samples is output at the correct interval to produce the target note frequency. The Delay() loop used for note duration timing was independently calibrated to 40 MHz for consistent song tempo.

Key lessons learned include the importance of matching all timing constants to the configured system clock, the design trade-off between square wave simplicity (P1) and sine wave audio quality (P2), the use of bitwise masking for efficient circular buffer indexing in ISRs, and the need to validate interrupt-driven audio frequency with an oscilloscope rather than relying solely on calculated values. Future enhancements could include adding more songs, a display showing the current note and song, or polyphony using multiple hardware timers.

---

## References

Texas Instruments. "TM4C123GH6PM Microcontroller Data Sheet." Texas Instruments Incorporated, 2025. https://www.ti.com/lit/ds/spms376e/spms376e.pdf

Texas Instruments. "EK-TM4C123GXL LaunchPad Evaluation Kit User Guide." Texas Instruments Incorporated, 2025. https://www.ti.com/tool/EK-TM4C123GXL

Valvano, Jonathan W. "Embedded Systems: Introduction to ARM Cortex-M Microcontrollers." 5th ed., 2014.

ChatGPT (OpenAI). "Assistance with Lab Report Formatting, Documentation Structure, and Grammar Review for CECS 447 Project 1."
