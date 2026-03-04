#include <Wire.h>
#include <Adafruit_DS248x.h>
#include "DS28E18.h"
#include "VCNL4040_DS28E18.h"

// --- Instances ---
Adafruit_DS248x ds2482;
DS28E18 ds28e18(ds2482);
VCNL4040_DS28E18 vcnl4040(ds28e18);

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println("--- DS28E18 + VCNL4040 Simple Example ---");

  if (!ds2482.begin(&Wire, 0x18))
  {
    Serial.println("DS2482 not found!");
    while (1)
      ;
  }
  Serial.println("DS2482 initialized.");

  if (!ds28e18.begin(true))
  {
    Serial.println("DS28E18 init failed!");
    while (1)
      ;
  }

  // Configure for single sensor
  ds28e18.skipROM();

  // Configure GPIOs (I2C Pullups on DS28E18 side)
  if (!ds28e18.initializeGPIO())
  {
    Serial.println("DS28E18 GPIO Init Failed");
    while (1)
      ;
  }

  // Clear any POR flags
  ds28e18.resetDeviceStatus();

  // Scan I2C bus (debug)
  DS28E18_Debug = false;
  ds28e18.scanI2C();

  Serial.println("Initializing VCNL4040...");

  if (!vcnl4040.begin())
  {
    Serial.println("VCNL4040 init failed!");
    Serial.print("ID: 0x");
    Serial.println(vcnl4040.getID(), HEX);
    while (1)
      ;
  }

  Serial.print("VCNL4040 ID: 0x");
  Serial.println(vcnl4040.getID(), HEX);
  Serial.println("VCNL4040 initialized successfully!");

  // Optional: Enable ambient light sensing
  vcnl4040.powerOnAmbient();

  Serial.println("System Ready. Starting measurements...\n");
}

void loop()
{
  // Read values
  uint16_t proximity = vcnl4040.getProximity();
  uint16_t ambient = vcnl4040.getAmbient();
  uint16_t white = vcnl4040.getWhite();

  // Print values
  Serial.print("Proximity: ");
  Serial.print(proximity);
  Serial.print("\tAmbient: ");
  Serial.print(ambient);
  Serial.print("\tWhite: ");
  Serial.println(white);

  delay(200);
}
