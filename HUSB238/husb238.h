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

    Exec Setup(const config_t& config);
    Exec ReadRegister(husb238::RegAddr regAddr, uint8& regVal);
    Exec WriteRegister(husb238::RegAddr regAddr, uint8 regVal);
    Exec GetAttachedStatus(bool& attached);
    Exec SendCommand(husb238::Command cmd);
    Exec GetPdAvailable(husb238::PdCapability& pd);
    Exec GetPdSelected(husb238::PdSelected& pd);
    Exec RequestPd(husb238::PdVoltage pd);
    Status GetStatus();
    static const char* GetPdVoltageStr(husb238::PdVoltage voltage);
    static const char* GetPdCurrentStr(husb238::PdCurrent current);

private:

    struct {
        config_t config;
        Status status;
        struct {
            drv::husb238::RegPdStatus0 status0;
            drv::husb238::RegPdStatus1 status1;
            drv::husb238::RegSrcPdo5V srcPdo5V;
            drv::husb238::RegSrcPdo9V srcPdo9V;
            drv::husb238::RegSrcPdo12V srcPdo12V;
            drv::husb238::RegSrcPdo15V srcPdo15V;
            drv::husb238::RegSrcPdo18V srcPdo18V;
            drv::husb238::RegSrcPdo20V srcPdo20V;
        } reg;
    } ctx = {};
};

} // End of namespace "drv"

