#include "SoftwareSerial.h"      
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
SoftwareSerial Esp32(3,2);  
#define REPORTING_PERIOD_MS     1000 // 2s lấy mẫu 

PulseOximeter pox;

uint32_t tsLastReport = 0;

// Callback (registered below) fired when a pulse is detected
void onBeatDetected()
{
    Serial.println("Beat!");
}

void setup()
{
    Serial.begin(115200);
    
Esp32.begin(9600);
    Serial.print("Initializing pulse oximeter..");
 if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }
    pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop()
{
     pox.update();
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
      Esp32.print( "@" + String(pox.getHeartRate()) +"#" + String(pox.getSpO2()) +"$"); // 
      //Esp32.println(); 
      Serial.print("Heart rate:");
        Serial.print(pox.getHeartRate());
        Serial.print("bpm / SpO2:");
        Serial.print(pox.getSpO2());
      Serial.println("%");
        tsLastReport = millis();
        
    } /// abcd
}
