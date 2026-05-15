# Evidence for Module Testing

## Module Test Evidence
**Date:** February 11, 2026  
**Tested by:** Alfredo Regla

### Piano Mode
Video demonstrates that pressing PE0-PE3 triggers correct notes (C, D, E, F) with proper octave switching via SW2. PF3 LED confirms SysTick is running. Sound stops when keys are released.
--
https://github.com/user-attachments/assets/64536841-7b20-482f-a398-8878c8d06aa8
### Autoplay Mode
Videos show continuous playback of each song with correct notes and timing. SW2 cycles through songs as expected. PF2 LED indicates autoplay mode is active.
--
https://github.com/user-attachments/assets/81954877-cfc2-498d-a18d-45f96a947c24

### Mode Switching
Video shows SW1 toggling between modes. LEDs change appropriately (PF2=green for piano, PF1=red for autoplay). Sound stops cleanly during transitions.
--
https://github.com/user-attachments/assets/fd381f38-1fc5-4169-b4c4-ff5a416fc4af

### Square Wave Output 
Logic Analyzer shows clean square wave transitions. Frequency measurements confirm accurate period generation by the SysTick timer.
 <img width="1668" height="658" alt="image" src="https://github.com/user-attachments/assets/e165392a-001e-4f96-8389-759f17d4a6ec" />
### Sine Wave Output
 Logic Analyzer shows smooth sine wave output on PB5-0. Measured frequencies match expected values.
<img width="1669" height="640" alt="image" src="https://github.com/user-attachments/assets/c6af2ca7-c89a-49b6-b68e-c307a2841c7f" />
<img width="1692" height="638" alt="image" src="https://github.com/user-attachments/assets/5327e35e-139c-4529-ba9f-5eecaf1c3aa0" />



