#include "DHT.h"
#include "ThingSpeak.h"
#include <WiFi.h>
#include <HTTPClient.h> // <<< ESSENCIAL para usar HTTPClient no ESP32

// -------- CONFIGURAÇÕES --------
#define DHTPIN 4       // Pino do DHT22 no ESP32
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// Credenciais WiFi
const char* ssid = "Eduardo";
const char* password = "12345678";

// Configuração ThingSpeak
unsigned long myChannelNumber = 3100155;
const char * myWriteAPIKey = "6VP1D9FYAEQ2RTQJ";

WiFiClient client;

// Config OpenWeather
const String apiKey = "ab9e9785e9f8509a131338a5c0bd9dc1";
const String cidade = "Leopoldina,BR";

float getOpenWeatherTemperature() {
    float tempC = NAN;
    HTTPClient http;

    String url = "http://api.openweathermap.org/data/2.5/weather?q=" + cidade + "&appid=" + apiKey + "&units=metric&lang=pt";

    http.begin(url); // inicia requisição HTTP
    int httpCode = http.GET();

    if (httpCode == 200) {
        String payload = http.getString();
        int index = payload.indexOf("\"temp\":");
        if (index > 0) {
            int start = index + 7;
            int end = payload.indexOf(",", start);
            String tempStr = payload.substring(start, end);
            tempC = tempStr.toFloat();
        }
    } else {
        Serial.println("Erro ao acessar OpenWeather: " + String(httpCode));
    }

    http.end();
    return tempC;
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  ThingSpeak.begin(client);

  Serial.println("Conectando ao WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
}

void loop() {
  // ---- Leitura DHT22 ----
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Falha ao ler do sensor DHT22!");
    delay(5000);
    return;
  }

  // ---- Leitura OpenWeather ----
  float t_ow = getOpenWeatherTemperature();
  if (isnan(t_ow)) {
    Serial.println("Falha ao obter temperatura do OpenWeather");
  }

  // ---- Exibe no Serial ----
  Serial.print("DHT22 -> Temp: "); Serial.print(t); Serial.print(" °C, Umid: "); Serial.println(h);
  Serial.print("OpenWeather -> Temp: "); Serial.println(t_ow);

  // ---- Envio para ThingSpeak ----
  ThingSpeak.setField(1, t);     // Campo 1 = DHT22 Temp
  ThingSpeak.setField(2, h);     // Campo 2 = DHT22 Umidade
  ThingSpeak.setField(3, t_ow);  // Campo 3 = Temp OpenWeather

  int httpCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (httpCode == 200) {
    Serial.println("Dados enviados ao ThingSpeak com sucesso!");
  } else {
    Serial.println("Erro ao enviar. Código HTTP: " + String(httpCode));
  }

  delay(20000); // Aguarda 20 segundos para próxima atualização
}
