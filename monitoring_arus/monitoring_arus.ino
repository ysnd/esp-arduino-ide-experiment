#include "CTBot.h" 
#include "WiFi.h"
#include "ACS712.h"
#define LAMPU_ON_CALLBACK  "lampuON"
#define LAMPU_OFF_CALLBACK "lampuOFF" 
#define MONITOR_ARUS "MONITOR" 
#define MON_LAMPU "MonitorLAMP"
float a;
CTBot myBot;
CTBotInlineKeyboard myKbd; 
ACS712  ACS(15, 3.3, 4095, 185);


String ssid = "";   
String pass = "";
String token = "HUAQNdYE";
uint8_t led = 2;  
uint8_t lampu_status;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println(__FILE__);
  Serial.print("ACS712_LIB_VERSION: ");
  Serial.println(ACS712_LIB_VERSION);
  ACS.autoMidPoint();
  Serial.print("MidPoint: ");
  Serial.print(ACS.getMidPoint());
  Serial.print(". Noise mV: ");
  Serial.println(ACS.getNoisemV());
  Serial.println("Starting TelegramBot...");
  delay(800);
  myBot.wifiConnect(ssid, pass);
  Serial.println("Menghubungkan WiFi ke SSID");
  delay(800);
  myBot.setTelegramToken(token);
  Serial.println("Menghubungkan ke telegram");
  delay(800);
  
  if (myBot.testConnection()) {
    Serial.println("Koneksi Telegram bagus");
    delay(800);
  } else {
    Serial.println("Koneksi telegram buruk");
    delay(800);
  }
    pinMode(led, OUTPUT);
    digitalWrite(led, HIGH); 
    lampu_status = 1;
  // inline keyboard customization
  myKbd.addButton("Hidupkan lampu", LAMPU_ON_CALLBACK, CTBotKeyboardButtonQuery);
  myKbd.addButton("Matikan lampu", LAMPU_OFF_CALLBACK, CTBotKeyboardButtonQuery);
  myKbd.addRow();
  myKbd.addButton("Monitoring ARUS", MONITOR_ARUS, CTBotKeyboardButtonQuery);
  myKbd.addButton("Status lampu", MON_LAMPU, CTBotKeyboardButtonQuery);
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
      else if (msg.messageType == CTBotMessageQuery) {
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
       }
       else if (msg.callbackQueryData.equals(MONITOR_SU)) {
        t = sensors.getTempCByIndex(0);
        if (isnan(t)) {
        Serial.println(F("Gagal membaca data Suhu!"));
        return;
      }
      
      myBot.sendMessage(msg.sender.id, (String)"Kondisi saat ini\nSuhu : " + t + " °C\n");
      myBot.endQuery(msg.callbackQueryID, "Hasil terbaca di chat", true);
        }  
    }
    digitalWrite(led, lampu_status); 
    mA = ACS.mA_AC();
    Serial.print("mA: ");
    Serial.print(mA);
    Serial.print(". Form factor: ");
    Serial.println(ACS.getFormFactor());
    delay(1000);     
  }
}  
