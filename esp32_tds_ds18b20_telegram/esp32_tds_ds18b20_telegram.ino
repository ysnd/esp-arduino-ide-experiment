#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP32Servo.h>

// WiFi credentials
const char* ssid = "";
const char* password = "";

// Telegram Bot credentials
#define BOT_TOKEN ""
#define CHAT_ID ""

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// DS18B20 sensor
#define ONE_WIRE_BUS 13
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Pins
const int LED_PIN = 14;
const int HEATER_PIN = 27;
const int SERVO_PIN = 26;

// Thresholds
const float TEMPERATURE_THRESHOLD_HIGH = 31.0;
const float TEMPERATURE_THRESHOLD_LOW = 22.0;

// Keyboard buttons
const String BUTTON_MONITOR = " Monitoring";
const String BUTTON_HEATER_ON = "Heater ON";
const String BUTTON_HEATER_OFF = "Heater OFF";

Servo servoPakan;

void handleInlineKeyboard(String queryId, String text) {
  if (text == BUTTON_HEATER_ON) {
    digitalWrite(HEATER_PIN, HIGH);
    bot.answerCallbackQuery(queryId, "Heater dihidupkan");
  } else if (text == BUTTON_HEATER_OFF) {
    digitalWrite(HEATER_PIN, LOW);
    bot.answerCallbackQuery(queryId, "Heater dimatikan");
  } else if (text == BUTTON_MONITOR) {
    sensors.requestTemperatures();
    float temperature = sensors.getTempCByIndex(0);

    String message = " Suhu: " + String(temperature) + " °C";

    bot.sendMessage(CHAT_ID, message);
    bot.answerCallbackQuery(queryId, "Monitoring");
  }
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize DS18B20 sensor
  sensors.begin();

  // Set pin modes
  pinMode(LED_PIN, OUTPUT);
  pinMode(HEATER_PIN, OUTPUT);

  // Attach servo
  servoPakan.attach(SERVO_PIN);

  // Start the bot connection
  client.setInsecure();
  bot.enableJSONStreaming(true);
  bot.setUpdateCallback(handleInlineKeyboard);
  bot.startBot();
}

void loop() {
  bot.loop();

  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);

  if (temperature >= TEMPERATURE_THRESHOLD_HIGH) {
    digitalWrite(HEATER_PIN, LOW);
  } else if (temperature <= TEMPERATURE_THRESHOLD_LOW) {
    digitalWrite(HEATER_PIN, HIGH);
  }

  delay(1000);
}
