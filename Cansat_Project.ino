#include "functions.h"

//team_id,time,num_of_telemetry,battery_vol,battery_vol_motor,height,velocity,gps_en,gps_uzun,picture_status,rpm1,rpm2,seperation_time,num_of_taken_photo,number_of_photos_sent_to_earth
//$2278  ,2   ,3               ,4          ,5                ,6     ,7       ,8     ,9       ,10            ,11  ,12  ,13             ,14                ,15!
int capture_stat = 0;
int pre_time = 0;
int cur_time = 0;

CanSat cnst;

//USART INTERRUPT handle  
ISR(USART2_RX_vect){

  char ch=receiveByte2();
  
  if(ch=='1'){   
    cnst.flag = true;
    printString("1 basildi\r\n");
    cnst.number_of_package = 0;
    cnst.timer = 0;
    cnst.base_pressure = cnst.bme.readPressure();
  }

  if (ch=='2') {
    cnst.motor_flag = true;
    printString("2 basildi\r\n");
  }
  if (ch =='3') {
    cnst.motor_flag = false;
    printString("3 basildi\r\n");
  }
  
  else if(ch=='0'){
    cnst.statu=0;
    printString("0 basildi\r\n");
  }
  else{
    //printString("Lalala\r\n");
    transmitByte(ch);
  }
}

//Timer INTERRUPT
ISR (TIMER1_OVF_vect)    // Timer1 ISR
{
  TCNT1 = 49911;   
  
  if(cnst.flag ){

    cur_time++;
    cnst.timer++;
    //Get actual data
    cnst.getData();
     
    //Get test data
//    String package = "";
//    char temp[100];
//    memset(temp, 0, sizeof temp);
//  
//    char a = (char)255;
//    package += a;
//    char b = (char)217;
//    package += b;
//    package += "$1,2,3,4,5,6,7,8,9,10,11,12,13,14,15!";
//
//    for(int i = 0;i < package.length(); i++){
//      transmitByte2(package[i]);
//    }
//    package = ""; 
  }
}

int k = 2;
void setup() {

  cnst.init();
  bool t = false;
  while(true){

    uint16_t ldrValue = analogRead(A3);
    if (ldrValue >= 650) cnst.motor_flag = true;
    else cnst.motor_flag = false;
     
    if (cnst.motor_flag) {
      cnst.runProp(250, 250);
    }
    else {
      cnst.stopProp();
    }
    
    if(cnst.flag){
      cur_time = millis();
      cnst.camera.capture(cnst.gps.date, cnst.gps.utc_time);
      Serial.print("Capture time:"  );
      cur_time = millis()-cur_time;
      Serial.println(float(cur_time)/1000);
    }
  }
}
  
void loop() {   
 
 // cnst.getData();
 //transmitByte(receiveByte1());
 //printString2("$2354,036,29,0,101960,26,6.4,06.01.2080.00:01:13,45.45N,65.12E,0,0,53!\r\n");
   
}
