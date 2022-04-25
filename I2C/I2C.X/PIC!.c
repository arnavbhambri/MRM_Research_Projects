/*
 * File:   PIC!.c
 * Author: Arnav
 *
 * Created on 17 April, 2022, 11:59 AM
 */

// PIC18C452 Configuration Bit Settings

// 'C' source line configuration statements

// CONFIG1L
#pragma config CP = OFF         // Code Protection bits (Program memory code protection off)

// CONFIG1H
#pragma config OSC = RCIO       // Oscillator Selection bits (RC oscillator w/OSC2 configured as RA6)
#pragma config OSCS = OFF       // Oscillator System Clock Switch Enable bit (Oscillator system clock switch option is disabled (main oscillator is source))

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOR = ON         // Brown-out Reset Enable bit (Brown-out Reset enabled)
#pragma config BORV = 25        // Brown-out Reset Voltage bits (VBOR set to 2.5V)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 128      // Watchdog Timer Postscale Select bits (1:128)

// CONFIG3H
#pragma config CCP2MX = ON      // CCP2 Mux bit (CCP2 input/output is multiplexed with RC1)

// CONFIG4L
#pragma config STVR = ON        // Stack Full/Underflow Reset Enable bit (Stack Full/Underflow will cause RESET)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#define _XTAL_FREQ 4000000

void I2C_Master_Init(const unsigned long c)
{
  TRISC3 = 1;        //Setting as input as given in datasheet
  TRISC4 = 1;  
  SSPCON1 = 0b00101000;
  SSPCON2 = 0;
  SSPADD = (_XTAL_FREQ/(4*c))-1;
  SSPSTAT = 0x80;
 }
char I2C_Start()
{   
    SSPCON2bits.SEN=1;		/* Send start pulse */
    while(SSPCON2bits.SEN);	/* Wait for completion of start pulse */
    SSPIF=0;
    if(!SSPSTATbits.S){		/* Check whether START detected last */
    return 0;
    }		
}
 void I2C_Ready()
{
    while(BCLIF);	/* Wait if bit collision interrupt flag is set*/

    /* Wait for Buffer full and read write flag*/
    while(SSPSTATbits.BF || (SSPSTATbits.R_nW));
    SSPIF=0;  		/* Clear SSPIF interrupt flag*/
}
void I2C_Master_Stop()
{
  I2C_Ready();
  SSPCON2bits.PEN = 1;           //Initiate stop condition
}
unsigned char I2C_Write(unsigned char Data)
{
    I2C_Ready();        // wait until I2C_Bus of PIC18F4550 microcontroller becomes free 
    SSPBUF = Data;      // store data inside SSPBUF register of PIC18F4550 microcontroller
    I2C_Ready();
    if(SSPCON2bits.ACKSTAT){
        return 1;
    }
    else{
        return 2;
    }                       //return status of data or address transmission
}

void main(void) {
  I2C_Master_Init(100000);      //Initialize I2C Master with 100KHz clock
  while(1)
  {
    I2C_Start();  
    I2C_Write(0xAA);            //Start condition
    I2C_Write('H');             //Write data
    I2C_Master_Stop();
    __delay_ms(40);
  }
}

