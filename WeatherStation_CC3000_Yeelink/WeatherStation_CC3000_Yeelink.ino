/*
=================================
 本作品采用知识共享 署名-非商业性使用-相同方式共享 3.0 未本地化版本 许可协议进行许可
 访问 http://creativecommons.org/licenses/by-nc-sa/3.0/ 查看该许可协议
 版权所有：@老潘orz
 =================================
 
 Microduino wiki:
 	http://wiki.microduino.cc
 
 包含程序库、硬件支持包的IDE可在Getting Start中下载:
 	http://www.microduino.cc/project/view?id=5482ba0248f8313548d82b8c
 */


#include "def.h"
#include "key.h"
#include "sensor.h"
#include "gps.h"
#include "osd.h"
#include "yeelink.h"
#include "wifi.h"

#include <TimerOne.h>

//================================
#include "U8glib.h"                               //调用库  

//================================
#include <Adafruit_CC3000.h>
#include <SPI.h>

//=============================
#include <Wire.h>                                 //调用库  
#include "I2Cdev.h"                             //调用库  
//温湿度   
#include <AM2321.h>                             //调用库  
//气压   
#include "BMP085.h"                             //调用库  
//光照   
#include <Adafruit_Sensor.h>                     //调用库  
#include <Adafruit_TSL2561_U.h>                 //调用库  


void setup(void)
{
  Serial.begin(115200);

  //----------------------------
  Serial.println(F("Hello, Microduino!")); 

  //----------------------------
  //  u8g.setRot180();

  //----------------------------
  osd_setup(1,"INIT WIFI");
  if(WIFI_init(0))
  {
    osd_setup(2,"CONNECT AP");  
    if(WIFI_init(1))
    {
      osd_setup(3,"GET DHCP");  
      if(WIFI_init(2))
      {
        osd_setup(4,"GET SERVER");  
        if(WIFI_init(3))
          NET_WEBSITE_sta =  true;
        else
          NET_WEBSITE_sta = false; 
      }
      else
        NET_WEBSITE_sta = false; 
    }
    else
      NET_WEBSITE_sta = false; 
  }
  else
    NET_WEBSITE_sta = false; 

  if(!NET_WEBSITE_sta)
    osd_setup(0,"NET ERROR");

  //----------------------------
  osd_setup(5,"INIT SENSOR");

  sensor_init();

  //----------------------------
  pinMode(5,INPUT_PULLUP);
  key_init();

  //----------------------------
  osd_sleep=millis();
  Timer1.initialize(100000); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
  Timer1.attachInterrupt( timerIsr ); // attach the service routine here
}

void timerIsr()
{
  if(key_get(5,1))
  {
    osd_sleep=millis();

    u8g.sleepOff();

    lcd_switch=true;
  }

  if(lcd_switch)
  {
    if (osd_sleep > millis()) osd_sleep = millis();
    if(millis()-osd_sleep>SLEEP_OSD)
    {
      lcd_switch=false;

      u8g.sleepOn();

      osd_sleep=millis();
    }
  }
}

void loop(void)
{
  if (sensor_time > millis()) sensor_time = millis();
  if(millis()-sensor_time>INTERVAL_SENSOR)
  {
    sensor_get();

    sensor_time=millis();
  }

  //-------------------------------------------------------------

  if (osd_time > millis()) osd_time = millis();
  if(millis()-osd_time>INTERVAL_OSD)
  {
    osd_main(NET_WEBSITE_sta);

    osd_time=millis();
  }


  //-------------------------------------------------------------
  if(updata_time>millis()) updata_time=millis();
  if(millis()-updata_time>INTERVAL_UPDATA)
  {
    NET_WEBSITE_sta=WIFI_pos();

    if(NET_WEBSITE_sta)
    {
      Serial.println("\n ---UPDATA OK---");

      WIFI_rec();
    }
    else
    {
      Serial.println("\n --UPDATA ERROR--");

      cc3000.reboot();

      //    if(WIFI_init(0))
      //{
      //freeMem("==============Step A");
      if(WIFI_init(1))
      {
        //freeMem("==============Step B");
        if(WIFI_init(2))
        {
          //freeMem("==============Step C");
          if(WIFI_init(3))
            NET_WEBSITE_sta =  true;
          else
            NET_WEBSITE_sta = false; 
        }
        else
          NET_WEBSITE_sta = false; 
      }
      else
        NET_WEBSITE_sta = false; 
      //}
      //    else
      //      NET_WEBSITE_sta = false; 

      //freeMem("==============Step D");

    }

    updata_time=millis();
  }

}

