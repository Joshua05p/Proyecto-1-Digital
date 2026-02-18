#include "config.h"

// ===== Variables =====
uint8_t Distancia = 0;
uint8_t Luz = 0;
char Color = 'N';

#define IO_LOOP_DELAY 5000
unsigned long lastUpdate = 0;

// UART buffer
String mensaje = "";

// ===== Feeds =====
AdafruitIO_Feed *DistanciaFeed = io.feed("Distancia");
AdafruitIO_Feed *ColorFeed = io.feed("Color");
AdafruitIO_Feed *LuzFeed = io.feed("Luz");

void setup() {
  
  Serial2.begin(9600);    // UART desde Arduino

  Serial.print("Connecting to Adafruit IO");

  io.connect();

  DistanciaFeed->onMessage(handleDistancia);
  ColorFeed->onMessage(handleColor);
  LuzFeed->onMessage(handleLuz);

  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println(io.statusText());
}

void loop() {

  io.run();

  // ===== LEER UART =====
  while (Serial2.available()) {
    char c = Serial2.read();

    if (c == '#') {
      procesarMensaje(mensaje);
      mensaje = "";
    } else {
      mensaje += c;
    }
  }

  // ===== PUBLICAR =====
  if (millis() > (lastUpdate + IO_LOOP_DELAY)) {

    DistanciaFeed->save(Distancia);
    LuzFeed->save(Luz);
    ColorFeed->save(String(Color));

    Serial.println("Datos enviados a Adafruit");

    lastUpdate = millis();
  }
}

void procesarMensaje(String msg) {

  int dIndex = msg.indexOf("D:");
  int lIndex = msg.indexOf(",L:");
  int cIndex = msg.indexOf(",C:");

  if (dIndex == -1 || lIndex == -1 || cIndex == -1) return;

  Distancia = msg.substring(dIndex+2, lIndex).toInt();
  Luz = msg.substring(lIndex+3, cIndex).toInt();
  Color = msg.substring(cIndex+3).charAt(0);

  Serial.println("----- UART -----");
  Serial.print("Distancia: "); Serial.println(Distancia);
  Serial.print("Luz: "); Serial.println(Luz);
  Serial.print("Color: "); Serial.println(Color);
}

// ===== Handlers =====
void handleDistancia(AdafruitIO_Data *data) {
  Serial.print("Feed Distancia: ");
  Serial.println(data->toInt());
}

void handleColor(AdafruitIO_Data *data) {
  Serial.print("Feed Color: ");
  Serial.println(data->toString());
}

void handleLuz(AdafruitIO_Data *data) {
  Serial.print("Feed Luz: ");
  Serial.println(data->toInt());
}
