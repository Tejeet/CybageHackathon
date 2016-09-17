 /*
 ***************** TeJEEt Magar ***********************
 *
 * --------Smart Tap Water Metering and Detection System
 *       And Automatic Tap Water Closing System---------
 *       
 *       COEP Cybage Hackathon 
 * 
 *     
 *
 *
 *
 *
 ******************************************************
 
 */
#include <ESP8266WiFi.h>

  String apiKey = "PLATEPZ1RHHEBYS0"; 
  const char* ssid = "Dlink"; 
  const char* password = "9881283312"; 
  const char* server = "api.thingspeak.com"; 

byte statusLed    = 2;

byte sensorInterrupt = 0; 
byte sensorPin       = 0;

float calibrationFactor = 4.5;

volatile byte pulseCount;  

float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

unsigned long oldTime;

WiFiClient client; 

void setup()
{
  
 
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
   
  pinMode(statusLed, OUTPUT);
  digitalWrite(statusLed, HIGH);  
  
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;

  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
}


void loop()
{

  


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
    Serial.print(".");             // Print the decimal point

    frac = (flowRate - int(flowRate)) * 10;
    Serial.print(frac, DEC) ;      // Print the fractional part of the variable
    Serial.print("L/min");
   
    Serial.print("  Current Liquid Flowing: ");             // Output separator
    Serial.print(flowMilliLitres);
    Serial.print("mL/Sec");

  
    Serial.print("  Output Liquid Quantity: ");             // Output separator
    Serial.print(totalMilliLitres);
    Serial.println("mL"); 

    // Reset the pulse counter so we can start incrementing again
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
   
                Serial.println("Data is Sent on thingSpeks Channel");
                Serial.print("\n\n");
      
             }
  

}


void pulseCounter()
{
 
  pulseCount++;
}

