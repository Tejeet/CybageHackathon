 /*
 ******************************* TeJEEt Magar ********************************
 *
 *
 *
 * ----Smart Tap Water Metering and Detection System
 *       And Automatic Tap Water Closing System---------
 *       
 *  In this Project we can able to calculate the current Flow rate
 *  of water to the respective user in any Flat systme Homes
 *
 *   Sometimes we forgot to close the tap of water so the Water Wastage
 *  may takes place so by this system we can close the main inlet of 
 *   our Water Tap any We can Save the Water from wastage.....
 *   
 *   Also we can give the alert to user about Leakage and from user side
 *   if no any action is takes place then System will automatically close
 *   the main inlet of user and make them Panalty for water wastage this 
 *   way we can save the water and make the People to aware about improper
 *   usage of water
 *       
 * 
 *     
 *            COEP ( Mind-Spark) Cybage Hackathon  
 *
 *
 *
 ***************************************************************************
 */


#include <ESP8266WiFi.h>
#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;


String apiKey = "PLATEPZ1RHHEBYS0"; 
const char* ssid = "Dlink"; 
const char* password = "9881283312"; 
const char* server = "api.thingspeak.com"; 



byte sensorInterrupt = 0; 
byte sensorPin       = 0;
byte Relay    = 16;

float calibrationFactor = 4.5;

volatile byte pulseCount;  

float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

unsigned long oldTime;

WiFiClient client; 


void setup()
       
{
          lcd.begin(16, 2);
          lcd.print("CYBAGE HAKATHON");
          delay(10);
          
           Serial.begin(115200);
           Serial.println();
           Serial.println();
           Serial.print("Connecting to ");
           Serial.println(ssid);
           WiFi.begin(ssid, password); 

           while (WiFi.status() != WL_CONNECTED)
              { 
                delay(500);
                Serial.print(".");
              }
           Serial.println("");
           Serial.println("WiFi connected");
           lcd.setCursor(0, 1); 

   
  pinMode(Relay, OUTPUT);
  digitalWrite(Relay, LOW);  
  
  pinMode(sensorPin, INPUT);
  digitalWrite(Relay, HIGH);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;

  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
}



void loop()

{

  if (totalMilliLitres >200)
      {
      breath(REG_RED);
      }
  if (totalMilliLitres <200)
      {
        breath(REG_GREEN);
      }
  if (totalMilliLitres <100)
      {
        breath(REG_BLUE);
      }


     if((millis() - oldTime) > 1000)   
      { 
      
        detachInterrupt(sensorInterrupt);
          
        flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
        oldTime = millis();
    
        flowMilliLitres = (flowRate / 60) * 1000;
        totalMilliLitres += flowMilliLitres;
      
        unsigned int frac;
    
   
        Serial.print("Flow rate: ");
        Serial.print(int(flowRate));  
        Serial.print(".");         

        frac = (flowRate - int(flowRate)) * 10;
        Serial.print(frac, DEC) ;    
        Serial.print("L/min");
   
        Serial.print("  Current Liquid Flowing: ");           
        Serial.print(flowMilliLitres);
        Serial.print("mL/Sec");

  
        Serial.print("  Output Liquid Quantity: ");          
        Serial.print(totalMilliLitres);
        Serial.println("mL");
    
        lcd.setCursor(0, 1); 
        lcd.print("Todays Use:");

        lcd.setCursor(12, 1);
        lcd.write(0b11011111); 
        lcd.print(totalMilliLitres);

       pulseCount = 0;
    

        attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
      }

  
  // For senfing data to Thingspeks Cloud
  if (client.connect(server,80))
      Serial.println("Sending Data to Thingspeks");
            
            {
                String postStr = apiKey;
                postStr +="&field1=";
                postStr += String(flowMilliLitres);
                postStr +="&field2=";
                postStr += String(totalMilliLitres);
                
                
                postStr += "\r\n\r\n"; 
                client.print("POST /update HTTP/1.1\n");
                client.print("Host: api.thingspeak.com\n");
                client.print("Connection: close\n");
                client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                client.print("Content-Type: application/x-www-form-urlencoded\n");
                client.print("Content-Length: ");
                client.print(postStr.length());
                client.print("\n\n");
                client.print(postStr);
                
   
                Serial.println("Data was Succesfully Sent on thingSpeks Channel");
                Serial.print("\n\n");

        // to close the main inlet Automatically if Lekage takes place 

                if (totalMilliLitres >= 300)
                {
                  digitalWrite(Relay, HIGH);
                }
                else
                {
                  digitalWrite(Relay, LOW);      
                }
      
             }
  

}


void pulseCounter()

{ 
  pulseCount++;
}

void breath(unsigned char color)
{
  for(int i=0; i<255; i++)
    {
        lcd.setPWM(color, i);
        delay(5);
    }
    delay(500);
  
  for(int i=254; i>=0; i--)
    {
        lcd.setPWM(color, i);
        delay(5);
    }

}
