#include "gps.h"
#include "uart.h"

  GPS :: GPS(void){
	
}
 bool GPS :: check(void){
    if(UCSR1A&(1<<RXC1))
    return 1;
    else
      return 0;  
 }

 void GPS :: get_data() {
  start_point:
  while(receiveByte1()!='M');
  
  if (receiveByte1() != 'C') goto start_point;
  receiveByte1();
  
  char ch;
  // take time from gps
  uint8_t index = 0;
  while ((ch = receiveByte1()) != ',') {
    utc_time[index++] = ch;
  }
  utc_time[index] = '\0';

  // pass [Navigation receiver warning A = OK, V = warning]
  while (receiveByte1() != ',');

  // take latitude
  index = 0;
  while ((ch = receiveByte1()) != ',') {
    latitude[index++] = ch;
  }
  latitude[index] = '\0';

  // pass North or South
  while (receiveByte1() != ',');

  // take longtitude
  index = 0;
  while ((ch = receiveByte1()) != ',') {
    longtitude[index++] = ch;
  }
  longtitude[index] = '\0';

  // pass East or West
  while (receiveByte1() != ',');

  // pass Speed over ground, Knots
  while (receiveByte1() != ',');

  // pass Course Made Good, True
  while (receiveByte1() != ',');

  // take date 
  index = 0;
  while ((ch = receiveByte1()) != ',') {
    date[index++] = ch;
  }
  date[index] = '\0';

  printString("date: ");
  printString(date);
  printString("\r\n");

  printString("time: ");
  utc_time[6] = '\0';
  printString(utc_time);
  printString("\r\n");
  
 }

// void GPS :: get_data(){
//    
//  while(receiveByte1()!='$');
//  receiveByte1();
//  receiveByte1();
//  receiveByte1();
//  if(receiveByte1()=='G')
//  {
//    uint8_t index=0;
//    while(1){
//      GPGGA[index]=receiveByte1();
//      if(GPGGA[index]=='*')
//      break;
//      index++;}
//      GPGGA[++index]='\r';
//      GPGGA[++index]='\n';
//      GPGGA[++index]='\0';
//  }
//  while(receiveByte1()!='M');
//    
//    uint8_t index=0;
//    while(1){
//      GPRMC[index]=receiveByte1();
//      if(GPRMC[index]=='*')
//      break;
//      index++;
//    }
//    GPRMC[++index]='\n';
//    GPRMC[++index]='\r';
//    GPRMC[++index]='\0';
//     
////    printString(GPGGA);
////    printString(GPRMC);
//   // printString("\r\n");
//    uint8_t i=0;
//    for(i = 0; i<6 ; i++){
//         utc_time[i]=GPGGA[2+i];
//    } utc_time[6]='\0';
//    for( i = 0; i<9; i++){
//        latitude[i]=GPGGA[13+i];
//    }   latitude[9]='\0';
//    
//    for( i = 0; i<10; i++){
//        longtitude[i]=GPGGA[25+i];
//    }  longtitude[10]='\0';
//  
//     for( i = 0; i<2; i++){
//        sat_num[i]=GPGGA[40+i];
//    }  sat_num[2]='\0';
//     
//     for( i = 0; i<6; i++){
//        date[i]=GPRMC[index-13+i];
//    }  date[6]='\0';
//
//
//    date_format();
//
//    printString("GPRMC: ");
//    printString(GPRMC);
////    printString("\r\n");
//    printString("GPGGA: ");
//    printString(GPGGA);
//    printString("UTC time: ");
//    printString(date_time);
//    printString("\r\n");
////    printString("Date: ");
////    printString(date);
////    printString("\r\n\r\n");
///*
//    printString(longtitude);
//    printString("\r\n");
//    printString(latitude);
//    printString("\r\n");
//    printString(sat_num);
//    printString("\r\n");
//    printString(date);
//    printString("\r\n");
//    printString(utc_time);
//    printString("\r\n");
//
//*/
//  }   

  
 void GPS :: date_format(void){
    date_time[0]=date[0];
    date_time[1]=date[1];
    date_time[2]='.';
    date_time[3]=date[2];
    date_time[4]=date[3];
    date_time[5]='.';
    date_time[6]='2';
    date_time[7]='0';
    date_time[8]=date[4];
    date_time[9]=date[5];
    date_time[10]='.';
    date_time[11]=utc_time[0];
    date_time[12]=utc_time[1];
    date_time[13]=':';
    date_time[14]=utc_time[2];
    date_time[15]=utc_time[3];
    date_time[16]=':';
    date_time[17]=utc_time[4];
    date_time[18]=utc_time[5];
    date_time[19]='\0';
    
    
 }
 
