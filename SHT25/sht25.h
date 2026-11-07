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
// DESCRIPTION:      Driver for Sensirion SHT25 temperature and humidity sensor IC

/******************************************************************************************************************************************
                                                                  INCLUDES
******************************************************************************************************************************************/

#pragma once
#include "common.h"
#include "timer.h"
#include "units.h"

/******************************************************************************************************************************************
                                                                   TYPES
******************************************************************************************************************************************/
namespace drv::sht25 {

constexpr uint8 I2C_ADDRESS = 0b1000000;

enum cmd_t {
    CMD_READ_CONV_RESULT    = 0x00,         // READ TEMP/RH CONVERSION RESULT
    CMD_TRIG_TEMP_MEAS_HM   = 0xE3,         // TRIGGER TEMPRATURE MEASUREMENT WITH HOLD MASTER
    CMD_TRIG_TEMP_MEAS_NHM  = 0xF3,         // TRIGGER TEMPRATURE MEASUREMENT WITH NO HOLD MASTER
    CMD_TRIG_RH_MEAS_HM     = 0xE5,         // TRIGGER HUMIDITY MEASUREMENT WITH HOLD MASTER
    CMD_TRIG_RH_MEAS_NHM    = 0xF5,         // TRIGGER HUMIDITY MEASUREMENT WITH NO HOLD MASTER
    CMD_WRITE_USER_REG      = 0xE6,
    CMD_READ_USER_REG       = 0xE7,
    CMD_SOFTWARE_RESET      = 0xFE,
    CMD_READ_EID_PART1      = 0x0FFA,       // COMMAND TO READ THE FIRST PART OF THE ELECTRONIC IDENTIFICATION CODE
    CMD_READ_EID_PART2      = 0xC9FC,       // COMMAND TO READ THE SECOND PART OF THE ELECTRONIC IDENTIFICATION CODE
};

enum tempRes_t : uint8 {
    TEMP_RES_11BIT,             // RESOLUTION 0.08degC; CONVERSION TIME 11ms
    TEMP_RES_12BIT,             // RESOLUTION 0.04degC; CONVERSION TIME 22ms
    TEMP_RES_13BIT,             // RESOLUTION 0.02degC; CONVERSION TIME 43ms
    TEMP_RES_14BIT,             // RESOLUTION 0.01degC; CONVERSION TIME 85ms
};

enum rhRes_t : uint8 {
    RH_RES_8BIT,                // RESOLUTION 0.4%; CONVERSION TIME 4ms
    RH_RES_10BIT,               // RESOLUTION 0.1%; CONVERSION TIME 9ms
    RH_RES_11BIT,               // RESOLUTION 0.05%; CONVERSION TIME 15ms
    RH_RES_12BIT,               // RESOLUTION 0.025%; CONVERSION TIME 29ms
};

union userReg_t {
    struct {
        uint8 convRes1      : 1;
        uint8 otpReloadOff  : 1;
        uint8 enableHeater  : 1;
        uint8               : 3;
        uint8 endOfBatt     : 1;
        uint8 convRes2      : 1;
    } bit;
    uint8 val;
};

using sensorEID_t = uint64;     // SENSOR UNIQUE ELECTRONIC IDENTIFICATION NUMBER
using I2cTxPoll_t = Exec (*)(uint8 slaveAddr, uint8* txData, uint8 txSize);
using I2cRxPoll_t = Exec (*)(uint8 slaveAddr, uint8* rxDest, uint8 rxSize);
using I2cGetStatus_t = Status (*)();

struct config_t {
    uint8           i2cAddr = I2C_ADDRESS;
    I2cTxPoll_t     I2cTxPoll = nullptr;
    I2cRxPoll_t     I2cRxPoll = nullptr;
    I2cGetStatus_t  I2cGetStatus = nullptr;
    const char*     label_p = nullptr;
};

/******************************************************************************************************************************************
                                                                    API
******************************************************************************************************************************************/

class SHT25 {
public:

	Exec    Setup(const config_t& config);
    Exec    SendCmd(cmd_t cmd);
    Exec    ReadData(cmd_t cmd, uint8* rxData_p, uint8 rxSize);
    Exec    WriteUserReg(userReg_t userReg);
	Exec    GetSensorEID(sensorEID_t& sensorEID);
    Exec    SoftwareReset();
    uint8   GetTempConvTime(tempRes_t res);
    uint8   GetRhConvTime(rhRes_t res);
    bool    CheckCrc(uint8* data, uint8 dataCount, uint8 checksum);
    void    ConvTempBinToDec(tempRes_t res, uint16 bin, bareOs::units::uniVal_t& decimal);
    void    ConvRhBinToDec(rhRes_t res, uint16 bin, bareOs::units::uniVal_t& decimal);
    Status  GetStatus();

private:

    struct {
        config_t config = {};
        Status status = Status::NONE;
    } ctx = {};
};

/******************************************************************************************************************************************
                                                                END OF FILE
******************************************************************************************************************************************/
} // End of namespace 'drv::sht25'
