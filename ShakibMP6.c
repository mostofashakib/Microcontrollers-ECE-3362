//-------------------------------------------------------------------------------
//   MSP430FG2553 - Simple UART 
//
//   Description: This is an example of how to setup a simple loopback
//   with the built-in UART - sends a short message (stored in ROM) via the 
//   UART in loopback mode and stores the received message in a buffer in RAM.
//  Baudrate set to 9600
//   
//   The internal DCO clock is used - it is fine tuned to 1 MHz by using the
//    manufacturing calibration data stored in the chip
//
//   Target: TI LaunchPad development board with MSP430G2553 device
//
//       Date:          3rd December 2018
//       Written by:     Mostofa Adib Shakib, ECE dept, Texas Tech University
//       Adapted from:   N/A
//      Assembler/IDE:  IAR Embedded Workbench 5.5
//
//      HW I/O assignments:
//      P1.0    LED1    (Active HIGH)RED
//      P1.1    not used   UART RX serial data (input to MSP430)
//      P1.2    not used   UART TX serial data (output from MSP430)
//      P1.3    (not used)PushButton (Active LOW) (internal Pullup Enabled)
//      P1.4    not used
//      P1.5    not used
//      P1.6    (not used) LED2    (Active HIGH)GREEN
//      P1.7    not used
//
//      P2.0    not used
//      P2.1    not used
//      P2.2    not used
//      P2.3    not used
//      P2.4    not used
//      P2.5    not used
//      P2.6    not used
//      P2.7    not used


#include  <msp430g2553.h> // System define for the micro that I am using

#define RXD        BIT1 //Check your launchpad rev to make sure this is the case. Set jumpers to hardware uart.
#define TXD        BIT2 // TXD with respect to what your sending to the computer. Sent data will appear on this line
#define BUTTON     BIT3

unsigned char message[16] = ("Mostofa 26 ");
void UART_TX(char * tx_data);

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD; // Stop the Watch dog
     
  //------------------- Configure the Clocks -------------------//
        
  if (CALBC1_1MHZ==0xFF)   // If calibration constant erased
     {    
        while(1);          // do not load, trap CPU!!
     } 

   DCOCTL  = 0;             // Select lowest DCOx and MODx settings
   BCSCTL1 = CALBC1_1MHZ;   // Set range
   DCOCTL  = CALDCO_1MHZ;   // Set DCO step + modulation 
      
  //---------------- Configuring the LED's ----------------------//
      
   P1DIR  |=  BIT0 + BIT6;  // P1.0 and P1.6 output
   P1OUT  &= ~BIT0 + BIT6;  // P1.0 and P1.6 = 0
      
  //--------- Setting the UART function for P1.1 & P1.2 --------//
      
   P1SEL  |=  BIT1 + BIT2;  // P1.1 UCA0RXD input
   P1SEL2 |=  BIT1 + BIT2;  // P1.2 UCA0TXD output
      
  //------------ Configuring the UART(USCI_A0) ----------------//
      
   UCA0CTL1 |=  UCSSEL_2 + UCSWRST;  // USCI Clock = SMCLK,USCI_A0 disabled
   UCA0BR0   =  104;                 // 104 From datasheet table-  
   UCA0BR1   =  0;                   // -selects baudrate =9600,clk = SMCLK
   UCA0MCTL  =  UCBRS_1;             // Modulation value = 1 from datasheet
   UCA0STAT |=  UCLISTEN;            // loop back mode enabled 
   UCA0CTL1 &= ~UCSWRST;             // Clear UCSWRST to enable USCI_A0
      
  //---------------- Enabling the interrupts ------------------//
      
   IE2 |= UCA0TXIE;                  // Enable the Transmit interrupt
   IE2 |= UCA0RXIE;                  // Enable the Receive  interrupt
   _BIS_SR(GIE);                     // Enable the global interrupt
      
  while(1)                           // While 1 is equal to 1 (forever)
    {
        UART_TX("Mostofa 26    \r\n");  // Transmit message
        __delay_cycles(100000);      //Debounce button so signal is not sent multiple times
    }    
}

void UART_TX(char * tx_data) // Define a function which accepts a character pointer to an array
{
    unsigned int i=0;
    while(message[i]) // Increment through array, look for null pointer (0) at end of string
    {
        while ((UCA0STAT & UCBUSY)); // Wait if line TX/RX module is busy with data
        UCA0TXBUF = message[i]; // Send out element i of tx_data array on UART bus
        i++; // Increment variable for array address
    }
    
}
  //-----------------------------------------------------------------------//
  //                Transmit and Receive interrupts                        //
  //-----------------------------------------------------------------------//

  #pragma vector = USCIAB0TX_VECTOR
  __interrupt void TransmitInterrupt(void)
  {
    P1OUT  ^= BIT0;//light up P1.0 Led on Tx 
  }

  #pragma vector = USCIAB0RX_VECTOR
  __interrupt void ReceiveInterrupt(void)
  {
    P1OUT  ^= BIT6;     // light up P1.6 LED on RX 
    IFG2 &= ~UCA0RXIFG; // Clear RX flag 
  }