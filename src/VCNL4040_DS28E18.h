#ifndef VCNL4040_DS28E18_H
#define VCNL4040_DS28E18_H

#include <Arduino.h>
#include "DS28E18.h"
#include "DS28E18_Sequencer.h"
#include "OneWireBus.h"

// VCNL4040 I2C Address
#define VCNL4040_ADDR 0x60

// VCNL4040 Command Codes (Register Addresses)
#define VCNL4040_ALS_CONF   0x00
#define VCNL4040_ALS_THDH   0x01
#define VCNL4040_ALS_THDL   0x02
#define VCNL4040_PS_CONF1   0x03  // Lower byte
#define VCNL4040_PS_CONF2   0x03  // Upper byte
#define VCNL4040_PS_CONF3   0x04  // Lower byte
#define VCNL4040_PS_MS      0x04  // Upper byte
#define VCNL4040_PS_CANC    0x05
#define VCNL4040_PS_THDL    0x06
#define VCNL4040_PS_THDH    0x07
#define VCNL4040_PS_DATA    0x08
#define VCNL4040_ALS_DATA   0x09
#define VCNL4040_WHITE_DATA 0x0A
#define VCNL4040_INT_FLAG   0x0B  // Upper byte
#define VCNL4040_ID         0x0C

// Used to select between upper and lower byte of command register
#define VCNL4040_LOWER true
#define VCNL4040_UPPER false

// ALS_CONF register masks and values
static const uint8_t VCNL4040_ALS_IT_MASK = (uint8_t)~((1 << 7) | (1 << 6));
static const uint8_t VCNL4040_ALS_IT_80MS = 0;
static const uint8_t VCNL4040_ALS_IT_160MS = (1 << 7);
static const uint8_t VCNL4040_ALS_IT_320MS = (1 << 6);
static const uint8_t VCNL4040_ALS_IT_640MS = (1 << 7) | (1 << 6);

static const uint8_t VCNL4040_ALS_PERS_MASK = (uint8_t)~((1 << 3) | (1 << 2));
static const uint8_t VCNL4040_ALS_PERS_1 = 0;
static const uint8_t VCNL4040_ALS_PERS_2 = (1 << 2);
static const uint8_t VCNL4040_ALS_PERS_4 = (1 << 3);
static const uint8_t VCNL4040_ALS_PERS_8 = (1 << 3) | (1 << 2);

static const uint8_t VCNL4040_ALS_INT_EN_MASK = (uint8_t)~((1 << 1));
static const uint8_t VCNL4040_ALS_INT_DISABLE = 0;
static const uint8_t VCNL4040_ALS_INT_ENABLE = (1 << 1);

static const uint8_t VCNL4040_ALS_SD_MASK = (uint8_t)~((1 << 0));
static const uint8_t VCNL4040_ALS_SD_POWER_ON = 0;
static const uint8_t VCNL4040_ALS_SD_POWER_OFF = (1 << 0);

// PS_CONF1 register masks and values
static const uint8_t VCNL4040_PS_DUTY_MASK = (uint8_t)~((1 << 7) | (1 << 6));
static const uint8_t VCNL4040_PS_DUTY_40 = 0;
static const uint8_t VCNL4040_PS_DUTY_80 = (1 << 6);
static const uint8_t VCNL4040_PS_DUTY_160 = (1 << 7);
static const uint8_t VCNL4040_PS_DUTY_320 = (1 << 7) | (1 << 6);

static const uint8_t VCNL4040_PS_PERS_MASK = (uint8_t)~((1 << 5) | (1 << 4));
static const uint8_t VCNL4040_PS_PERS_1 = 0;
static const uint8_t VCNL4040_PS_PERS_2 = (1 << 4);
static const uint8_t VCNL4040_PS_PERS_3 = (1 << 5);
static const uint8_t VCNL4040_PS_PERS_4 = (1 << 5) | (1 << 4);

static const uint8_t VCNL4040_PS_IT_MASK = (uint8_t)~((1 << 3) | (1 << 2) | (1 << 1));
static const uint8_t VCNL4040_PS_IT_1T = 0;
static const uint8_t VCNL4040_PS_IT_15T = (1 << 1);
static const uint8_t VCNL4040_PS_IT_2T = (1 << 2);
static const uint8_t VCNL4040_PS_IT_25T = (1 << 2) | (1 << 1);
static const uint8_t VCNL4040_PS_IT_3T = (1 << 3);
static const uint8_t VCNL4040_PS_IT_35T = (1 << 3) | (1 << 1);
static const uint8_t VCNL4040_PS_IT_4T = (1 << 3) | (1 << 2);
static const uint8_t VCNL4040_PS_IT_8T = (1 << 3) | (1 << 2) | (1 << 1);

static const uint8_t VCNL4040_PS_SD_MASK = (uint8_t)~((1 << 0));
static const uint8_t VCNL4040_PS_SD_POWER_ON = 0;
static const uint8_t VCNL4040_PS_SD_POWER_OFF = (1 << 0);

// PS_CONF2 register masks and values
static const uint8_t VCNL4040_PS_HD_MASK = (uint8_t)~((1 << 3));
static const uint8_t VCNL4040_PS_HD_12_BIT = 0;
static const uint8_t VCNL4040_PS_HD_16_BIT = (1 << 3);

static const uint8_t VCNL4040_PS_INT_MASK = (uint8_t)~((1 << 1) | (1 << 0));
static const uint8_t VCNL4040_PS_INT_DISABLE = 0;
static const uint8_t VCNL4040_PS_INT_CLOSE = (1 << 0);
static const uint8_t VCNL4040_PS_INT_AWAY = (1 << 1);
static const uint8_t VCNL4040_PS_INT_BOTH = (1 << 1) | (1 << 0);

// PS_CONF3 register masks and values
static const uint8_t VCNL4040_PS_SMART_PERS_MASK = (uint8_t)~((1 << 4));
static const uint8_t VCNL4040_PS_SMART_PERS_DISABLE = 0;
static const uint8_t VCNL4040_PS_SMART_PERS_ENABLE = (1 << 4);

static const uint8_t VCNL4040_PS_AF_MASK = (uint8_t)~((1 << 3));
static const uint8_t VCNL4040_PS_AF_DISABLE = 0;
static const uint8_t VCNL4040_PS_AF_ENABLE = (1 << 3);

static const uint8_t VCNL4040_PS_TRIG_MASK = (uint8_t)~((1 << 2));
static const uint8_t VCNL4040_PS_TRIG_TRIGGER = (1 << 2);

// PS_MS register masks and values
static const uint8_t VCNL4040_WHITE_EN_MASK = (uint8_t)~((1 << 7));
static const uint8_t VCNL4040_WHITE_ENABLE = 0;
static const uint8_t VCNL4040_WHITE_DISABLE = (1 << 7);

static const uint8_t VCNL4040_PS_MS_MASK = (uint8_t)~((1 << 6));
static const uint8_t VCNL4040_PS_MS_DISABLE = 0;
static const uint8_t VCNL4040_PS_MS_ENABLE = (1 << 6);

static const uint8_t VCNL4040_LED_I_MASK = (uint8_t)~((1 << 2) | (1 << 1) | (1 << 0));
static const uint8_t VCNL4040_LED_50MA = 0;
static const uint8_t VCNL4040_LED_75MA = (1 << 0);
static const uint8_t VCNL4040_LED_100MA = (1 << 1);
static const uint8_t VCNL4040_LED_120MA = (1 << 1) | (1 << 0);
static const uint8_t VCNL4040_LED_140MA = (1 << 2);
static const uint8_t VCNL4040_LED_160MA = (1 << 2) | (1 << 0);
static const uint8_t VCNL4040_LED_180MA = (1 << 2) | (1 << 1);
static const uint8_t VCNL4040_LED_200MA = (1 << 2) | (1 << 1) | (1 << 0);

// Interrupt flags
static const uint8_t VCNL4040_INT_FLAG_ALS_LOW = (1 << 5);
static const uint8_t VCNL4040_INT_FLAG_ALS_HIGH = (1 << 4);
static const uint8_t VCNL4040_INT_FLAG_CLOSE = (1 << 1);
static const uint8_t VCNL4040_INT_FLAG_AWAY = (1 << 0);


class VCNL4040_DS28E18 {
public:
    // Constructor for single DS28E18 device
    VCNL4040_DS28E18(DS28E18 &ds28e18_instance);
    
    // Constructor for OneWireBus with multiple devices
    VCNL4040_DS28E18(OneWireBus &busInstance, uint8_t deviceIndex = 0);

    // Initialize the sensor with default settings
    bool begin();
    
    // Check if sensor is responding
    bool isConnected();
    
    // Get the device ID (should return 0x0186)
    uint16_t getID();

    // --- Proximity Functions ---
    uint16_t getProximity();
    void powerOnProximity();
    void powerOffProximity();
    void setProxIntegrationTime(uint8_t timeValue);
    void setProxResolution(uint8_t resolutionValue);
    void setProxInterruptPersistance(uint8_t persValue);
    void setProxInterruptType(uint8_t interruptValue);
    void setProxHighThreshold(uint16_t threshold);
    void setProxLowThreshold(uint16_t threshold);
    void setProxCancellation(uint16_t cancelValue);

    // --- Ambient Light Functions ---
    uint16_t getAmbient();
    void powerOnAmbient();
    void powerOffAmbient();
    void setAmbientIntegrationTime(uint16_t timeValue);
    void setAmbientInterruptPersistance(uint8_t persValue);
    void enableAmbientInterrupts();
    void disableAmbientInterrupts();
    void setALSHighThreshold(uint16_t threshold);
    void setALSLowThreshold(uint16_t threshold);

    // --- White Channel Functions ---
    uint16_t getWhite();
    void enableWhiteChannel();
    void disableWhiteChannel();

    // --- IR LED Functions ---
    void setIRDutyCycle(uint16_t dutyValue);
    void setLEDCurrent(uint8_t currentValue);

    // --- Smart Persistence ---
    void enableSmartPersistance();
    void disableSmartPersistance();

    // --- Active Force Mode ---
    void enableActiveForceMode();
    void disableActiveForceMode();
    void takeSingleProxMeasurement();

    // --- Logic Output Mode ---
    void enableProxLogicMode();
    void disableProxLogicMode();

    // --- Interrupt Status ---
    bool isClose();
    bool isAway();
    bool isLight();
    bool isDark();

    // --- Device Selection (for bus mode) ---
    void setDeviceIndex(uint8_t index);
    uint8_t getDeviceIndex() const { return deviceIndex; }

private:
    DS28E18 *dsPtr = nullptr;
    OneWireBus *busPtr = nullptr;
    uint8_t deviceIndex = 0;
    
    DS28E18_Sequencer seq;
    
    // Internal helper to get the active DS28E18 reference
    DS28E18 &activeDS();
    
    // Low-level I2C via DS28E18 sequencer
    uint16_t readCommand(uint8_t commandCode);
    bool writeCommand(uint8_t commandCode, uint16_t value);
    uint8_t readCommandLower(uint8_t commandCode);
    uint8_t readCommandUpper(uint8_t commandCode);
    bool writeCommandLower(uint8_t commandCode, uint8_t newValue);
    bool writeCommandUpper(uint8_t commandCode, uint8_t newValue);
    
    // Bit manipulation helper
    void bitMask(uint8_t commandAddress, bool commandHeight, uint8_t mask, uint8_t thing);
};

#endif
