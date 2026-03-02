#include "VCNL4040_DS28E18.h"

VCNL4040_DS28E18::VCNL4040_DS28E18(DS28E18 &ds28e18_instance)
    : dsPtr(&ds28e18_instance), busPtr(nullptr), deviceIndex(0) {}

VCNL4040_DS28E18::VCNL4040_DS28E18(OneWireBus &busInstance, uint8_t index)
    : dsPtr(nullptr), busPtr(&busInstance), deviceIndex(index) {}

DS28E18 &VCNL4040_DS28E18::activeDS() {
    if (busPtr) return busPtr->device(deviceIndex);
    return *dsPtr;
}

void VCNL4040_DS28E18::setDeviceIndex(uint8_t index) {
    deviceIndex = index;
}

bool VCNL4040_DS28E18::begin() {
    if (!isConnected()) return false;
    if (getID() != 0x0186) return false;
    
    setLEDCurrent(200);
    setIRDutyCycle(40);
    setProxIntegrationTime(8);
    setProxResolution(16);
    enableSmartPersistance();
    powerOnProximity();
    return true;
}

bool VCNL4040_DS28E18::isConnected() {
    uint16_t id = readCommand(VCNL4040_ID);
    return (id != 0);
}

uint16_t VCNL4040_DS28E18::getID() {
    return readCommand(VCNL4040_ID);
}

uint16_t VCNL4040_DS28E18::readCommand(uint8_t commandCode) {
    DS28E18 &ds = activeDS();
    seq.clear();
    
    // I2C Write: Send register address
    seq.addStart();
    seq.addWriteByte(VCNL4040_ADDR, commandCode);
    
    // I2C Read: Read 2 bytes (LSB first, then MSB) via Repeated Start
    seq.addStart();
    seq.addRead(VCNL4040_ADDR, 2);
    seq.addStop();
    
    if (!ds.writeSequencer(0, seq.getBuffer(), seq.getLength())) return 0;
    
    uint8_t result;
    if (!ds.runSequencer(0, seq.getLength(), result)) {
        return 0;
    }
    
    // Calculate data offset in response:
    // [0] Result (0xAA)
    // [1] Start
    // [2-5] Write Pkt (0xE3, Len, Addr, Cmd)
    // [6] Start
    // [7-9] Write Addr Read (0xE3, 0x01, Addr|1)
    // [10-11] Read Op (0xD3, Len)
    // [12-13] DATA (2 bytes) <--- Offset is 12
    uint16_t dataOffset = 12;
    
    uint8_t rawData[20];
    uint16_t readLen = 0;
    if (!ds.readSequencer(0, rawData, seq.getLength(), readLen)) return 0;
    
    uint8_t lsb = rawData[dataOffset];
    uint8_t msb = rawData[dataOffset + 1];
    
    return ((uint16_t)msb << 8) | lsb;
}

bool VCNL4040_DS28E18::writeCommand(uint8_t commandCode, uint16_t value) {
    DS28E18 &ds = activeDS();
    seq.clear();
    seq.addStart();
    uint8_t writeData[3];
    writeData[0] = commandCode;
    writeData[1] = value & 0xFF;        // LSB
    writeData[2] = (value >> 8) & 0xFF; // MSB
    seq.addWrite(VCNL4040_ADDR, writeData, 3);
    seq.addStop();
    if (!ds.writeSequencer(0, seq.getBuffer(), seq.getLength())) return false;
    uint8_t result;
    if (!ds.runSequencer(0, seq.getLength(), result)) return false;
    return true;
}

uint8_t VCNL4040_DS28E18::readCommandLower(uint8_t commandCode) { return (readCommand(commandCode) & 0xFF); }
uint8_t VCNL4040_DS28E18::readCommandUpper(uint8_t commandCode) { return (readCommand(commandCode) >> 8); }
bool VCNL4040_DS28E18::writeCommandLower(uint8_t commandCode, uint8_t newValue) {
    uint16_t commandValue = readCommand(commandCode);
    commandValue = (commandValue & 0xFF00) | newValue;
    return writeCommand(commandCode, commandValue);
}
bool VCNL4040_DS28E18::writeCommandUpper(uint8_t commandCode, uint8_t newValue) {
    uint16_t commandValue = readCommand(commandCode);
    commandValue = (commandValue & 0x00FF) | ((uint16_t)newValue << 8);
    return writeCommand(commandCode, commandValue);
}
void VCNL4040_DS28E18::bitMask(uint8_t commandAddress, bool commandHeight, uint8_t mask, uint8_t thing) {
    uint8_t registerContents = (commandHeight == VCNL4040_LOWER) ? readCommandLower(commandAddress) : readCommandUpper(commandAddress);
    registerContents &= mask;
    registerContents |= thing;
    if (commandHeight == VCNL4040_LOWER) writeCommandLower(commandAddress, registerContents);
    else writeCommandUpper(commandAddress, registerContents);
}

// All API passthroughs...
uint16_t VCNL4040_DS28E18::getProximity() { return readCommand(VCNL4040_PS_DATA); }
void VCNL4040_DS28E18::powerOnProximity() { bitMask(VCNL4040_PS_CONF1, VCNL4040_LOWER, VCNL4040_PS_SD_MASK, VCNL4040_PS_SD_POWER_ON); }
void VCNL4040_DS28E18::powerOffProximity() { bitMask(VCNL4040_PS_CONF1, VCNL4040_LOWER, VCNL4040_PS_SD_MASK, VCNL4040_PS_SD_POWER_OFF); }
void VCNL4040_DS28E18::setProxIntegrationTime(uint8_t timeValue) {
    if (timeValue > 7) timeValue = VCNL4040_PS_IT_8T; else if (timeValue > 3) timeValue = VCNL4040_PS_IT_4T; else if (timeValue > 2) timeValue = VCNL4040_PS_IT_3T; else if (timeValue > 1) timeValue = VCNL4040_PS_IT_2T; else timeValue = VCNL4040_PS_IT_1T;
    bitMask(VCNL4040_PS_CONF1, VCNL4040_LOWER, VCNL4040_PS_IT_MASK, timeValue);
}
void VCNL4040_DS28E18::setProxResolution(uint8_t resolutionValue) {
    resolutionValue = (resolutionValue > 15) ? VCNL4040_PS_HD_16_BIT : VCNL4040_PS_HD_12_BIT;
    bitMask(VCNL4040_PS_CONF2, VCNL4040_UPPER, VCNL4040_PS_HD_MASK, resolutionValue);
}
void VCNL4040_DS28E18::setProxInterruptPersistance(uint8_t persValue) { bitMask(VCNL4040_PS_CONF1, VCNL4040_LOWER, VCNL4040_PS_PERS_MASK, persValue); }
void VCNL4040_DS28E18::setProxInterruptType(uint8_t interruptValue) { bitMask(VCNL4040_PS_CONF2, VCNL4040_UPPER, VCNL4040_PS_INT_MASK, interruptValue); }
void VCNL4040_DS28E18::setProxHighThreshold(uint16_t threshold) { writeCommand(VCNL4040_PS_THDH, threshold); }
void VCNL4040_DS28E18::setProxLowThreshold(uint16_t threshold) { writeCommand(VCNL4040_PS_THDL, threshold); }
void VCNL4040_DS28E18::setProxCancellation(uint16_t cancelValue) { writeCommand(VCNL4040_PS_CANC, cancelValue); }
uint16_t VCNL4040_DS28E18::getAmbient() { return readCommand(VCNL4040_ALS_DATA); }
void VCNL4040_DS28E18::powerOnAmbient() { bitMask(VCNL4040_ALS_CONF, VCNL4040_LOWER, VCNL4040_ALS_SD_MASK, VCNL4040_ALS_SD_POWER_ON); }
void VCNL4040_DS28E18::powerOffAmbient() { bitMask(VCNL4040_ALS_CONF, VCNL4040_LOWER, VCNL4040_ALS_SD_MASK, VCNL4040_ALS_SD_POWER_OFF); }
void VCNL4040_DS28E18::setAmbientIntegrationTime(uint16_t timeValue) {
    if (timeValue > 639) timeValue = VCNL4040_ALS_IT_640MS; else if (timeValue > 319) timeValue = VCNL4040_ALS_IT_320MS; else if (timeValue > 159) timeValue = VCNL4040_ALS_IT_160MS; else timeValue = VCNL4040_ALS_IT_80MS;
    bitMask(VCNL4040_ALS_CONF, VCNL4040_LOWER, VCNL4040_ALS_IT_MASK, timeValue);
}
void VCNL4040_DS28E18::setAmbientInterruptPersistance(uint8_t persValue) { bitMask(VCNL4040_ALS_CONF, VCNL4040_LOWER, VCNL4040_ALS_PERS_MASK, persValue); }
void VCNL4040_DS28E18::enableAmbientInterrupts() { bitMask(VCNL4040_ALS_CONF, VCNL4040_LOWER, VCNL4040_ALS_INT_EN_MASK, VCNL4040_ALS_INT_ENABLE); }
void VCNL4040_DS28E18::disableAmbientInterrupts() { bitMask(VCNL4040_ALS_CONF, VCNL4040_LOWER, VCNL4040_ALS_INT_EN_MASK, VCNL4040_ALS_INT_DISABLE); }
void VCNL4040_DS28E18::setALSHighThreshold(uint16_t threshold) { writeCommand(VCNL4040_ALS_THDH, threshold); }
void VCNL4040_DS28E18::setALSLowThreshold(uint16_t threshold) { writeCommand(VCNL4040_ALS_THDL, threshold); }
uint16_t VCNL4040_DS28E18::getWhite() { return readCommand(VCNL4040_WHITE_DATA); }
void VCNL4040_DS28E18::enableWhiteChannel() { bitMask(VCNL4040_PS_MS, VCNL4040_UPPER, VCNL4040_WHITE_EN_MASK, VCNL4040_WHITE_ENABLE); }
void VCNL4040_DS28E18::disableWhiteChannel() { bitMask(VCNL4040_PS_MS, VCNL4040_UPPER, VCNL4040_WHITE_EN_MASK, VCNL4040_WHITE_DISABLE); }
void VCNL4040_DS28E18::setIRDutyCycle(uint16_t dutyValue) {
    if (dutyValue > 319) dutyValue = VCNL4040_PS_DUTY_320; else if (dutyValue > 159) dutyValue = VCNL4040_PS_DUTY_160; else if (dutyValue > 79) dutyValue = VCNL4040_PS_DUTY_80; else dutyValue = VCNL4040_PS_DUTY_40;
    bitMask(VCNL4040_PS_CONF1, VCNL4040_LOWER, VCNL4040_PS_DUTY_MASK, dutyValue);
}
void VCNL4040_DS28E18::setLEDCurrent(uint8_t currentValue) {
    if (currentValue > 199) currentValue = VCNL4040_LED_200MA; else if (currentValue > 179) currentValue = VCNL4040_LED_180MA; else if (currentValue > 159) currentValue = VCNL4040_LED_160MA; else if (currentValue > 139) currentValue = VCNL4040_LED_140MA; else if (currentValue > 119) currentValue = VCNL4040_LED_120MA; else if (currentValue > 99) currentValue = VCNL4040_LED_100MA; else if (currentValue > 74) currentValue = VCNL4040_LED_75MA; else currentValue = VCNL4040_LED_50MA;
    bitMask(VCNL4040_PS_MS, VCNL4040_UPPER, VCNL4040_LED_I_MASK, currentValue);
}
void VCNL4040_DS28E18::enableSmartPersistance() { bitMask(VCNL4040_PS_CONF3, VCNL4040_LOWER, VCNL4040_PS_SMART_PERS_MASK, VCNL4040_PS_SMART_PERS_ENABLE); }
void VCNL4040_DS28E18::disableSmartPersistance() { bitMask(VCNL4040_PS_CONF3, VCNL4040_LOWER, VCNL4040_PS_SMART_PERS_MASK, VCNL4040_PS_SMART_PERS_DISABLE); }
void VCNL4040_DS28E18::enableActiveForceMode() { bitMask(VCNL4040_PS_CONF3, VCNL4040_LOWER, VCNL4040_PS_AF_MASK, VCNL4040_PS_AF_ENABLE); }
void VCNL4040_DS28E18::disableActiveForceMode() { bitMask(VCNL4040_PS_CONF3, VCNL4040_LOWER, VCNL4040_PS_AF_MASK, VCNL4040_PS_AF_DISABLE); }
void VCNL4040_DS28E18::takeSingleProxMeasurement() { bitMask(VCNL4040_PS_CONF3, VCNL4040_LOWER, VCNL4040_PS_TRIG_MASK, VCNL4040_PS_TRIG_TRIGGER); }
void VCNL4040_DS28E18::enableProxLogicMode() { bitMask(VCNL4040_PS_MS, VCNL4040_UPPER, VCNL4040_PS_MS_MASK, VCNL4040_PS_MS_ENABLE); }
void VCNL4040_DS28E18::disableProxLogicMode() { bitMask(VCNL4040_PS_MS, VCNL4040_UPPER, VCNL4040_PS_MS_MASK, VCNL4040_PS_MS_DISABLE); }
bool VCNL4040_DS28E18::isClose() { return (readCommandUpper(VCNL4040_INT_FLAG) & VCNL4040_INT_FLAG_CLOSE); }
bool VCNL4040_DS28E18::isAway() { return (readCommandUpper(VCNL4040_INT_FLAG) & VCNL4040_INT_FLAG_AWAY); }
bool VCNL4040_DS28E18::isLight() { return (readCommandUpper(VCNL4040_INT_FLAG) & VCNL4040_INT_FLAG_ALS_HIGH); }
bool VCNL4040_DS28E18::isDark() { return (readCommandUpper(VCNL4040_INT_FLAG) & VCNL4040_INT_FLAG_ALS_LOW); }

