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
// CREATION DATE:    March 2024
// DESCRIPTION:      FTDI BT81x display driver

/******************************************************************************************************************************************
                                                                  INCLUDES
******************************************************************************************************************************************/

#include "bt81x.h"
#include "utils.h"

/******************************************************************************************************************************************
                                                              TYPES & DEFINES
******************************************************************************************************************************************/
namespace drv::bt81x {

#undef ACTION_ON_ERR
#undef CODE_LABEL
#undef CODE_STATUS
#define ACTION_ON_ERR   ctx.status = Status::ERR
#define CODE_LABEL      ctx.config.label_p
#define CODE_STATUS     ctx.status

/******************************************************************************************************************************************
                                                         PUBLIC FUNCTION DEFINITIONS
******************************************************************************************************************************************/

Exec BT81x::Setup(const config_t& config)
{
    ASSERT(config.SpiTxPoll != nullptr, "SPI TX POLL FUNC");
    ASSERT(config.SpiTxClbk != nullptr, "SPI TX CLBK FUNC");
    ASSERT(config.SpiTxRxPoll != nullptr, "SPI TX/RX POLL FUNC");
    ASSERT(config.SpiTxRxClbk != nullptr, "SPI TX/RX CLBK FUNC");
    ASSERT(config.SpiStatus != nullptr, "SPI GET CODE_STATUS FUNC");
    ASSERT(config.SpiStatus() == Status::OK, "SPI BUS");
    ctx.config = config;
    ctx.status = Status::OK;
    return Exec::OK;
}

Exec BT81x::ReadReg(regAddr_t regAddr, regWidth_t regSize, uint8* regVal_p)
{
    CHECK_STATUS();
    uint8 txData[] = {(uint8)(regAddr >> 16), (uint8)(regAddr >> 8), (uint8)regAddr, 0xFF}; // 0xFF -> Dummy byte
    RET_EXEC(ctx.config.SpiTxRxPoll(txData, sizeof(txData), regVal_p, regSize, false));
}

Exec BT81x::WriteReg(regAddr_t regAddr, regWidth_t regSize, uint32 data)
{
    CHECK_STATUS();
    uint8 txData[7];
    uint8 byteNum = 0;
    txData[byteNum++] = 0x80 | (uint8)(regAddr >> 16);
    txData[byteNum++] = (uint8)(regAddr >> 8);
    txData[byteNum++] = (uint8)regAddr;
    memcpy(&txData[byteNum], (uint8*)&data, regSize);
    RET_EXEC(ctx.config.SpiTxPoll(txData, 3 + regSize, false));
}

Exec BT81x::ReadMem(uint32 addr, uint8* data_p, uint16 size, Clbk_t Clbk)
{
    CHECK_STATUS();
    utils::RotateBytewise((uint8*)&addr, MEM_ADDR_SIZE_RD, (uint8*)&addr);
    RET_EXEC(ctx.config.SpiTxRxClbk((uint8*)&addr, MEM_ADDR_SIZE_RD, data_p, size, false, Clbk));
}

Exec BT81x::WriteMemPoll(uint32 addr, uint8* data_p, uint16 size)
{
    CHECK_STATUS();
    addr |= (uint32)1 << 23;    // Set address bit #24 to 1 to indicate write operation
    utils::RotateBytewise((uint8*)&addr, MEM_ADDR_SIZE_WR, (uint8*)&addr);
    CHECK_EXEC(ctx.config.SpiTxPoll((uint8*)&addr, MEM_ADDR_SIZE_WR, true));
    RET_EXEC(ctx.config.SpiTxPoll(data_p, size, false));
}

Exec BT81x::WriteMemClbk(uint32 addr, uint8* data_p, uint16 size, Clbk_t Clbk)
{
    CHECK_STATUS();
    addr |= (uint32)1 << 23;    // Set address bit #24 to 1 to indicate write operation
    utils::RotateBytewise((uint8*)&addr, MEM_ADDR_SIZE_WR, (uint8*)&addr);
    CHECK_EXEC(ctx.config.SpiTxPoll((uint8*)&addr, MEM_ADDR_SIZE_WR, true));
    RET_EXEC(ctx.config.SpiTxClbk(data_p, size, false, Clbk));
}

Exec BT81x::ReadCmdWrite(uint16& ramPos)
{
    CHECK_STATUS();
    RET_EXEC(ReadReg(REG_CMD_WRITE, REG_16BIT, (uint8*)&ramPos));
}

Exec BT81x::WriteCmdWrite(uint16 ramPos)
{
    CHECK_STATUS();
    RET_EXEC(WriteReg(REG_CMD_WRITE, REG_16BIT, (uint32)ramPos));
}

Exec BT81x::ReadCmdRead(uint16& ramPos)
{
    CHECK_STATUS();
    RET_EXEC(ReadReg(REG_CMD_READ, REG_16BIT, (uint8*)&ramPos));
}

Exec BT81x::SendSysCmd(systemCmd_t cmd, uint8 param)
{
    CHECK_STATUS();
    uint8 txData[] = {cmd, param, 0}; // 0 -> Dummy byte
    RET_EXEC(ctx.config.SpiTxPoll(txData, sizeof(txData), false));
}

Exec BT81x::SetBacklight(uint8 lumination)
{
    CHECK_STATUS();
    if (lumination > BL_MAX_LUM_VAL) {
        REPORT_WAR("BACKLIGHT LUMINATION MAX %d", BL_MAX_LUM_VAL);
        lumination = BL_MAX_LUM_VAL;
    }
    RET_EXEC(WriteReg(REG_PWM_DUTY, REG_8BIT, lumination));
}

Exec BT81x::SoftwareReset()
{
    RET_EXEC(SendSysCmd(SYSCMD_PWRDOWN, 0));
}

Status BT81x::GetStatus()
{
    return ctx.status;
}

/******************************************************************************************************************************************
                                                         PRIVATE FUNCTION DEFINITIONS
******************************************************************************************************************************************/

void BT81x::SpiDoneClbkDummy()
{
    return;
}

} // END OF NAMESPACE 'drv::bt81x'
/******************************************************************************************************************************************
                                                                END OF FILE
******************************************************************************************************************************************/
