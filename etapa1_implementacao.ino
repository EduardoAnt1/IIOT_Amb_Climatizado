#include "DHT.h"

#define DHTPIN 4   
#define DHTTYPE DHT22   

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial.println("Leitura do sensor DHT22...");

  dht.begin();
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Falha ao ler do sensor DHT22!");
    return;
  }

  Serial.print("Umidade: ");
  Serial.print(h);
  Serial.print(" %  |  Temperatura: ");
  Serial.print(t);
  Serial.println(" °C");

  delay(2000); 
}

