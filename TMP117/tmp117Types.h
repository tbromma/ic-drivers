/******************************************************************************************************************************************
                                                               START OF FILE
*******************************************************************************************************************************************
                                                @
                                                @
                                                @@@ @@@@@@@@@@
                @@@@@@@@@@@@@  @@@        @@@   @ @@          @    @@@@@@@@@@@    @@@   @@@        @@@   @@@@@       @@@@@
                     @@@       @@@        @@@   @@@            @@  @@@      @@@   @@@   @@@        @@@   @@@@@@     @@@@@@
                     @@@       @@@        @@@  @  @             @  @@@      @@@   @@@   @@@        @@@   @@@ @@     @@ @@@
                     @@@       @@@@@@@@@@@@@@  @  @             @  @@@@@@@@@@@    @@@   @@@        @@@   @@@ @@@   @@@ @@@
                     @@@       @@@        @@@  @                @  @@@@@@@@@      @@@   @@@        @@@   @@@  @@@ @@@  @@@
                     @@@       @@@        @@@  @@              @@  @@@    @@@     @@@   @@@        @@@   @@@   @@@@@   @@@
                     @@@       @@@        @@@    @            @    @@@     @@@    @@@    @@@@@@@@@@@@    @@@    @@@    @@@
                     @@@       @@@        @@@     @@@@@@@@@@@@     @@@      @@@   @@@      @@@@@@@@      @@@     @     @@@

                     S      P      A      C      E        T      E      C      H      N      O      L      O      G      Y

*******************************************************************************************************************************************
                                                                 FILE INFO
******************************************************************************************************************************************/

// CREATED BY:       Tobiasz Bromma (tobiasz.bromma@thoriumspace.com)
// CREATION DATE:    October 2024
// DESCRIPTION:      Driver for TMP117 temperature sensor IC

/******************************************************************************************************************************************
                                                                  INCLUDES
******************************************************************************************************************************************/

#pragma once
#include "common.h"

namespace drv::tmp117 {

static constexpr uint16 DEVICE_ID = 0x0117;

enum I2cAddr : uint8 {
    I2C_ADDR_GND = 0b1001000,       // Sensor I2C address when ADD0 pin is connected to GND
    I2C_ADDR_VCC = 0b1001001,       // Sensor I2C address when ADD0 pin is connected to VCC
    I2C_ADDR_SDA = 0b1001010,       // Sensor I2C address when ADD0 pin is connected to SDA line
    I2C_ADDR_SCL = 0b1001011,       // Sensor I2C address when ADD0 pin is connected to SCL line
};

enum RegAddr : uint8 {
    REG_TEMP        = 0x00,     // Temperature result register
    REG_CONFIG      = 0x01,     // Configuration register
    REG_THIGH_LIM   = 0x02,     // Temperature high limit register
    REG_TLOW_LIM    = 0x03,     // Temperature low limit register
    REG_EEPROM_UL   = 0x04,     // EEPROM unlock register
    REG_EEPROM1     = 0x05,     // EEPROM1 register
    REG_EEPROM2     = 0x06,     // EEPROM2 register
    REG_TEMP_OFFSET = 0x07,     // Temperature offset register
    REG_EEPROM3     = 0x08,     // EEPROM3 register
    REG_DEVICE_ID   = 0x0F,     // Device ID register
};

enum Mode : uint16 {
    MODE_CONTINOUS = 0b00,
    MODE_SHUTDOWN = 0b01,
    MODE_ONESHOT = 0b11,
};

enum ConvRate : uint16 {
    CONV_RATE_64Hz,
    CONV_RATE_8Hz,
    CONV_RATE_4Hz,
    CONV_RATE_2Hz,
    CONV_RATE_1Hz,
    CONV_RATE_0p25Hz,
    CONV_RATE_0p125Hz,
    CONV_RATE_0p0625Hz,
};

enum AvgSamp : uint16 {
    AVG_SAMP_1,
    AVG_SAMP_8,
    AVG_SAMP_32,
    AVG_SAMP_64,
};

union regTemp_t {
    struct {
        uint16 TEMP : 16;
    } bit;
    uint16 val;
};

union regConfig_t {
    struct {
        uint16                      : 1;
        uint16      SOFT_RST        : 1;
        uint16      DR_ALERT        : 1;
        uint16      POL             : 1;
        uint16      THERM_ALERT     : 1;
        AvgSamp     AVG             : 2;
        ConvRate    CONV            : 3;
        Mode        MOD             : 2;
        uint16      EEPROM_BUSY     : 1;
        uint16      DATA_READY      : 1;
        uint16      LOW_ALERT       : 1;
        uint16      HIGH_ALERT      : 1;
    } bit;
    uint16 val;
};

union regTempHigh_t {
    struct {
        uint16 HIGH   : 16;
    } bit;
    uint16 val;
};

union regTempLow_t {
    struct {
        uint16 LOW    : 16;
    } bit;
    uint16 val;
};

union regEepromUnlock_t {
    struct {
        uint16        : 14;
        uint16 BUSY   : 1;
        uint16 UNLOCK : 1;
    } bit;
    uint16 val;
};

union regEeprom1_t {
    struct {
        uint16 DATA   : 16;
    } bit;
    uint16 val;
};

union regEeprom2_t {
    struct {
        uint16 DATA   : 16;
    } bit;
    uint16 val;
};

union regTempOffset_t {
    struct {
        uint16 OFFSET : 16;
    } bit;
    uint16 val;
};

union regEeprom3_t {
    struct {
        uint16 DATA   : 16;
    } bit;
    uint16 val;
};

union regDevId_t {
    struct {
        uint16 ID   : 12;
        uint16 REV  : 4;
    } bit;
    uint16 val;
};

} // End of namespace "drv::tmp117"
