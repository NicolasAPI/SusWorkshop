#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/fpu.h"

// Praepozessor-Makros
#define SAMPLERATE 44000

//Funktionen-Deklarationen
void adcIntHandler(void);
void setup(void);
// hier nach Bedarf noch weitere Funktionsdeklarationen einfuegen

// globale Variablen
const float DoublePi = 6.283185308;
int32_t bufferSample[440];
int32_t arrayCoefficient[440] = {0};
int32_t grenze = 4000;
int32_t sampleIndex = 0;
// hier nach Bedarf noch weitere globale Variablen einfuegen

void main(void) { // nicht veraendern!! Bitte Code in adcIntHandler einfuegen
    setup();
    while (1) {
    }
}

void setup(void) { //konfiguriert den Mikrocontroller

    // konfiguriere SystemClock
    SysCtlClockSet(
    SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    uint32_t period = SysCtlClockGet() / SAMPLERATE;

    // aktiviere Peripherie
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    // aktiviere Gleitkommazahlen-Modul
    FPUEnable();
    FPUStackingEnable();
    FPULazyStackingEnable();
    FPUFlushToZeroModeSet(FPU_FLUSH_TO_ZERO_EN);

    // konfiguriere GPIO
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_2);
    GPIOPinTypeGPIOOutput(
            GPIO_PORTB_BASE,
            GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4
                    | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    // konfiguriere Timer
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, period - 1);
    TimerControlTrigger(TIMER0_BASE, TIMER_A, true);
    TimerEnable(TIMER0_BASE, TIMER_A);

    // konfiguriere ADC
    ADCClockConfigSet(ADC0_BASE, ADC_CLOCK_RATE_FULL, 1);
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_TIMER, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0,
    ADC_CTL_CH1 | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);
    ADCIntClear(ADC0_BASE, 3);
    ADCIntRegister(ADC0_BASE, 3, adcIntHandler);
    ADCIntEnable(ADC0_BASE, 3);

}

void adcIntHandler(void) {
    uint32_t adcInputValue;

    // Input in Variable speichern
    ADCSequenceDataGet(ADC0_BASE, 3, &adcInputValue);
    bufferSample[sampleIndex] = adcInputValue;

    // Index zurücksetzen
    if (sampleIndex >= 440) {
        sampleIndex = 0;
        uint32_t k;
        uint32_t i;
        float imCoefficient = 0;
        float reCoefficient = 0;
        // Real- und Imaginärkoeffizienten berechnen
        for (k=1;k<440;k++) {
            for (i=1;i<440;i++) {
                imCoefficient += bufferSample[i]*sinf(DoublePi*i*k/440);
                reCoefficient += bufferSample[i]*cosf(DoublePi*i*k/440);
            }
            // DFT Koeffizienten berechnen
            arrayCoefficient[k] = sqrt(imCoefficient*imCoefficient + reCoefficient*reCoefficient)/440;
            imCoefficient = 0;
            reCoefficient = 0;
        }

        // Maximum-Suche
        float Coefficient = 0;
        int co = 0;
        for (k=1;k<440;k++) {
            if (arrayCoefficient[k]>Coefficient) {
                Coefficient = arrayCoefficient[k];
                co = k;
            }
        }
        co=co*100;
        // LEDs ansteuern
                if (co >= grenze) {
                    GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0xFF);
                    }
                    else if (co >= 7*grenze/8) {
                        GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, 0xFF);
                        GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_7, 0x00);
                    }
                    else if (co >= 6*grenze/8) {
                        GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5, 0xFF);
                        GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_6|GPIO_PIN_7, 0x00);
                    }
                    else if (co >= 5*grenze/8) {
                        GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4, 0xFF);
                        GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0x00);
                    }
                    else if (co >= 4*grenze/8) {
                        GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0xFF);
                        GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0x00);
                    }
                    else if (co >= 3*grenze/8) {
                        GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2, 0xFF);
                        GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0x00);
                    }
                    else if (co >= 2*grenze/8) {
                        GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1, 0xFF);
                        GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0x00);
                    }
                    else if (co > grenze/8) {
                        GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_0, 0xFF);
                        GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0x00);
                    }
                    else if (co < grenze/8) {
                        GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0x00);
                    }
    }
    else {
        // Index erhöhen
        sampleIndex++;
    }

    // am Ende von adcIntHandler, Interrupt-Flag loeschen
    ADCIntClear(ADC0_BASE, 3);
}
