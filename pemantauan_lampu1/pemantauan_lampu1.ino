#include "CTBot.h" 
#include "WiFi.h"
#include "ACS712.h"
float a;
ACS712  ACS(15, 3.3, 4095, 185);
#define LED1_ON_CALLBACK  "led1ON"
#define LED1_OFF_CALLBACK "led1OFF" 
#define MON_LED1 "MonitorLAMP"
#define MON_ARUS "MonitorARUS"
CTBot myBot;
CTBotInlineKeyboard myKbd; 

String ssid = "";   
String pass = "";
String token = "UAQNdYE";
uint8_t led1 = 2;  
uint8_t led2 = 5;
uint8_t led1_status;
uint8_t led2_status;

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
  myBot.wifiConnect(ssid, pass);
  Serial.println("Menghubungkan WiFi ke SSID");
  myBot.setTelegramToken(token);
  Serial.println("Menghubungkan ke telegram");

  
  if (myBot.testConnection()) {
    Serial.println("Koneksi Telegram bagus");
    delay(800);
  } 
  else {
    Serial.println("Koneksi telegram buruk");
    delay(800);
  }
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
      else if (msg.callbackQueryData.equals(MON_ARUS)) {
        a = ACS.mA_AC();
        if (isnan(a)) {
        Serial.println(F("Gagal membaca data arus"));
        return;
      }
      myBot.sendMessage(msg.sender.id, (String)"Arus : " + a + " mA");
      myBot.endQuery(msg.callbackQueryID, "Hasil terbaca di chat", true);
      }      
    }
    digitalWrite(led1, led1_status);
    }         
  }
int mA = ACS.mA_AC();
Serial.print("mA: ");
Serial.print(mA);
Serial.print(". Form factor: ");
Serial.println(ACS.getFormFactor());
delay(1000); 
}
