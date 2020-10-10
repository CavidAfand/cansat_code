#include "functions.h"


CanSat::CanSat():bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK){}

extern int capture_stat;
char utc[7];
extern volatile int captured_photo_number;
extern volatile int sent_photo_number;

//   Adafruit_BMP280 bme; 
#define SEALEVELPRESSURE_HPA (1013.25)

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

void CanSat :: runProp(uint8_t speed1,uint8_t speed2){
  analogWrite(MOTOR1, speed1);
  analogWrite(MOTOR2, speed2);
}

void CanSat :: stopProp() {
  analogWrite(MOTOR1, 0);
  analogWrite(MOTOR2, 0);
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

  // servo init
  servo.attach(SERVO);
  servo.write(60);

  
  //Starting UART Communications
  initUSART();
  initUSART1();
  initUSART2(); 


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
  base_altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  altitude = base_altitude;
  pre_altitude = base_altitude;
  
  Serial.println(base_pressure);
  Serial.print("base alt: ");
  Serial.println(base_altitude);
  _delay_ms(1000);

  uint16_t pre_voltage1_analog1 = analogRead(BATTERY1);
  uint16_t pre_voltage1_analog2 = analogRead(BATTERY1);
  uint16_t pre_voltage1_analog3 = analogRead(BATTERY1);
  pre_voltage1 = ((pre_voltage1_analog1 + pre_voltage1_analog2 + pre_voltage1_analog3) / 3) * (5.00 / 1023.00) * 2.14;// * 2;
  uint16_t pre_voltage2_analog1 = analogRead(BATTERY2);
  uint16_t pre_voltage2_analog2 = analogRead(BATTERY2);
  uint16_t pre_voltage2_analog3 = analogRead(BATTERY2);
  pre_voltage2 = ((pre_voltage2_analog1 + pre_voltage2_analog2 + pre_voltage2_analog3) / 3) * (5.00 / 1023.00) * 1.89;// * 2 / 3;

  
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

  

  // external interrupt configuration for calculating motor rotation
  EICRB = ((1 << ISC41) | (1 << ISC40)) | ((1 << ISC51) | (1 << ISC50)); // Rising mode for both pin
  EIMSK = ((1 << INT4) | (1 << INT5)); // enable interrupt for digital pins 2 and 3
  sei(); // enable global interrupt 
  // end of external interrupt configuration
      
  //Start Timer 5 Configurations 

  TCNT4 = 49911;   // for 1 sec at 16 MHz 

  TCCR4A = 0x00;
  TCCR4B = ((1<<CS40) | (1<<CS42));  // Timer mode with 1024 prescler
  TIMSK4 = (1 << TOIE4) ;
  flag = true;

  
  /// Timer configurations end

  // Motor initialize
  pinMode(MOTOR1, OUTPUT);
  pinMode(MOTOR2, OUTPUT);

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


char* CanSat :: getBattery(uint16_t batteryPin) {
  char str[10];
  uint16_t volt_analog1 = analogRead(batteryPin);
  uint16_t volt_analog2 = analogRead(batteryPin);
  uint16_t volt_analog3 = analogRead(batteryPin);
  float voltage;
  if (batteryPin == BATTERY1) {
   voltage = ((volt_analog1 + volt_analog2 + volt_analog3) / 3) * (5.00 / 1023.00) * 2.14 ;// * 2;
  }
  else if (batteryPin == BATTERY2){
    voltage = ((volt_analog1 + volt_analog2 + volt_analog3) / 3) * (5.00 / 1023.00) * 1.89;// * 3 / 2;
  }
//  float voltage = ((volt_analog1 + volt_analog2 + volt_analog3) / 3) * (5.00 / 1023.00) * 2;
//  if (batteryPin == BATTERY1) {
//    if (voltage > pre_voltage1) voltage = pre_voltage1;
//    else if (voltage < pre_voltage1) pre_voltage1 = voltage;
//  }
//  else if (batteryPin == BATTERY2) {
//    if (voltage > pre_voltage2) voltage = pre_voltage2;
//    else if (voltage < pre_voltage2) pre_voltage2 = voltage;
//  }
  dtostrf(voltage, 3, 1, str);
  return str;
}

char* CanSat :: calculateAltitude() {
  char str[15];
  memset(str, '\0', 15);
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA) - base_altitude;
  int altitude_int = (int)(altitude * 100);

  if (altitude_int < 0 && altitude_int/100 == 0) sprintf(str, "-%d.%d", altitude_int/100, abs(altitude_int%100));
  else sprintf(str, "%d.%d", altitude_int/100, abs(altitude_int%100));
  return str;
}

char* CanSat :: calculateSpeed() {
  char str[10];
  memset(str, '\0', 10);

  spee = altitude - pre_altitude;
  pre_altitude = altitude;
  int spee_int = (int)(spee * 100);

  if(spee_int < 0 && spee_int/100 == 0) sprintf(str, "%d.%d", spee_int/100, abs(spee_int%100));
  else sprintf(str, "%d.%d", spee_int/100, abs(spee_int%100));
  return str;
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
  
  //Concat time device is on  - 2nd data
  memset(temp, 0, sizeof (temp));
  itoa(timer,temp,10);
  strcat(package,temp); 
  strcat(package,",");

  //Concat number of package generated - 3rd data
  number_of_package++;
  memset(temp, 0, sizeof (temp));
  itoa(number_of_package,temp,10);
  strcat(package,temp);
  strcat(package,",");

  //bat_vol1 - 4th data
  strcat(package, getBattery(BATTERY1));
  strcat(package,",");

  //bat_vo12 - 5th data
  strcat(package, getBattery(BATTERY2));
  strcat(package,",");

  //reset temporary String (temp) & Concat altitude to package - 6th data
//  memset(temp, 0, sizeof( temp));  
//  itoa(altitude,temp,10);
  strcat(package,calculateAltitude());
  strcat(package,",");
  
  //Concat speed  - 7th data
//  memset(temp, 0, sizeof(temp));
//  spee = pre_altitude-altitude;
//  itoa(spee,temp,10);
//  pre_altitude=altitude;
  strcat(package,calculateSpeed());
  strcat(package,",");

  //Concat latitude & longtitude
  gps.get_data();
  
  // 8th data
  strcat(package,gps.latitude);
  strcat(package,"N,");
  
  // 9th data
  strcat(package,gps.longtitude);
  strcat(package,"E,");
  
  //picture status - 10th data
  memset(temp, 0, sizeof(temp));
  itoa(capture_stat,temp,10);
  capture_stat = 0;
  strcat(package,temp);
  strcat(package,",");
  
  //rpm1 - 11th data 
//  strcat(package,"100");
  memset(temp, 0, sizeof(temp));
  itoa(motor_above_rotation*60, temp, 10);
  motor_above_rotation = 0;
  strcat(package,temp);
  strcat(package,",");
  
  //rpm2 - 12th data
//  strcat(package,"100");
  memset(temp, 0, sizeof(temp));
  itoa(motor_under_rotation*60, temp, 10);
  motor_under_rotation = 0;
  strcat(package,temp);
  strcat(package,",");
  
  //seperation time - 13th data
  if (separated == false) {
    strcat(package,"------");
    strcat(package,",");
  }
  else {
    strcat(package, separation_time);
//    strcat(package, "time");
    strcat(package, ",");
  }

  //num of taken photos - 14th data
  memset(temp, 0, sizeof(temp));
  itoa(captured_photo_number, temp, 10);
  strcat(package,temp);
  strcat(package,",");

  //num of taken photos - 15th data
  memset(temp, 0, sizeof(temp));
  itoa(sent_photo_number, temp, 10);
  strcat(package,temp);
  
  //end
  strcat(package,"!");
  
  printString2(package);

  printString(package);
  printString("\r\n");
  
  
}


void CanSat :: reset_counter() {
  timer = 0;
  number_of_package = 0;

  base_altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  altitude = base_altitude;
//  pre_altitude = altitude; 

  separated = false;
  reset_command = true;
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

  
