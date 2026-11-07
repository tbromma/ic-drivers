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
// DESCRIPTION:      Driver for Sensirion SHT4xA temperature and humidity sensor IC family

/******************************************************************************************************************************************
                                                                  INCLUDES
******************************************************************************************************************************************/

#pragma once
#include "common.h"
#include "timer.h"
#include "units.h"
#include "utils.h"

/******************************************************************************************************************************************
                                                                   TYPES
******************************************************************************************************************************************/
namespace drv {
namespace sht4xa {

constexpr uint8 I2C_DEFAULT_ADDR = 0b1000100;

enum class Command : uint8 {
    MEAS_TEMP_RH_HIGH_RES   = 0xFD, // Measure T & RH with high precision (high repeatability)
    MEAS_TEMP_RH_MEDIUM_RES = 0xF6, // Measure T & RH with medium precision (medium repeatability)
    MEAS_TEMP_RH_LOW_RES    = 0xE0, // Measure T & RH with lowest precision (low repeatability)
    READ_SERIAL_NUMBER      = 0x89, // Read IC serial number (ID)
    SOFT_RESET              = 0x94, // Perform software reset
    ACTIVATE_HEATER_1sec    = 0x2F, // Activate heater with 110mW for 1s including a high precision measurement just before deactivation
    ACTIVATE_HEATER_100msec = 0x24, // Activate heater with 110mW for 0.1s including a high precision measurement just before deactivation
};

// Measurement accuracy
enum class Resolution : uint8 {
    LOW,
    MEDIUM,
    HIGH,
};

using serialNum_t = uint32;         // Sensor unique electronic 32-bit serial number

struct __packed convData_t {
    uint16 tempBin;
    uint8 tempCrc;
    uint16 rhBin;
    uint8 rhCrc;
};

} // End of namespace 'sht4xa'
/******************************************************************************************************************************************
                                                                    API
******************************************************************************************************************************************/

using namespace sht4xa;
using namespace bareOs;

class SHT4xA {
public:

    using I2cTxPoll_t = Exec (*)(uint8 slaveAddr, uint8* txData, uint8 txSize);
    using I2cRxPoll_t = Exec (*)(uint8 slaveAddr, uint8* rxDest, uint8 rxSize);
    using I2cGetStatus_t = Status (*)();

    struct config_t {
        uint8           i2cAddr = sht4xa::I2C_DEFAULT_ADDR;
        I2cTxPoll_t     I2cTxPoll = nullptr;
        I2cRxPoll_t     I2cRxPoll = nullptr;
        I2cGetStatus_t  I2cGetStatus = nullptr;
        const char*     label_p = nullptr;
    };

	Exec  Setup(const config_t& config);
    Exec  SendCmd(Command cmd);
    Exec  TriggConversion(Resolution res);
    Exec  SoftwareReset();
    Exec  ReadConvResult(convData_t& data);
	Exec  ReadSensorSn(serialNum_t& serial);

    static uint8   GetConvTimeMs(Resolution res);
    static bool    CheckCrc(uint8* data, uint8 dataSize, uint8 checksum);
    static void    ConvTempBinToDec(Resolution res, uint16 bin, units::uniVal_t& decimal);
    static void    ConvRhBinToDec(Resolution res, uint16 bin, units::uniVal_t& decimal);

private:

    static constexpr uint16 CRC_POLYNOMIAL = 0b100110001;   // P(x)=x^8+x^5+x^4+1
    static constexpr uint8 convTimeMs[] = {2, 5, 9};        // Values in ms derived from datasheet (Resolution -> Low, Medium High)

    struct __packed serialData_t {
        uint16 highWord;
        uint8 crc1;
        uint16 lowWord;
        uint8 crc2;
    };

    struct {
        config_t config = {};
        Status status = Status::NONE;
    } ctx = {};
};

/******************************************************************************************************************************************
                                                                END OF FILE
******************************************************************************************************************************************/
} // End of namespace 'drv::sht25'
