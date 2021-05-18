//Elctronica Digital 2
//Laboratorio 7
//Mario Estrada - 18123


//Librerias
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/sysctl.c"
#include "driverlib/interrupt.h"
#include "driverlib/interrupt.c"
#include "driverlib/gpio.h"
#include "driverlib/gpio.c"
#include "driverlib/uart.h"
#include "driverlib/uart.c"
#include "driverlib/timer.h"
#include "driverlib/timer.c"
#include "grlib/grlib.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include <string.h>

//Variables
char C1 ='a';
char C2 ='a';
int ch = 0;
int Flg = false;

//Funciones
void UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count);
void UARTIntHandler(void);
void Timer0IntHandler(void);

//Main
int main(void)
{
    //Configuracion de clock y perifericos
    SysCtlClockSet ( SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ ) ;
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOF ) ;
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)){}


    //Comunicacion UART
    SysCtlPeripheralEnable (SYSCTL_PERIPH_UART0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0)){}
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    IntMasterEnable();
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTDisable(UART0_BASE);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    IntEnable (INT_UART0);
    UARTIntEnable (UART0_BASE, UART_INT_RX);
    UARTEnable (UART0_BASE);
    IntPrioritySet(INT_UART0, 0x0);
    IntRegister(INT_UART0, UARTIntHandler);
    UARTFIFOEnable(UART0_BASE);
    UARTFIFOLevelSet(UART0_BASE,UART_FIFO_TX1_8,UART_FIFO_RX1_8);

    //Configuracion Timer0
    SysCtlPeripheralEnable (SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)){}
    SysCtlPeripheralReset (SYSCTL_PERIPH_TIMER0);
    SysCtlDelay (5);
    TimerDisable(TIMER0_BASE, TIMER_A|TIMER_B);
    TimerConfigure (TIMER0_BASE,TIMER_CFG_PERIODIC);
    TimerLoadSet (TIMER0_BASE, TIMER_A, 20000000 -1);
    TimerEnable (TIMER0_BASE, TIMER_A|TIMER_B);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0IntHandler);
    IntEnable(INT_TIMER0A);
    TimerEnable(TIMER0_BASE, TIMER_A);


    //Seteado de puertos para salidas de leds
    GPIOPinTypeGPIOOutput (GPIO_PORTF_BASE,GPIO_PIN_1) ;
    GPIOPinTypeGPIOOutput (GPIO_PORTF_BASE, GPIO_PIN_2) ;
    GPIOPinTypeGPIOOutput (GPIO_PORTF_BASE, GPIO_PIN_3) ;
    while(1){}
}

//Funcion Timer Handler
void Timer0IntHandler(){
    TimerIntClear(TIMER0_BASE,  TIMER_TIMA_TIMEOUT);
        if (Flg){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x0);
        }else{
            switch(C1){
            case 'b':
               GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0x04);
               C2='b';
               break;
            case 'g':
               GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0x08);
               C2='g';
               break;
            case 'o':
               GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x0);
               C2='o';
               break;
            case 'r':
               GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0x02);
               C2='r';
               break;
            }
        }
        Flg = !Flg;


}
void UARTIntHandler(){
    uint32_t ui32Status;
        ui32Status = UARTIntStatus(UART0_BASE, true);
        UARTIntClear(UART0_BASE, ui32Status);
        while(UARTCharsAvail(UART0_BASE))
        {
            C1=UARTCharGet(UART0_BASE);
            UARTCharPutNonBlocking(UART0_BASE,C1);
            if(C1==C2){C1='o';}
        }


}
void UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
    while(ui32Count--)
    {
        UARTCharPutNonBlocking(UART0_BASE, *pui8Buffer++);
    }
}


