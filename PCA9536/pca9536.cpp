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

#include "pca9536.h"
#include "utils.h"

#undef CODE_LABEL
#undef CODE_STATUS
#define CODE_LABEL      ctx.config.label_p
#define CODE_STATUS     ctx.status

/******************************************************************************************************************************************
                                                         PUBLIC FUNCTION DEFINITION's
******************************************************************************************************************************************/

namespace drv {

using namespace pca9536;

Exec PCA9536::Setup(const config_t& config)
{
    ASSERT(config.I2cTxPoll != nullptr, "I2C TX DATA FUNC");
    ASSERT(config.I2cTxRxPoll != nullptr, "I2C TXRX DATA FUNC");
    ASSERT(config.I2cGetStatus != nullptr, "I2C GET CODE_STATUS FUNC");
    ASSERT(config.I2cGetStatus() == Status::OK, "I2C BUS");

    ctx.config = config;
    ctx.status = Status::OK;
    return Exec::OK;
}

Exec PCA9536::ReadRegister(RegAddr regAddr, uint8& regVal)
{
    CHECK_STATUS();
    CHECK_EXEC(ctx.config.I2cTxRxPoll(ctx.config.i2cAddr, (uint8*)&regAddr, sizeof(RegAddr), &regVal, sizeof(regVal)));
    CHECK_EXEC(utils::RotateBytewise(&regVal, sizeof(regVal), &regVal));
    return Exec::OK;
}

Exec PCA9536::WriteRegister(RegAddr regAddr, uint8 regVal)
{
    uint8 txBuff[] = {regAddr, regVal};
    RET_EXEC(ctx.config.I2cTxPoll(ctx.config.i2cAddr, txBuff, sizeof(txBuff)));
}

Status PCA9536::GetStatus()
{
    return ctx.status;
}

} // End of namespace "drv"
