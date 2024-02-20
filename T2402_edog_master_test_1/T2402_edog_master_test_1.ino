
#include <Wire.h>
#include <stdlib.h>
#include "edog.h"


uint32_t next_power_off_ms;
uint32_t next_watchdog_reset_ms;


void setup() {
  Wire.begin();
  Wire.setClock(100000);
  Serial.begin(9600); 
  delay(3000);
  Serial.println("\n\nT2402_edog_master_test_1");
  Serial.print (__DATE__); Serial.print ("  "); Serial.println(__TIME__);

  edog_initialize();
  edog_set_wd_timeout(1000);
  delay(1);
  edog_set_sleep_time(1000);
  delay(10);

next_power_off_ms = millis() + 1000;
next_watchdog_reset_ms  = millis() + 500;

}


void loop()
{
  if (millis() > next_watchdog_reset_ms)
  {
    edog_clear_watchdog();
    next_watchdog_reset_ms  = millis() + 500;
  }
  if (millis() > next_power_off_ms)
  {
    edog_switch_off();
    next_power_off_ms = millis() + 1000;
  }
}



