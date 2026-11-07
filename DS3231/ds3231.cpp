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
// DESCRIPTION:      Driver for the DS3231 RTC IC

/******************************************************************************************************************************************
                                                                  INCLUDES
******************************************************************************************************************************************/

#include "ds3231.h"

/******************************************************************************************************************************************
                                                              TYPES & DEFINES
******************************************************************************************************************************************/

#undef CODE_LABEL
#undef CODE_STATUS
#define CODE_LABEL  ctx.config.label_p
#define CODE_STATUS ctx.status

/******************************************************************************************************************************************
                                                         PUBLIC FUNCTION DEFINITIONS
******************************************************************************************************************************************/
namespace drv {
using namespace ds3231;

Exec DS3231::Setup(const config_t& config)
{
    //-----------------------------------------------------------------------------------
    // Check config

    ASSERT(config.I2cTxPoll != nullptr, "I2C TX DATA FUNC.");
    ASSERT(config.I2cTxRxPoll != nullptr, "I2C TXRTX DATA FUNC.")
    ASSERT(config.I2cGetStatus != nullptr, "I2C GET CODE_STATUS FUNC.");
    ASSERT(config.I2cGetStatus() == Status::OK, "I2C BUS INVALID CODE_STATUS");
    ASSERT(config.label_p != nullptr, "CODE_LABEL MISSING");

    ctx.config = config;
    ctx.status = Status ::INIT;

    //-----------------------------------------------------------------------------------
    // Check control register read value

    regCtrl_t regCtrl = {};
    CHECK_EXEC(GetCtrlReg(regCtrl));
    ASSERT(regCtrl.val == CTRL_REG_RESET_VAL, "CONTROL REG READ CHECK MISMATCH");

    //-----------------------------------------------------------------------------------
    // Setup status register

    regStat_t regStat = {};
    regStat.bit.OSF = 0;
    regStat.bit.EN32kHz = 1;
    CHECK_EXEC(SetStatusReg(regStat));

    //-----------------------------------------------------------------------------------

    ctx.status = Status::OK;
	return Exec::OK;
}

Exec DS3231::ReadRegister(regAddr_t regAddr, uint8& regVal)
{
    CHECK_STATUS();
    RET_EXEC(ctx.config.I2cTxRxPoll(ctx.config.i2cAddr, (uint8*)&regAddr, sizeof(regAddr_t), &regVal, sizeof(uint8)));
}

Exec DS3231::WriteRegister(regAddr_t regAddr, uint8 regVal)
{
    CHECK_STATUS();
    uint8 txData[2];
    txData[0] = regAddr;
    txData[1] = regVal;
    RET_EXEC(ctx.config.I2cTxPoll(ctx.config.i2cAddr, txData, sizeof(txData)));
}

Exec DS3231::ReadMultipleRegs(regAddr_t regStart, uint8* regData_p, uint8 byteCnt)
{
    ASSERT(regData_p != nullptr, "VARIABLE 'regData_p'");
    ASSERT(byteCnt <= DS3231_REG_CNT, "BYTE COUNT TO READ");
    ASSERT(((uint8)regStart + (byteCnt - 1)) <= REG_TEMP_LSB, "LAST READ ADDRESS");
    RET_EXEC(ctx.config.I2cTxRxPoll(ctx.config.i2cAddr, (uint8*)&regStart, sizeof(regAddr_t), regData_p, byteCnt));
}

Exec DS3231::WriteMultipleRegs(regAddr_t regStart, uint8* regData_p, uint8 byteCnt)
{
    ASSERT(regData_p != nullptr, "VARIABLE 'regData_p'");
    ASSERT(byteCnt <= DS3231_REG_CNT, "BYTE COUNT TO WRITE");
    ASSERT(((uint8)regStart + (byteCnt - 1)) <= REG_TEMP_LSB, "LAST WRITE ADDRESS");

    uint8 txData[DS3231_REG_CNT + 1] = {};
    txData[0] = (uint8)regStart;
    memcpy(&txData[1], regData_p, byteCnt);
    RET_EXEC(ctx.config.I2cTxPoll(ctx.config.i2cAddr, txData, sizeof(regAddr_t) + byteCnt));
}

Exec DS3231::GetCtrlReg(regCtrl_t& regCtrl)
{
    CHECK_STATUS();
    CHECK_EXEC(ReadRegister(REG_CONTROL, regCtrl.val));
    return Exec::OK;
}

Exec DS3231::SetCtrlReg(regCtrl_t regCtrl)
{
    CHECK_STATUS();
    CHECK_EXEC(WriteRegister(REG_CONTROL, regCtrl.val));
    return Exec::OK;
}

Exec DS3231::GetStatusReg(regStat_t& regStat)
{
    CHECK_STATUS();
    CHECK_EXEC(ReadRegister(REG_STATUS, regStat.val));
    return Exec::OK;
}

Exec DS3231::SetStatusReg(regStat_t regStat)
{
    CHECK_STATUS();
    RET_EXEC(WriteRegister(REG_STATUS, regStat.val));
}

void DS3231::ConvTempBinToDec(uint16 bin, bareOs::units::uniVal_t& temp)
{
    temp.value = (int16)((((int16)bin / _6BIT_MAX_VAL) * 25) / 10);     // Resolution is 0.25*C per bit
    temp.commaPos = bareOs::units::COMMA_POS_1;                         // Result has 1 comma resolution -> 23.1*C
    temp.prefix = bareOs::units::PREFIX_none;
    temp.unit = bareOs::units::UNIT_TEMPERATURE_C;
}

Status DS3231::GetStatus()
{
    return ctx.status;
}

/******************************************************************************************************************************************
                                                                END OF FILE
******************************************************************************************************************************************/
} // END OF NAMESPACE 'drv::ds3231'