#include <Wire.h>
#include "CTBot.h" 
#include "WiFi.h"
#include <LiquidCrystal_I2C.h>
#include "ACS712.h"
ACS712  ACS(15, 3.3, 4095, 185);
float t;
#define LED1_ON_CALLBACK  "led1ON"
#define LED1_OFF_CALLBACK "led1OFF" 
#define MON_LED1 "MonitorLAMP"
#define MON_ARUS "MonitorARUS"
LiquidCrystal_I2C lcd(0x27, 16, 2);
CTBot myBot;
CTBotInlineKeyboard myKbd; 

String ssid = "";   
String pass = "";
String token = "HUAQNdYE";
uint8_t led1 = 2;  
uint8_t led2 = 5;
uint8_t led1_status;
uint8_t led2_status;

void setup() {
  Serial.begin(9600);
  Serial.println(__FILE__);
  Serial.print("ACS712_LIB_VERSION: ");
  Serial.println(ACS712_LIB_VERSION);
  lcd.begin();       
  lcd.backlight(); 
  Serial.println("Starting TelegramBot...");
  lcd.setCursor(0,0);
  lcd.print("  Memulai  bot  ");
  lcd.setCursor(0,1);
  lcd.print("    Telegram    ");
  delay(800);
  lcd.clear();
  
  myBot.wifiConnect(ssid, pass);
  Serial.println("Menghubungkan WiFi ke SSID");
  lcd.setCursor(0,0);
  lcd.print(" Menghubungkan ");
  lcd.setCursor(0,1);
  lcd.print("      WiFi      ");
  delay(800);
  lcd.clear();
  
  myBot.setTelegramToken(token);
  Serial.println("Menghubungkan ke telegram");
  lcd.setCursor(0,0);
  lcd.println("  Menghubungkan ");
  lcd.setCursor(0,1);
  lcd.print("  Telegram bot.  ");
  delay(800);
  lcd.clear();
  
  if (myBot.testConnection()) {
    Serial.println("Koneksi Telegram bagus");
    lcd.setCursor(0,0);
    lcd.print(" Koneksi bagus ");
    delay(800);
    lcd.clear();
  

  } else {
    Serial.println("Koneksi telegram buruk");
    lcd.setCursor(0,0);
    lcd.print(" Koneksi buruk ");
    delay(800);
    lcd.clear();
  }
    ACS.autoMidPoint();
    pinMode(led1, OUTPUT);
    digitalWrite(led1, HIGH); 
    pinMode(led2, OUTPUT);
    digitalWrite(led2, HIGH);
    led1_status = 1;
    led2_status = 1;
  // inline keyboard customization
  myKbd.addButton("Hidupkan lampu", LED1_ON_CALLBACK, CTBotKeyboardButtonQuery);
  myKbd.addButton("Matikan lampu", LED1_OFF_CALLBACK, CTBotKeyboardButtonQuery);
  myKbd.addRow();
  myKbd.addButton("Status lampu", MON_LED1, CTBotKeyboardButtonQuery);
  myKbd.addButton("Monitoring Arus", MON_ARUS, CTBotKeyboardButtonQuery);
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
        myBot.sendMessage(msg.sender.id, "Tombol :", myKbd);
      }
      else {
        myBot.sendMessage(msg.sender.id, "Hallo silahkan tekan --> /start");
      }
    } else if (msg.messageType == CTBotMessageQuery) {
      // received a callback query message
      if (msg.callbackQueryData.equals(LED1_ON_CALLBACK)) {
        led1_status = 0;
        myBot.endQuery(msg.callbackQueryID, "Lampu telah dihidupkan", true);
      } else if (msg.callbackQueryData.equals(LED1_OFF_CALLBACK)) {
        led1_status = 1;
        myBot.endQuery(msg.callbackQueryID, "Lampu telah dimatikan", true);
      } else if (msg.callbackQueryData.equals(MON_LED1)) {
         if (led1_status)
          myBot.endQuery(msg.callbackQueryID, "Lampu mati", true);
          else 
          myBot.endQuery(msg.callbackQueryID, "Lampu hidup", true);
        }
        else if (msg.callbackQueryData.equals(MONITOR_ARUS)) {
        float f1 = ACS.mA_AC();
        float f2 = ACS.mA_AC_sampling();
        if (isnan(h) || isnan(t)) {
        Serial.println(F("Gagal membaca data dari sensor DHT!"));
        return;        
    }
    digitalWrite(led1, led1_status);
    } 
    lcd.clear();

  float f1 = ACS.mA_AC();
  float f2 = ACS.mA_AC_sampling();

  lcd.setCursor(0, 0);
  lcd.print("mA: ");
  lcd.print(f1, 1);

  lcd.setCursor(10, 0);
  lcd.print(f2, 1);

  lcd.setCursor(0, 1);
  lcd.print("FF: ");
  lcd.print(f1 / f2);
  delay(1000);       
}
