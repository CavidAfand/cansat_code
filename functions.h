#include<avr/io.h>
#include<avr/interrupt.h>
#include <Servo.h>

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
   volatile int number_of_package  =0;
   volatile int timer=0;
   volatile bool flag=false;
   volatile bool motor_flag = false;
   volatile char separation_time[14];
   volatile bool separated = false;
   volatile uint16_t motor_above_rotation = 0;
   volatile uint16_t motor_under_rotation = 0;
   bool camera_flag = false;
   bool start_flag  = false ;
   bool altitude_flag  = false ;
   bool light_flag  = false ;
   bool capture_flag  = false ;
   bool propeller_flag  = false ;
   bool fly_flag  = false ;

   
   int statu = 0;
   volatile float altitude = 0;
   volatile float base_altitude = 0;
   volatile float pre_altitude  = 0;
   volatile float spee = 0;
   float pre_voltage1 = 0;
   float pre_voltage2 = 0;



   Adafruit_BME680  bme;
   volatile Servo servo;

   
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
   char* calculateAltitude();
   char* calculateSpeed();
   void runProp(uint8_t speed1,uint8_t speed2);
   void stopProp();
   void changeBaudrate();
   void reset_counter();
   void sendPacket(byte *packet, byte len);
      
};
