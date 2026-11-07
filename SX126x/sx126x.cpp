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
// CREATION DATE:    November 2023
// DESCRIPTION:      Driver for Semtechs SX126x radio IC

/******************************************************************************************************************************************
                                                                  INCLUDES
******************************************************************************************************************************************/

#include "sx126x.h"
#include "timer.h"
#include "utils.h"

/******************************************************************************************************************************************
                                                              TYPES & DEFINES
******************************************************************************************************************************************/
namespace drv {

#undef CODE_LABEL
#undef CODE_STATUS
#define CODE_LABEL  ctx.config.label_p
#define CODE_STATUS ctx.status

/******************************************************************************************************************************************
                                                         PUBLIC FUNCTION DEFINITIONS
******************************************************************************************************************************************/

Exec SX126x::Setup(const config_t& config)
{
    ASSERT(config.SpiTxPoll != nullptr, "SPI TX DATA FUNC");
    ASSERT(config.SpiTxRxPoll != nullptr, "SPI TX/RX FUNC");
    ASSERT(config.SpiStatus != nullptr, "SPI GET CODE_STATUS FUNC");
    ASSERT(config.SpiStatus() == Status::OK, "SPI BUS");

    ctx.config = config;
    ctx.status = Status ::INIT;

    //-----------------------------------------------------------------------------------
    // Get radio status (chip hardware status)

    for (uint8 retry = 3; retry > 0; retry--) {
        CHECK_EXEC(GetRadioStatus(ctx.radioStat));
        if (ctx.radioStat.bit.chipMode == MODE_STBY_RC) break;
        bareOs::Timer::DelayMs(10);
    }

    //-----------------------------------------------------------------------------------
    // SPI write-read check

    readReg_t readReg = {};
    CHECK_EXEC(WriteRegister(REG_LoRaSyncWordLSB, WRITE_READ_CHECK_VAL));
    CHECK_EXEC(ReadRegister(REG_LoRaSyncWordLSB, readReg));
    ASSERT(readReg.value == WRITE_READ_CHECK_VAL, "REGISTER WRITE-READ CHECK FAILED");

    //-----------------------------------------------------------------------------------
    // Restore default register value after check above

    CHECK_EXEC(WriteRegister(REG_LoRaSyncWordLSB, REG_LORA_SYNC_WORD_LSB_INIT_VAL));
    CHECK_EXEC(ReadRegister(REG_LoRaSyncWordLSB, readReg));
    ASSERT(readReg.value == REG_LORA_SYNC_WORD_LSB_INIT_VAL, "RESTORING DEFAULT REG. VALUE FAILED");

    //-----------------------------------------------------------------------------------

    ctx.status = Status::OK;
    return Exec::OK;
}

Exec SX126x::ReadRegister(regAddr_t regAddr, readReg_t& readReg)
{
    CHECK_STATUS();

    //-----------------------------------------------------------------------------------
    // Read data from register

    uint8 buff[3];
    buff[0] = CMD_READ_REGISTER;
    buff[1] = (uint8)(regAddr >> 8);
    buff[2] = (uint8)regAddr;

    RET_EXEC(ctx.config.SpiTxRxPoll(buff, sizeof(buff), (uint8*)&readReg, sizeof(readReg_t), false));
}

Exec SX126x::WriteRegister(regAddr_t regAddr, uint8 regVal)
{
    CHECK_STATUS();

    //-----------------------------------------------------------------------------------
    // Write data register

    uint8 buff[4];
    buff[0] = CMD_WRITE_REGISTER;
    buff[1] = (uint8)(regAddr >> 8);
    buff[2] = (uint8)regAddr;
    buff[3] = regVal;
    RET_EXEC(ctx.config.SpiTxPoll(buff, sizeof(buff), false));
}

Exec SX126x::SetSleep(sleepCfg_t config)
{
    CHECK_EXEC(SendCommand(CMD_SET_SLEEP, &config.val, 1));
    bareOs::Timer::DelayMs(1);
    return Exec::OK;
}

Exec SX126x::SetStandby(stdbyMode_t stdbyMode)
{
    CHECK_EXEC(SendCommand(CMD_SET_STANDBY, (uint8*)&stdbyMode, 1));
    return Exec::OK;
}

Exec SX126x::SetPacketType(packetType_t packetType)
{
    CHECK_EXEC(SendCommand(CMD_SET_PACKET_TYPE, (uint8*)&packetType, 1));
    return Exec::OK;
}

Exec SX126x::SetRfFrequency(uint32 rfFreqHz)
{
    uint32 freqRegValRot;
    uint32 freqRegVal = (uint32)(((uint64)rfFreqHz * utils::Exp(2, 25)) / XTAL_FREQ_Hz);
    CHECK_EXEC(utils::RotateBytewise((uint8*)&freqRegVal, sizeof(uint32), (uint8*)&freqRegValRot));
    CHECK_EXEC(SendCommand(CMD_SET_RF_FREQUENCY, (uint8*)&freqRegValRot, sizeof(uint32)));
    return Exec::OK;
}

Exec SX126x::SetTxParams(int8 pwrDbm, rampTime_t rampTime)
{
    ASSERT((pwrDbm >= TX_PWR_MIN_dBm) && (pwrDbm <= TX_PWR_MAX_dBm), "TX POWER OUT OF RANGE");
    int8 params[] = {pwrDbm, (int8)rampTime};
    CHECK_EXEC(SendCommand(CMD_SET_TXPARAMS, (uint8*)params, sizeof(params)));
    return Exec::OK;
}

Exec SX126x::SetPaConfig(uint8 paDutyCycle, uint8 hpMax, uint8 devSel, uint8 paLut)
{
    uint8 params[] = {paDutyCycle, hpMax, devSel, paLut};
    CHECK_EXEC(SendCommand(CMD_SET_PACONFIG, params, sizeof(params)));
    return Exec::OK;
}

Exec SX126x::SetBufferBaseAddress(uint8 txBaseAddr, uint8 rxBaseAddr)
{
    uint8 params[] = {txBaseAddr, rxBaseAddr};
    CHECK_EXEC(SendCommand(CMD_SET_BUFFER_BASE_ADDR, params, sizeof(params)));
    return Exec::OK;
}

Exec SX126x::SetLoraModParams(loraSf_t loraSf, loraBw_t loraBw, loraCr_t loraCr, loraLowDrOpt_t lowDrOpt)
{
    uint8 params[] = {(uint8)loraSf, (uint8)loraBw, (uint8)loraCr, lowDrOpt};
    CHECK_EXEC(SendCommand(CMD_SET_MODULATIONPARAMS, params, sizeof(params)));

    //-----------------------------------------------------------------------------------
    // Errata workaround for BW = 500kHz

    readReg_t reg;
    CHECK_EXEC(ReadRegister(REG_TxModulation, reg));

    if (loraBw == LORA_BW_500) {
        reg.value &= 0xFB;
        CHECK_EXEC(WriteRegister(REG_TxModulation, reg.value));
    }
    else {
        reg.value |= 0x04;
        CHECK_EXEC(WriteRegister(REG_TxModulation, reg.value));
    }

    //-----------------------------------------------------------------------------------

    return Exec::OK;
}

Exec SX126x::SetPacketParams(uint16 preambleLen, loraHeader_t header, uint8 payloadLen, loraCrc_t crc, loraIq_t iq)
{
    uint8 params[] = {(uint8)(preambleLen >> 8), (uint8)preambleLen, header, payloadLen, crc, iq};
    CHECK_EXEC(SendCommand(CMD_SET_PACKETPARAMS, params, sizeof(params)));
    return Exec::OK;
}

Exec SX126x::SetDioIrqParams(irqMask_t irqEn, irqMask_t irqDio1, irqMask_t irqDio2, irqMask_t irqDio3)
{
    uint8 params[] = {  (uint8)(irqEn.val >> 8), (uint8)irqEn.val,
                        (uint8)(irqDio1.val >> 8), (uint8)irqDio1.val,
                        (uint8)(irqDio2.val >> 8), (uint8)irqDio2.val,
                        (uint8)(irqDio3.val >> 8), (uint8)irqDio3.val   };

    CHECK_EXEC(SendCommand(CMD_CFG_DIOIRQ, (uint8*)params, sizeof(params)));
    return Exec::OK;
}

Exec SX126x::SetTxInfinitePreamble()
{
    RET_EXEC(SendCommand(CMD_SET_TXCONTINUOUSPREAMBLE, nullptr, 0));
}

Exec SX126x::WriteBuffer(uint8 offset, uint8* data_p, uint8 dataSize)
{
    uint8 params[] = {CMD_WRITE_BUFFER, offset};
    CHECK_EXEC(ctx.config.SpiTxPoll(params, sizeof(params), true));
    RET_EXEC(ctx.config.SpiTxPoll(data_p, dataSize, false));
}

Exec SX126x::ReadBuffer(uint8 offset, uint8* data_p, uint8 dataSize)
{
    uint8 params[] = {CMD_READ_BUFFER, offset, 0};
    RET_EXEC(ctx.config.SpiTxRxPoll(params, sizeof(params), data_p, dataSize, false));
}

Exec SX126x::SetTx(uint16 timeoutMs)
{
    uint32 regValTimeout;

    if (timeoutMs == 0)                 regValTimeout = 0;
    else if (timeoutMs == UINT16_MAX)   regValTimeout = 0xFFFFFF;
    else {
        regValTimeout = (uint32)(((uint64)timeoutMs * 1000000) / 15625);
        CHECK_EXEC(utils::RotateBytewise((uint8*)&regValTimeout, 3, (uint8*)&regValTimeout));
    }

    RET_EXEC(SendCommand(CMD_SET_TX, (uint8*)&regValTimeout, 3));
}

Exec SX126x::SetRx(uint16 timeoutMs)
{
    uint32 regValTimeout;

    if (timeoutMs == 0)                 regValTimeout = 0;
    else if (timeoutMs == UINT16_MAX)   regValTimeout = 0xFFFFFF;
    else {
        regValTimeout = (uint32)(((uint64)timeoutMs * 1000000) / 15625);
        CHECK_EXEC(utils::RotateBytewise((uint8*)&regValTimeout, 3, (uint8*)&regValTimeout));
    }

    RET_EXEC(SendCommand(CMD_SET_RX, (uint8*)&regValTimeout, 3));
}

Exec SX126x::GetRxBufferStatus(rxBuffStat_t& buffStat)
{
    uint8 params[] = {CMD_GET_RXBUFFERSTATUS};
    RET_EXEC(ctx.config.SpiTxRxPoll(params, sizeof(params), (uint8*)&buffStat, sizeof(rxBuffStat_t), false));
}

Exec SX126x::GetRxPacketStatus(rxPacketStat_t& packetStat)
{
    uint8 params[] = {CMD_GET_PACKETSTATUS};
    RET_EXEC(ctx.config.SpiTxRxPoll(params, sizeof(params), (uint8*)&packetStat, sizeof(rxPacketStat_t), false));
}

Exec SX126x::GetIrqStatus(irqMask_t& irqStat)
{
    uint8 params[] = {CMD_GET_IRQSTATUS, 0};
    CHECK_EXEC(ctx.config.SpiTxRxPoll(params, sizeof(params), (uint8*)&irqStat.val, sizeof(irqMask_t), false));
    CHECK_EXEC(utils::RotateBytewise((uint8*)&irqStat.val, sizeof(uint16), (uint8*)&irqStat.val));
    return Exec::OK;
}

Exec SX126x::ClearIrqStatus(irqMask_t irqMask)
{
    uint8 params[] = {(uint8)(irqMask.val >> 8), (uint8)irqMask.val};
    CHECK_EXEC(SendCommand(CMD_CLR_IRQSTATUS, (uint8*)params, sizeof(params)));
    return Exec::OK;
}

Exec SX126x::GetRadioStatus(chipStat_t& status)
{
    uint8 params[] = {CMD_GET_STATUS};
    RET_EXEC(ctx.config.SpiTxRxPoll(params, sizeof(params), &status.val, sizeof(chipStat_t), false));
}

Exec SX126x::GetDeviceErrors(devErr_t& errors)
{
    uint8 params[] = {CMD_GET_DEV_ERRORS, 0};
    CHECK_EXEC(ctx.config.SpiTxRxPoll(params, sizeof(params), (uint8*)&errors.val, sizeof(devErr_t), false));
    CHECK_EXEC(utils::RotateBytewise((uint8*)&errors.val, sizeof(uint16), (uint8*)&errors.val));
    return Exec::OK;
}

Exec SX126x::ClearDeviceErrors()
{
    RET_EXEC(SendCommand(CMD_CLR_DEV_ERRORS, nullptr, 0));
}

Status SX126x::GetStatus()
{
    return ctx.status;
}

/******************************************************************************************************************************************
                                                         PRIVATE FUNCTION DEFNITIONS
******************************************************************************************************************************************/

Exec SX126x::SendCommand(opCmd_t cmd, uint8* data, uint8 dataSize)
{
    ASSERT((sizeof(opCmd_t) + dataSize) <= CMD_BUFF_SIZE, "CMD DATA SIZE BIGGER THAN BUFF");
    uint8 buff[CMD_BUFF_SIZE] = {};
    buff[0] = cmd;
    memcpy(&buff[1], data, dataSize);
    RET_EXEC(ctx.config.SpiTxPoll(buff, sizeof(opCmd_t) + dataSize, false));
}

/******************************************************************************************************************************************
                                                                END OF FILE
******************************************************************************************************************************************/
} // End of namespace 'drv::sx126x'