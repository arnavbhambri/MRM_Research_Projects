/*
 * File:   PIC1.c
 * Author: Arnav
 *
 * Created on 11 April, 2022, 7:47 PM
 */



// PIC18F452 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1H
#pragma config OSC = RCIO       // Oscillator Selection bits (RC oscillator w/ OSC2 configured as RA6)
#pragma config OSCS = OFF       // Oscillator System Clock Switch Enable bit (Oscillator system clock switch option is disabled (main oscillator is source))

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOR = ON         // Brown-out Reset Enable bit (Brown-out Reset enabled)
#pragma config BORV = 20        // Brown-out Reset Voltage bits (VBOR set to 2.0V)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 128      // Watchdog Timer Postscale Select bits (1:128)

// CONFIG3H
#pragma config CCP2MUX = ON     // CCP2 Mux bit (CCP2 input/output is multiplexed with RC1)

// CONFIG4L
#pragma config STVR = ON        // Stack Full/Underflow Reset Enable bit (Stack Full/Underflow will cause RESET)
#pragma config LVP = ON         // Low Voltage ICSP Enable bit (Low Voltage ICSP enabled)

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000200-001FFFh) not code protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) not code protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) not code protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) not code protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot Block (000000-0001FFh) not code protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000200-001FFFh) not write protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) not write protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) not write protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) not write protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0001FFh) not write protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000200-001FFFh) not protected from Table Reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) not protected from Table Reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) not protected from Table Reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) not protected from Table Reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0001FFh) not protected from Table Reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>



#include <stdlib.h>
#include <stdio.h>
short z;

void UART_Write(char data)
{
  while(!TRMT); //wait till transmission is over
  TXREG = data;
}

void send_string(char s[])
{
	int i =0;
	
	while (s[i] != 0x00)
	{
		UART_Write(s[i]);
		i++;
	}
}
void uart(){
    #define BAUDRATE 9600  //bps
    #define _XTAL_FREQ 4000000
    SPBRG=25;
    INTCONbits.GIE=1;
    INTCONbits.PEIE=1;
    TRISCbits.RC6=0;
    TRISCbits.RC7=1;
    RCSTAbits.SPEN=1;
    PIE1bits.RCIE=1;
    RCSTAbits.CREN=0;
    RCSTAbits.CREN=1;
    TXSTA=0b01100100;
}

void I2C_Slave_Initialization(char address)
{

  TRISC3 = 1;     // Set RC3/SDA pins as a input pin
  TRISC4 = 1;     // Set RC4/SCL pins as a input pin
  SSPADD = address;  // store the address of slave in SSPADD register
  SSPSTAT = 0x80; //  set standard slew rate
  SSPCON1 = 0x36;
  SSPCON2 = 0x01; // Enable Clock Stretching
  PIR1bits.SSPIF = 0;      // Enable Interrupts
  PIE1bits.SSPIE = 1;     // Enable  PICI2C module interrupt
  INTCONbits.PEIE = 1;    // Enable peripheral interrupt
  INTCONbits.GIE = 1;    // Enable global interrupt

}

void __interrupt(high_priority) high_isr(void)
{
  if(SSPIF == 1)
  {
    SSPCON1bits.CKP = 0;

    if ((SSPCON1bits.SSPOV==1) || (SSPCON1bits.WCOL==1))
    {
      z = SSPBUF; // Read the previous value to clear the buffer
      SSPCON1bits.SSPOV = 0; // Clear the overflow flag
      SSPCON1bits.WCOL = 0;  // Clear the collision bit
      SSPCON1bits.CKP = 1;
    }
    if(!SSPSTATbits.D_nA && !SSPSTATbits.R_nW)
    {
      z = SSPBUF;
      UART_Write(z);
      while(!BF);
      PORTD = SSPBUF;
      SSPCON1bits.CKP = 1;
    }
    else if(!SSPSTATbits.D_nA && SSPSTATbits.R_nW)
    {
      z = SSPBUF;
      UART_Write(z);
      BF = 0;
      SSPBUF = PORTB;
      SSPCON1bits.CKP = 1;
      while(SSPSTATbits.BF);
    }

    SSPIF = 0;
  }
}

void main(void) {
    uart(); 
    TRISB = 0xFF;   
    I2C_Slave_Initialization(0xAA); //Initialize as a I2C Slave with address 0x30
    while(1){
        
    };
}

