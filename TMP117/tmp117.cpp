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

#include "tmp117.h"
#include "utils.h"

#undef CODE_LABEL
#undef CODE_STATUS
#define CODE_LABEL  ctx.config.label_p
#define CODE_STATUS ctx.status

/******************************************************************************************************************************************
                                                         PUBLIC FUNCTION DEFINITION's
******************************************************************************************************************************************/

namespace drv {

using namespace tmp117;

Exec TMP117::Setup(const config_t& config)
{
    //-----------------------------------------------------------------------------------
    // Check config

    ASSERT(config.I2cTxPoll != nullptr, "I2C TX DATA FUNC");
    ASSERT(config.I2cTxRxPoll != nullptr, "I2C TXRX DATA FUNC");
    ASSERT(config.I2cGetStatus != nullptr, "I2C GET CODE_STATUS FUNC");
    ASSERT(config.I2cGetStatus() == Status::OK, "I2C BUS");

    ctx.config = config;
    ctx.status = Status ::INIT;

    //-----------------------------------------------------------------------------------
    // Read sensor ID to check communication

    regDevId_t regDevId = {};
    ReadRegister(REG_DEVICE_ID, regDevId.val);

    if (regDevId.bit.ID != DEVICE_ID) {
        RET_ERR("Device ID Mismatch");
    }

    //-----------------------------------------------------------------------------------

    ctx.status = Status::OK;
    return Exec::OK;
}

Exec TMP117::ReadRegister(RegAddr regAddr, uint16& regVal)
{
    CHECK_STATUS();
    CHECK_EXEC(ctx.config.I2cTxRxPoll(ctx.config.i2cAddr, (uint8*)&regAddr, sizeof(RegAddr), (uint8*)&regVal, sizeof(regVal)));
    CHECK_EXEC(utils::RotateBytewise((uint8*)&regVal, sizeof(regVal), (uint8*)&regVal));
    return Exec::OK;
}

Exec TMP117::WriteRegister(RegAddr regAddr, uint16 regVal)
{
    uint8 txBuff[] = {regAddr, (uint8)(regVal >> 8), (uint8)regVal};
    RET_EXEC(ctx.config.I2cTxPoll(ctx.config.i2cAddr, txBuff, sizeof(txBuff)));
}

float TMP117::ConvTempBinToDec(regTemp_t result)
{
    return (float)(int16(result.bit.TEMP)) * TEMP_CONV_VAL;
}

Status TMP117::GetStatus()
{
    return ctx.status;
}

} // End of namespace "drv"
