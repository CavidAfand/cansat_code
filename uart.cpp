#include "uart.h"
#include <util/setbaud.h>
#include "functions.h"
#include <HardwareSerial.h>

void initUSART(void) {

    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    #if USE_2X
    UCSR0A |= (1 << U2X0);
    #else
    UCSR0A &= ~(1 << U2X0);
    #endif
    /* requires BAUD */
    /* defined in setbaud.h *//* Enable USART transmitter/receiver */
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); 
	/* 8 data bits, 1 stop bit*/
}

void initUSART1(void){

  Serial1.begin(9600);
  CanSat cnst;
  cnst.changeBaudrate();
  delay(1000);
//  Serial1.begin(115200);
    UBRR1H = UBRRH_VALUE;
    UBRR1L = UBRRL_VALUE;
    #if USE_2X
    UCSR1A |= (1 << U2X1);
    #else
    UCSR1A &= ~(1 << U2X1);
    #endif
	
    UCSR1B = (1 << TXEN1) | (1 << RXEN1);
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10); 

}

void initUSART2(void) {

    UBRR2H = UBRRH_VALUE;
    UBRR2L = UBRRL_VALUE;
    #if USE_2X
    UCSR2A |= (1 << U2X2);
    #else
    UCSR2A &= ~(1 << U2X2);
    #endif
    /* requires BAUD */
    /* defined in setbaud.h *//* Enable USART transmitter/receiver */
    UCSR2B = (1 << TXEN2) | (1 << RXEN2)| (1<<RXCIE2);
    UCSR2C = (1 << UCSZ21) | (1 << UCSZ20); 
	/* 8 data bits, 1 stop bit*/
}

void transmitByte(uint8_t data) {
    /* Wait for empty transmit buffer */
    loop_until_bit_is_set(UCSR0A, UDRE0);
    /* send data */
    UDR0 = data;
}

uint8_t receiveByte(void) {
    loop_until_bit_is_set(UCSR0A, RXC0);
    return UDR0;
    /* Wait for incoming data */
    /* return register value */
}

void transmitByte1(uint8_t data) {
    /* Wait for empty transmit buffer */
    loop_until_bit_is_set(UCSR1A, UDRE1);
    /* send data */
    UDR1 = data;
}

uint8_t receiveByte1(void) {
    loop_until_bit_is_set(UCSR1A, RXC1);
    return UDR1;
    /* Wait for incoming data */
    /* return register value */
}

void transmitByte2(uint8_t data) {
    /* Wait for empty transmit buffer */
    loop_until_bit_is_set(UCSR2A, UDRE2);
    /* send data */
    UDR2 = data;
}

uint8_t receiveByte2(void) {
    loop_until_bit_is_set(UCSR2A, RXC2);
    return UDR2;
    /* Wait for incoming data */
    /* return register value */
}

void printString(const char myString[]) {
    uint8_t i = 0;
    while (myString[i]) {
        transmitByte(myString[i]);
        i++;
    }
}

void printString1(const char myString[]) {
    uint8_t i = 0;
    while (myString[i]) {
        transmitByte1(myString[i]);
        i++;
    }
}

void printString2(const char myString[]) {
    uint8_t i = 0;
    while (myString[i]) {
        transmitByte2(myString[i]);
        i++;
    }
}

void printByte(int byte){
  transmitByte('0'+ (byte/1000)); 
  transmitByte('0'+ ((byte/100)%10));       
  transmitByte('0'+ ((byte/10) % 10));
  transmitByte('0'+ (byte % 10));   
}


void printByte1(int byte){
  transmitByte1('0'+ (byte/1000)); 
  transmitByte1('0'+ ((byte/100)%10));     
  transmitByte1('0'+ ((byte/10) % 10));
  transmitByte1('0'+ (byte % 10));   
}
void printBin(uint8_t byte){
    while(byte)
    {
        transmitByte('0'+(byte%2));
        byte/=2;
    }
}
