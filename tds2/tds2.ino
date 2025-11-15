#include "CTBot.h"
#include "WiFi.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP32Servo.h>
float t;
const int oneWireBus = 13;   
const int led = 14; 
const int heat = 27; 
int servoPin = 26;
int lampu_status;
int heater_status;
Servo servol;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
#define LAMPU_ON_CALLBACK  "lampuON"
#define LAMPU_OFF_CALLBACK "lampuOFF" 
#define MONITOR_SU "MONITOR" 
#define SERV_MAKAN_CALLBACK "servomakan"
#define MON_LAMPU "MonitorLAMP"
#define MON_HEAT "MonitorHEAT"
CTBot myBot;
CTBotInlineKeyboard myKbd; 

String ssid = "";   
String pass = "";
String token = "";

#define TdsSensorPin 12
#define VREF 3.3              // analog reference voltage(Volt) of the ADC
#define SCOUNT  30            // sum of sample point

int analogBuffer[SCOUNT];     // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0;
int copyIndex = 0;

float averageVoltage = 0;
float tdsValue = 0;
float temperature = 25;       // current temperature for compensation

// median filtering algorithm
int getMedianNum(int bArray[], int iFilterLen){
  int bTab[iFilterLen];
  for (byte i = 0; i<iFilterLen; i++)
  bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++) {
    for (i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0){
    bTemp = bTab[(iFilterLen - 1) / 2];
  }
  else {
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  }
  return bTemp;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Memulai Smart Fish Tank");
  myBot.wifiConnect(ssid, pass);
  Serial.println("Menghubungkan alat ke WiFi");
  myBot.setTelegramToken(token);
  Serial.println("Menghubungkan alat ke Telegram");
  delay(800);
  if (myBot.testConnection()) {
    Serial.println("Sukses terhubung ke Telegram");
    delay(800);
  } else {
    Serial.println("Gagal terhubung ke Telegram");
    delay(800);
  }
  pinMode(TdsSensorPin,INPUT);
  sensors.begin();
  servol.attach(servoPin);
  pinMode(heat, OUTPUT);
  digitalWrite(heat, HIGH);
  heater_status = 1;
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  lampu_status = 1;
  servol.write(0);
// inline keyboard customization
  myKbd.addButton("Hidupkan lampu", LAMPU_ON_CALLBACK, CTBotKeyboardButtonQuery);
  myKbd.addButton("Matikan lampu", LAMPU_OFF_CALLBACK, CTBotKeyboardButtonQuery);
  myKbd.addButton("Beri pakan", SERV_MAKAN_CALLBACK, CTBotKeyboardButtonQuery);  
  myKbd.addRow();
  myKbd.addButton("Suhu saat ini", MONITOR_SU, CTBotKeyboardButtonQuery);
  myKbd.addRow();
  myKbd.addButton("Status lampu", MON_LAMPU, CTBotKeyboardButtonQuery);
  myKbd.addButton("Status Heater", MON_HEAT, CTBotKeyboardButtonQuery);

}

void loop() 
{
  // a variable to store telegram message data
  TBMessage msg;
  
  // if there is an incoming message...
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
        lampu_status = 0;
        myBot.endQuery(msg.callbackQueryID, "Lampu telah dihidupkan", true);
      } else if (msg.callbackQueryData.equals(LAMPU_OFF_CALLBACK)) {
        lampu_status = 1;
        myBot.endQuery(msg.callbackQueryID, "Lampu telah dimatikan", true);
      } else if (msg.callbackQueryData.equals(MON_LAMPU)) {
         if (lampu_status)
          myBot.endQuery(msg.callbackQueryID, "Lampu mati", true);
          else 
          myBot.endQuery(msg.callbackQueryID, "Lampu hidup", true);
      }  else if (msg.callbackQueryData.equals(MON_HEAT)) {
         if (heater_status)
          myBot.endQuery(msg.callbackQueryID, "Heater mati", true);
          else 
          myBot.endQuery(msg.callbackQueryID, "Heater hidup", true);         
      } else if (msg.callbackQueryData.equals(MONITOR_SU)) {
        t = sensors.getTempCByIndex(0);
        if (isnan(t)) {
        Serial.println(F("Gagal membaca data Suhu!"));
        return;
      }
      
      myBot.sendMessage(msg.sender.id, (String)"Kondisi saat ini\nSuhu : " + t + " °C\n");
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
    sensors.requestTemperatures(); 
    t = sensors.getTempCByIndex(0);      

  if (t > 31) {
    myBot.sendMessage(msg.sender.id, (String)"Suhu Terlalu Panas!\nSuhu : " + t + " °C\nHeater dimatikan."); 
    digitalWrite(heat, HIGH);
    Serial.print("Suhu : "); 
    Serial.print(t);
    Serial.println("°C ");
    Serial.println("Suhu Panas mematikan Heater"); 
    heater_status = 1;
    delay(10000);
    }

  else if (t < 23) {
    myBot.sendMessage(msg.sender.id, (String)"Suhu Dingin!\nSuhu : " + t + " °C\nHeater dihidupkan."); 
    digitalWrite(heat, LOW);
    Serial.print("Suhu : ");
    Serial.print(t);
    Serial.println("°C "); 
    Serial.println("Suhu Dingin menghidupkan Heater"); 
    heater_status = 0;
    delay(10000);

  } 
  else {
  sensors.requestTemperatures(); 
  // Mendapatkan suhu dalam Celcius
  float temperatureC = sensors.getTempCByIndex(0);
  // Menampilkan suhu ke Serial Monitor
  static unsigned long analogSampleTimepoint = millis();
  if(millis()-analogSampleTimepoint > 40U){     //every 40 milliseconds,read the analog value from the ADC
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);    //read the analog value and store into the buffer
    analogBufferIndex++;
    if(analogBufferIndex == SCOUNT){ 
      analogBufferIndex = 0;
    }
  }   
  
  static unsigned long printTimepoint = millis();
  if(millis()-printTimepoint > 800U){
    printTimepoint = millis();
    for(copyIndex=0; copyIndex<SCOUNT; copyIndex++){
      analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
      
      // read the analog value more stable by the median filtering algorithm, and convert to voltage value
      averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 4096.0;
      
      //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0)); 
      float compensationCoefficient = 1.0+0.02*(temperature-25.0);
      //temperature compensation
      float compensationVoltage=averageVoltage/compensationCoefficient;
      
      //convert voltage value to tds value
      tdsValue=(133.42*compensationVoltage*compensationVoltage*compensationVoltage - 255.86*compensationVoltage*compensationVoltage + 857.39*compensationVoltage)*0.5;
      
      //Serial.print("voltage:");
      //Serial.print(averageVoltage,2);
      //Serial.print("V   ");
      Serial.print("TDS Value:");
      Serial.print(tdsValue,0);
      Serial.println("ppm");
    }
  }
  Serial.print("Suhu (C): ");
  Serial.print(temperatureC);
  delay(1000);
} 
}     
