#include <avr/io.h>
#include "uart.h"
#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include <SD.h>
#include "memorysaver.h"
//#include "functions.h"

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
  void capture(const char *dateString, const char *timeString);
  uint8_t read_fifo_burst(ArduCAM myCAM, const char *dateString, const char *timeString);

};
