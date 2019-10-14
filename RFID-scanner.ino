#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266HTTPClient.h>

#include "settings.h"

const byte reverseTable[256] = {0, 128, 64, 192, 32, 160, 96, 224, 16, 144, 80, 208, 48, 176, 112, 240, 8, 136, 72, 200, 40, 168, 104, 232, 24, 152, 88, 216, 56, 184, 120, 248, 4, 132, 68, 196, 36, 164, 100, 228, 20, 148, 84, 212, 52, 180, 116, 244, 12, 140, 76, 204, 44, 172, 108, 236, 28, 156, 92, 220, 60, 188, 124, 252, 2, 130, 66, 194, 34, 162, 98, 226, 18, 146, 82, 210, 50, 178, 114, 242, 10, 138, 74, 202, 42, 170, 106, 234, 26, 154, 90, 218, 58, 186, 122, 250, 6, 134, 70, 198, 38, 166, 102, 230, 22, 150, 86, 214, 54, 182, 118, 246, 14, 142, 78, 206, 46, 174, 110, 238, 30, 158, 94, 222, 62, 190, 126, 254, 1, 129, 65, 193, 33, 161, 97, 225, 17, 145, 81, 209, 49, 177, 113, 241, 9, 137, 73, 201, 41, 169, 105, 233, 25, 153, 89, 217, 57, 185, 121, 249, 5, 133, 69, 197, 37, 165, 101, 229, 21, 149, 85, 213, 53, 181, 117, 245, 13, 141, 77, 205, 45, 173, 109, 237, 29, 157, 93, 221, 61, 189, 125, 253, 3, 131, 67, 195, 35, 163, 99, 227, 19, 147, 83, 211, 51, 179, 115, 243, 11, 139, 75, 203, 43, 171, 107, 235, 27, 155, 91, 219, 59, 187, 123, 251, 7, 135, 71, 199, 39, 167, 103, 231, 23, 151, 87, 215, 55, 183, 119, 247, 15, 143, 79, 207, 47, 175, 111, 239, 31, 159, 95, 223, 63, 191, 127, 255};

SoftwareSerial uart(RX_pin, TX_pin); // Only RX used
BearSSL::WiFiClientSecure client;

void setup() {
  // initialize serial:
  Serial.begin(9600);
  while (!Serial) {};
  uart.begin(9600);

  Serial.print("Connecting to ");
  Serial.println(ssid); 
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected");
  Serial.println(WiFi.localIP());

  client.setInsecure();
  Serial.println("Start");  
}

void loop() {
  byte data[255];
  readUART(data);

  
  uint id = getEM(data + 3);
  char idString[11];
  sprintf(idString, "%010u", id); // Zero pad. All card numbers are 10 digits long
  
  Serial.print("EM ");
  Serial.println(idString);

  String response = httpPostEM(client, host, "checkin/post/", String(idString));

  Serial.println(response);
  playSound(response, buzzer_pin);
}

String httpPostEM(BearSSL::WiFiClientSecure client, const char* host, const char* link, String EM) {
  HTTPClient https;
  if (https.begin(client, String(host) + link)) {  // HTTPS
    https.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = https.POST("card_id=" + EM + "&secret=" + secret);

    if (httpCode == 200) { // Success
      return https.getString();
    } else {
      return String(httpCode);
    }
    https.end();
  } 
  else {
    return "Unable to connect";
  }
}

uint getEM(byte* cardData) {
  return reverseTable[cardData[0]] << 24 | reverseTable[cardData[1]] << 16 | reverseTable[cardData[2]] << 8 | reverseTable[cardData[3]];
}

byte readByte() {
  while (!(uart.available())){ yield(); }
  return (byte)uart.read();
}

void readUART(byte* output) {
  while (readByte() != 0x02);
  
  byte length = readByte() - 2;
  output[0] = length + 1;
  for (byte index = 1; index < length + 1; index++) {
    output[index] = readByte();
  }
}

void playSound(String response, int buzzer_pin) {
  if (response=="check in") {
    checkInTone(buzzer_pin);
  }
  else if (response=="check out") {
    checkOutTone(buzzer_pin);
  }
  else {
    accessDeniedTone(buzzer_pin);
  }
}

void play(int pin, int t) {
  if (t) {
    tone(pin, 1200);
    delay(100);
    noTone(pin);
    delay(100);
    tone(pin, 1400);
    delay(200);
    noTone(pin);
  } else {
    tone(pin, 1200);
    delay(100);
    noTone(pin);
    delay(100);
    tone(pin, 800);
    delay(200);
    noTone(pin);
  }
}

void checkInTone(int buzzer_pin){
  tone(buzzer_pin, 523.25);
  delay(200);
  noTone(buzzer_pin);
  tone(buzzer_pin, 783.99);
  delay(200);
  noTone(buzzer_pin);
  tone(buzzer_pin, 1046.50);
  delay(400);
  noTone(buzzer_pin);
}

void checkOutTone(int buzzer_pin) {
  tone(buzzer_pin, 523.25);
  delay(200);
  noTone(buzzer_pin);
  tone(buzzer_pin, 392.00);
  delay(200);
  noTone(buzzer_pin);
  tone(buzzer_pin, 261.63);
  delay(400);
  noTone(buzzer_pin);
}

void accessDeniedTone(int buzzer_pin){
  tone(buzzer_pin, 261.63);
  delay(100);
  noTone(buzzer_pin);
  delay(100);
  tone(buzzer_pin, 261.63);
  delay(100);
  noTone(buzzer_pin);
  delay(100);
  tone(buzzer_pin, 261.63);
  delay(600);
  noTone(buzzer_pin);
}
