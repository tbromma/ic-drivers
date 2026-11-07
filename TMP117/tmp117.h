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
#include "tmp117Types.h"

namespace drv {

class TMP117 {
public:

    using I2cTxPoll_t = Exec (*)(uint8 slaveAddr, uint8* txData, uint8 txSize);
    using I2cTxRxPoll_t = Exec (*)(uint8 slaveAddr, uint8* txSrc, uint8 txSize, uint8* rxDest, uint8 rxSize);
    using I2cGetStatus_t = Status (*)();

    struct config_t {
        uint8           i2cAddr = tmp117::I2C_ADDR_GND;
        I2cTxPoll_t     I2cTxPoll = nullptr;
        I2cTxRxPoll_t   I2cTxRxPoll = nullptr;
        I2cGetStatus_t  I2cGetStatus = nullptr;
        const char*     label_p = nullptr;
    };

    Exec          Setup(const config_t& config);
    Exec          ReadRegister(tmp117::RegAddr regAddr, uint16& regVal);
    Exec          WriteRegister(tmp117::RegAddr regAddr, uint16 data);
    static float    ConvTempBinToDec(tmp117::regTemp_t result);
    Status          GetStatus();

private:

    static constexpr float  TEMP_CONV_VAL = 0.0078125;  // One LSB equals 7.8125m°C

    struct {
        config_t config;
        Status status;
    } ctx = {};
};

} // End of namespace "drv"

