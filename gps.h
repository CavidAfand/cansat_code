#include <avr/io.h>

class GPS{
  public:
  
  	char  longtitude[12];
  	char  latitude[12];
  	char  sat_num[3];
    //char  header[6];
    char  utc_time[10];
    char  date[7];
    char  date_time[25];
    char  GPGGA[100];
    char  GPRMC[100];  	

  
  	GPS();
  	bool check();
    void get_data();
    void date_format();
    void get_data_new();
};
