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
// CREATION DATE:    July 2023
// DESCRIPTION:      Driver for the DS3231 RTC IC

/******************************************************************************************************************************************
                                                                  INCLUDES
******************************************************************************************************************************************/

#pragma once
#include "common.h"
#include "systime.h"
#include "units.h"

/******************************************************************************************************************************************
                                                                   TYPES
******************************************************************************************************************************************/
namespace drv {
namespace ds3231 {

constexpr uint8 I2C_ADDR = 0b1101000;
constexpr uint16 START_YEAR = 2000;     // The DS3231 internal fixed start year
constexpr uint8 DS3231_REG_CNT = 19;    // The DS3231 RTC clock has 19 registers in total; REG_TEMP_LSB -> last register

enum regAddr_t : uint8 {
    REG_TIME_SEC        = 0x00,
    REG_TIME_MIN        = 0x01,
    REG_TIME_HOUR       = 0x02,
    REG_TIME_DAY        = 0x03,
    REG_TIME_DATE       = 0x04,
    REG_TIME_MONTH      = 0x05,
    REG_TIME_YEAR       = 0x06,

    REG_ALARM1_SEC      = 0x07,
    REG_ALARM1_MIN      = 0x08,
    REG_ALARM1_HOUR     = 0x09,
    REG_ALARM1_DAY      = 0x0A,

    REG_ALARM2_MIN      = 0x0B,
    REG_ALARM2_HOUR     = 0x0C,
    REG_ALARM2_DAY      = 0x0AD,

    REG_CONTROL         = 0x0E,
    REG_STATUS          = 0x0F,

    REG_AGING           = 0x10,
    REG_TEMP_MSB        = 0x11,
    REG_TEMP_LSB        = 0x12,
};

enum rateSelect_t {
	RS_1HZ      = 0b00,
	RS_1024Hz   = 0b01,
	RS_4096Hz   = 0b10,
	RS_8192Hz   = 0b11,
};

union regSec_t {
    struct {
        uint8 bcd1          : 4;
        uint8 bcd10         : 3;
        uint8               : 1;
    } bit;
    uint8 val;
};

union regMin_t {
    struct {
        uint8 bcd1          : 4;
        uint8 bcd10         : 3;
        uint8               : 1;
    } bit;
    uint8 val;
};

union regHour_t {
    struct {
        uint8 bcd1          : 4;
        uint8 bcd10         : 1;
        uint8 bcd20         : 1;
        uint8 _12_24_Cfg    : 1;        // 0 -> 24 HOUR SYSTEM, 1 -> 12 HOUR SYSTEM. AM/PM INFO IN BIT 'bcd20', 0 -> AM / 1 -> PM
        uint8               : 1;
    } bit;
    uint8 val;
};

union regDayOfWeek_t {
    struct {
        uint8 bcd1          : 3;
        uint8               : 5;
    } bit;
    uint8 val;
};

union regDay_t {
    struct {
        uint8 bcd1          : 4;
        uint8 bcd10         : 2;
        uint8               : 2;
    } bit;
    uint8 val;
};

union regMonth_t {
    struct {
        uint8 bcd1          : 4;
        uint8 bcd10         : 1;
        uint8               : 2;
        uint8 century       : 1;        // SET TO 1 WHEN THE YEAR REGISTER OVERFLOWS FROM 99 TO 0
    } bit;
    uint8 val;
};

union regYear_t {
    struct {
        uint8 bcd1          : 4;
        uint8 bcd10         : 4;
    } bit;
    uint8 val;
};

union regCtrl_t {
    struct {
        uint8           A1IE    : 1;        // ALARM 1 INTERRUPT ENABLE
        uint8           A2IE    : 1;        // ALARM 2 INTERRUPT ENABLE
        uint8           INTCN   : 1;        // INTERRUPT CTRL -> 0 = SQUARE WAVE OUTPUT ON INT PIN : 1 = ALARM INT
        rateSelect_t    RS      : 2;        // RATE SELECT OF THE SQUARE WAVE OUTPUT FREQUENCY
        uint8           CONV    : 1;        // TRIGGER TEMPRATURE CONVERSION
        uint8           BBSQW   : 1;        // 0 = SQUARE WAVE OUTPUT IS OFF WHEN "INTCN = 1" & IN BATTERY BACKUP MODE
                                            // 1 = SQUARE WAVE OUTPUT IS ON WHEN "INTCN = 1" & IN BATTERY BACKUP MODE
        uint8           EOSC    : 1;        // 0 = OSCILATOR ON : 1 = OSCILLATOR OFF
    } bit;
    uint8 val;
};

union regStat_t {
    struct {
        uint8           A1F         : 1;    // ALARM 1 INTERRUPT FLAG
        uint8           A2F         : 1;    // ALARM 2 INTERRUPT FLAG
        uint8           BSY         : 1;    // 1 = IC BUSY WITH TEMPRATURE CONVERSION
        uint8           EN32kHz     : 1;    // 1 = ENABLE THE 32.768kHz OUTPUT
        uint8                       : 3;    // RESERVED
        uint8           OSF         : 1;    // OSCILLATOR STOP FLAG
    } bit;
    uint8 val;
};

} // End of namespace 'ds3231'
/******************************************************************************************************************************************
                                                                    API
******************************************************************************************************************************************/

class DS3231 {
public:

    using I2cTxPoll_t = Exec (*)(uint8 slaveAddr, uint8* txData, uint8 txSize);
    using I2cTxRxPoll_t = Exec (*)(uint8 slaveAddr, uint8* txSrc, uint8 txSize, uint8* rxDest, uint8 rxSize);
    using I2cGetStatus_t = Status (*)();

    struct config_t {
        uint8           i2cAddr = ds3231::I2C_ADDR;
        I2cTxPoll_t     I2cTxPoll = nullptr;
        I2cTxRxPoll_t   I2cTxRxPoll = nullptr;
        I2cGetStatus_t  I2cGetStatus = nullptr;
        const char*     label_p = nullptr;
    };

    Exec    Setup(const config_t& config);
    Exec    ReadRegister(ds3231::regAddr_t regAddr, uint8& regVal);
    Exec    WriteRegister(ds3231::regAddr_t regAddr, uint8 regVal);
    Exec    ReadMultipleRegs(ds3231::regAddr_t regStart, uint8* regData, uint8 byteCnt);
    Exec    WriteMultipleRegs(ds3231::regAddr_t regStart, uint8* regData, uint8 byteCnt);
    Exec    GetCtrlReg(ds3231::regCtrl_t& ctrlRegVal);
    Exec    SetCtrlReg(ds3231::regCtrl_t ctrlRegVal);
    Exec    GetStatusReg(ds3231::regStat_t& statRegVal);
    Exec    SetStatusReg(ds3231::regStat_t statRegVal);
    void    ConvTempBinToDec(uint16 bin, bareOs::units::uniVal_t& temp);
    Status  GetStatus();

private:

    static constexpr uint8 CTRL_REG_RESET_VAL = 0b00011100;
    static constexpr uint8 _6BIT_MAX_VAL = 64;

    struct {
        config_t    config = {};
        Status      status = Status::NONE;
    } ctx = {};
};

} // End of namespace 'drv'
/******************************************************************************************************************************************
                                                                END OF FILE
******************************************************************************************************************************************/
