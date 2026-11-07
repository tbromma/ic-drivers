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
// DESCRIPTION:      Driver for PCA9536 GPIO expander IC

/******************************************************************************************************************************************
                                                                  INCLUDES
******************************************************************************************************************************************/

#pragma once
#include "common.h"
#include "ioExpanderTypes.h"

namespace drv {

class IOEXPAND {
public:

    using I2cTxPoll_t = Exec (*)(uint8 slaveAddr, uint8* txData, uint8 txSize);
    using I2cTxRxPoll_t = Exec (*)(uint8 slaveAddr, uint8* txSrc, uint8 txSize, uint8* rxDest, uint8 rxSize);
    using I2cGetStatus_t = Status (*)();

    struct config_t {
        uint8           i2cAddr = ioExpand::I2C_DEFAULT_ADDR;
        I2cTxPoll_t     I2cTxPoll = nullptr;
        I2cTxRxPoll_t   I2cTxRxPoll = nullptr;
        I2cGetStatus_t  I2cGetStatus = nullptr;
        const char*     label_p = nullptr;
    };

    Exec Setup(const config_t& config);
    Exec ReadRegister(ioExpand::RegAddr regAddr, uint8& regVal);
    Exec ReadRegister16Bit(ioExpand::RegAddr regAddr, uint16& regVal);
    Exec WriteRegister(ioExpand::RegAddr regAddr, uint8 data);
    Exec WriteRegister16Bit(ioExpand::RegAddr regAddr, uint16 data);
    Status GetStatus();

private:

    struct {
        config_t config;
        Status status;
    } ctx = {};
};

} // End of namespace "drv"

