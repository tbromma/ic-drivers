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

// CREATED BY:      Tobiasz Bromma (tobiasz.bromma@thoriumspace.com)
// CREATION DATE:   June 2024
// DESCRIPTION:     Low level dirver for AT24Cxx serial EEPROM IC

/******************************************************************************************************************************************
                                                                  INCLUDES
******************************************************************************************************************************************/

#include "at24cxx.h"

/******************************************************************************************************************************************
                                                          PRIVATE TYPES & VARIABLES
******************************************************************************************************************************************/
namespace drv::at24cxx {

#undef CODE_LABEL
#define CODE_LABEL ctx.config.label_p

/******************************************************************************************************************************************
                                                         PUBLIC FUNCTION DEFINITIONS
******************************************************************************************************************************************/

Exec AT24Cxx::Setup(const config_t& config)
{
    ctx.config = config;
    ctx.status = Status ::INIT;

    //-----------------------------------------------------------------------------------
    // Check config

    ASSERT(ctx.config.memSize != 0, "MEMORY SIZE");
    ASSERT(ctx.config.pageSize != 0, "PAGE SIZE");
    ASSERT(ctx.config.i2cAddr != 0, "I2C ADDRESS ");
    ASSERT(ctx.config.I2cTxData != nullptr, "I2C TX DATA FUNC.");
    ASSERT(ctx.config.I2cTxRxData != nullptr, "I2C TXRTX DATA FUNC.")
    ASSERT(ctx.config.I2cGetStatus != nullptr, "I2C GET STATUS FUNC.");
    ASSERT(ctx.config.I2cGetStatus() == Status::OK, "I2C BUS INVALID STATUS");

    //-----------------------------------------------------------------------------------
    // Read one byte to check I2C communication

    uint8 redByte;
    if (ReadMemory(0, &redByte, 1, true) == Exec::ERR) {
        REPORT_FAT("READ CHECK FAILED");
    }

    //-----------------------------------------------------------------------------------

    ctx.status = Status::OK;
    return Exec::OK;
}

Status AT24Cxx::GetStatus()
{
    return ctx.status;
}

/******************************************************************************************************************************************
                                                         PRIVATE FUNCTION DEFINITIONS
******************************************************************************************************************************************/

Exec AT24Cxx::ReadMemory(uint16 address, uint8* data_p, uint8 size, bool wait)
{
    ASSERT(address <= ctx.config.memSize, "INVALID ADDRESS");
    ASSERT((address + size) <= ctx.config.memSize, "MEMORY ACCESS");
    ASSERT(data_p != nullptr, "VARIABLE 'data_p'");
    RET_EXEC(ctx.config.I2cTxRxData(ctx.config.i2cAddr, (uint8*)&address, sizeof(address), data_p, size, wait, nullptr));
}

Exec AT24Cxx::WriteByte(uint16 address, uint8 byte, bool wait)
{
    ASSERT(address <= ctx.config.memSize, "INVALID ADDRESS");
    uint8 txData[] = {(uint8)(address >> 8), (uint8)address, byte};
    RET_EXEC(ctx.config.I2cTxData(ctx.config.i2cAddr, txData, sizeof(txData), wait, nullptr));
}

/******************************************************************************************************************************************
                                                                END OF FILE
******************************************************************************************************************************************/
} // END OF NAMESPACE 'drv::at24cxx'
