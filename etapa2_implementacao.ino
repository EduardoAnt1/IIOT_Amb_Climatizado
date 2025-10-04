#include "DHT.h"
#include "ThingSpeak.h"
#include <WiFi.h>

// -------- CONFIGURAÇÕES --------
#define DHTPIN 4       // Pino do DHT22 no ESP32 (ajuste se precisar)
#define DHTTYPE DHT22   // Tipo de sensor

DHT dht(DHTPIN, DHTTYPE);

// Credenciais WiFi
const char* ssid = "Eduardo";         // Substitua pelo nome da sua rede WiFi
const char* password = "12345678";    // Substitua pela senha do WiFi

// Configuração ThingSpeak
unsigned long myChannelNumber = 3100155;       // ID do seu canal
const char * myWriteAPIKey = "6VP1D9FYAEQ2RTQJ";     // API Key de escrita

WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(100);

  dht.begin();

  WiFi.mode(WIFI_STA);  
  ThingSpeak.begin(client);
}

void loop() {
  // ---- Conexão WiFi ----
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Conectando-se a: ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, password);
      delay(5000);
      Serial.print(".");
    }
    Serial.println("\nWiFi conectado!");
  }

  // ---- Leitura DHT22 ----
  float h = dht.readHumidity();
  float t = dht.readTemperature();     // Celsius
  float f = dht.readTemperature(true); // Fahrenheit

  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Falha ao ler do sensor DHT22!");
    return;
  }

  Serial.print("Umidade: ");
  Serial.print(h);
  Serial.print(" % | Temperatura: ");
  Serial.print(t);
  Serial.println(" °C");

  // ---- Envio para ThingSpeak ----
  ThingSpeak.setField(1, t);  // Campo 1 = Temperatura
  ThingSpeak.setField(2, h);  // Campo 2 = Umidade

  int httpCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (httpCode == 200) {
    Serial.println("Dados enviados ao ThingSpeak com sucesso!");
  } else {
    Serial.println("Erro ao enviar. Código HTTP: " + String(httpCode));
  }

  delay(20000); // Aguarda 20 segundos para próxima leitura (limite do ThingSpeak)
}
