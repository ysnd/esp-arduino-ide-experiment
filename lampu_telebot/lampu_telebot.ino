#include "CTBot.h" 
const int sensorIn = 15;
float t;      
int mVperAmp = 185;          
int Watt = 0;
double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;
#define LED1_ON_CALLBACK  "led1ON"
#define LED1_OFF_CALLBACK "led1OFF" 
#define MON_LED1 "MonitorLAMP"
#define MON_ARUS "MonitorARUS"
CTBot myBot;
CTBotInlineKeyboard myKbd; 

String ssid = "";   
String pass = "";
String token = "XaotiHUAQNdYE";
uint8_t led1 = 2;  
uint8_t led2 = 4;
uint8_t led1_status;
uint8_t led2_status;

float getVPP()
{
  float result;
  int readValue;                // value read from the sensor
  int maxValue = 4096;             // store max value here
  int minValue = 0;          // store min value here ESP32 ADC resolution
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue = analogRead(sensorIn);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
           /*record the minimum sensor value*/
           minValue = readValue;
       }
   }
   
   // Subtract min from max
   result = ((maxValue - minValue) * 3.3)/4096.0; //ESP32 ADC resolution 4096
      
   return result;
 }
void setup() {
  Serial.begin(115200);
  Serial.println("Starting TelegramBot...");
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
    pinMode(led1, OUTPUT);
    digitalWrite(led1, HIGH); 
    pinMode(led2, OUTPUT);
    digitalWrite(led2, HIGH);
    led1_status = 1;
    led2_status = 1;
  // inline keyboard customization
  myKbd.addButton("Hidupkan lampu1", LED1_ON_CALLBACK, CTBotKeyboardButtonQuery);
  myKbd.addButton("Matikan lampu1", LED1_OFF_CALLBACK, CTBotKeyboardButtonQuery);
  myKbd.addRow();
  myKbd.addButton("Status lampu1", MON_LED1, CTBotKeyboardButtonQuery);
  myKbd.addButton("Monitor Arus dan Daya", MON_ARUS, CTBotKeyboardButtonQuery);
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
      } else if (msg.callbackQueryData.equals(MON_ARUS)) {
        Voltage = getVPP();
        VRMS = (Voltage/2.0) *0.707; 
        AmpsRMS = ((VRMS * 1000)/mVperAmp)-0.3;
        Watt = (AmpsRMS*240/1.2);
        if (isnan(AmpsRMS)|| isnan(Watt)) {
        Serial.println(F("Gagal membaca data"));
        return;
      }
      
      myBot.sendMessage(msg.sender.id, (String)"Arus : " + AmpsRMS + " Ampere""\nDaya : " + Watt + " Watt");
      myBot.endQuery(msg.callbackQueryID, "Hasil terbaca di chat", true);
        }
    digitalWrite(led1, led1_status);
    }   
    Serial.println (""); 
    Voltage = getVPP();
    VRMS = (Voltage/2.0) *0.707; 
    AmpsRMS = ((VRMS * 1000)/mVperAmp)-0.3;
    Serial.print(AmpsRMS);
    Serial.print(" Amps RMS  ---  ");
    Watt = (AmpsRMS*240/1.2);
    Serial.print(Watt);
    Serial.println(" Watts");
    delay (100);
}
}
