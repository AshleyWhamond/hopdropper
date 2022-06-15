/*
 * Project hopdropper
 * Description: Particle Argon project to monitor Co2 levels and trigger alerts
 * Author: Ashley
 * Date: 25/05/22
 */

/*Library used for Co2 Sensor
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

SCD30 airSensor;
int magnetPower = D7;
int i = 0;
float gravity = 1.055; 
uint16_t currentCo2 = 0;
uint16_t peakCo2 = 0;
boolean dropHops = false;

// records the peak level of Co2 recorded during the timeframe
uint16_t getPeakCo2(uint16_t co2){
  if(currentCo2 < co2){
    currentCo2 = co2;
  }
  return currentCo2;
}

//cut power to the electromagnet
void turnOffMag()
{
  digitalWrite(magnetPower, LOW);
}

// restor power to the magnet
void resetMag()
{
  digitalWrite(magnetPower, HIGH);
}

//poorly named function that takes in the obove function as arguments
int sendCo2Level(String command);



void setup()
{
  Wire.stretchClock(true); //This is apparently required for the I2C interface
  Wire.begin();
  pinMode(magnetPower, OUTPUT); //set the pin connected to the magnet to output
  digitalWrite(magnetPower, HIGH); //engage the magnet by setting to HIGH

  Particle.function("sendData", sendCo2Level); //Particle function that sets up the sendCozLevel function to be accessible with the sendData key

  Particle.publish("co2ppm", "{\"trigger\":" + String(dropHops) + "}"); // Custom webhooks to send to Datacake. These set the data to defaults 
  Particle.publish("co2ppm", "{\"peak\":" + String(peakCo2) + "}");


  airSensor.begin(); // This will cause readings to occur every two seconds
}

void loop()
{
  uint16_t co2 = airSensor.getCO2();

  Particle.publish("co2ppm", "{\"co2\":" + String(co2) + "}"); // send Co2 webhook

  peakCo2 = getPeakCo2(co2); // get the peak co2

  Particle.publish("co2ppm", "{\"peak\":" + String(peakCo2) + "}"); // send peak webhook

  // trigger if the co2 level drops to half of the peak level recorded
  if (co2 <= peakCo2 / 2){
    dropHops = true;
    Particle.publish("co2ppm", "{\"trigger\":" + String(dropHops) + "}");
  }


  delay(60000); // do this every minute.
}

//define the arguments 
int sendCo2Level (String command) {
  uint16_t co2 = airSensor.getCO2();
  //manually send the co2 data
  if(command == "send"){
    bool success = Particle.publish("co2ppm", "{\"co2\":" + String(co2) + "}");
    return 1;

  }
  // call to drop the hop bag
  else if(command == "dropMag") {
    turnOffMag();
    return 1;
  }
  //call to reset the magnet
  else if(command == "resetMag") {
    resetMag();
    return 1;
  }
  else{
    bool success = Particle.publish("error", "1");
    return -1;
  }
}
