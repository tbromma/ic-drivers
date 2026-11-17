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

#include "husb238.h"

#undef CODE_LABEL
#undef CODE_STATUS
#define CODE_LABEL  ctx.config.label_p
#define CODE_STATUS ctx.status

/******************************************************************************************************************************************
                                                         PUBLIC FUNCTION DEFINITION's
******************************************************************************************************************************************/

namespace drv {

using namespace husb238;

Exec HUSB238::Setup(const config_t& config)
{
    //-----------------------------------------------------------------------------------
    // Check config

    ASSERT(config.I2cTxPoll != nullptr, "I2C TX DATA FUNC");
    ASSERT(config.I2cTxRxPoll != nullptr, "I2C TXRX DATA FUNC");
    ASSERT(config.I2cGetStatus != nullptr, "I2C GET STATUS FUNC");
    ASSERT(config.I2cGetStatus() == Status::OK, "I2C BUS");

    ctx.config = config;
    ctx.status = Status ::INIT;

    //-----------------------------------------------------------------------------------
    // Read sensor ID to check communication

    RegPdStatus0 regPdStatus0 = {};

    if (ReadRegister(REG_PD_STATUS0, regPdStatus0.val) == Exec::ERR) {
        ctx.status = Status::ERR;
        return Exec::ERR;
    }

    //-----------------------------------------------------------------------------------

    ctx.status = Status::OK;
    return Exec::OK;
}

Exec HUSB238::ReadRegister(RegAddr regAddr, uint8& regVal)
{
    CHECK_STATUS();
    CHECK_EXEC(ctx.config.I2cTxRxPoll(ctx.config.i2cAddr, (uint8*)&regAddr, sizeof(RegAddr), &regVal, sizeof(regVal)));
    return Exec::OK;
}

Exec HUSB238::WriteRegister(RegAddr regAddr, uint8 regVal)
{
    CHECK_STATUS();
    uint8 txBuff[] = {regAddr, regVal};
    RET_EXEC(ctx.config.I2cTxPoll(ctx.config.i2cAddr, txBuff, sizeof(txBuff)));
}

Status HUSB238::GetStatus()
{
    return ctx.status;
}

Exec HUSB238::GetAttachedStatus(bool& attached)
{
    CHECK_STATUS();
    CHECK_EXEC(ReadRegister(REG_PD_STATUS1, ctx.reg.status1.val));
    attached = ctx.reg.status1.bit.ATTACH;
    return Exec::OK;
}

Exec HUSB238::GetPdAvailable(PdCapability& pd)
{
    CHECK_STATUS();
    pd = {};
    CHECK_EXEC(ReadRegister(REG_SRC_PDO_5V, ctx.reg.srcPdo5V.val));
    CHECK_EXEC(ReadRegister(REG_SRC_PDO_9V, ctx.reg.srcPdo9V.val));
    CHECK_EXEC(ReadRegister(REG_SRC_PDO_12V, ctx.reg.srcPdo12V.val));
    CHECK_EXEC(ReadRegister(REG_SRC_PDO_15V, ctx.reg.srcPdo15V.val));
    CHECK_EXEC(ReadRegister(REG_SRC_PDO_18V, ctx.reg.srcPdo18V.val));
    CHECK_EXEC(ReadRegister(REG_SRC_PDO_20V, ctx.reg.srcPdo20V.val));

    if (ctx.reg.srcPdo5V.bit.DETECT) {
        pd.pd5VDetected = true;
        pd.pd5VCurrent = ctx.reg.srcPdo5V.bit.CURRENT;
    }
    if (ctx.reg.srcPdo9V.bit.DETECT) {
        pd.pd9VDetected = true;
        pd.pd9VCurrent = ctx.reg.srcPdo9V.bit.CURRENT;
    }
    if (ctx.reg.srcPdo12V.bit.DETECT) {
        pd.pd12VDetected = true;
        pd.pd12VCurrent = ctx.reg.srcPdo12V.bit.CURRENT;
    }
    if (ctx.reg.srcPdo15V.bit.DETECT) {
        pd.pd15VDetected = true;
        pd.pd15VCurrent = ctx.reg.srcPdo15V.bit.CURRENT;
    }
    if (ctx.reg.srcPdo18V.bit.DETECT) {
        pd.pd18VDetected = true;
        pd.pd18VCurrent = ctx.reg.srcPdo18V.bit.CURRENT;
    }
    if (ctx.reg.srcPdo20V.bit.DETECT) {
        pd.pd20VDetected = true;
        pd.pd20VCurrent = ctx.reg.srcPdo20V.bit.CURRENT;
    }
    pd.dataValid = true;
    return Exec::OK;
}

Exec HUSB238::GetPdSelected(PdSelected& pd)
{
    CHECK_STATUS();
    CHECK_EXEC(ReadRegister(REG_PD_STATUS0, ctx.reg.status0.val));
    pd.voltage = ctx.reg.status0.bit.PD_VOLTAGE;
    pd.current = ctx.reg.status0.bit.PD_CURRENT;
    pd.wattage = pdVoltageFloat[(uint8)pd.voltage] * pdCurrentFloat[(uint8)pd.current];
    pd.valid = true;
    return Exec::OK;
}

Exec HUSB238::SendCommand(Command cmd)
{
    CHECK_STATUS();
    RegGoCommand regCommand = {};
    regCommand.bit.COMMAND_FUNC = cmd;
    CHECK_EXEC(WriteRegister(REG_GO_COMMAND0, regCommand.val));
    return Exec::OK;
}

Exec HUSB238::RequestPd(PdVoltage pd)
{
    CHECK_STATUS();
    RegSrcPdo regSrcPdo = {};
    RegGoCommand regCommand = {};

    switch (pd) {
        case PdVoltage::PD_5V: regSrcPdo.bit.PDO_SELECT = PDO_5V; break;
        case PdVoltage::PD_9V: regSrcPdo.bit.PDO_SELECT = PDO_9V; break;
        case PdVoltage::PD_12V: regSrcPdo.bit.PDO_SELECT = PDO_12V; break;
        case PdVoltage::PD_15V: regSrcPdo.bit.PDO_SELECT = PDO_15V; break;
        case PdVoltage::PD_18V: regSrcPdo.bit.PDO_SELECT = PDO_18V; break;
        case PdVoltage::PD_20V: regSrcPdo.bit.PDO_SELECT = PDO_20V; break;
        default: { RET_ERR("Invalid option!"); }
    }

    regCommand.bit.COMMAND_FUNC = CMD_REQ_PDO_SELECT;
    CHECK_EXEC(WriteRegister(REG_SRC_PDO, regSrcPdo.val));
    CHECK_EXEC(WriteRegister(REG_GO_COMMAND0, regCommand.val));
    return Exec::OK;
}


const char* HUSB238::GetPdVoltageStr(husb238::PdVoltage voltage)
{
    return pdVoltageStr[(uint8)voltage];
}

const char* HUSB238::GetPdCurrentStr(husb238::PdCurrent current)
{
    return pdCurrentStr[(uint8)current];
}

} // End of namespace "drv"
