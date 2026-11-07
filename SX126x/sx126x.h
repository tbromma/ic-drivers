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

#pragma once
#include "common.h"
//#include "sx126xTypes.h"

/******************************************************************************************************************************************
                                                                    API
******************************************************************************************************************************************/
namespace drv {

class SX126x {
public:

    #include "sx126xTypes.h"

    using SpiTxPoll_t = Exec (*)(uint8* txData, uint16 txSize, bool keepSsLow);
    using SpiTxRxPoll_t = Exec (*)(uint8* txSrc, uint16 txSize, uint8* rxDest, uint16 rxSize, bool keepSsLow);
    using SpiStatus_t = Status (*)();

    struct config_t {
        SpiTxPoll_t     SpiTxPoll = nullptr;
        SpiTxRxPoll_t   SpiTxRxPoll = nullptr;
        SpiStatus_t     SpiStatus = nullptr;
        const char*     label_p = nullptr;
    };

	Exec  Setup(const config_t& config);
    Exec  ReadRegister(regAddr_t reagAddr, readReg_t& readReg);
    Exec  WriteRegister(regAddr_t regAddr, uint8 regVal);
    Exec  SetSleep(sleepCfg_t config);
    Exec  SetStandby(stdbyMode_t stdbyMode);
    Exec  SetPacketType(packetType_t packetType);
    Exec  SetRfFrequency(uint32 rfFreqHz);
    Exec  SetTxParams(int8 pwrDbm, rampTime_t rampTime);
    Exec  SetPaConfig(uint8 paDutyCycle, uint8 hpMax, uint8 devSel, uint8 paLut);
    Exec  SetBufferBaseAddress(uint8 txBaseAddr, uint8 rxBaseAddr);
    Exec  SetLoraModParams(loraSf_t loraSf, loraBw_t loraBw, loraCr_t loraCr, loraLowDrOpt_t lowDrOpt);
    Exec  SetPacketParams(uint16 preambleLen, loraHeader_t header, uint8 payloadLen, loraCrc_t crc, loraIq_t iq);
    Exec  SetDioIrqParams(irqMask_t irqEn, irqMask_t irqDio1, irqMask_t irqDio2, irqMask_t irqDio3);
    Exec  SetTxInfinitePreamble();
    Exec  WriteBuffer(uint8 offset, uint8* data, uint8 dataSize);
    Exec  ReadBuffer(uint8 offset, uint8* data, uint8 dataSize);
    Exec  SetTx(uint16 timeoutMs);
    Exec  SetRx(uint16 timeoutMs);
    Exec  GetRxBufferStatus(rxBuffStat_t& buffStat);
    Exec  GetRxPacketStatus(rxPacketStat_t& packetStat);
    Exec  GetIrqStatus(irqMask_t& irqStat);
    Exec  ClearIrqStatus(irqMask_t irqMask);
    Exec  GetRadioStatus(chipStat_t& status);
    Exec  GetDeviceErrors(devErr_t& errors);
    Exec  ClearDeviceErrors();
    Status  GetStatus();

private:

    static constexpr uint32 XTAL_FREQ_Hz = 32000000;
    static constexpr uint8 WRITE_READ_CHECK_VAL = 0xA5;
    static constexpr uint8 REG_LORA_SYNC_WORD_LSB_INIT_VAL = 0x24;
    static constexpr uint8 CMD_BUFF_SIZE = 20;
    static constexpr int8 TX_PWR_MIN_dBm = -9;
    static constexpr int8 TX_PWR_MAX_dBm = 22;

    Exec SendCommand(opCmd_t cmd, uint8* data, uint8 dataSize);

    struct {
        config_t    config;
        Status    status;
        chipStat_t  radioStat;
    } ctx = {};
};

/******************************************************************************************************************************************
                                                                END OF FILE
******************************************************************************************************************************************/
} // End of namespace 'drv::sx126x'