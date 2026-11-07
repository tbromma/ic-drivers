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

// CREATED BY:  Tobiasz Bromma (tobiasz.bromma@thoriumspace.com)
// DESCRIPTION: Low level driver for Hynetek HUSB238 USB-C PD (Power Delivery) controller IC

/******************************************************************************************************************************************
                                                                  INCLUDES
******************************************************************************************************************************************/

#pragma once
#include "common.h"
#include "husb238Types.h"

namespace drv {

class HUSB238 {
public:

    using I2cTxPoll_t = Exec (*)(uint8 slaveAddr, uint8* txData, uint8 txSize);
    using I2cTxRxPoll_t = Exec (*)(uint8 slaveAddr, uint8* txSrc, uint8 txSize, uint8* rxDest, uint8 rxSize);
    using I2cGetStatus_t = Status (*)();

    struct config_t {
        uint8           i2cAddr = husb238::I2C_ADDR;
        I2cTxPoll_t     I2cTxPoll = nullptr;
        I2cTxRxPoll_t   I2cTxRxPoll = nullptr;
        I2cGetStatus_t  I2cGetStatus = nullptr;
        const char*     label_p = nullptr;
    };

    Exec    Setup(const config_t& config);
    Exec    ReadRegister(husb238::RegAddr regAddr, uint8& regVal);
    Exec    WriteRegister(husb238::RegAddr regAddr, uint8 regVal);
    Status  GetStatus();

private:

    struct {
        config_t config;
        Status status;
    } ctx = {};
};

} // End of namespace "drv"

