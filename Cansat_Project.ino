#include "functions.h"

//team_id,time,num_of_telemetry,battery_vol,battery_vol_motor,height,velocity,gps_en,gps_uzun,picture_status,rpm1,rpm2,seperation_time,num_of_taken_photo,number_of_photos_sent_to_earth
//$2278  ,2   ,3               ,4          ,5                ,6     ,7       ,8     ,9       ,10            ,11  ,12  ,13             ,14                ,15!
int capture_stat = 0;
int pre_time = 0;
int cur_time = 0;
volatile int captured_photo_number = 0;
volatile int sent_photo_number = 0; 

CanSat cnst;

//USART INTERRUPT handle  
ISR(USART2_RX_vect){
  char ch=receiveByte2();
  if (ch == '1')  {
    // sifirlama emri gelib
    cnst.reset_counter();
  }
}

//Timer INTERRUPT
ISR (TIMER1_OVF_vect)    // Timer1 ISR
{
  TCNT1 = 49911;   
  
  cur_time++;
  cnst.timer++;
  //Get actual data
  cnst.getData();

  if (cnst.separated == true && cnst.timer%2==0 && cnst.camera_flag == false) {
    cnst.camera_flag = true;
  }
     
}


void setup() {
  
  cnst.init();

  while(true){

    if (cnst.separated == false) {
    uint16_t ldrValue1 = analogRead(A3);
    uint16_t ldrValue2 = analogRead(A3);
    uint16_t ldrValue3 = analogRead(A3);

       
      if (ldrValue1 >= 70 && ldrValue2 >= 700 && ldrValue3 >= 700/* && cnst.height >= 350*/) {
        if (cnst.separated == false) {
          memset(cnst.separation_time, '\0', 14);
          strcat(cnst.separation_time, cnst.gps.utc_time);
          cnst.motor_flag = true;
          cnst.separated = true;
        }
      }
   }

   if (cnst.spee == 0) {
    cnst.motor_flag == false;
   }
     
    if (cnst.motor_flag) {
      cnst.runProp(250, 250);
    }
    else {
      cnst.stopProp();
    }
    
    if(cnst.camera_flag){
      cur_time = millis();
      cnst.camera.capture(cnst.gps.utc_time);
      Serial.print("Capture time:"  );
      cur_time = millis()-cur_time;
      Serial.println(float(cur_time)/1000);
      if (cur_time < 1.0)
      cnst.camera_flag = false;
    }
  }
}
  
void loop() {   
 
}
