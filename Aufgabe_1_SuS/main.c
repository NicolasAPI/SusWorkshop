#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

// Praeprozessor-Makros
#define BUFFER_SIZE 1000
#define SAMPLERATE 44000


// Funktionen-Deklarationen
void adcIntHandler(void);
void setup(void);

// globale Variablen
int32_t bufferSample[BUFFER_SIZE] = {0}; //Ringpuffer-Deklarationen
uint32_t index = 0;
float sum = 0;
// hier weitere benötigten globalen Variablen einfuegen

void main(void){ // nicht veraendern!! Bitte Code in adcIntHandler einfuegen
    setup();
    while(1){}
}

void setup(void){// konfiguriert den MiKrocontroller

    // konfiguriere System-Clock
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    uint32_t period = SysCtlClockGet()/SAMPLERATE;

    // aktiviere Peripherie
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    // konfiguriere GPIO
    GPIOPinTypeADC(GPIO_PORTE_BASE,GPIO_PIN_2);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

    // konfiguriere Timer
    TimerConfigure(TIMER0_BASE,TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, period - 1);
    TimerControlTrigger(TIMER0_BASE,TIMER_A,true);
    TimerEnable(TIMER0_BASE,TIMER_A);

    // konfiguriere ADC
    ADCClockConfigSet(ADC0_BASE,ADC_CLOCK_RATE_FULL,1);
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_TIMER, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH1|ADC_CTL_IE|ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);
    ADCIntClear(ADC0_BASE,3);
    ADCIntRegister(ADC0_BASE,3,adcIntHandler);
    ADCIntEnable(ADC0_BASE,3);

}

void adcIntHandler (void){
   uint32_t adcInputValue;
   //uint32_t altestesAbtastwert;
   float lautstaerke;

   ADCSequenceDataGet(ADC0_BASE,3,&adcInputValue);
   if(index >= BUFFER_SIZE){
       index = 0;
   }
   /*altestesAbtastwert = bufferSample[index];
   bufferSample[index] = adcInputValue^2;
   sum = sum - altestesAbtastwert + bufferSample[index];
   */


   sum -= bufferSample[index];
   bufferSample[index] = adcInputValue*adcInputValue;
   sum += bufferSample[index];

   lautstaerke = sum/BUFFER_SIZE;
   index ++;

   if (lautstaerke >= 2.7*2.7*2.7*2.7*2.7*2.7*2.7*2.7*2.7*2.7) {
       GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0xFF);
       }
       else if (lautstaerke >= 2.7*2.7*2.7*2.7*2.7*2.7*2.7*2.7*2.7) {
           GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, 0xFF);
           GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_7, 0x00);
       }
       else if (lautstaerke >= 2.7*2.7*2.7*2.7*2.7*2.7*2.7*2.7) {
           GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5, 0xFF);
           GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_6|GPIO_PIN_7, 0x00);
       }
       else if (lautstaerke >= 2.7*2.7*2.7*2.7*2.7*2.7*2.7) {
           GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4, 0xFF);
           GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0x00);
       }
       else if (lautstaerke >= 2.7*2.7*2.7*2.7*2.7*2.7) {
           GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0xFF);
           GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0x00);
       }
       else if (lautstaerke >= 2.7*2.7*2.7*2.7*2.7) {
           GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2, 0xFF);
           GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0x00);
       }
       else if (lautstaerke >= 2.7*2.7*2.7*2.7) {
           GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1, 0xFF);
           GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0x00);
       }
       else if (lautstaerke > 2.7*2.7*2.7) {
           GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_0, 0xFF);
           GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0x00);
       }
       else {
           GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0x00);
   }


//lol
   // am Ende von adcIntHandler, Interrupt-Flag loeschen
   ADCIntClear(ADC0_BASE,3);
}
