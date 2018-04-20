#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>

constexpr uint8_t RST_PIN = 9;
constexpr uint8_t SS_PIN = 4;

int buzzer_pin = 15;

MFRC522 mfrc522(SS_PIN, RST_PIN);

const char* ssid = "";
const char* password = "";
const char* secret = "";
const char* url = "https://makentnu.no/checkin/post/";

String card_id = "";
String last_card_id = "";
int last_scan;
int rescan_time = 10000;
 
void setup() {
  //Serial.begin(9600);
  SPI.begin();          // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  
  
  // Connect to WiFi network
  //Serial.print("Connecting to ");
  //Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }
  //Serial.println("Connected");

  last_scan = millis();
}
 
void loop() {
    // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  card_id = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    card_id.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
    card_id.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  if (card_id == last_card_id and millis() - rescan_time < last_scan) {
    return;
  }
  
  last_scan = millis();
  last_card_id = card_id;
  
  play(buzzer_pin, 1);

  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpPOSTCode = http.POST("card_id=" + card_id + "&secret=" + secret);
  Serial.println(httpPOSTCode);
  //http.writeToStream(&Serial);
  http.end();
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
void checkInTone(int buzzer){
  tone(buzzer, 523.25);
  delay(200);
  noTone(buzzer);
  tone(buzzer, 783.99);
  delay(200);
  noTone(buzzer);
  tone(buzzer, 1046.50);
  delay(400);
  noTone(buzzer);
}
void checkOutTone(int buzzer) {
  tone(buzzer, 523.25);
  delay(200);
  noTone(buzzer);
  tone(buzzer, 392.00);
  delay(200);
  noTone(buzzer);
  tone(buzzer, 261.63);
  delay(400);
  noTone(buzzer);
}
void accessDeniedTone(int buzzer){
  tone(buzzer, 261.63);
  delay(100);
  noTone(buzzer);
  delay(100);
  tone(buzzer, 261.63);
  delay(100);
  noTone(buzzer);
  delay(100);
  tone(buzzer, 261.63);
  delay(600);
  noTone(buzzer);
}

