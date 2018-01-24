#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>

constexpr uint8_t RST_PIN = 9;
constexpr uint8_t SS_PIN = 4;

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
  SPI.begin();          // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  //Serial.begin(9600);
  
  // Connect to WiFi network
  /*Serial.print("Connecting to ");
  Serial.println(ssid);*/
 
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
  // TODO: beep();

  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.POST("card_id=" + card_id + "&secret=" + secret);
  http.writeToStream(&Serial);
  http.end();
}
