// --- WIFI ---
#include <ESP8266WiFi.h>
const char* ssid = "VIVOFIBRA-5081";
const char* password = "6LyMbiRkKB";
WiFiClient nodemcuClient;

// --- MQTT ---
#include <PubSubClient.h>
const char* mqtt_Broker = "192.168.15.7";  // broker local
const char* mqtt_ClientID = "termometro";
int broker_port = 1883;
PubSubClient client(nodemcuClient);
const char* topicoTemperatura = "termometro-temperatura";  // publish
const char* topicoUmidade = "termometro-umidade";          // publish

// --- DHT ---
#include <DHT.h>
#define DHTPIN D3
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
float umidade;
float temperatura;

// --- DISPLAY ---
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BUTTON_A 0
#define BUTTON_B 16
#define BUTTON_C 2
#define WIRE Wire

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &WIRE);

void setup() {
  configurarDisplay();
  conectarWifi();
  client.setServer(mqtt_Broker, broker_port);
}

void loop() {

  if (!client.connected()) {
    reconectarMQTT();
  }
  medirTemperaturaUmidade();
  publicarTemperaturaUmidadeNoTopico();
  mostrarTemperaturaUmidade();
}
//--
void reconectarMQTT() {
  while (!client.connected()) {
    client.connect(mqtt_ClientID);
  }
}
void configurarDisplay() {

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  Serial.println("OLED begun");

  display.display();
  delay(1000);
  display.clearDisplay();
  display.display();

  //Serial.println("IO test");

  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
}
//--
void publicarTemperaturaUmidadeNoTopico() {
  client.publish(topicoTemperatura, String(temperatura).c_str(), true);
  client.publish(topicoUmidade, String(umidade).c_str(), true);
}
void medirTemperaturaUmidade() {
  //Serial.begin(9600);
  //Serial.println(F("DHTxx test!"));
  dht.begin();
  umidade = dht.readHumidity();
  temperatura = dht.readTemperature();
  delay(5000);
}

void mostrarTemperaturaUmidade() {
  mostrarMensagemNoDisplay("Temperatura", (temperatura), " C");
  mostrarMensagemNoDisplay("Umidade", (umidade), " %");
}

void conectarWifi() {

  delay(10);
  WiFi.begin(ssid, password);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Conectando");
  display.display();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.print(".");
    display.display();
  }
  display.clearDisplay();
  display.println("Wifi");
  display.println("IPLocal:");
  display.println(WiFi.localIP());
  display.display();
}

void mostrarMensagemNoDisplay(const char* texto1, float medicao, const char* texto2) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(texto1);
  display.setTextSize(2);
  display.setCursor(15, 15);
  display.print((String)medicao);
  display.setTextSize(1);
  display.print(texto2);
  display.display();
  delay(2000);
  display.clearDisplay();
}
