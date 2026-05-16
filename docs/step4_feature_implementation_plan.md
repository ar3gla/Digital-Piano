# Project 1 Step 4 - Feature Implementation Plan & Task Assignment

## Module Overview
| Module Name | Description | Primary Requirements |
|------------|-------------|---------------------|
| main_p1.c | Main program for System 1 (square wave) | Implement square wave system, toggle GPIO in timer ISR |
| main_p2.c | Main program for System 2 (DAC sine wave) | Implement DAC sine wave system, 6-bit R2R with 64-sample sine table |
| SysTick.c | SysTick timer configuration and interrupt handling | Hardware timer interrupts, waveform generation timing |
| GPIO.c | Piano keys and switches (GPIO interrupts) | Piano key input, switch interrupts for mode/octave/song selection |
| PLL.c | Phase-locked loop for clock configuration | Configure system clock between 16-80 MHz |
| Music.c | Song data tables and auto-play song sequences | Store songs with note+duration tables, auto-play logic |

---

## Task Assignments
| Team Member | Modules Assigned | Requirements Owned |
|------------|------------------|--------------------|
| Alfredo Regla | All modules | Full system implementation including clock config, GPIO, SysTick, music logic, and DAC |

---

## Detailed Requirements by Module

### main_p1.c (System 1 - Square Wave)
- Implement square wave system with same user behavior as System 2
- Default to Piano mode on reset
- Toggle GPIO in timer ISR to generate square wave at correct note frequency

### main_p2.c (System 2 - DAC Sine Wave)
- Implement DAC sine wave system with same user behavior as System 1
- Default to Piano mode on reset
- Use 6-bit R2R DAC with 64-sample sine table; timer ISR advances sample index

### GPIO.c
- Piano keys press-and-hold plays continuously; release stops immediately
- Switch 1 toggles Piano ↔ Auto-play mode
- Switch 2 cycles octaves (Lower→Middle→Upper) in Piano mode, starts in Lower after reset
- Switch 2 cycles songs in Auto-play mode; restarts immediately
- Use interrupts for push buttons/switches

### SysTick.c
- Use interrupts for hardware timer
- Timer ISR for square wave generation (System 1)
- Timer ISR for DAC sample updates (System 2)
- Timer for song note duration tracking (quarter note = 4 units)

### PLL.c
- Configure system clock between 16-80 MHz

### Music.c
- Store three songs (Mary Had a Little Lamb, Twinkle Twinkle, Happy Birthday)
- Loop current song until changed
- Start auto-play at octave last used in Piano mode
- Song tables with note + duration (quarter note = 4 units)
- (Extra credit) Add fourth song option

---

## System Constraints (All Team Members)
- Assume only one piano key pressed at a time
- System clock is 40 MHz
- Use interrupts for both hardware timer AND buttons/switches
- Hardware: TM4C123 LaunchPad + 4 piano keys + speaker (System 2 adds amplifier and 6-bit R2R DAC)
