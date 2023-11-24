#include <WiFi.h>
#include <esp_now.h>
#include <Adafruit_NeoPixel.h>

#include "espnow_msg.h"

Adafruit_NeoPixel LED_pixel(1, 7, NEO_GRB + NEO_KHZ800);

void setPixel(int level) {
   LED_pixel.clear();
   if (level) {
      LED_pixel.setPixelColor(0, LED_pixel.Color(0,0,255));
   }
   LED_pixel.show();
}

#define BUZZER_PATTERN_LEN  6
#define BUZZER_DELAY        1000
const int buzzer_pattern[BUZZER_PATTERN_LEN] = { HIGH, LOW, HIGH, LOW, HIGH, LOW };

int buzzer_state=BUZZER_PATTERN_LEN;
unsigned long buzzer_start;


void start_buzzer() {
   if (buzzer_state == BUZZER_PATTERN_LEN) {
      buzzer_state = 0;
      buzzer_start = millis();
   }
}


void cycle_buzzer() {
   if (buzzer_state == BUZZER_PATTERN_LEN) {
      return;
   }

   setPixel(buzzer_pattern[buzzer_state]);

   if (millis() - buzzer_start > BUZZER_DELAY) {
      buzzer_state++;
      buzzer_start = millis();
   }
}

#define WIFI_STA_WAIT 10000

void infra_setup() {

   WiFi.begin("ssid", "password");  //  if this actually connects, unicast ESP-NOW stops working.

   unsigned long wifi_start_time=millis();

   while (WiFi.status() != WL_CONNECTED) {
      if (millis() - wifi_start_time > WIFI_STA_WAIT) {
         break;
      }
      delay(100);
   }

   if (WiFi.status() == WL_CONNECTED) {
      Serial.println(WiFi.localIP());
   }
//   else {
//      WiFi.disconnect();
      WiFi.softAP("softap", "password", 5, 0);
      Serial.println(WiFi.softAPIP());
//   }
   Serial.print("Wi-Fi Channel: ");
   Serial.println(WiFi.channel());  //  Yes, I do know the correct channel my regular AP is on and have verified it with other scanners.

}

void infra_loop() {
}

void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
   Serial.println("msg received");
   start_buzzer();
}

void setup() {
   Serial.begin(115200);

   LED_pixel.begin();
   LED_pixel.clear();
   LED_pixel.show();

   pinMode(9, INPUT_PULLUP);

#define DEBUG
#ifdef DEBUG
   delay(1000);
   Serial.println();
   Serial.println();
   Serial.print("MAC Address:  ");
   Serial.println(WiFi.macAddress());
#endif

   WiFi.disconnect();
   WiFi.persistent(false);
   WiFi.mode(WIFI_AP_STA);

   WiFi.setSleep(false);
   
   infra_setup();

   if (esp_now_init() != 0) {
      Serial.println("Error initializing ESP-NOW");
      delay(10000);
      ESP.deepSleep(0);
      return;
   }

   esp_now_register_recv_cb(reinterpret_cast<esp_now_recv_cb_t>(OnDataRecv));
}

uint32_t debounce=0;

void loop() {
   infra_loop();
   cycle_buzzer();

   if (!digitalRead(9) && (millis()-debounce > 1000)) {
      debounce = millis();
      Serial.println("disconnecting WiFi");
      WiFi.disconnect();
   }

   delay(100);
}
