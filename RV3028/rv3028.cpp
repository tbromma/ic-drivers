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
// CREATION DATE:    September 2025
// DESCRIPTION:      Driver for the RV3028 RTC IC

/******************************************************************************************************************************************
                                                                  INCLUDES
******************************************************************************************************************************************/

#include "rv3028.h"

/******************************************************************************************************************************************
                                                              TYPES & DEFINES
******************************************************************************************************************************************/

#undef CODE_LABEL
#undef CODE_STATUS
#define CODE_LABEL      ctx.config.label_p
#define CODE_STATUS     ctx.status

/******************************************************************************************************************************************
                                                         PUBLIC FUNCTION DEFINITIONS
******************************************************************************************************************************************/
namespace drv {
using namespace rv3028;

Exec RV3028::Setup(const config_t& config)
{
    //-----------------------------------------------------------------------------------
    // Check config

    ASSERT(config.I2cTxPoll != nullptr, "'I2cTxPoll' function not configured!");
    ASSERT(config.I2cTxRxPoll != nullptr, "'I2cTxRxPoll' function not configured!")
    ASSERT(config.I2cGetStatus != nullptr, "'I2cGetStatus' function not configured!");
    ASSERT(config.I2cGetStatus() == Status::OK, "I2C bus status invalid!");
    ASSERT(config.label_p != nullptr, "CODE_LABEL MISSING");

    ctx.config = config;
    ctx.status = Status ::INIT;

    //-----------------------------------------------------------------------------------
    // Check I2C communication

    constexpr uint8 TEST_VALUE = 0b01010101;

    RegGpBits regGpRead;
    RegGpBits regGpTest = {.val = TEST_VALUE};
    CHECK_EXEC(ReadRegister(RegAddr::REG_GP_FLAGS, regGpRead.val));
    CHECK_EXEC(WriteRegister(RegAddr::REG_GP_FLAGS, regGpTest.val));
    regGpTest.val = 0;
    CHECK_EXEC(ReadRegister(RegAddr::REG_GP_FLAGS, regGpTest.val));
    ASSERT(regGpTest.val == TEST_VALUE, "I2C write/read test failed!");

    // Recover previous register value
    WriteRegister(RegAddr::REG_GP_FLAGS, regGpRead.val);

    //-----------------------------------------------------------------------------------

    ctx.status = Status::OK;
	return Exec::OK;
}

Exec RV3028::ReadRegister(RegAddr regAddr, uint8& regVal)
{
    CHECK_STATUS();
    RET_EXEC(ctx.config.I2cTxRxPoll(ctx.config.i2cAddr, (uint8*)&regAddr, sizeof(RegAddr), &regVal, sizeof(uint8)));
}

Exec RV3028::WriteRegister(RegAddr regAddr, uint8 regVal)
{
    CHECK_STATUS();
    uint8 txData[2];
    txData[0] = (uint8)regAddr;
    txData[1] = regVal;
    RET_EXEC(ctx.config.I2cTxPoll(ctx.config.i2cAddr, txData, sizeof(txData)));
}

Exec RV3028::ReadMultipleRegs(RegAddr startAddr, uint8* regData_p, uint8 byteCnt)
{
    ASSERT(regData_p != nullptr, "Variable 'regData_p' is nullpointer!");
    ASSERT(((uint8)startAddr + (byteCnt - 1)) <= (uint8)RegAddr::REG_EEPROM_BACKUP, "Register read address out of range!");
    RET_EXEC(ctx.config.I2cTxRxPoll(ctx.config.i2cAddr, (uint8*)&startAddr, sizeof(RegAddr), regData_p, byteCnt));
}

Exec RV3028::WriteMultipleRegs(RegAddr startAddr, uint8* regData_p, uint8 byteCnt)
{
    ASSERT(regData_p != nullptr, "Variable 'regData_p' is nullpointer!");
    ASSERT(((uint8)startAddr + (byteCnt - 1)) <= (uint8)RegAddr::REG_EEPROM_BACKUP, "Register read address out of range!");

    uint8 txData[REGISTER_CNT + 1] = {};
    txData[0] = (uint8)startAddr;
    memcpy(&txData[1], regData_p, byteCnt);
    RET_EXEC(ctx.config.I2cTxPoll(ctx.config.i2cAddr, txData, sizeof(RegAddr) + byteCnt));
}

Status RV3028::GetStatus()
{
    return ctx.status;
}

/******************************************************************************************************************************************
                                                                END OF FILE
******************************************************************************************************************************************/
} // END OF NAMESPACE 'drv::ds3231'