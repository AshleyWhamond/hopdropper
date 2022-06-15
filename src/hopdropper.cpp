/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/ashkl/Desktop/Deakin_22/SIT_210/Project/hopdropper/hopdropper/src/hopdropper.ino"
/*
 * Project hopdropper
 * Description:
 * Author:
 * Date:
 */

/*
  Reading CO2, humidity and temperature from the SCD30
  By: Nathan Seidle
  SparkFun Electronics
  Date: May 22nd, 2018
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

*/

#include <Wire.h>

// Click here to get the library: http://librarymanager/All#SparkFun_SCD30
#include "../lib/SparkFun_SCD30_Arduino_Library/src/SparkFun_SCD30_Arduino_Library.h"

uint16_t getPeakCo2(uint16_t co2);
void turnOffMag();
void resetMag();
void setup();
void loop();
int sendCo2Level (String command);
#line 23 "c:/Users/ashkl/Desktop/Deakin_22/SIT_210/Project/hopdropper/hopdropper/src/hopdropper.ino"
SCD30 airSensor;
int magnetPower = D7;
int i = 0;
float gravity = 1.055; 
uint16_t currentCo2 = 0;
uint16_t peakCo2 = 0;
boolean dropHops = false;

uint16_t getPeakCo2(uint16_t co2){
  if(currentCo2 < co2){
    currentCo2 = co2;
  }
  return currentCo2;
}


void turnOffMag()
{
  digitalWrite(magnetPower, LOW);
}
void resetMag()
{
  digitalWrite(magnetPower, HIGH);
}

int sendCo2Level(String command);



void setup()
{
  Wire.stretchClock(true);
  Wire.begin();
  pinMode(magnetPower, OUTPUT);
  digitalWrite(magnetPower, HIGH);

  Particle.function("sendData", sendCo2Level);

  Particle.publish("co2ppm", "{\"trigger\":" + String(dropHops) + "}");
  Particle.publish("co2ppm", "{\"peak\":" + String(peakCo2) + "}");


  airSensor.begin(); // This will cause readings to occur every two seconds
}

void loop()
{
  uint16_t co2 = airSensor.getCO2();

  Particle.publish("co2ppm", "{\"co2\":" + String(co2) + "}");

  peakCo2 = getPeakCo2(co2);

  Particle.publish("co2ppm", "{\"peak\":" + String(peakCo2) + "}");

  if (co2 <= peakCo2 / 2){
    dropHops = true;
    Particle.publish("co2ppm", "{\"trigger\":" + String(dropHops) + "}");
  }


  delay(60000);
}

int sendCo2Level (String command) {
  uint16_t co2 = airSensor.getCO2();
  if(command == "send"){
    bool success = Particle.publish("co2ppm", "{\"co2\":" + String(co2) + "}");
    return 1;

  }
  else if(command == "dropMag") {
    turnOffMag();
    return 1;
  }
  else if(command == "resetMag") {
    resetMag();
    return 1;
  }
  else{
    bool success = Particle.publish("error", "1");
    return -1;
  }
}
