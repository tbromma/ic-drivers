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
// CREATION DATE:   July 2024
// DESCRIPTION:     SD Card driver

/******************************************************************************************************************************************
                                                                  INCLUDES
******************************************************************************************************************************************/

#include "sdCard.h"
#include "utils.h"
#include "timer.h"

/******************************************************************************************************************************************
                                                         PRIVATE TYPES & VARIABLES
******************************************************************************************************************************************/
namespace drv {

#undef CODE_LABEL
#undef CODE_STATUS
#undef ACTION_ON_ERR
#define CODE_LABEL      ctx.config.label_p
#define CODE_STATUS     ctx.status
#define ACTION_ON_ERR   ctx.status = Status::ERR;

/******************************************************************************************************************************************
                                                         PUBLIC FUNCTION DEFINITIONS
******************************************************************************************************************************************/

Exec SDCARD::Setup(const config_t& config)
{
    ctx.status = Status ::INIT;
    ctx.config = config;

    ASSERT(ctx.config.SpiTxPoll != nullptr, "SPI TX (POLL) DATA FUNC");
    ASSERT(ctx.config.SpiRxPoll != nullptr, "SPI RX (POLL) DATA FUNC");
    ASSERT(ctx.config.SpiTxClbk != nullptr, "SPI TX (CLBK) DATA FUNC");
    ASSERT(ctx.config.SpiRxClbk != nullptr, "SPI RX (CLBK) DATA FUNC");
    ASSERT(ctx.config.SpiCsSet != nullptr, "SPI CS SET FUNC");
    ASSERT(ctx.config.SpiCsClear != nullptr, "SPI CS CLEAR FUNC");
    ASSERT(ctx.config.SpiStatus != nullptr, "SPI GET CODE_STATUS FUNC");
    ASSERT(ctx.config.SpiStatus() == Status::OK, "SPI BUS");

    ctx.status = Status::OK;
    return Exec::OK;
}

Exec SDCARD::InitBus()
{
    ctx.config.SpiCsClear();
    uint8 txDummy[10];
    memset(txDummy, 0xFF, sizeof(txDummy));
    RET_EXEC(ctx.config.SpiTxPoll(txDummy, sizeof(txDummy)));
}

Exec SDCARD::SendCmd(SdCmd cmd, uint32 arg, bool keepSsLow, respR1_t& resp)
{
    //-----------------------------------------------------------------------------------
    // Sending command

    uint8 crc = 0xFF;

    if (cmd == SdCmd::CMD0)         crc = CRC_CMD0;
    else if (cmd == SdCmd::CMD8)    crc = CRC_CMD8;
    else if (cmd == SdCmd::CMD9)    crc = CRC_CMD9;

    crc = (crc << 1) | 1;
    cmd = (SdCmd)((uint8)cmd | (1 << 6));

    ctx.config.SpiCsSet();
    uint8 txBuff[] = {(uint8)cmd, (uint8)(arg >> 24), (uint8)(arg >> 16), (uint8)(arg >> 8), (uint8)(arg), crc,};
    EXEC_ON_ERR(ctx.config.SpiTxPoll(txBuff, sizeof(txBuff)), ctx.config.SpiCsClear());

    //-----------------------------------------------------------------------------------
    // Reading R1 response

    for (uint8 cnt = 0; cnt < R1_RETRY_CNT; cnt++) {
        EXEC_ON_ERR(ctx.config.SpiRxPoll(&resp.val, sizeof(uint8)), ctx.config.SpiCsClear());
        if (resp.bit.ZERO == false) {
            if (keepSsLow == false) ctx.config.SpiCsClear();
            return Exec::OK;
        }
    }

    //-----------------------------------------------------------------------------------

    ctx.config.SpiCsClear();
    return Exec::ERR;
}

Exec SDCARD::SendCmd(SdCmd cmd, uint32 arg, bool keepSsLow, uint8 expResp, respR1_t& resp)
{
    //-----------------------------------------------------------------------------------
    // Sending command

    uint8 crc = 0xFF;

    if (cmd == SdCmd::CMD0)         crc = CRC_CMD0;
    else if (cmd == SdCmd::CMD8)    crc = CRC_CMD8;

    crc = (crc << 1) | 1;
    cmd = (SdCmd)((uint8)cmd | (1 << 6));

    ctx.config.SpiCsSet();
    uint8 txBuff[] = {(uint8)cmd, (uint8)(arg >> 24), (uint8)(arg >> 16), (uint8)(arg >> 8), (uint8)(arg), crc,};
    EXEC_ON_ERR(ctx.config.SpiTxPoll(txBuff, sizeof(txBuff)), ctx.config.SpiCsClear());

    //-----------------------------------------------------------------------------------
    // Reading and checking R1 response

    for (uint8 cnt = 0; cnt < R1_RETRY_CNT; cnt++) {
        EXEC_ON_ERR(ctx.config.SpiRxPoll(&resp.val, sizeof(uint8)), ctx.config.SpiCsClear());
        if (resp.bit.ZERO == 0) {
            if (resp.val == expResp) {
                if (keepSsLow == false) ctx.config.SpiCsClear();
                return Exec::OK;
            }
            if (resp.bit.ILL_CMD        ||
                resp.bit.CRC_ERR        ||
                resp.bit.ERASE_SEQ_ERR  ||
                resp.bit.ADDR_ERR       ||
                resp.bit.PARAM_ERR) break;
        }
    }

    //-----------------------------------------------------------------------------------

    ctx.config.SpiCsClear();
    return Exec::OK;
}

Exec SDCARD::ReadResponseR1(respR1_t& resp, bool keepSsLow)
{
    for (uint8 cnt = 0; cnt < R1_RETRY_CNT; cnt++) {
        EXEC_ON_ERR(ctx.config.SpiRxPoll(&resp.val, sizeof(uint8)), ctx.config.SpiCsClear());
        if (resp.bit.ZERO == false) {
            if (keepSsLow == false) ctx.config.SpiCsClear();
            return Exec::OK;
        }
    }
    ctx.config.SpiCsClear();
    return Exec::OK;
}

Exec SDCARD::ReadResponseR7(respR1_t& r1, respR7_t& r7, bool keepSsLow)
{
    for (uint8 cnt = 0; cnt < R1_RETRY_CNT; cnt++) {
        EXEC_ON_ERR(ctx.config.SpiRxPoll(&r1.val, sizeof(uint8)), ctx.config.SpiCsClear());
        if (r1.bit.ZERO == false) {
            EXEC_ON_ERR(ctx.config.SpiRxPoll((uint8*)&r7.val, sizeof(uint32)), ctx.config.SpiCsClear());
            utils::RotateBytewise((uint8*)&r7.val, sizeof(uint32), (uint8*)&r7.val);
            if (keepSsLow == false) ctx.config.SpiCsClear();
            return Exec::OK;
        }
    }
    ctx.config.SpiCsClear();
    return Exec::OK;
}

Exec SDCARD::ReadOcrReg(respR1_t& r1, regOcr_t& ocr)
{
    CHECK_EXEC(SendCmd(drv::SDCARD::SdCmd::CMD58, 0, true, r1));

    if (r1.bit.ZERO == false) {
        EXEC_ON_ERR(ctx.config.SpiRxPoll((uint8*)&ocr.val, sizeof(uint32)), ctx.config.SpiCsClear());
        utils::RotateBytewise((uint8*)&ocr.val, sizeof(uint32), (uint8*)&ocr.val);
    }

    ctx.config.SpiCsClear();
    return Exec::OK;
}

Exec SDCARD::ReadCsdReg(respR1_t& r1, sdCard::RegCSDv2& regCsd)
{
    CHECK_EXEC(SendCmd(drv::SDCARD::SdCmd::CMD9, 0, true, r1));

    if (r1.bit.ZERO == false) {
        uint8 token;
        uint8 retryCnt = 0;
        uint8 rxData[18];
        do {
            if (retryCnt++ == REG_CSD_READ_TOKEN_RETRY_CNT) {
                ctx.config.SpiCsClear();
                return Exec::ERR;
            }
            EXEC_ON_ERR(ctx.config.SpiRxPoll(&token, sizeof(token)), ctx.config.SpiCsClear());
        } while (token != TOKEN_WR_RD);
        EXEC_ON_ERR(ctx.config.SpiRxPoll(rxData, sizeof(rxData)), ctx.config.SpiCsClear());
        utils::RotateBytewise(rxData, sizeof(regCsd.val), regCsd.val);
    }

    ctx.config.SpiCsClear();
    return Exec::OK;
}

Status SDCARD::GetStatus()
{
    return ctx.status;
}

Exec SDCARD::ReadSector(uint32 sectorNum, respR1_t& respR1, uint16& crc16, uint8* buff_p, Clbk_t Clbk)
{
    ASSERT(ctx.ClbkEvent == nullptr, "WAITING FOR CALLBACK, RISK OF OVERRUN");

    uint8 response = 0;
    uint16 tokenRetry;

    //-----------------------------------------------------------------------------------
    // If card busy, wait till idle

    tokenRetry = TOKEN_RETRY_CNT;
    ctx.config.SpiCsSet();
    while (1) {
        EXEC_ON_ERR(ctx.config.SpiRxPoll(&response, sizeof(response)), ctx.config.SpiCsClear());
        if (response == TOKEN_CARD_IDLE) break;
        if (--tokenRetry == 0) {
            ctx.config.SpiCsClear();
            RET_ERR("Card idle token retry cnt exceeded!");
        }
    }
    ctx.config.SpiCsClear();

    //-----------------------------------------------------------------------------------
    // Send write sector command and wait for token

    tokenRetry = TOKEN_RETRY_CNT;
    CHECK_EXEC(SendCmd(SdCmd::CMD17, sectorNum, true, respR1));

    if (respR1.val == 0x00) {
        do {
            EXEC_ON_ERR(ctx.config.SpiRxPoll(&response, sizeof(response)), ctx.config.SpiCsClear());
            if (response == TOKEN_WR_RD) goto READ_BLOCK;
        } while (--tokenRetry);
    }

    ctx.config.SpiCsClear();
    return Exec::ERR;

    //-----------------------------------------------------------------------------------

    READ_BLOCK:
    EXEC_ON_ERR(ctx.config.SpiRxClbk(buff_p, SECTOR_SIZE, ClbkDummy), ctx.config.SpiCsClear());
    EXEC_ON_ERR(ctx.config.SpiRxPoll((uint8*)&crc16, CRC16_SIZE), ctx.config.SpiCsClear());
    ctx.config.SpiCsClear();
    return Exec::OK;
}

Exec SDCARD::WriteSector(uint32 sectorNum, uint8* buff_p, respR1_t& respR1, Clbk_t Clbk)
{
    uint16 CRC16dummy = 0xFFFF;
    uint8 response;
    uint16 tokenRetry;

    //-----------------------------------------------------------------------------------
    // If card busy, wait till idle

    tokenRetry = TOKEN_RETRY_CNT;
    ctx.config.SpiCsSet();
    while (1) {
        EXEC_ON_ERR(ctx.config.SpiRxPoll(&response, sizeof(response)), ctx.config.SpiCsClear());
        if (response == TOKEN_CARD_IDLE) break;
        if (--tokenRetry == 0) {
            ctx.config.SpiCsClear();
            RET_ERR("Card idle token retry cnt exceeded!");
        }
    }
    ctx.config.SpiCsClear();

    //-----------------------------------------------------------------------------------
    // Write sector

    CHECK_EXEC(SendCmd(SdCmd::CMD24, sectorNum, true, respR1));
    EXEC_ON_ERR(ctx.config.SpiTxClbk((uint8*)&TOKEN_WR_RD, sizeof(TOKEN_WR_RD), ClbkDummy), ctx.config.SpiCsClear());
    EXEC_ON_ERR(ctx.config.SpiTxClbk(buff_p, SECTOR_SIZE, ClbkDummy), ctx.config.SpiCsClear());
    EXEC_ON_ERR(ctx.config.SpiTxPoll((uint8*)&CRC16dummy, sizeof(CRC16dummy)), ctx.config.SpiCsClear());

    tokenRetry = TOKEN_RETRY_CNT;

    while (1) {
        EXEC_ON_ERR(ctx.config.SpiRxPoll(&response, sizeof(response)), ctx.config.SpiCsClear());
        if ((response & TOKEN_DATA_RESP_MASK) != TOKEN_DATA_RESP) break;
        if (--tokenRetry == 0) {
            ctx.config.SpiCsClear();
            RET_ERR("Data WR token retry cnt exceeded!");
        }
    }

    ctx.config.SpiCsClear();
    return Exec::OK;
}

void SDCARD::ClbkDummy()
{
    return;
}

void SDCARD::ClbkReadSectorDone()
{
    sdCard_p->ctx.config.SpiCsClear();
    if (sdCard_p->ctx.ClbkEvent) {
        sdCard_p->ctx.ClbkEvent();
        sdCard_p->ctx.ClbkEvent = nullptr;
    }
    sdCard_p = nullptr;
}

void SDCARD::ClbkWriteSectorDone()
{
    sdCard_p->ctx.config.SpiCsClear();
    if (sdCard_p->ctx.ClbkEvent) {
        sdCard_p->ctx.ClbkEvent();
        sdCard_p->ctx.ClbkEvent = nullptr;
    }
    sdCard_p = nullptr;
}

} // End of namespace 'drv'
/******************************************************************************************************************************************
                                                                END OF FILE
******************************************************************************************************************************************/
