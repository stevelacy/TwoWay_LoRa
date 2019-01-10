#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include "SSD1306.h"

#define SCK      5    // GPIO5  -- SX1278's SCK
#define MISO     19   // GPIO19 -- SX1278's MISO
#define MOSI     27   // GPIO27 -- SX1278's MOSI
#define SS       18   // GPIO18 -- SX1278's CS
#define RST      14   // GPIO14 -- SX1278's RESET
#define DI0      26   // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define BAND     433E6
#define LED_PIN  2
#define OLED_PIN 16
#define BTN_PIN  0

unsigned int counter = 0;
unsigned int pressed = 0;

SSD1306 display(0x3c, 4, 15);
String packSize = "--";
String packet;

void setup() {
  pinMode(OLED_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT);

  digitalWrite(OLED_PIN, LOW);    // set GPIO16 low to reset OLED
  delay(50);
  digitalWrite(OLED_PIN, HIGH); // while OLED is running, must set GPIO16 in high

  Serial.begin(9600);
  while (!Serial);
  Serial.println();
  Serial.println("LoRa Sender Test");

  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("init ok");
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "LoRa");
  display.drawString(0, 15, "Waiting for connections");
  display.display();
}

void loop() {
  // receive packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) { cbk(packetSize); }

  if (digitalRead(BTN_PIN) == LOW) {
    digitalWrite(LED_PIN, HIGH); // turn the LED on (HIGH is the voltage level)
    if (pressed != 1) {
      pressed = 1;
      sendMsg();
    }
  }
  else {
    digitalWrite(LED_PIN, LOW); // turn the LED off by making the voltage LOW
    pressed = 0;
  }
  delay(50);
}

void sendMsg() {
  // send packet
  counter++;
  LoRa.beginPacket();
  LoRa.print("hello ");
  LoRa.print(counter);
  LoRa.endPacket();
}

void cbk(int packetSize) {
  packet = "";
  packSize = String(packetSize, DEC);
  for (int i = 0; i < packetSize; i++) { packet += (char) LoRa.read(); }
  loraData("RSSI " + String(LoRa.packetRssi(), DEC));
}

void loraData(String msg){
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0 , 15 , "Received "+ packSize + " bytes");
  display.drawStringMaxWidth(0 , 26 , 128, packet);
  display.drawString(0, 0, msg);
  display.display();
}
