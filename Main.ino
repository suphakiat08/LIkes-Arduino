#include <SimpleTimer.h>
#include "WiFiClient.h"

SimpleTimer timer;

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("Started.");
  connect_wifi();
  connect_mqtt();
  timer.setInterval(2000, client_loop);
}

void loop() {
  timer.run();
}
