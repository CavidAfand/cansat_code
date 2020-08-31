
#ifndef UART_H_INCLUDED
#define UART_H_INCLUDED

//#define BAUD 9600
#define BAUD 115200
#include<inttypes.h>
#include <avr/io.h>

void initUSART(void);
void initUSART1(void);
void initUSART2(void);
void transmitByte(uint8_t data);
uint8_t receiveByte(void);
void transmitByte1(uint8_t data);
uint8_t receiveByte1(void);
void transmitByte2(uint8_t data);
uint8_t receiveByte2(void);
void printString(const char *myString);
void printString1(const char *myString);
void printString2(const char *myString);
void printBin(uint8_t byte);
void printByte(int byte);
#endif // UART_H_INCLUDED/
