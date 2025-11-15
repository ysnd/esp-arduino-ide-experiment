#include <string.h>
// Device libraries (Arduino ESP32/ESP8266 Cores)
#include <Arduino.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#else // jika menggunakan ESP32
#include <WiFi.h>
#endif
// Custom libraries
#include <utlgbotlib.h>
//Inisialisasi Motor Servo
#include <Servo.h>
static const int servoPin = 0;
Servo servo1;
#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float h, t;
#define WIFI_SSID "yourssid"
#define WIFI_PASS "yourpassword"

#define MAX_CONN_FAIL 50
#define MAX_LENGTH_WIFI_SSID 31
#define MAX_LENGTH_WIFI_PASS 63

#define TLG_TOKEN "" 

#define DEBUG_LEVEL_UTLGBOT 0

#define PIN_RELAY 5 /

const char TEXT_START[] =
  "Hello, im a Bot goblog running in an ESP microcontroller that let you turn on/off a RELAY/light.\n"
  "\n"
  "Check /menu command to see how to use me.";

char buff[100];
boolean state_t, state_h;

const char TEXT_HELP[] =
  "Available Commands:\n"
  "\n"
  "/mulai - Memulai Teks.\n"
  "/menu - Menampilkan menu Teks.\n"
  "/lampuON - Hidupkan Lampu.\n"
  "/lampuOff - Matikan Lampu.\n"
  "/statusLampu - Menampilkan Status Lampu.\n"
  "/statussuhu - Menampilkan Temperatur.\n"
  "/statuskelembaban - Menampilkan Kelembaban.\n";
  "/putar180 - Memutar servo 180 derajat.\n";
  "/putar90 - Memutar servo 90 derajat.\n";
  "/putar0 - Memutar servo 0 derajat.\n";
  "/statusservo - Cek status Motor Servo.\n";
  

void wifi_init_stat(void);
bool wifi_handle_connection(void);


// Create Bot object
uTLGBot Bot(TLG_TOKEN);

// RELAY status
uint8_t relay_status;

void setup(void)
{
  Bot.set_debug(DEBUG_LEVEL_UTLGBOT);
  Serial.begin(9600);
  digitalWrite(PIN_RELAY, HIGH);
  pinMode(PIN_RELAY, OUTPUT);
  relay_status = 1;

  wifi_init_stat();

  Serial.println("Waiting for WiFi connection.");
  while (!wifi_handle_connection())
  {
    Serial.print(".");
    delay(500);
  }
  dht.begin();
  Bot.getMe();
}

void loop()
{
  if (!wifi_handle_connection())
  {
    // Wait 100ms and check again
    delay(100);
    return;
  }

  // Check for Bot received messages
  while (Bot.getUpdates())
  {
    Serial.println("Received message:");
    Serial.println(Bot.received_msg.text);
    Serial.println(Bot.received_msg.chat.id);

    if (strncmp(Bot.received_msg.text, "/mulai", strlen("/mulai")) == 0)
    {
      Bot.sendMessage(Bot.received_msg.chat.id, TEXT_START);
    }

    else if (strncmp(Bot.received_msg.text, "/menu", strlen("/menu")) == 0)
    {
      Bot.sendMessage(Bot.received_msg.chat.id, TEXT_HELP);
    }

    else if (strncmp(Bot.received_msg.text, "/lampuON", strlen("/lampuON")) == 0)
    {
      relay_status = 0;
      Serial.println("Command /relayon received.");
      Serial.println("Turning on the Relay.");

      Bot.sendMessage(Bot.received_msg.chat.id, "Lampu on.");
    }

    else if (strncmp(Bot.received_msg.text, "/lampuOff", strlen("/lampuOff")) == 0)
    {
      relay_status = 1;
      // Show command reception through Serial
      Serial.println("Command /relayoff received.");
      Serial.println("Turning off the RELAY.");

      // Send a Telegram message to notify that the RELAY has been turned off
      Bot.sendMessage(Bot.received_msg.chat.id, "Lampu off.");
    }

    // If /RELAYstatus command was received
    else if (strncmp(Bot.received_msg.text, "/statusLampu", strlen("/statusLampu")) == 0)
    {
      // Send a Telegram message to notify actual RELAY status
      if (relay_status)
        Bot.sendMessage(Bot.received_msg.chat.id, "The RELAY is off.");
      else
        Bot.sendMessage(Bot.received_msg.chat.id, "The RELAY is on.");
    }
    //servo
    if (Bot.received_msg.text, "/putar180") {
      servo1.write(180);  // memutar servo 180 derajat
      Bot.sendMessage(Bot.received_msg.chat.id, "Motor Servo Telah Berputar 180 Derajat", "");
    }

    if (Bot.received_msg.text, "/putar90") {
      servo1.write(90);   //memutar servo 90 derajat
      Bot.sendMessage(Bot.received_msg.chat.id, "Motor Servo Telah Berputar 90 Derajat", "");
    }
    if (Bot.received_msg.text, "/putar0") {
      servo1.write(0);   //memutar servo 0 derajat
      Bot.sendMessage(Bot.received_msg.chat.id, "Motor Servo Telah Berputar kembali ke 0 Derajat", "");
    }

    if (Bot.received_msg.text, "/statusservo") {
        Bot.sendMessage(Bot.received_msg.chat.id, "Servo Dalam Keadaan ON, Siap Dikendalikan", "");
        servo1.write(180);
        delay(1000);
        servo1.write(0);
        delay(1000);
        servo1.write(90);
        delay(1000);
    }     
    else if (strncmp(Bot.received_msg.text, "/statussuhu", strlen("/statussuhu")) == 0)
    {
      t = dht.readTemperature();
      if (isnan(h) || isnan(t)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
      }
      String msg = "Status Suhu :";
      msg += t;
      msg += " °C\n";

      msg.toCharArray(buff, 100);
      Bot.sendMessage(Bot.received_msg.chat.id, buff);
    }
    else if (strncmp(Bot.received_msg.text, "/statuskelembaban", strlen("/statuskelembaban")) == 0)
    {
      h = dht.readHumidity();
      if (isnan(h) || isnan(t)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
      }
      String msg = "Status Kelembaban :";
      msg += h;
      msg += " %Rh\n";

      msg.toCharArray(buff, 100);
      Bot.sendMessage(Bot.received_msg.chat.id, buff);
    }

    digitalWrite(PIN_RELAY, relay_status);
    // Feed the Watchdog
    yield();
  }

  h = dht.readHumidity();
  t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  if (t > 37 && state_t == 0) {
    state_t = 1;
    String msg = "Status Suhu :";
    msg += t;
    msg += " °C\n";
    msg += "Hati Hati Panas.";

    msg.toCharArray(buff, 100);
    Bot.sendMessage("556538872", buff);
  }
  else if (t <= 37) {
    state_t = 0;

  }

  if (h < 60 && state_h == 0) {
    state_h = 1;
    String msg = "Status Kelembaban :";
    msg += h;
    msg += " %Rh\n";
    msg += "Hati Hati Kering.";

    msg.toCharArray(buff, 100);
    Bot.sendMessage("556538872", buff);
  }
  else if (h >= 60) {
    state_h = 0;
  }
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));
  delay(1000);
}


// Init WiFi interface
void wifi_init_stat(void)
{
  Serial.println("Initializing TCP-IP adapter...");
  Serial.print("Wifi connecting to SSID: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.println("TCP-IP adapter successfuly initialized.");
}


bool wifi_handle_connection(void)
{
  static bool wifi_connected = false;

  // Device is not connected
  if (WiFi.status() != WL_CONNECTED)
  {
    // Was connected
    if (wifi_connected)
    {
      Serial.println("WiFi disconnected.");
      wifi_connected = false;
    }

    return false;
  }
  // Device connected
  else
  {
    // Wasn't connected
    if (!wifi_connected)
    {
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());

      wifi_connected = true;
    }

    return true;
  }
}
