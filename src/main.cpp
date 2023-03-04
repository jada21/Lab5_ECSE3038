/* 620141014
JADA CAMPBELL
ECSE3038 LAB4  */
#include <Arduino.h>
#include <Arduino.h>
#include <Wifi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "env.h"
#define LED1 13
#define LED2 12
#define LED3 32
char * endpoint = API_URL;   //api url
bool matrix[8][3] = { {true, true, true}, 
                      {false, true, true},
                      {true, false, true},
                      {true, true, false},
                      {false, false, false},
                      {true, true, false},
                      {false, true, true},
                      {false, true, false} };

void setup() 
{
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  Serial.begin(9600);
  WiFi.begin(WIFI_NAME, WIFI_PASS);
  Serial.println("Connecting");

  while(WiFi.status() != WL_CONNECTED)    
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() 
{
  if(WiFi.status()== WL_CONNECTED)
  {
    int j =0;
    for (int i = 0; i < 8; i++)
    {
      HTTPClient http;  
      http.begin(endpoint);

      // Specify headers
      http.addHeader("X-API-Key", API_KEY);
      http.addHeader("Content-Type", "application/json");
      
      StaticJsonDocument<76> doc;               // Empty JSONDocument
      String httpRequestData;                   //Emtpy string to be used to store HTTP request data string
      int httpResponseCode;
      String http_response;
      StaticJsonDocument<1024> doc1;
      
      Serial.print("Row: ");                    //for personal debugging
      Serial.println(i);
      doc["light_switch_1"] = matrix[i][j];
      doc["light_switch_2"] = matrix[i][j+1];
      doc["light_switch_3"] = matrix [i][j+2];

      serializeJson(doc, httpRequestData);
      httpResponseCode = http.PUT(httpRequestData);
      http_response;

      if (httpResponseCode>0) 
      {
        Serial.print("HTTP Response code from request: ");
        Serial.println(httpResponseCode);

        Serial.print("HTTP Response from server: ");
        http_response = http.getString();
        Serial.println(http_response);
        Serial.println(" ");
        
        http.end();
        DeserializationError error = deserializeJson(doc1, http_response);

        if (error)                              //if deserializtion fails
        {
          Serial.print("deserializeJson() failed: ");
          Serial.println(error.c_str());
          return;
        }

        const bool led1_state = doc1["light_switch_1"];
        const bool led2_state = doc1["light_switch_2"]; 
        const bool led3_state = doc1["light_switch_3"];

        Serial.println("Data:");
        Serial.println(led1_state);
        Serial.println(led2_state);
        Serial.println(led3_state);
        digitalWrite(LED1, led1_state);
        digitalWrite(LED2, led2_state);
        digitalWrite(LED3, led3_state);
      }

      else                                      //if http response code is negative
      {
        Serial.print("Error code: ");
        http_response = http.getString();
        Serial.println(http_response);
      }

      j=0;
      delay(2000);
    }                                         //close for loop
  }                                           //end if wifi connected

  else 
  {
    Serial.println("WiFi Disconnected");
  }

}