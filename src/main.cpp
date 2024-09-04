#include <Arduino.h>
#include <WiFiManager.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SH110X.h>
#include <DHT.h>
#include <ESP32Encoder.h>
//#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "blink.h"
#include "config.h"
#include "controlConstants.h"

// Logo
#include "logo.h"

// OTA
#include <AsyncElegantOTA.h>
const char* ssid = "";
const char* password = "";
//const char* ssid = "iPhone de Juan";
//const char* password = "HelpUsObiJuan";
//const char* ssid = "ACNET2";
//const char* password = "";
//const char* ssid = "TheShield";
//const char* password = "JamesBond007";
//#define CONNECT_TIME 10000
#define USE_MQTT false

AsyncWebServer server(80);
blink parpadeo(LED_BUILTIN);
DHT dht(DHT_PIN, DHT22);
ESP32Encoder myEnc;
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
Signals signals = {10, 0, 0, 0};



WiFiClient client;

// ***** FUNCIONES DE INICIALIZACION ***** //
void displayLogo(unsigned long interval){
  static unsigned long previousMillis = millis();
  if(millis() - previousMillis > interval){
    previousMillis = millis();
    display.clearDisplay();
    display.drawBitmap(0, 0, logo_UTN_128x64, 126, 126, 1); 
    display.display();
    delay(5000);
  }
  

}

void displayAPInfo(const char* ssid) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);

    display.println(F("Modo AP Activado"));
    display.println();
    display.println(F("SSID:"));
    display.println(ssid);
    display.println();
    display.println(F("IP:"));
    display.println(WiFi.softAPIP()); // Muestra la IP del AP

    display.display(); // Actualiza el display con la nueva información
}

void displayInit(){
  display.begin(I2C_ADDRESS, true); // Address 0x3C default
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.println(F("Hola Mundo!"));
  display.println(F("Datos de Compilacion:"));
  display.printf("Fecha %s\n",__DATE__);
  display.printf("Hora: %s\n",__TIME__);
  display.println(F("Conectando a Wifi..."));
  display.display();
}

void dhtInit(){
  dht.begin();
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  Serial.printf("Temperatura: %d°C\tHumedad: %d%\n",
                (int)temp,
                (int)hum);
}

void myEncInit(){
  myEnc.attachSingleEdge(PIN_ENC_A, PIN_ENC_B);
  myEnc.setCount(signals.enc);
}

void spiffsInit(){
  if(!SPIFFS.begin(true)){
    Serial.println("Error al inicializar SPIFFs. El sistema se detendra");
    while(1){
      delay(100);
    }
  }
  else{
    Serial.println("SPIFFS inicializado con exito");
    Serial.println("Listado de archivos disponibles:");
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while(file){
      Serial.printf("Archivo: %s\n", file.name());
      file.close();
      file = root.openNextFile();
    }
    root.close();
    
  }

}

void wifiInit() {
    bool resultado;
    WiFi.mode(WIFI_STA);
    WiFiManager wm;

    if (digitalRead(PIN_ENC_PUSH) == LOW) {
        Serial.println("Configurando WiFi...");
        
        wm.setConfigPortalTimeout(120);
        Serial.println(wm.getDefaultAPName());
        const char* apName = "OnDemandAP"; // Asegúrate de que este es el SSID que deseas
        displayAPInfo(wm.getDefaultAPName().c_str());
        if (!wm.startConfigPortal(apName)) {
            Serial.println("Failed to connect and hit timeout");
            delay(3000);
            ESP.restart();
            delay(5000);
        } else {
            // Aquí sabemos que el AP está activo y el portal está corriendo
            
        }

        Serial.println("Connected...yeey :)");
    } else {
        resultado = wm.autoConnect(ssid, password);

        if (!resultado) {
            Serial.println("No se pudo conectar a la red. Llamen a los Avengers");
        } else {
            Serial.println("Conectadazo");
        }
    }
}



void displayWiFi(){
  display.clearDisplay();
  display.setCursor(0,0);
  if(WiFi.status() == WL_CONNECTED){
    display.println(F("Conectado al Wifi"));
    display.println(F("IP:"));
    display.println(WiFi.localIP());
    
  }
  else{
    display.println(F("No se pudo conectar a WiFi"));
  }
  display.display();

}

void serverInit(){
  server.on("/home", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasArg("getJson")) {
      // Obtener las últimas lecturas de temperatura y otros datos
      //float temperatureRef = myEnc.getCount(); // Reemplaza esta función por la que obtiene la temperatura de referencia
      //float temperatureReal = dht.readTemperature(); // Reemplaza esta función por la que obtiene la temperatura real
      // Calcular el error (puedes ajustar esto según tu lógica)
      //float error = temperatureRef - temperatureReal;

      // Crear un objeto JSON con los datos
      String json = "{ \"temperature\": " + String(signals.temp, 2) + ", \"humidity\": " + String(signals.hum, 2) + ", \"encoder\": " + String(signals.enc, 2) + ", \"pot\": " + String(signals.pot, 2) + " }";
      //Serial.println(json);
      // Enviar el objeto JSON como respuesta
      request->send(200, "application/json", json);
    } else {
      request->send(SPIFFS, "/index.html", "text/html");
    }
  });
  // Agregamos OTA
  AsyncElegantOTA.begin(&server);
  server.begin();
}

// ***** FUNCIONES DE OPERACION ***** //


void displayUpdate(){
  static Signals signalsOld = {0}; 
  if((signals.enc != signalsOld.enc) ||
     (signals.temp != signalsOld.temp) ||
     (signals.hum != signalsOld.hum)  ||
     (signals.pot != signalsOld.pot)) {  // Basta con revisar si alguna de estas dos señales cambió, ya que ellas arrastran los cambios de error y acción de control
        signalsOld.enc = signals.enc;
        signalsOld.temp = signals.temp;
        signalsOld.hum = signals.hum;
        signalsOld.pot = signals.pot;
        display.clearDisplay();
        display.setCursor(0,0);
        display.setTextSize(1);
        display.println("         DATOS");
        display.printf("Temp: %.2fC\nHum:  %.2f%%\nEnc:  %.2f\nPot:  %.2f\n",
                       signals.temp,
                       signals.hum,
                       signals.enc,
                       signals.pot);
        display.println();
        if (WiFi.status() != WL_CONNECTED) {
            display.println("WiFi no conectado");
        } else {
            display.printf("IP: %s%s\n", WiFi.localIP().toString(), "/home");
            display.printf("RSSI: %d dBm\n", WiFi.RSSI());
        }
        

        display.display();
      }
 

}

void signalsUpdate(unsigned long intervalo){
  static unsigned long previousMillis = 0;        // will store last time LED was updated
	//const long interval = 1000;           // interval at which to blink (milliseconds)
	unsigned long currentMillis = millis();
	
	
	if(currentMillis - previousMillis > intervalo) 
	{
		previousMillis = currentMillis;
    signals.enc = myEnc.getCount();
    if(signals.enc > 40){
      signals.enc = 40;
      myEnc.setCount(40);
    }
    if(signals.enc < 1){
      signals.enc = 1;
      myEnc.setCount(1);
    }
    signals.temp = dht.readTemperature();
    signals.hum = dht.readHumidity();
    signals.pot = float(map(analogRead(PIN_ADC),4095,0,0,100));
    analogWrite(PIN_LED_G, map(analogRead(PIN_ADC),4095,0,0,255));
  }
}

void setup() {
  delay(1000);
  Serial.begin(BAUDRATE);
  Serial.println(F("Hola Mundo!"));
  Serial.printf("Fecha y hora de compilación: %s, %s\n", __DATE__, __TIME__);
  Serial.println(F("Iniciando Entradas Digitales"));
  pinMode(PIN_ENC_PUSH,INPUT_PULLUP);
  pinMode(PIN_LED_G, OUTPUT);
  digitalWrite(PIN_LED_G, LOW);
  Serial.println(F("Iniciando Display"));
  displayInit();
  displayLogo(0);
  Serial.println(F("Iniciando DHT22"));
  dhtInit();
  Serial.println(F("Configurando ADC..."));
  analogReadResolution(12);
  Serial.println(F("Inicializando Encoder"));
  myEncInit();
  Serial.println(F("Inicializando SPIFFS"));
  spiffsInit();
  Serial.println(F("Inicializando WiFi"));
  wifiInit();
  displayWiFi();
  serverInit();
  Serial.println("Setup terminado");
  delay(2000);
}

void loop() {
  signalsUpdate(100);
  displayUpdate();
  displayLogo(120000);
  parpadeo.update(signals.enc * 10);
 
}

