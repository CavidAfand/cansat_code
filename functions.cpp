#include "functions.h"

CanSat::CanSat():bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK){}

extern int capture_stat;
char utc[7];

void CanSat :: buzzer(uint8_t times){

  pinMode(BUZZER,OUTPUT);
  while(times){
  digitalWrite(BUZZER,HIGH);
  _delay_ms(500);
  digitalWrite(BUZZER,LOW);
  _delay_ms(500);
  --times;
  }
}

void CanSat :: runProp(){
  pinMode(PROPELLER, OUTPUT);
  digitalWrite(PROPELLER, HIGH);
}

void CanSat:: sendPacket(byte *packet, byte len) {
  for (byte i=0; i < len; i++) {
    Serial1.write(packet[i]);
  }
}

void CanSat::changeBaudrate() {
  byte packet[] = {
        0xB5, // sync char 1
        0x62, // sync char 2
        0x06, // class
        0x00, // id
        0x14, // length
        0x00, // 
        0x01, // payload
        0x00, // payload
        0x00, // payload
        0x00, // payload
        0xD0, // payload
        0x08, // payload
        0x00, // payload
        0x00, // payload
        0x00, // payload
        0xC2, // payload
        0x01, // payload
        0x00, // payload
        0x07, // payload
        0x00, // payload
        0x03, // payload
        0x00, // payload
        0x00, // payload
        0x00, // payload
        0x00, // payload
        0x00, // payload
        
        0xC0, // CK_A
        0x7E, // CK_B
    };
    sendPacket(packet, sizeof(packet));
    printString("Changed");
}

void CanSat :: init(void){
  
  //Starting UART Communications
  initUSART();
  initUSART1();
  initUSART2(); 


  //Start Timer Configurations 

  TCNT1 = 49911;   // for 1 sec at 16 MHz 

  TCCR1A = 0x00;
  TCCR1B = (1<<CS10) | (1<<CS12);  // Timer mode with 1024 prescler
  TIMSK1 = (1 << TOIE1) ;

  if(bme.begin()){
     
    printString("BME680 initialized success.\n\r");
    printString2("@BMP280 initialized success.*\n\r");
  
  }
  else{ 
    printString("BME680 initialized failed. Check wiring.\n\r");
    printString("@BMP280 initialized failed. Check wiring.*\n\r");
  }
  
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
 
  if (! bme.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }
  char temp[10];
  base_pressure = bme.readPressure();
  
  Serial.println(base_pressure);
  _delay_ms(1000);


  
  if(gps.check()) {

    printString("GPS initialized success.\n\r");
    
////    printString2("@GPS initialized success.*\n\r");
//    printString1("$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n");
//    printString1("$PMTK251,115200*1F\r\n");
//    changeBaudrate();

    gps.get_data();
    gps.get_data();
    gps.get_data();
    printString("GPS OKKK...\r\n");
  }
  else{ 
    printString("GPS initialized failed. Check wiring.\n\r");
//    printString2("@GPS initialized failed. Check wiring.*\n\r");
  }
  
  camera.init();
      
  servo.attach(43);
  servo.write(0);

//  buzzer(3);
}

void CanSat :: intochar (unsigned long n, char* pres ){
    pres[5]='0'+n%10;
    n/=10;
    pres[4]='0'+n%10;
    n/=10;
    pres[3]='0'+n%10;
    n/=10;
    pres[2]='0'+n%10;
    n/=10;
    pres[1]='0'+n%10;
    n/=10;
    pres[0]='0'+n%10;
    
    pres[6]='\0';
  }


void CanSat :: getData(void){
  
  //Declare Strings 
  char package[120];
  char temp[15];

  //Reset memory of Strings 
  memset(package, 0, sizeof package);
  // FF
  memset(temp, 0, sizeof (temp));
  
  //Concat id of team (2278)
//  strncpy(package, "$3292,",6);
  temp[0] = (char)255;
  temp[1] = (char)217;
  temp[2] = '$';
  temp[3] = '2';
  temp[4] = '2';
  temp[5] = '7';
  temp[6] = '8';
  temp[7] = ',';
  strcat(package,temp); 
  
  //Concat time device is on
  memset(temp, 0, sizeof (temp));
  itoa(timer,temp,10);
  strcat(package,temp); 
  strcat(package,",");

  //Concat number of package generated
  number_of_package++;
  memset(temp, 0, sizeof (temp));
  itoa(number_of_package,temp,10);
  strcat(package,temp);
  strcat(package,",");

  //bat_vol1
  strcat(package,"9");
  strcat(package,",");

  //bat_vo12
  strcat(package,"11");
  strcat(package,",");
  
  //read relative altitude respect to base
  altitude = bme.readAltitude(base_pressure/100.0);//(float)(bme.pressure / 100.0)
  //Serial.println(altitude);

  //reset temporary String (temp) & Concat altitude to package
  memset(temp, 0, sizeof( temp));  // butun sizeof-lari deyisdim, moterize elave etdik
  itoa(altitude,temp,10);
  strcat(package,temp);
  strcat(package,",");
  
  //Concat speed 
  memset(temp, 0, sizeof(temp));
  spee = pre_altitude-altitude;
  
  itoa(spee,temp,10);
  pre_altitude=altitude;
  strcat(package,temp);
  strcat(package,",");

  //Concat latitude & longtitude
  gps.get_data();
  strcat(package,gps.longtitude);
  strcat(package,"E,");
  strcat(package,gps.latitude);
  strcat(package,"N,");

//  strcat(package,"491215");
//  strcat(package,",");
//  strcat(package,"401542");
//  strcat(package,",");
  
  //picture status
  memset(temp, 0, sizeof(temp));
  itoa(capture_stat,temp,10);
  capture_stat = 0;
  strcat(package,temp);
  strcat(package,",");
  
  //rpm1
  strcat(package,"100");
  strcat(package,",");
  
  //rpm2
  strcat(package,"100");
  strcat(package,",");
  
  //seperation time
  strcat(package,"28092018153512");
  strcat(package,",");

  //num of taken photos
  strcat(package,"5");
  strcat(package,",");

  //num of taken photos
  strcat(package,"5");
  
  //end
  strcat(package,"!");
  
  printString2(package);
//  printString2("\r\n");
  printString(package);
  printString("\r\n");
  
  
//  Serial.println(gps.utc_time);
//  for(int i = 0;i<7;i++){
//    utc[i] = gps.utc_time[i];
//  }
}


//
//float CanSat::readAltitude(float seaLevelhPa) {
//  bme.performReading();
//    
//  float altitude;
//  
//  float Pressure = bme.pressure; // in Si units for Pascal
//  Pressure /= 100;
//
//  altitude = 44330 * (1.0 - pow(Pressure / seaLevelhPa, 0.1903));
//
//  return altitude;
//}

  
