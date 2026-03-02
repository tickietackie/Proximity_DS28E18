# VCNL4040_DS28E18

Arduino library for the VCNL4040 proximity and ambient light sensor, connected via the DS28E18 1-Wire to I2C bridge.

## Overview

The VCNL4040 is a proximity and ambient light sensor from Vishay. This library adapts the [SparkFun VCNL4040 Arduino Library](https://github.com/sparkfun/SparkFun_VCNL4040_Arduino_Library) to work with the DS28E18 1-Wire to I2C bridge, enabling remote I2C sensor placement over a simple 1-Wire bus.

## Features

- **Proximity Sensing**: Detect objects qualitatively up to 20cm
- **Ambient Light Sensing**: Measure ambient light levels
- **White Channel**: Read white light measurements
- **Full Configuration**: Access to all VCNL4040 settings (LED current, duty cycle, integration time, etc.)
- **Single or Multi-Device**: Works with single DS28E18 or OneWireBus for multiple devices

## Hardware Requirements

- DS2482-100 or DS2482-800 1-Wire Master
- DS28E18 1-Wire to I2C Bridge
- VCNL4040 Sensor
- Arduino-compatible MCU

## Wiring

```
MCU ──I2C──> DS2482 ──1-Wire──> DS28E18 ──I2C──> VCNL4040
```

## Installation

### PlatformIO

Add to `platformio.ini`:
```ini
lib_deps =
    https://github.com/DanielSart/VCNL4040_DS28E18.git
    https://github.com/DanielSart/DS28E18_DS2482_Library.git
    adafruit/Adafruit DS248x @ ^1.0.1
```

### Arduino IDE

1. Download this repository as ZIP
2. Sketch → Include Library → Add .ZIP Library
3. Also install dependencies: `DS28E18_DS2482_Library` and `Adafruit DS248x`

## Quick Start

```cpp
#include <Wire.h>
#include <Adafruit_DS248x.h>
#include "DS28E18.h"
#include "VCNL4040_DS28E18.h"

Adafruit_DS248x ds2482;
DS28E18 ds28e18(ds2482);
VCNL4040_DS28E18 vcnl4040(ds28e18);

void setup() {
  Serial.begin(115200);
  
  ds2482.begin(&Wire, 0x18);
  ds28e18.begin(true);
  ds28e18.skipROM();
  ds28e18.initializeGPIO();
  ds28e18.resetDeviceStatus();
  
  if (vcnl4040.begin()) {
    Serial.println("VCNL4040 Ready!");
    vcnl4040.powerOnAmbient();  // Optional: enable ALS
  }
}

void loop() {
  Serial.print("Prox: ");
  Serial.print(vcnl4040.getProximity());
  Serial.print(" ALS: ");
  Serial.println(vcnl4040.getAmbient());
  delay(100);
}
```

## API Reference

### Core Functions
- `begin()` - Initialize sensor with defaults (returns false if sensor not found)
- `isConnected()` - Check if sensor responds on I2C
- `getID()` - Read device ID (should return 0x0186)

### Proximity
- `getProximity()` - Read proximity value (0-65535)
- `powerOnProximity()` / `powerOffProximity()` - Power control
- `setProxIntegrationTime(time)` - Set integration (1-8)
- `setProxResolution(res)` - Set resolution (12 or 16 bit)
- `setLEDCurrent(mA)` - Set IR LED current (50-200 mA)

### Ambient Light
- `getAmbient()` - Read ambient light value
- `powerOnAmbient()` / `powerOffAmbient()` - Power control
- `setAmbientIntegrationTime(ms)` - Set integration (80-640 ms)

### White Channel
- `getWhite()` - Read white light value
- `enableWhiteChannel()` / `disableWhiteChannel()`

## License

MIT License - Based on SparkFun VCNL4040 Arduino Library
