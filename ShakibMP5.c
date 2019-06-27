//******************************************************************************
//  MSP430G2xx3 Demo - Simple Kitchen Timer
//
//  Description; This is a simple kitchen timer which uses timer and port interrupts
//  to replicate a simple kitchen timer.
//  ACLK = n/a, MCLK = SMCLK = default DCO
//
//                MSP430G2xx3
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |             P1.0|-->LED
//
//  This is Project 3 in C language 
//  Target: TI LaunchPad development board with MSP430G2553 device
//  D. Dang
//  Texas Instruments, Inc
//  December 2010
//  Written by: Mostofa Adib Shakib
//  Date: 11/22/2018
//   Built with CCS Version 4.2.0 and IAR Embedded Workbench Version: 5.10
//******************************************************************************

#include  <msp430g2553.h>
//------------------------------------------------------------------------------
//Definition of constants
//------------------------------------------------------------------------------
//Segment Position
#define SEG_A (0xFE)
#define SEG_B (0xFD)
#define SEG_C (0xFB)
#define SEG_D (0xF7)
#define SEG_E (0xEF)
#define SEG_F (0xDF)
#define SEG_G (0xBF)
#define SEG_DP (0x7F)

//Digital Display Positions
#define DIG_3 (0xFE)
#define DIG_2 (0xFD)
#define DIG_1 (0xFB)
#define DIG_0 (0xF7)
#define DP_COM (0xEF)

//Pushbutton constants
#define PB_0 (0x20)
#define PB_1 (0x40)
#define PB_2 (0x80)

//Value Patterns
#define ONE   (0xF9)
#define TWO   (0xA4)
#define THREE (0xB0)
#define FOUR  (0x99)
#define FIVE  (0x92)
#define SIX   (0x82)
#define SEVEN (0xF8)
#define EIGHT (0x80)
#define NINE  (0x98)
#define ZERO  (0xC0)
#define Dots  (0xF8)
#define Cc    (0xC6)
#define E     (0x8E)

//------------------------------------------------------------------------------
//Definition of Variables
//------------------------------------------------------------------------------
int patterntable[10]={ZERO,ONE,TWO,THREE,FOUR,FIVE,SIX,SEVEN,EIGHT,NINE};
int var3=0;
int var2=0;
int a=0;
int b=0;
int c=0;
int d=0;

//------------------------------------------------------------------------------
//Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void main(void)
{
WDTCTL=WDTPW+WDTHOLD;  //Stop WDT
P1DIR=0x0FF;           //Set pins to outputs
P2DIR=0x01F;           //Set pushbutton pins to outputs
P2SEL&=~ PB_1+PB_2;
P2REN =PB_0+PB_1+PB_2;  //Turn on internal resistor
P2OUT =PB_0+PB_1+PB_2;  // Set to pull up mode

P2OUT=0x0FF;
P1OUT=0xFF;

P2IE =PB_0+PB_1+PB_2;   //enable interrupt for pushbuttons
P2IES=PB_0+PB_1+PB_2;
P2IFG=0x0;
 
//Timer setup
TA1CCR0 = 12500;
TA1CCTL0=CCIE;
TA1CTL=TASSEL_2+ID_3+MC_1;

//Display C for kitchen timer
P1OUT=0x0FF;
  for(int i=0;i<30000;i++){
    P2OUT=DIG_0;
    P1OUT=Cc;
  }
    for(int i=0;i<30000;i++){
    P1OUT=0xFF;
}
_BIS_SR(GIE);                           // interrupt
for(;;){                               // The different options for different switches
  if(d == 0){
    for(int j=0;j<30000;j++){
      P1OUT=0xFF;
      P2OUT=DIG_0;
      P1OUT=ZERO;
      P1OUT=0xFF;
      P2OUT=DIG_1;
      P1OUT=patterntable[a];
      P1OUT=0xFF;
      P2OUT=DIG_2;
      P1OUT=patterntable[b];
      P1OUT=0xFF;
      P2OUT=DIG_3;
      P1OUT=ZERO;
      P1OUT=0xFF;
      P1OUT=Dots;
      P2OUT=DP_COM;
      P1OUT=0xFF;
    }
  }
  if(d == 1){
      for(int n=0;n<10000;n++){
      P1OUT=0xFF;
      P2OUT=DIG_0;
      P1OUT=ZERO;
      P1OUT=0xFF;
      P2OUT=DIG_1;
      P1OUT=ZERO;
      P1OUT=0xFF;
      P2OUT=DIG_2;
      P1OUT=ZERO;
      P1OUT=0xFF;
      P2OUT=DIG_3;
      P1OUT=ZERO;
      P1OUT=0xFF;
      P1OUT=Dots;
      P2OUT=DP_COM;
      P1OUT=0xFF;
    }  
      for(int f=0;f<10000;f++){
      P1OUT=0xFF;
      P1OUT=0xFF;
      P1OUT=0xFF;
      P1OUT=0xFF;
      P1OUT=0xFF;
      P1OUT=0xFF;
      P1OUT=0xFF;
      P1OUT=0xFF;
      P1OUT=0xFF;
      P1OUT=0xFF;
      P1OUT=0xFF;
      P1OUT=0xFF;
     }
  }
}
}
//------------------------------------------------------------------------------
//interrupts
//------------------------------------------------------------------------------
//Port interrupt
#pragma vector=PORT2_VECTOR
__interrupt void Port_2 (void){
if (P2IFG & PB_1) {
    c=1;
    d=0;
  }
else if (P2IFG & PB_0){
    a=6;
    b=2;
    c=0;
    d=0;
  }
else if (P2IFG & PB_2){
    a=0;
    b=0;
    d=0;
    c=0;
  }
P2IFG=0;
}
//Timer Interrupt
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer_A1_ISR (void){
  if((c & 1)&&((a | b) != 0)){
    d=0;
    if(a >= 0){
      a--;
      if(a == 0xFFFF){
        a=9;
        b--;
      }
      if((a == 0) && (b == 0)){
        d=1;
        c=0;
    }
  }
  }
}