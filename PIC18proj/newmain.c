
// PIC18C452 Configuration Bit Settings

// 'C' source line config statements

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



#include <xc.h>
#include<stdio.h>
#include<stdlib.h>
#define _XTAL_FREQ 4000000


#define buzzer PORTAbits.RA1

void msdelay (unsigned int itime)
{
int i,j;
for(i=0;i<itime;i++){
for(j=0;j<135;j++);
    }
}

void ddelay(){
   //TIMER
    INTCON2bits.TMR0IP = 1;       // Timer0 high priority
    INTCONbits.PEIE = 1;
    INTCONbits.TMR0IE=1;
    TMR0H = 0;                    //clear timer
    TMR0L = 0; 
    T0CON=0b00101011;
    while(!INTCONbits.TMR0IF){
        
    };                                 //Check for timer overflow
        INTCONbits.TMR0IF=0;
    T0CONbits.TMR0ON=0;

}

void ledblnk(){
    TRISAbits.RA1=0;
    while(1){
        buzzer=1;
        ddelay();
        buzzer=0;
        ddelay();
    }
}
int conv(int a)  
{  
    // declaration of variables  
    int binary_num, decimal_num = 0, bse = 1, rem;  
    binary_num = a; // assign the binary number to the binary_num variable  
     
     
    while ( a > 0)  
    {  
        rem = a % 10; /* divide the binary number by 10 and store the remainder in rem variable. */  
        decimal_num = decimal_num + rem * bse;  
        a = a / 10; // divide the number with quotient  
        bse = bse * 2;  
    }  
 
     return decimal_num; // print the decimal    
}

int ReadPOT(void)
{
    TRISAbits.TRISA0=1;
    ADCON1 = 0b00010011; //right aligned higher 2-ahresh lower 8- adresl
    ADCON0=0b10000010;
    ADRESH=0;
    ADRESL=0;
    ADCON0bits.ADON=1;
    msdelay(100);
    ADCON0bits.GO_DONE=1;
    while(ADCON0bits.GO_DONE!=0);
    return conv((ADRESH << 8) + ADRESL);
}

void main(){
    //for the potentiometer
    //For the PWM    
    TRISCbits.RC2=0;    // setting pin as  PWM out //
    PR2 = 99;           //period register value=[XTAL/4(Fpwm(taken as 1khz)*(n))]-1//
    T2CON=1;             // prescalar(n)//
    CCP1CON=0X0C;       //setting pwm output//
    TMR2=0x00;             //clearing tmr2 //
    T2CONbits.TMR2ON=1;  //switching on timer 2//
    while(1){
        //PWM
        CCPR1L=48;  //duty cycle register= duty cycle*PR2//
        //LED
        ledblnk();
    }
   
}