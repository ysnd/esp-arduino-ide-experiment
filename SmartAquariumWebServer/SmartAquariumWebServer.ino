#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const char *ssid = "";
const char *password = "";

OneWire oneWire(4);
DallasTemperature sensors(&oneWire);
String temperatureF = "";
String temperatureC = "";

// Timer variables
unsigned long lastTime = 0;  
unsigned long timerDelay = 30000;

AsyncWebServer server(80);

String readDSTemperatureC() {
  // Call sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
  sensors.requestTemperatures(); 
  float tempC = sensors.getTempCByIndex(0);

  if(tempC == -127.00) {
    Serial.println("Failed to read from DS18B20 sensor");
    return "--";
  } else {
    Serial.print("Temperature Celsius: ");
    Serial.println(tempC); 
  }
  return String(tempC);
}

String readDSTemperatureF() {
  // Call sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
  sensors.requestTemperatures(); 
  float tempF = sensors.getTempFByIndex(0);

  if(int(tempF) == -196){
    Serial.println("Failed to read from DS18B20 sensor");
    return "--";
  } else {
    Serial.print("Temperature Fahrenheit: ");
    Serial.println(tempF);
  }
  return String(tempF);
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Smart Aquarium Dashboard</title>
  <style>
    body {
      margin: 0;
      font-family: Arial, sans-serif;
    }
    .navbar {
      list-style-type: none;
      margin: 0;
      padding: 0;
      overflow: hidden;
      background-color: #333;
      position: fixed;
      top: 0;
      width: 100%;
    }
    .navbar a {
      margin-left: auto;
      display: block;
      color: white;
      text-align: center;
      padding: 14px 16px;
      text-decoration: none;
    }
    .navbar a:hover {
      background-color: #111;
    }
    ul.navsamping {
      list-style-type: none;
      margin: 0;
      padding: 0;
      width: 25%;
      background-color: #f1f1f1;
      position: fixed;
      height: 100%;
      overflow: auto;
      z-index: 1;
    }
    ul.navsamping li a {
      display: block;
      color: #000;
      padding: 8px 16px;
      text-decoration: none;
    }
    ul.navsamping li a.active {
      background-color: #4CAF50;
      color: white;
    }
    ul.navsamping li a:hover:not(.active) {
      background-color: #555;
      color: white;
    }
    div.logo {
      text-align: center;
      font-family: 'Franklin Gothic Medium', 'Arial Narrow', Arial, sans-serif;
      font-size: 40px;
    }
    div.isi {
      margin-left: 25%;
      padding: 1px 16px;
      height: 1000px;
    }
    @media screen and (max-width: 500px) {
      ul.navsamping {
        width: 100%;
        height: auto;
        position: relative;
      }
      ul.navsamping li a {
        float: left;
        padding: 15px;
      }
      div.isi {margin-left: 0;}
    }

    @media screen and (max-width: 250px) {
      ul.navsamping li a {
        text-align: center;
        float: none;
      }
    }
  </style>
</head>
<body>
  <div class="navbar">
    <a href="#logout">Logout</a>
  </div>

  <ul class="navsamping">
    <div class="logo">Fisher.io</div>
    <li><a class="active" href="dashboard.html">Dashboard</a></li>
    <li><a href="data.html">Data</a></li>
  </ul>
  
  <div class="isi">
    <br><br>
    <h2>Smart Aquarium Overview</h2>
    <p>
      <span>Temperature Celsius</span> 
      <span id="temperaturec">%TEMPERATUREC%</span>
      <sup class="units">&deg;C</sup>
    </p>
    <p>
      <span>Temperature Fahrenheit</span>
      <span id="temperaturef">%TEMPERATUREF%</span>
      <sup class="units">&deg;F</sup>
    </p>
  </div>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperaturec").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperaturec", true);
  xhttp.send();
}, 10000) ;
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperaturef").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperaturef", true);
  xhttp.send();
}, 10000);
</script>
</html>
)rawliteral";


String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATUREC"){
    return temperatureC;
  }
  else if(var == "TEMPERATUREF"){
    return temperatureF;
  }
  return String();
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  sensors.begin();
  temperatureC = readDSTemperatureC();
  temperatureF = readDSTemperatureF();
// Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  
  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
server.on("/temperaturec", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send(200, "text/plain", temperatureC.c_str(), "Access-Control-Allow-Origin: *");
});

server.on("/temperaturef", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send(200, "text/plain", temperatureF.c_str(), "Access-Control-Allow-Origin: *");
});


  // Start server
  server.begin();
}
 
void loop(){
  if ((millis() - lastTime) > timerDelay) {
    temperatureC = readDSTemperatureC();
    temperatureF = readDSTemperatureF();
    lastTime = millis();
  }  
}
