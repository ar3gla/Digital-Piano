// ModuleTest.c
// Course number: CECS 447
// Term: Spring 2026
// Project number: Project 1 – Part 1
// Project description: Module-by-module testing for Digital Piano
// Team #: 7
// Team members: Adrian Gustilo, Alfredo Regla, Gerald Calero, Paris Talebi

#include "tm4c123gh6pm.h"
#include "GPIO.h"
#include "SysTick.h"
#include "music.h"

// preprocessor directives for module name
#define GPIO_INPUT_TEST
//#define GPIO_OUTPUT_TEST
//#define SYSTICK_TEST
//#define MUSIC_TEST
volatile uint8_t mode = PIANO;
volatile uint8_t current_octave = 0;
extern volatile uint8_t current_song;

void System_Init(void);

#if defined(GPIO_INPUT_TEST)
void GPIO_Input_Init(void);
void GPIO_Input_Test(void);
#endif

#if defined(GPIO_OUTPUT_TEST)
void GPIO_Output_Init(void);
void GPIO_Output_Test(void);
#endif

#if defined(SYSTICK_TEST)
void SysTick_Module_Init(void);
void SysTick_Module_Test(void);
#endif

#if defined(MUSIC_TEST)
void Music_Module_Init(void);
void Music_Module_Test(void);
#endif

int main(void){
    System_Init();
    
    while(1){
        #if defined(GPIO_INPUT_TEST)
        GPIO_Input_Test();
        #endif
        
        #if defined(GPIO_OUTPUT_TEST)
        GPIO_Output_Test();
        #endif
        
        #if defined(SYSTICK_TEST)
        SysTick_Module_Test();
        #endif
        
        #if defined(MUSIC_TEST)
        Music_Module_Test();
        #endif
    }
}

void System_Init(void){
    #if defined(GPIO_INPUT_TEST)
    GPIO_Input_Init();
    #endif
    
    #if defined(GPIO_OUTPUT_TEST)
    GPIO_Output_Init();
    #endif
    
    #if defined(SYSTICK_TEST)
    SysTick_Module_Init();
    #endif
    
    #if defined(MUSIC_TEST)
    Music_Module_Init();
    #endif
}

#if defined(GPIO_INPUT_TEST)
void GPIO_Input_Init(void){
    PortE_Init();
    PortF_Init();
    SysTick_Init();
    Speaker_Init();
}

void GPIO_Input_Test(void){
    if(mode == AUTOPLAY){
        autoplay(mysong[current_song]);
    } else {
        piano_mode();
    }
}
#endif

#if defined(GPIO_OUTPUT_TEST)
void GPIO_Output_Init(void){
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;
    while((SYSCTL_RCGC2_R & SYSCTL_RCGC2_GPIOF) == 0){}
    
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
    GPIO_PORTF_CR_R |= 0x0E;
    GPIO_PORTF_DIR_R |= 0x0E;
    GPIO_PORTF_AFSEL_R &= ~0x0E;
    GPIO_PORTF_DEN_R |= 0x0E;
    GPIO_PORTF_PCTL_R &= ~0x0000FFF0;
    GPIO_PORTF_AMSEL_R &= ~0x0E;
}

void GPIO_Output_Test(void){
    GPIO_PORTF_DATA_R = 0x02;
    Delay();
    
    GPIO_PORTF_DATA_R = 0x04;
    Delay();
    
    GPIO_PORTF_DATA_R = 0x08;
    Delay();
    
    GPIO_PORTF_DATA_R = 0x0E;
    Delay();
    
    GPIO_PORTF_DATA_R = 0x00;
    Delay();
}
#endif

#if defined(SYSTICK_TEST)
void SysTick_Module_Init(void){
    SysTick_Init();
    Speaker_Init();
}

void SysTick_Module_Test(void){
    SysTick_Set_Current_Note(Tone_Tab[C4]);
    SysTick_start();
    Delay();
    Delay();
    SysTick_stop();
    Delay();
    
    SysTick_Set_Current_Note(Tone_Tab[D4]);
    SysTick_start();
    Delay();
    Delay();
    SysTick_stop();
    Delay();
    
    SysTick_Set_Current_Note(Tone_Tab[E4]);
    SysTick_start();
    Delay();
    Delay();
    SysTick_stop();
    Delay();
    
    SysTick_Set_Current_Note(Tone_Tab[F4]);
    SysTick_start();
    Delay();
    Delay();
    SysTick_stop();
    Delay();
    Delay();
}
#endif

#if defined(MUSIC_TEST)
void Music_Module_Init(void){
    SysTick_Init();
    Speaker_Init();
}

void Music_Module_Test(void){
    uint8_t i, j;
    
    for(i = 0; i < 24 && mysong[0][i].delay; i++){
        if(mysong[0][i].tone_index == PAUSE){
            SysTick_stop();
        } else {
            SysTick_Set_Current_Note(Tone_Tab[mysong[0][i].tone_index]);
            SysTick_start();
        }
        
        for(j = 0; j < mysong[0][i].delay; j++){
            Delay();
        }
    }
    
    SysTick_stop();
    
    for(j = 0; j < 15; j++){
        Delay();
    }
}
#endif
