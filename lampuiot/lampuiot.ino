#include "CTBot.h"
CTBot myBot;

String ssid  = ""; 
String pass  = ""; 
String token = "";

const int lampu = 5;                        

void setup() {
  // initialize the Serial
  Serial.begin(115200);
  Serial.println("Starting TelegramBot...");

  // connect the ESP8266 to the desired access point
  myBot.wifiConnect(ssid, pass);

  // set the telegram bot token
  myBot.setTelegramToken(token);

  // check if all things are ok
  if (myBot.testConnection())
    Serial.println("\ntestConnection OK");
  else
    Serial.println("\ntestConnection NOK");

  // set the pin connected to the LED to act as output pin
  pinMode(lampu, OUTPUT);
  digitalWrite(lampu, HIGH); // turn off the led (inverted logic!)

}

void loop() {
     
  // a variable to store telegram message data
  TBMessage msg;

  // if there is an incoming message...
  if (myBot.getNewMessage(msg)) {
    if (msg.text.equalsIgnoreCase("On")) {                   //Perintah dari telegram ke perangkat
      digitalWrite(lampu, LOW);                              //Lampi dihidupkan
      myBot.sendMessage(msg.sender.id, "The Led is now ON"); //Balasan dari perangkat ke Bot Telegram
    }
    else if (msg.text.equalsIgnoreCase("Off")) {              //Perintah dari telegram ke perangkat
      digitalWrite(lampu, HIGH);                              //Lampi dihidupkan
      myBot.sendMessage(msg.sender.id, "The Led is now OFF"); //Balasan dari perangkat ke Bot Telegram
    }
    else {                                                    // otherwise...
      // generate the message for the sender
      String reply;
      reply = (String)"Welcome " + msg.sender.username + (String)". Try LIGHT ON or LIGHT OFF.";
      myBot.sendMessage(msg.sender.id, reply);             // and send it
    }
   
  }
  // wait 500 milliseconds
  delay(500);
}
