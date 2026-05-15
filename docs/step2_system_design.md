# Step 2 — System Design 

**Project:** CECS 447 – Project 1: Digital Piano

---

## Solution A — Digital Square Wave Output (System P1)

**High-level description:**  
This solution implements System P1, which generates digital square wave signals directly from a GPIO pin to drive the speaker through an amplifier. The system uses a SysTick timer interrupt running at 2× the note frequency to toggle the output pin, creating precise square wave tones for each musical note. Note durations are controlled by software loops, and GPIO interrupts handle user input from piano keys and mode switches.

---

## 1. Hardware Design

### GPIO Pin Assignment
- PA5: Square wave output to speaker/amplifier
- PE0: Piano key C input with interrupt
- PE1: Piano key D input with interrupt
- PE2: Piano key E input with interrupt
- PE3: Piano key F input with interrupt
- PF4: Switch 1 input with interrupt (switches between piano and auto play)
- PF0: Switch 2 input with interrupt (octave/song selection)
- PF1: LED output (Piano mode indicator)
- PF2: LED output (Auto-play mode indicator)
- PF3: LED output (Note playing indicator)

---

### Peripherals Used
- **SysTick Timer** — Generates periodic interrupts at 2× note frequency for square wave generation
- **GPIO Port A** — Digital output (PA5) for square wave generation
- **GPIO Port E** — Digital inputs with interrupts (PE0-PE3) for piano key detection
- **GPIO Port F** — Digital inputs with interrupts (PF0, PF4) for mode/octave switches; Digital outputs (PF1-PF3) for LEDs

---

### External Components
- **Speaker** — Driven by the amplifier to produce audible sound
- **4 Push Buttons** — Piano Keys (C, D, E, F) connected to Port E
- **Amplifier** — Amplifies the digital square wave signal from PA5

---

## 2. Software Architecture

### Modules and Interfaces (API)

**SysTick.c / .h**
- `SysTick_Init(void)` — Initialize SysTick timer
- `SysTick_start(void)` — Start SysTick timer
- `SysTick_stop(void)` — Stop SysTick timer
- `SysTick_Set_Current_Note(unsigned long n_value)` — Set reload value for note frequency
- `SysTick_Handler(void)` — ISR for square wave generation (toggles PA5)

**GPIO.c / .h**
- `GPIO_Init()` — Configure all GPIO pins
- `PortE_Handler()` — ISR for piano key interrupts
- `PortF_Handler()` — ISR for mode/octave switches

**Music.c / .h**
- `play_a_song(void)` — Play predefined song
- `Music_Init(void)` — Initialize music system

---

### Data Flow

<img width="1079" height="618" alt="image" src="https://github.com/user-attachments/assets/74eccaf3-612b-4daf-8cd4-bfb99ab9b505" />

### Call Graph


<img width="1112" height="883" alt="image" src="https://github.com/user-attachments/assets/0a068f4f-79c9-4dab-a269-93b33c7b7ece" />


## 3. Timing and Accuracy Considerations

The SysTick timer runs at a 32 MHz system clock frequency, providing sub-microsecond timing resolution with minimal interrupt latency. For square wave generation, SysTick interrupts occur at 2× the note frequency to toggle the PA5 output pin, creating the desired square wave. For example, for middle C (523 Hz), the SysTick interrupt fires at 1046 Hz. Note durations are controlled by software loops that count the appropriate number of cycles based on the tempo and note length specified in the song data.

---

## 4. Advantages and Tradeoffs

### Advantages
- Simple hardware implementation requiring only GPIO pins
- No external DAC components needed
- Easier to debug and verify signal generation
- Onboard LEDs simplify mode indication

### Tradeoffs
- Square wave produces a  tone quality compared to sine waves
- Higher CPU overhead due to frequent SysTick interrupts
- Software loops for note duration cause blocking behavior

---

## 5. Intended Use

This solution serves as the System P1 implementation for the project, demonstrating digital square wave generation using GPIO and timer interrupts. It provides a baseline implementation that is simple to construct and debug, making it ideal for validating the core functionality of the digital piano including user input handling, mode switching, and basic audio output. The simplicity of this approach allows for rapid prototyping and testing before moving to more complex analog implementations.

---

---

## Solution B — Analog Sine Wave Output via DAC (System P2)

**High-level description:**  
This solution implements System P2, which generates analog sine wave signals using a 6-bit R-2R ladder DAC. The system uses a SysTick timer interrupt running at 64× the note frequency to sequentially output values from a 64-sample sine lookup table to the DAC, reconstructing a smooth analog sine wave. This approach produces higher-quality audio with fewer harmonics compared to square wave generation, though it requires additional external components and more complex hardware setup.

---

## 1. Hardware Design

### GPIO Pin Assignment
- PB0-PB5: 6-bit DAC output (digital sine wave samples)
- PE0: Piano key C input with interrupt
- PE1: Piano key D input with interrupt
- PE2: Piano key E input with interrupt
- PE3: Piano key F input with interrupt
- PF4: Switch 1 input with interrupt (switches between piano and auto play)
- PF0: Switch 2 input with interrupt (octave/song selection)
- PF1: LED output (Piano mode indicator)
- PF2: LED output (Auto-play mode indicator)
- PF3: LED output (Note playing indicator)

---

### Peripherals Used
- **SysTick Timer** — Generates periodic interrupts at 64× note frequency for sine wave sample output
- **GPIO Port B** — 6-bit digital output (PB0-PB5) for DAC control
- **GPIO Port E** — Digital inputs with interrupts (PE0-PE3) for piano key detection
- **GPIO Port F** — Digital inputs with interrupts (PF0, PF4) for mode/octave switches; Digital outputs (PF1-PF3) for LEDs
- **6-bit R-2R DAC** — Converts 64-level digital sine wave samples to analog voltage output

---

### External Components
- **6-bit R-2R Ladder DAC** — Constructed with resistors (typically 10kΩ and 20kΩ values) to convert digital signals to analog voltage
- **Speaker** — Driven by the amplifier to produce audible sound
- **4 Push Buttons** — Piano Keys (C, D, E, F) connected to Port E
- **Amplifier** — Amplifies the analog sine wave signal from the DAC

---

## 2. Software Architecture

### Modules and Interfaces (API)

**SysTick.c / .h**
- `SysTick_Init(void)` — Initialize SysTick timer
- `SysTick_start(void)` — Start SysTick timer
- `SysTick_stop(void)` — Stop SysTick timer
- `SysTick_Set_Current_Note(unsigned long n_value)` — Set reload value for note frequency
- `SysTick_Handler(void)` — ISR for sine wave generation (outputs DAC values)

**GPIO.c / .h**
- `GPIO_Init()` — Configure all GPIO pins
- `PortE_Handler()` — ISR for piano key interrupts
- `PortF_Handler()` — ISR for mode/octave switches

**DAC6bit.c / .h**
- `DAC_Init()` — Configure 6-bit DAC on Port B
- `DAC_Out(unsigned char data)` — Output 6-bit value to DAC

**Music.c / .h**
- `play_a_song(void)` — Play predefined song
- `Music_Init(void)` — Initialize music system
- `SineTable[64]` — 64-sample sine wave lookup table



### Data Flow
<img width="1201" height="632" alt="image" src="https://github.com/user-attachments/assets/aa904828-d42b-40c8-ac85-d600a7da5fc2" />


### Call Graph
<img width="1208" height="877" alt="image" src="https://github.com/user-attachments/assets/591ad222-ca23-42a1-803d-712d06f351b1" />


## 3. Timing and Accuracy Considerations

The SysTick timer runs at a 32 MHz system clock frequency, providing sub-microsecond timing resolution with minimal interrupt latency. For sine wave generation, SysTick interrupts occur at 64× the note frequency to output successive samples from the 64-entry sine lookup table to the DAC. For example, for middle C (523 Hz), the SysTick interrupt fires at 33,472 Hz (523 × 64). Each interrupt outputs one sample from the sine table to Port B, which the R-2R DAC converts to an analog voltage level. Note durations are controlled by software loops, similar to System P1.

---

## 4. Advantages and Tradeoffs

### Advantages
- Higher audio quality with smooth sine wave output
- Reduced harmonic distortion compared to square waves
- Demonstrates digital-to-analog conversion
- Onboard LEDs show mode 

### Tradeoffs
- Requires external DAC components (resistors for R-2R ladder)
- More complex hardware setup and circuit construction
- Higher CPU overhead due to 64× interrupt frequency
- Software loops for note duration still cause blocking behavior
- One note at a time

---

## 5. Intended Use

This solution serves as the System P2 implementation for the project, demonstrating analog sine wave generation through digital-to-analog conversion. It provides a higher-quality audio output compared to the square wave approach, making it suitable for applications where sound quality is important. This implementation showcases the principles of DAC operation, lookup tables, and high-frequency timer interrupts. The added complexity in both hardware and software makes it an excellent learning platform for understanding the tradeoffs between digital and analog signal generation in embedded systems.

---
