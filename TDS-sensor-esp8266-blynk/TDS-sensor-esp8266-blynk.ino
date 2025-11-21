#define BLYNK_TEMPLATE_ID ""
#define BLYNK_DEVICE_NAME ""
#define BLYNK_AUTH_TOKEN ""


// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>

#include <WiFiClient.h>


#include <OneWire.h>
#include <DallasTemperature.h>

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)


// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "";
char pass[] = "";



char auth[] = BLYNK_AUTH_TOKEN;

namespace pin {
const byte tds_sensor = A0;
const byte one_wire_bus = 0; // Dallas Temperature Sensor
}

namespace device {
float aref = 3.3; // Vref, this is for 3.3v compatible controller boards, for arduino use 5.0v.
}

namespace sensor {
float ec = 0;
unsigned int tds = 0;
float waterTemp = 0;
float ecCalibration = 1;
}

OneWire oneWire(pin::one_wire_bus);
DallasTemperature dallasTemperature(&oneWire);


void setup() {
  Serial.begin(115200); // Dubugging on hardware Serial 0
  Blynk.begin(auth, ssid, pass);
  dallasTemperature.begin();
}


void loop() {
   Blynk.run();
  readTdsQuick();
   
  delay(1000);

}

void readTdsQuick() {
  dallasTemperature.requestTemperatures();
  sensor::waterTemp = dallasTemperature.getTempCByIndex(0);
  float rawEc = analogRead(pin::tds_sensor) * device::aref / 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
  float temperatureCoefficient = 1.0 + 0.02 * (sensor::waterTemp - 25.0); // temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
  sensor::ec = (rawEc / temperatureCoefficient) * sensor::ecCalibration; // temperature and calibration compensation
  sensor::tds = (133.42 * pow(sensor::ec, 3) - 255.86 * sensor::ec * sensor::ec + 857.39 * sensor::ec) * 0.5; //convert voltage value to tds value
  Serial.print(F("TDS:")); Serial.println(sensor::tds);
  Serial.print(F("EC:")); Serial.println(sensor::ec, 2);
  Serial.print(F("Temperature:")); Serial.println(sensor::waterTemp,2);

    Blynk.virtualWrite(V0,(sensor::tds));

   Blynk.virtualWrite(V2,(sensor::ec));

     Blynk.virtualWrite(V1,(sensor::waterTemp));

}
