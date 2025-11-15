#include <ESP32Servo.h>
#include "WiFi.h"
#include "BluetoothSerial.h" 


BluetoothSerial ESP_BT; //Object for Bluetooth

int incoming;
int servoPin = 13;
Servo Servoz;

void setup() {
  Serial.begin(9600); //Start Serial monitor in 9600
  ESP_BT.begin("ESP32_SERVO_TESTER"); //Name of your Bluetooth Signal
  Serial.println("Bluetooth Device is Ready to Pair");
  Servoz.attach(13);

}

void loop() {
  
  if (ESP_BT.available()) //Check if we receive anything from Bluetooth
  {
    incoming = ESP_BT.read(); //Read what we recevive 
    Serial.print("Received:"); Serial.println(incoming);
    
    if (incoming == 48)
        {
       Servoz.write(0);
       ESP_BT.println("Servo bergerak 0 derajat");
        }
    if (incoming == 49)
        {
        Servoz.write(45);
        ESP_BT.println("Servo bergerak 45 derajat");
        }
        
    if (incoming == 50)
        {
       Servoz.write(90);
       ESP_BT.println("Servo bergerak 90 derajat");
        }
    if (incoming == 51)
        {
       Servoz.write(135);
       ESP_BT.println("Servo bergerak 135 derajat");
        }    
    if (incoming == 52)
        {
       Servoz.write(180);
       ESP_BT.println("Servo bergerak 180 derajat");
        }     
    
  }
  delay(20);
}
