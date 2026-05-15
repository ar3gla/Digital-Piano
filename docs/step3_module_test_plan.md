# Step 3 – Module-by-Module Test Plan

## Project
CECS 447 – Project 1: Digital Piano

## Purpose
This document defines the independent test plan for each software module in the Digital Piano project.
Each module is tested in isolation using ModuleTest.c prior to full system integration.

---

## Module Overview

| Module Name | Description |
|------------|-------------|
| GPIO_Input | Piano keys and switches (GPIO interrupts) |
| GPIO_Output | LED indicators and speaker control |
| SysTick | Timer-based square wave generation |
| Music | Tone tables and song playback |

---

## Test Environment

- Hardware: TM4C123 LaunchPad
- Clock: 40 MHz
- Tools: Keil, Visual Studio Code
- Observation: LEDs, Switches, Speaker output

---

## Module Test Cases

### Module: GPIO_Input

| Test ID | Input | Expected Output |
|-------|-------|-----------------|
| GPIO-01 | Press PE0 | C note detected |
| GPIO-02 | Release PE0 | Stop event detected |
| GPIO-03 | Press PF4 | Mode toggle detected |
| GPIO-04 | Press PF0 | Octave/song change detected |

Edge cases:
- Button bounce → single event only
- No input → no interrupts

Owner: Alfredo Regla  
Reviewer: Alfredo Regla  
Acceptance: All events detected correctly

---

### Module: GPIO_Output

| Test ID | Input | Expected Output |
|-------|-------|-----------------|
| LED-01 | Set PF1 | Red LED on |
| LED-02 | Set PF2 | Blue LED on |
| LED-03 | Set PF3 | Green LED on |

Edge cases:
- Invalid pin → no change
- All pins off → LEDs off

Owner: Alfredo Regla  
Reviewer: Alfredo Regla  
Acceptance: All LEDs respond correctly

---

### Module: SysTick

| Test ID | Period | Expected |
|-------|--------|----------|
| TMR-01 | C4 (76335) | Correct toggle rate |
| TMR-02 | E4 (60605) | Correct toggle rate |
| TMR-03 | Start/Stop | Enable/disable works |

Edge cases:
- Timer disabled → no toggles
- Invalid period → safe handling

Owner: Alfredo Regla  
Reviewer: Alfredo Regla  
Acceptance: ±5% accuracy

---

### Module: Music

| Test ID | Input | Expected Output |
|-------|-------|-----------------|
| MUS-01 | Play Song 0 | Happy Birthday plays |
| MUS-02 | Play Song 1 | Mary Had a Little Lamb plays |
| MUS-03 | Change song | New song starts |

Edge cases:
- Invalid song index → no playback
- Mode change → song stops

Owner: Alfredo Regla  
Reviewer: Gerald Calero  
Acceptance: All songs play correctly

---

## Evidence
Evidence stored in /evidence folder.

---
