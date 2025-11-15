#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP32Servo.h>

const int oneWireBus = 15;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

#define ON_Board_LED 2 
#define LAMPU 19 
#define HEATER 18 
int servoPin = 5;
Servo servol;
const char* ssid = "";
const char* password = "";

String postData = ""; 
String payload = "";  

float kirim_suhu;
String kirim_status_baca_suhu = "";
String kirim_status_lampu = "";

void control_LEDs() {
  Serial.println();
  Serial.println("---------------control_LEDs()");
  JSONVar myObject = JSON.parse(payload);


  if (JSON.typeof(myObject) == "undefined") {
    Serial.println("Parsing input failed!");
    Serial.println("---------------");
    return;
  }

  if (myObject.hasOwnProperty("lampu")) {
    Serial.print("myObject[\"lampu\"] = ");
    Serial.println(myObject["lampu"]);
  }

  if (myObject.hasOwnProperty("heater")) {
    Serial.print("myObject[\"heater\"] = ");
    Serial.println(myObject["heater"]);
  }
  if (myObject.hasOwnProperty("pakan")) {
    Serial.print("myObject[\"pakan\"] = ");
    Serial.println(myObject["pakan"]);
  }

  if(strcmp(myObject["lampu"], "ON") == 0)   {
    digitalWrite(LAMPU, HIGH);  
    Serial.println("LAMPU ON"); 
    }
  if(strcmp(myObject["lampu"], "OFF") == 0)  {
    digitalWrite(LAMPU, LOW);   
    Serial.println("LAMPU OFF");
    }
  if(strcmp(myObject["heater"], "ON") == 0)   {
    digitalWrite(HEATER, HIGH);  
    Serial.println("HEATER ON"); 
    }
  if(strcmp(myObject["heater"], "OFF") == 0)  {
    digitalWrite(HEATER, LOW);   
    Serial.println("HEATER OFF");
    }
  if(strcmp(myObject["pakan"], "ON") == 0)   {
    servol.write(30);
    delay(1000);
    servol.write(0);  
    Serial.println("Pakan diberi"); 
    }
  if(strcmp(myObject["pakan"], "OFF") == 0)  { 
    Serial.println("Pakan tidak diberikan");
    }
  Serial.println("---------------");
}

void get_suhu_data() {
  Serial.println();
  Serial.println("-------------get_suhu_data()");
  int analogValue = analogRead(34);

  sensors.requestTemperatures();
  kirim_suhu = sensors.getTempCByIndex(0);

  if (kirim_suhu < 0)
  {
    Serial.println("Failed to read from DS18B20 sensor!");
    kirim_suhu = 0.00;
    kirim_status_baca_suhu = "Failed";
  }
  else
  {
    kirim_status_baca_suhu = "Success";
  }
   

  Serial.print("LDR = ");
  Serial.print(analogValue);   


  if (analogValue < 1000) {
    Serial.println(" => HIDUP");
    kirim_status_lampu = "ON";
  } else {
    Serial.println(" => MATI");
    kirim_status_lampu = "OFF";
  }

  Serial.printf("Suhu : %.2f °C\n", kirim_suhu);
  Serial.printf("Status Membaca Suhu : %s\n", kirim_status_baca_suhu);
  Serial.println("-------------");
}

void setup() {
  Serial.begin(115200); 
  pinMode(ON_Board_LED,OUTPUT); 
  pinMode(LAMPU,OUTPUT); 
  pinMode(HEATER,OUTPUT); 
  digitalWrite(ON_Board_LED, HIGH);
  digitalWrite(LAMPU, HIGH);
  digitalWrite(HEATER, HIGH);
  servol.attach(servoPin);
  servol.write(0); 
  delay(2000);
  digitalWrite(ON_Board_LED, LOW); 
  digitalWrite(LAMPU, LOW); 
  digitalWrite(HEATER, LOW); 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println();
  Serial.println("-------------");
  Serial.print("Connecting");

  int connecting_process_timed_out = 20; 
  connecting_process_timed_out = connecting_process_timed_out * 2;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(ON_Board_LED, HIGH);
    delay(250);
    digitalWrite(ON_Board_LED, LOW);
    delay(250);
  
    if(connecting_process_timed_out > 0) connecting_process_timed_out--;
    if(connecting_process_timed_out == 0) {
      delay(1000);
      ESP.restart();
    }
  }
  digitalWrite(ON_Board_LED, LOW);
  Serial.println();
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.println("-------------");

  sensors.begin();

  delay(2000);
}

void loop() {
  if(WiFi.status()== WL_CONNECTED) {
    HTTPClient http; 
    int httpCode;     
    postData = "id=ESP32_1";
    payload = "";
  
    digitalWrite(ON_Board_LED, HIGH);
    Serial.println();
    Serial.println("---------------getdata.php");
    http.begin("http://192.168.1.1/Aquas/getdata.php"); 
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");        
   
    httpCode = http.POST(postData); 
    payload = http.getString();     
  
    Serial.print("httpCode : ");
    Serial.println(httpCode); 
    Serial.print("payload  : ");
    Serial.println(payload);  
    
    http.end();  //--> Close connection
    Serial.println("---------------");
    digitalWrite(ON_Board_LED, LOW);
    //........................................ 

  
    control_LEDs();
    delay(1000);

    
    get_suhu_data();
  
    
    String lampu_state = "";
    String heater_state = "";
    String pakan_state = "";

    if (digitalRead(LAMPU) == 1) lampu_state = "ON";
    if (digitalRead(LAMPU) == 0) lampu_state = "OFF";
    if (digitalRead(HEATER) == 1) heater_state = "ON";
    if (digitalRead(HEATER) == 0) heater_state = "OFF";
    if (digitalRead(servoPin) == 1) pakan_state = "ON";
    if (digitalRead(servoPin) == 0) pakan_state = "OFF";

    
    postData = "id=ESP32_1";
    postData += "&suhu=" + String(kirim_suhu);
    postData += "&status_baca_suhu=" + kirim_status_baca_suhu;
    postData += "&lampu=" + lampu_state;
    postData += "&heater=" + heater_state;
    postData += "&pakan=" + pakan_state;
    payload = "";
  
    digitalWrite(ON_Board_LED, HIGH);
    Serial.println();
    Serial.println("---------------updatedata_and_recordtable.php");
    http.begin("http://192.168.1.1/Aquas/updatedata_and_recordtable.php");  
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");  
   
    httpCode = http.POST(postData); 
    payload = http.getString();  
  
    Serial.print("httpCode : ");
    Serial.println(httpCode); 
    Serial.print("payload  : ");
    Serial.println(payload);  
    
    http.end();  //Close connection
    Serial.println("---------------");
    digitalWrite(ON_Board_LED, LOW);
    //........................................ 
    
    delay(4000);
  }
  //---------------------------------------- 
}
