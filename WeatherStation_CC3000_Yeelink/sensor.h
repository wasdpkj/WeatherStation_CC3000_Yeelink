#include "arduino.h"

//2,传感器部分================================   
//温湿度   
#include <AM2321.h>                             //调用库  
AM2321 am2321;   
//气压   
#include "BMP085.h"                             //调用库  
BMP085 barometer;   
//光照   
#include <Adafruit_Sensor.h>                     //调用库  
#include <Adafruit_TSL2561_U.h>                 //调用库  
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_LOW, 12345);   

//传感器的值设置
float sensor_tem,sensor_hum,sensor_alt,sensor_pre,sensor_lux; //温度、湿度、海拔、气压、光照

unsigned long sensor_time=millis();

void sensor_init()
{
  Serial.println();   

  //初始化-气压   
  barometer.initialize();   
  Serial.println(barometer.testConnection() ? "BMP085 successful" : "BMP085 failed");   

  //初始化-光照   
  Serial.println(tsl.begin() ? "TSL2561 successful" : "TSL2561 failed");   
  tsl.enableAutoGain(true);                                  // 在1x 与16x 增益中切换  
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      //13MS的采样速度  
}

void sensor_get()
{
  //获取温湿度============================================== 
  am2321.read();   
  sensor_tem = am2321.temperature / 10.0;   
  sensor_hum = am2321.humidity / 10.0;   

  //获取气压、海拔========================================== 
  barometer.setControl(BMP085_MODE_TEMPERATURE);   
  unsigned long lastMicros = micros();      
  //先获取温度，用来补偿气压值  
  while (micros() - lastMicros < barometer.getMeasureDelayMicroseconds());    
  barometer.getTemperatureC();  
  barometer.setControl(BMP085_MODE_PRESSURE_3);   
  //得出气压值  
  while (micros() - lastMicros < barometer.getMeasureDelayMicroseconds());  
  sensor_pre = barometer.getPressure();  
  //结合气压值，以标准大气压得出海拔值  
  sensor_alt = barometer.getAltitude(sensor_pre);    

  //获取光照===============================================    
  sensors_event_t event;   
  tsl.getEvent(&event);   
  (event.light)?  sensor_lux = event.light : Serial.println("Sensor overload");   
}
