#include <WiFi.h>
 
 
const char * ID = "360wifi";
const char * PASSWORD = "625625625";
 
 
void setup() 
{
  Serial.begin(115200);
  
  Serial.println("WiFi:");
  Serial.println(ID);
  Serial.println("PASSWORLD:");
  Serial.println(PASSWORD);
  
  WiFi.begin(ID,PASSWORD);
  
  while(WiFi.status()!=WL_CONNECTED)
  {
    delay(500);
    Serial.println("connect...");
  }
 
  Serial.println("connect success!");
 
}
 
void loop() 
{
  Serial.println("connect success!");
  delay(5000);
}