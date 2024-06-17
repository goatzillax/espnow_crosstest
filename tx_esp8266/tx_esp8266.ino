#include <ESP8266WiFi.h>
#include <espnow.h>
#include "espnow_msg.h"

int wifi_search_chan;

struct_espnow_msg msg;

enum e_xmit_state {
   XMIT_IDLE,
   XMIT_STARTED,
   XMIT_FINISHED
};

enum e_xmit_state xmit_state;

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
   digitalWrite(LED_BUILTIN, HIGH);
   if (sendStatus == 0) {
      Serial.println("Delivery success");
   } else {
      Serial.println("Delivery fail");
   }
   xmit_state = XMIT_FINISHED;
}

void deepSleep() {
   delay(1000);
   ESP.deepSleep(0);
}

void setup() {
   Serial.begin(115200);

   Serial.println();
   Serial.println();

   pinMode(LED_BUILTIN, OUTPUT);

   WiFi.disconnect();
   WiFi.persistent(false);
   WiFi.mode(WIFI_STA);

   if (esp_now_init() != 0) {
      Serial.println("Error initializing ESP-NOW");
      deepSleep();
   }

   esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
   esp_now_register_send_cb(OnDataSent);

   xmit_state = XMIT_IDLE;

   wifi_search_chan = 3;  //  set ur channel here
   if (esp_now_add_peer(masterDeviceMac, ESP_NOW_ROLE_SLAVE, wifi_search_chan, NULL, 0)) {
      Serial.println("error adding peer");
      deepSleep();
   }
   wifi_set_channel(wifi_search_chan);

}

void loop() {
   switch (xmit_state) {
      case XMIT_FINISHED:
         xmit_state = XMIT_IDLE;
         deepSleep();
         break;
      case XMIT_IDLE:
         {
            msg.count = 0;
            xmit_state = XMIT_STARTED;
            digitalWrite(LED_BUILTIN, LOW);
            esp_now_send(masterDeviceMac, (uint8_t *)&msg, sizeof(msg));
         }
      case XMIT_STARTED:
         delay(100);
         break;
      default:
         Serial.println("WAT?");
         break;
   }
}
