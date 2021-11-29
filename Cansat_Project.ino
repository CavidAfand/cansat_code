#include "functions.h"

//team_id,time,num_of_telemetry,battery_vol,battery_vol_motor,height,velocity,gps_en,gps_uzun,picture_status,rpm1,rpm2,seperation_time,num_of_taken_photo,number_of_photos_sent_to_earth
//$2278  ,2   ,3               ,4          ,5                ,6     ,7       ,8     ,9       ,10            ,11  ,12  ,13             ,14                ,15!
int capture_stat = 0;
int pre_time = 0;
int cur_time = 0;
volatile int captured_photo_number = 0;
volatile int sent_photo_number = 0; 
volatile char image_names[8];

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
ISR (TIMER4_OVF_vect)    // Timer1 ISR
{
  TCNT4 = 49911;   

  if (cnst.flag == true) {
    cur_time++;
    cnst.timer++;
    //Get actual data
    cnst.getData();
    
    if (cnst.separated == true && cnst.timer%2==0 && cnst.camera_flag == false) {
      cnst.camera_flag = true;
      memset(image_names, '\0', 8);
      strcat(image_names, cnst.gps.utc_time);
    }
  }
     
}

// Infrared_above (D2) - external interrupt
ISR(INT4_vect)  {
  cnst.motor_above_rotation++;
}

// Infrared_above (D3) - external interrupt
ISR(INT5_vect) {
  cnst.motor_under_rotation++;
}

void setup() {
  
  cnst.init();

  uint16_t alt_image_num = 1000;

  while(true){
    // check if model was separated or not
    if (cnst.separated == false) {
      // check light sensor
      uint16_t ldrValue1 = analogRead(A3);
      uint16_t ldrValue2 = analogRead(A3);
      uint16_t ldrValue3 = analogRead(A3);
      uint16_t ldrValue = (ldrValue1 + ldrValue2 + ldrValue3) / 3;

      // check if model is being separated or not
      if (ldrValue >= 700 && cnst.altitude >= 1.1) {
        if (cnst.separated == false) {
          memset(cnst.separation_time, '\0', 14);
          strcat(cnst.separation_time, cnst.gps.utc_time);

          if (cnst.reset_command == true) {
            cnst.motor_flag = true;
            cnst.separated = true;
          }
        }
      }
    }


    // motor will rotate if model was separated
    if (cnst.motor_flag && cnst.reset_command) {
      cnst.servo.write(113);
      delay(450);
      cnst.runProp(255, 255);
    }


    // buzzer beeps if reset command was received
    if (cnst.reset_buzzer_beep == true) {
      cnst.buzzer_ms(500);
      cnst.reset_buzzer_beep = false;
    }
    
    // capture photo
    if(cnst.camera_flag && cnst.flag && cnst.reset_command){
      char alt_image_name[5];
      sprintf(alt_image_name, "%d", alt_image_num);
      cur_time = millis();
      cnst.camera.capture(image_names, alt_image_name);
      Serial.print("Capture time:"  );
      cur_time = millis()-cur_time;
      Serial.println(float(cur_time)/1000);
      cnst.camera_flag = false;
    }
    alt_image_num++;

    // mission completed, leave loop
//    if (cnst.flag == true && cnst.separated == true && cnst.model_speed < 1) {
//      cnst.stopProp();
//      cnst.flag = false;
//    }

  }
}
  
void loop() {   
 
}
