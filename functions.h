#include<avr/io.h>
#include<avr/interrupt.h>
#include <Servo.h>

#include <avr/io.h>
#include "uart.h"
#include "defines.h"
#include "camera.h"
#include <Wire.h>
#include <SPI.h>
//#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"


#include "gps.h"

class CanSat{
  public :   
   int number_of_package  =0;
   int timer=0;
   volatile bool flag=false;
   volatile bool motor_flag = false;
   bool start_flag  = false ;
   bool altitude_flag  = false ;
   bool light_flag  = false ;
   bool capture_flag  = false ;
   bool propeller_flag  = false ;
   bool fly_flag  = false ;

   
   int statu = 0;
   int altitude = 0;
   int pre_altitude  = 0;
   int spee = 0;

//   Adafruit_BMP280 bme; 
   #define SEALEVELPRESSURE_HPA (1013.25)

   Adafruit_BME680  bme;

   
   CanSat();
   void init();
//      float readAltitude(float);
   void buzzer (uint8_t);
   void intochar(unsigned long,char*);
   GPS gps;
   Camera camera;
   float base_pressure;
   uint16_t height;
   void getData();
   char* getBattery(uint16_t batteryPin);
   void runProp(uint8_t speed1,uint8_t speed2);
   void stopProp();
   void changeBaudrate();
   void sendPacket(byte *packet, byte len);
      
};
