# CECS 447 – Project 1: Digital Piano & Music Box

**Course:** CECS 447 - Embedded Systems  
**Project:** Project 1 – Digital Piano  
**Instructor:** Dr. Min He  
**Author:** Alfredo Regla  
**Institution:** California State University, Long Beach

---

## Overview

A digital piano and music box implemented on the TM4C123 LaunchPad in two audio output configurations.

| System | Audio Method | Output Pin |
|--------|-------------|------------|
| Part 1 (P1) | Square wave via GPIO toggle under SysTick interrupt | PA5 |
| Part 2 (P2) | Sine wave via 6-bit R-2R DAC with 64-sample lookup table | PB5–PB0 |

Both systems run at 40 MHz, support four piano keys (PE0–PE3), two modes (Piano / Auto-Play), three octaves, and four auto-play songs.

### Features

- **Piano Mode** – Press PE0–PE3 to play notes C, D, E, F; SW2 cycles octaves
- **Auto-Play Mode** – SW1 toggles modes; SW2 cycles through 4 songs
- **LED indicators** – PF1 (Piano), PF2 (Auto-Play), PF3 (note active)

---

## Repository Structure

```
├── src/
│   ├── startup.s               # TM4C123 startup assembly
│   ├── tm4c123gh6pm.h          # TM4C123 register definitions
│   ├── common/
│   │   ├── module_test.c       # Module test harness
│   │   ├── pll.c / pll.h       # 40 MHz PLL clock config
│   ├── part1/                  # System P1 – Square wave
│   │   ├── main_p1.c
│   │   ├── gpio.c / gpio.h
│   │   ├── music.c / music.h
│   │   └── systick.c / systick.h
│   └── part2/                  # System P2 – DAC sine wave
│       ├── main_p2.c
│       ├── gpio.c / gpio.h
│       ├── music.c / music.h
│       └── systick.c / systick.h
├── keil/
│   ├── part1/project_p1.uvprojx
│   └── part2/project_p2.uvprojx
├── docs/
│   ├── step2_system_design.md
│   ├── step3_module_test_plan.md
│   ├── step4_feature_implementation_plan.md
│   ├── step5_cer_evidence_pack.md
│   ├── step6_submission_checklist.md
│   └── traceability.csv
├── evidence/
│   └── README.md
└── scripts/
    └── score_traceability.py
```

---

## Hardware

- TM4C123GH6PM LaunchPad (40 MHz)
- 4× tactile push buttons (PE0–PE3, piano keys)
- LM386 audio amplifier + 8Ω speaker
- 6-bit R-2R ladder DAC (P2 only) — 1kΩ/2kΩ resistors on PB5–PB0
