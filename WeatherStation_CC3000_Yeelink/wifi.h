#include "arduino.h"

boolean NET_WEBSITE_sta=false;

unsigned long updata_time=millis();

//================================
#include <Adafruit_CC3000.h>

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   2  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  9
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,SPI_CLOCK_DIV2); // you can change this clock speed


uint32_t ip;

Adafruit_CC3000_Client www;

//========================================
void listSSIDResults(void)
{
  uint8_t valid, rssi, sec, index;
  char ssidname[33]; 

  index = cc3000.startSSIDscan();

  Serial.print(F("Networks found: ")); 
  Serial.println(index);
  Serial.println(F("================================================"));

  while (index) {
    index--;

    valid = cc3000.getNextSSID(&rssi, &sec, ssidname);

    Serial.print(F("SSID Name    : ")); 
    Serial.print(ssidname);
    Serial.println();
    Serial.print(F("RSSI         : "));
    Serial.println(rssi);
    Serial.print(F("Security Mode: "));
    Serial.println(sec);
    Serial.println();
  }
  Serial.println(F("================================================"));

  cc3000.stopSSIDscan();
}

/**************************************************************************/
/*!
 @brief  Tries to read the IP address and other connection details
 */
/**************************************************************************/
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;

  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("\r\nUnable to retrieve the IP Address!"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); 
    cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); 
    cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); 
    cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); 
    cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); 
    cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}


bool WIFI_get_dhcp()
{
  bool cache;
  //  Serial.println(F("Request DHCP");

  while (!cc3000.checkDHCP())
  {
    cache=false;
    delay(100); // ToDo: Insert a DHCP timeout!
  }  
  cache=true;

  if(!cache)
    return false;
  else
  {
    /* Display the IP address DNS, Gateway, etc. */
    while (! displayConnectionDetails()) 
    {
      cache=false;
      delay(500);
    }
    cache=true;

    return cache ? true :false;
  }
}

bool WIFI_get_server_ip(char _website[])
{
  ip = 0;
  // Try looking up the website's IP address
  Serial.print(_website); 
  Serial.print(F(" -> "));
  while (ip == 0) {
    if (! cc3000.getHostByName(_website, &ip)) {
      Serial.println(F("Couldn't resolve!"));
    }
    Serial.println(F("while!"));

    delay(500);
  }

  cc3000.printIPdotsRev(ip);
  Serial.println(F("  "));

  return ip ? true :false;
}

boolean WIFI_pos()  //num,vol
{
  // Optional: Do a ping test on the website
  /*
  Serial.print(F("\n\rPinging "); cc3000.printIPdotsRev(ip); Serial.print("...");  
   replies = cc3000.ping(ip, 5);
   Serial.print(replies); Serial.println(F(" replies");
   */


  www = cc3000.connectTCP(ip, 80);
  Serial.println("Ping");

  if (www.connected()) 
  {
    yeelink_begin();
    //----------------
    Serial.println("post.start.......................");
    // send the HTTP PUT request:

    www.print("POST ");
    www.print("/v1.1/device/");
    www.print(net_device);
    www.print("/datapoints");
    www.print(" HTTP/1.1\r\n");
    www.print("Host: ");
    www.println(website);
    www.print("Accept: */*\r\n");
    www.print("U-ApiKey: ");
    www.println(apiKey);
    www.print("Content-Length: ");
    www.println(buf.length());

    www.print("Content-Type: application/x-www-form-urlencoded\r\n");
    www.print("Connection: close\r\n");
    www.print("\r\n");

    // here's the actual content of the PUT request:
    www.println(buf);

    Serial.println("post.end.........................");

    return true;
  }
  else  
    return false;
}


void WIFI_rec()
{
  Serial.println(F("read.start........................."));

  while (www.connected()) 
  {
    while (www.available()) 
    {
      char c=www.read();

      Serial.print(c);
    }
  }
  www.close();

  /* You need to make sure to clean up after yourself or the CC3000 can freak out */
  /* the next time your try to connect ... */
  //cc3000.disconnect();
  //Serial.println(F("\n\nDisconnecting"));

  Serial.println(F("read.end..........................."));
}


bool WIFI_init(int _WIFI_init)
{
  //1/4----------------------------------------------------------
  Serial.println(F("\n......CC3000_init"));

  switch(_WIFI_init)
  {
  case 0:
    {
      Serial.println("\n 1/4 begin");
      if (!cc3000.begin())
      {
        Serial.println("\n 1/4 begin ERROR");
        return false;
      }
      else
      {
        Serial.println("1/4 begin OK");
        return true;
      }
    }  
    break;
  case 1:
    {
      Serial.println("\n 2/4 connect AP READY");
      if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) 
      {
        Serial.println("2/4 connect AP ERROR");
        Serial.println(F("Failed!"));
        return false;
      }
      else
      {
        Serial.println("\n2/4 connect AP OK");
        return true;
      }
    }
    break;
  case 2:
    {
      Serial.println("\n 3/4 dhcp READY");
      if(!WIFI_get_dhcp())
      {
        Serial.println("3/4 dhcp ERROR");
        Serial.println(F("Failed!"));
        return false;
      }
      else
      {
        Serial.println("3/4 dhcp OK");
        return true;
      }

    }
    break;
  case 3:
    {
      Serial.println("\n 4/4 server READY");
      if(!WIFI_get_server_ip(website))
      {
        Serial.println("4/4 server ERROR");
        Serial.println(F("Failed!"));
        return false;
      }
      else
      {
        Serial.println("4/4 server OK");
        return true;
      }
    }
    break;
  }


  //----------------------------------------------------------
  //  cc3000.stop();
  // Optional SSID scan
  // listSSIDResults();
}
