#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include "CTBot.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP32Servo.h>

#define LIGHT_ON_CALLBACK  "lightON"  // callback data sent when "LIGHT ON" button is pressed
#define LIGHT_OFF_CALLBACK "lightOFF" // callback data sent when "LIGHT OFF" button is pressed
#define LAMPU_ON_CALLBACK  "lampuON"
#define LAMPU_OFF_CALLBACK "lampuOFF" 
#define MONITOR_SU "MONITOR" 
#define SERV_MAKAN_CALLBACK "servomakan"
#define MON_LAMPU "MonitorLAMP"
#define MON_HEAT "MonitorHEAT"

CTBot myBot;
CTBotInlineKeyboard myKbd;  // custom inline keyboard object helper

const char* ssid = "";
const char* password = "";
const char* token = "";
const char* sheetId = "";
const char* range = "Sheet1!A1:E1"; // The range where you want to append data (change it as needed)
WiFiClientSecure client;
OneWire oneWire(13);
DallasTemperature sensors(&oneWire);
Servo servol;

int led = 14;              
int servoPin = 26;
const int heat = 27; 
int lampu_status;
int heater_status;
bool hasResponded = false;
int tdsstat;
float Vref = 3.3;
float ec = 0;
unsigned int tds = 0;
float t = 0;
float ecCalibration = 1;
const byte tds_pin = A0;

void setup() {
  Serial.begin(115200);
  Serial.println("Memulai Smart Aquarium TelegramBot...");
  myBot.wifiConnect(ssid, password);
  myBot.setTelegramToken(token);
  if (myBot.testConnection())
    Serial.println("\nTerhubung ke bot Telegram");
  else
    Serial.println("\nTidak Terhubung silahkan cek koneksi internet");

  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  lampu_status = 0;
  servol.attach(servoPin);
  servol.write(0);
  pinMode(heat, OUTPUT);
  digitalWrite(heat, LOW);
  sensors.begin();

  myKbd.addButton("Hidupkan lampu", LAMPU_ON_CALLBACK, CTBotKeyboardButtonQuery);
  myKbd.addButton("Matikan lampu", LAMPU_OFF_CALLBACK, CTBotKeyboardButtonQuery);
  myKbd.addButton("Beri pakan", SERV_MAKAN_CALLBACK, CTBotKeyboardButtonQuery);  
  myKbd.addRow();
  myKbd.addButton("Monitor Parameter Aquarium", MONITOR_SU, CTBotKeyboardButtonQuery);
  myKbd.addRow();
  myKbd.addButton("Status lampu", MON_LAMPU, CTBotKeyboardButtonQuery);
  myKbd.addButton("Status Heater", MON_HEAT, CTBotKeyboardButtonQuery);
}

void loop() {
  readTdsQuick();

  TBMessage msg;
  if (myBot.getNewMessage(msg)) {
    // check what kind of message I received
    if (msg.messageType == CTBotMessageText) {
      // received a text message
      if (msg.text.equalsIgnoreCase("/start")) {
        myBot.sendMessage(msg.sender.id, "Perintah yang tersedia :", myKbd);
      }
      else {
        myBot.sendMessage(msg.sender.id, "Selamat datang, untuk menampilkan perintah tekan --> /start");
      }
    } else if (msg.messageType == CTBotMessageQuery) {
      // received a callback query message
      if (msg.callbackQueryData.equals(LAMPU_ON_CALLBACK)) {
        lampu_status = 1;
        myBot.endQuery(msg.callbackQueryID, "Lampu telah dihidupkan", true);
      } 
      else if (msg.callbackQueryData.equals(LAMPU_OFF_CALLBACK)) {
        lampu_status = 0;
        myBot.endQuery(msg.callbackQueryID, "Lampu telah dimatikan", true);
      } 
      else if (msg.callbackQueryData.equals(MON_LAMPU)) {
         if (lampu_status)
          myBot.endQuery(msg.callbackQueryID, "Lampu hidup", true);
        else 
          myBot.endQuery(msg.callbackQueryID, "Lampu mati", true);
      } 
      else if (msg.callbackQueryData.equals(MON_HEAT)) {
        if (heater_status)
          myBot.endQuery(msg.callbackQueryID, "Heater hidup", true);
        else 
          myBot.endQuery(msg.callbackQueryID, "Heater mati", true);         
      } 
      else if (msg.callbackQueryData.equals(MONITOR_SU)) {
        myBot.sendMessage(msg.sender.id, (String)"Kondisi saat ini\nSuhu : " + t + " °C\nTDS : " + tds + " ppm\nEC : " + ec + " mS/cm");
        myBot.endQuery(msg.callbackQueryID, "Hasil terbaca di chat", true);
      }
      else if (msg.callbackQueryData.equals(SERV_MAKAN_CALLBACK)) {
        servol.write(90);
        delay(1000);
        servol.write(0);
        myBot.endQuery(msg.callbackQueryID, "Makanan diberi!.", true);
      }
    }
    digitalWrite(led, lampu_status);
    digitalWrite(heat, heater_status);
  }
  if (t > 31) {
    heaterOff();
    Serial.print("Suhu : ");
    Serial.print(t);
    Serial.println(" °C ");
    Serial.println("Suhu Panas mematikan Heater");
    } 
  else if (t < 22) {
    heaterOn();
    Serial.print("Suhu : ");
    Serial.print(t);
    Serial.println(" °C ");
    Serial.println("Suhu Dingin menghidupkan Heater");
  } 
  else if (tds < 300 ){
    tdsstat=0;
  }
  else if (tds > 300 && tdsstat < 1) {
    Serial.println("TDS tinggi segera mengganti air aquarium");
    tdsstat=1;
    myBot.sendMessage(msg.sender.id, (String)"TDS Tinggi\nTDS : " + tds + " ppm\nJumlah Zat Padat yang Terlarut dalam aquarium sangat tinggi, Segeralah mengganti air aquarium" );
  }
  if (!hasResponded) {
    if (t < 22) {
      myBot.sendMessage(msg.sender.id, "Suhu Dingin!\nSuhu : " + String(t) + " °C\nHeater dihidupkan.");
    } else if (t > 31) {
      myBot.sendMessage(msg.sender.id, "Suhu Terlalu Panas!\nSuhu : " + String(t) + " °C\nHeater dimatikan.");
    } 
      hasResponded = true;
    }
  // Log data to the spreadsheet
  logToSpreadsheet(tds, t, ec, heater_status);

  delay(1000);
}

void readTdsQuick() {
  float voltage = analogRead(tds_pin) * Vref / 1024.0;
  float tdsValue = voltageToTds(voltage);
  if (tdsValue > 0 && tdsValue < 2000) {
    tds = (unsigned int)tdsValue;
  }

  sensors.requestTemperatures();
  t = sensors.getTempCByIndex(0);
  ec = tdsToEc(tdsValue);
}

float voltageToTds(float voltage) {
  return voltage * 1000 / 9.8;
}

float tdsToEc(float tds) {
  return tds * ecCalibration / 1000;
}
void heaterOn() {
  if (!heater_status) {
    digitalWrite(heat, HIGH);
    heater_status = 1;
    hasResponded = false; // Reset status respons
    }
  }
void heaterOff() {
  if (heater_status) {
    digitalWrite(heat, LOW);
    heater_status = 0;
    hasResponded = false; // Reset status respons
    }
  }
  
void logToSpreadsheet(unsigned int tds, float t, float ec, int heaterStatus) {
  DynamicJsonDocument json(200);
  json["tds"] = tds;
  json["temperature"] = t;
  json["ec"] = ec;
  json["heater_status"] = heaterStatus;

  String jsonString;
  serializeJson(json, jsonString);

  String url = "https://script.google.com/macros/s/" + String(sheetId) + "/exec";
  String payload = "data=" + jsonString;

  client.setTimeout(20000); // Set timeout to 20 seconds

  // Connect to the server
  if (client.connect("script.google.com", 443)) {
    // Send the HTTP POST request
    client.println("POST " + url + " HTTP/1.1");
    client.println("Host: script.google.com");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(payload.length()));
    client.println();
    client.println(payload);
  }

  // Wait for the server's response
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }

  // Disconnect from the server
  client.stop();
}
