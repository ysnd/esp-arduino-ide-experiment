#include <ESP32Servo.h>
#include "BluetoothSerial.h"

BluetoothSerial ESP_BT;
Servo serpo1;
Servo serpo2;


int servo1Pos;
int servo2Pos;
int servo1PPos;
int servo2PPos;
int servo01SP[50], servo02SP[50];
String dataIn = "";
char incoming;

void setup() {
  Serial.begin(115200); //Start Serial monitor in 9600
  ESP_BT.begin("ESP32ArmRobot"); //Name of your Bluetooth Signal
  Serial.println("Bluetooth Device is Ready to Pair");
  serpo1.attach(2);
  serpo2.attach(4);
  delay(20);
  // Robot arm initial position
  servo1PPos=90;
  servo2PPos = 150;
  serpo1.write(servo1PPos);
  serpo2.write(servo2PPos);
  Serial.println("posisi servo di set");
  Serial.println("Posisi Servo 1 :");
  Serial.println(servo1PPos);
  Serial.println("Posisi Servo 2 :");
  Serial.println(servo2PPos);
  

}

void loop() {
  if (ESP_BT.available()) //Check if we receive anything from Bluetooth
  {
    char incoming = ESP_BT.read(); //Read what we recevive 
    if (incoming != '\n'){
      dataIn += String(incoming);
    }
    else{
      dataIn = "";
    }
    Serial.println("Received:"); Serial.println(dataIn);
   // If "Waist" slider has changed value - Move Servo 1 to position
    if (dataIn.startsWith("s1")) {
      String dataInS = dataIn.substring(2, dataIn.length()); // Extract only the number. E.g. from "s1120" to "120"
      servo1Pos = dataInS.toInt();  // Convert the string into integer
      Serial.println(servo1Pos);            // We use for loops so we can control the speed of the servo
      // If previous position is bigger then current position
      if (servo1PPos > servo1Pos) {
        for ( int j = servo1PPos; j >= servo1Pos; j--) {   // Run servo down
          serpo1.write(j);
          delay(20);    // defines the speed at which the servo rotates
        }
      }
      // If previous position is smaller then current position
      if (servo1PPos < servo1Pos) {
        for ( int j = servo1PPos; j <= servo1Pos; j++) {   // Run servo up
          serpo1.write(j);
          delay(20);
        }
      }
      servo1PPos = servo1Pos;   // set current position as previous position
    }
    
    // Move Servo 2
    if (dataIn.startsWith("s2")) {
      String dataInS = dataIn.substring(2, dataIn.length());
      servo2Pos = dataInS.toInt();
      Serial.println(servo2Pos);
      if (servo2PPos > servo2Pos) {
        for ( int j = servo2PPos; j >= servo2Pos; j--) {
          serpo2.write(j);
          delay(50);
        }
      }
      if (servo2PPos < servo2Pos) {
        for ( int j = servo2PPos; j <= servo2Pos; j++) {
          serpo2.write(j);
          delay(50);
        }
      }
      servo2PPos = servo2Pos;
    }
        
  }
}  