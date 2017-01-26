#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <BME280.h>
#define SERIAL_BAUD 115200

/*   ==== BME280のプロパティとグローバルの値 ==== */
BME280 bme;
bool metric =true;

void printBME280Data();
void printBME280CalculatedData();

/*  === 終わり ===    */

/* ====　wifiの設定 ====*/
const char* ssid = "xxxx";//自分のルータの名前
const char* password = "xxxx";//自分のルータのパスワード


ESP8266WebServer server(80);

const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(led, 0);
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
  
}
void setup(){
  /* ==== BME280のセットアップ ==== */
  Serial.begin(SERIAL_BAUD);
   while(!Serial) {} // Wait
  while(!bme.begin()){
    Serial.println("Could not find BME280 sensor!");
    delay(1000);
  }
  /* === 終わり === */
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(SERIAL_BAUD);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/data",printBME280Data);
  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}


/* ==== Functions ==== */
void printBME280Data(){
  float temp(NAN), hum(NAN), pres(NAN);
   uint8_t pressureUnit(3);   // unit: B000 = Pa, B001 = hPa, B010 = Hg, B011 = atm, B100 = bar, B101 = torr, B110 = N/m^2, B111 = psi
  bme.ReadData(pres, temp, hum, metric, pressureUnit);                // Parameters: (float& pressure, float& temp, float& humidity, bool hPa = true, bool celsius = false)
  /* Alternatives to ReadData():
    float ReadTemperature(bool celsius = false);
    float ReadPressure(uint8_t unit = 0);
    float ReadHumidity();

    Keep in mind the temperature is used for humidity and
    pressure calculations. So it is more effcient to read
    temperature, humidity and pressure all together.
   */
   String data="Temp: "+String(temp)+String(metric ? 'C' :'F')+"\t\tHumidity: "+String(hum)+"% RH"+"\t\tPressure: "+String(pres)+" atm";
  server.send(200,"text/plain",data);
}
void printBME280CalculatedData(){
  float altitude = bme.CalculateAltitude(metric);
  float dewPoint = bme.CalculateDewPoint(metric);
  server.send(200,"text/plain","\t\tAltitude: ");
  server.send(200,"text/plain",String(altitude));
  server.send(200,"text/plain",String((metric ? "m" : "ft")));
  server.send(200,"text/plain","\t\tDew point: ");
  server.send(200,"text/plain",String(dewPoint));
  server.send(200,"text/plain","°"+ String(metric ? 'C' :'F'));

}
/* ==== END Functions ==== */
