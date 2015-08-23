#include "arduino.h"

#define DEBUG

//WIFI--------------------
#define WLAN_SSID       "Makermodule"        // 你的WIFI名字
#define WLAN_PASS       "microduino"        //你的WIFI密码

#define WLAN_SECURITY   WLAN_SEC_WPA2    // 你的WIFI加密方式： WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2

//YEELINK-----------------
#define NUM 6
#define website "api.yeelink.net"
#define apiKey  "**"  //API KEY,在YEELINK里的账户-我的账户设置中  

#define net_device "6816"
char* net_sensor[NUM]=
{
  "**","**","**","**","**","**"
}; 

#define INTERVAL_UPDATA          30000

//SENSOR------------------
#define INTERVAL_SENSOR          2000

//OSD---------------------
#define INTERVAL_OSD             2000
#define SLEEP_OSD                10000


//========================
//Code to print out the free memory
void freeMem(char* message) 
{
  extern unsigned int __heap_start, *__brkval; 
  unsigned int v;
  Serial.print(message);
  Serial.print(":");
  Serial.println((unsigned int) &v - (__brkval == 0 ? (unsigned int) &__heap_start : (unsigned int) __brkval));
}
