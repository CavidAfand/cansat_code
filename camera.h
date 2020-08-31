#include <avr/io.h>
#include "uart.h"
#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include <SD.h>
#include "memorysaver.h"

#define   FRAMES_NUM    0x00
#define   SD_CS 53

class Camera{
  public:

  bool is_header = false;
  int total_time = 0;
  const int CS = 49; //Camera CS
  ArduCAM myCAM;

  Camera();
  void init(); 
  void capture();
  uint8_t read_fifo_burst(ArduCAM myCAM);

};
