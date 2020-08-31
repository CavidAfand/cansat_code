#include "camera.h"

  Camera :: Camera(void) : myCAM(OV2640, CS){}
  
  extern int capture_stat;
  extern char utc[7];
  
  void Camera :: init(void){    
    
    uint8_t vid, pid;
    uint8_t temp;
    Wire.begin();
    printString("Arducam Start\n");
    // set the CS as an output:
    pinMode(CS, OUTPUT);
    digitalWrite(CS, HIGH);

    // initialize SPI:
    SPI.begin();
    //Reset the CPLD
    myCAM.write_reg(0x07, 0x80);
    delay(100);
    myCAM.write_reg(0x07, 0x00);
    delay(100); 
  
    while(1){
      //Check if the ArduCAM SPI bus is OK
      myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
      temp = myCAM.read_reg(ARDUCHIP_TEST1);
      if(temp != 0x55){
        printString("SPI interface Error!\n");
        delay(1000);continue;
      }
      else{
        printString("SPI interface OK.\n");
        break;
      }
    }
    while(1){
      //Check if the camera module type is OV2640
      myCAM.wrSensorReg8_8(0xff, 0x01);
      myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
      myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
      if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 ))){
        Serial.println(F("Can't find OV2640 module!"));
        delay(1000);
        continue;
      }
      else{
        Serial.println(F("OV2640 detected."));
        break;
      } 
    }
    
    //Initialize SD Card
    while(!SD.begin(SD_CS)){
      printString("SD Card Error!\n");
      _delay_ms(1000);
    }
    printString("SD Card detected.\n");
    //Change to JPEG capture mode and initialize the OV5640 module
    myCAM.set_format(JPEG);
    myCAM.InitCAM();
//    myCAM.set_bit(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);
    myCAM.clear_fifo_flag();
//    myCAM.write_reg(ARDUCHIP_FRAMES, FRAMES_NUM);
    myCAM.OV2640_set_JPEG_size(OV2640_640x480);delay(1000);
    
    } 



void Camera :: capture(void){
  
    myCAM.flush_fifo();
    myCAM.clear_fifo_flag();
    
    //Start capture
    myCAM.start_capture();  
    printString("start capture.\n");
    while ( !myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)); 
    printString("CAM Capture Done.\n");
    read_fifo_burst(myCAM);
    myCAM.clear_fifo_flag();
//    _delay_ms(500);
  
  } 


uint8_t Camera :: read_fifo_burst(ArduCAM myCAM)
{
  uint8_t temp = 0, temp_last = 0;
  uint32_t length = 0;
  static int i = 0;
  static int k = 0;
  char str[8];
  char len[8];
  File outFile;
  byte buf[256]; 
  length = myCAM.read_fifo_length();
  Serial.print(F("The fifo length is :"));
  Serial.println(length, DEC);
  itoa(length, len, 10);
  
  if (length >= MAX_FIFO_SIZE) //8M
  {
    Serial.println("Over size.");
    return 0;
  }
  if (length == 0 ) //0 kb
  {
    Serial.println(F("Size is 0."));
    return 0;
  } 
  myCAM.CS_LOW();
  myCAM.set_fifo_burst();//Set fifo burst mode
  i = 0;
  while ( length-- )
  {
    temp_last = temp;
    temp =  SPI.transfer(0x00);
    //Read JPEG data from FIFO
    if ( (temp == 0xD9) && (temp_last == 0xFF) ) //If find the end ,break while,
    {
      buf[i++] = temp;  //save the last  0XD9     
      //Write the remain bytes in the buffer
      myCAM.CS_HIGH();
      outFile.write(buf, i);
      for(int t=0; t<i;t++){
          transmitByte2(buf[t]);
          
        }
          
      //Close the file
      outFile.close();
      Serial.println(F("OK"));
      capture_stat = 1;
      
      is_header = false;
      myCAM.CS_LOW();
      myCAM.set_fifo_burst();
      i = 0;
    }  
    if (is_header == true)
    { 
      //Write image data to buffer if not full
      if (i < 256)
       buf[i++] = temp;
      else
      {
        //Write 256 bytes image data to file
        myCAM.CS_HIGH();
        outFile.write(buf, 256);
        for(int t=0; t<256;t++){
          transmitByte2(buf[t]);
        }
        _delay_ms(5);
        
        i = 0;
        buf[i++] = temp;
        myCAM.CS_LOW();
        myCAM.set_fifo_burst();
      }        
    }
    else if ((temp == 0xD8) & (temp_last == 0xFF))
    {
      is_header = true;
      myCAM.CS_HIGH();
      //Create a avi file
      k = k + 10;
      itoa(k, str, 10);  // ad qoyma
  //    Serial.print(utc[0]);
  //    Serial.println(utc[1]);
  //    
  //    strcat(str, utc);
      strcat(str, ".jpg");
      //Open the new file
      outFile = SD.open(str, O_WRITE | O_CREAT | O_TRUNC);
      if (! outFile)
      {
        Serial.println(F("File open failed"));
        while (1);
      }
      myCAM.CS_LOW();
      myCAM.set_fifo_burst();   
      buf[i++] = temp_last;
      buf[i++] = temp;   
    }
  }
    myCAM.CS_HIGH();
    return 1;
}
     
