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

#pragma once
#include "common.h"
#include "sdCardTypes.h"

/******************************************************************************************************************************************
                                                                   TYPES
******************************************************************************************************************************************/
namespace drv {

class SDCARD {
public:

    using Clbk_t = void (*)();
    using SpiTxPoll_t = Exec (*)(uint8* txData, uint16 txSize);
    using SpiRxPoll_t = Exec (*)(uint8* rxData, uint16 rxSize);
    using SpiTxClbk_t = Exec (*)(uint8* txData, uint16 txSize, Clbk_t Clbk);
    using SpiRxClbk_t = Exec (*)(uint8* rxData, uint16 rxSize, Clbk_t Clbk);
    using SpiCsSet_t = void (*)();
    using SpiCsClear_t = void (*)();
    using SpiStatus_t = Status (*)();

    struct config_t {
        SpiTxPoll_t     SpiTxPoll = nullptr;
        SpiRxPoll_t     SpiRxPoll = nullptr;
        SpiTxClbk_t     SpiTxClbk = nullptr;
        SpiRxClbk_t     SpiRxClbk = nullptr;
        SpiCsSet_t      SpiCsSet = nullptr;
        SpiCsClear_t    SpiCsClear = nullptr;
        SpiStatus_t     SpiStatus = nullptr;
        const char*     label_p = nullptr;
    };

    static const uint32 CMD8_ARG = 0x000001AA;
    static const uint32 ACMD41_ARG = 0x40000000;

    enum class CardType : uint8 {
        NONE        = 0,
        VER1        = 1,
        VER2        = 2,
        SDHC_SDXC   = 3,
    };

    enum class SdCmd : uint8 {
        CMD0                    = 0x00,     // Go idle state
        CMD1                    = 0x01,     // CMD1
        CMD8                    = 0x08,     // Interface condition
        CMD9                    = 0x09,     // Read CSD register
        CMD16                   = 0x10,     // Set block length
        CMD17                   = 0x11,     // Read single block
        CMD24                   = 0x18,     // Write single block
        CMD55                   = 0x37,     // Application specific command
        CMD58                   = 0x3A,     // Read OCR register
        ACMD41                  = 0x29,     // Sends host capacity support information and activates the card's initialization process
    };

    union respR1_t {
        struct {
            uint8 IDLE_STATE    : 1;
            uint8 ERASE_RESET   : 1;
            uint8 ILL_CMD       : 1;
            uint8 CRC_ERR       : 1;
            uint8 ERASE_SEQ_ERR : 1;
            uint8 ADDR_ERR      : 1;
            uint8 PARAM_ERR     : 1;
            uint8 ZERO          : 1;        // Should always be set to 0
        } bit;
        uint8 val;
    };

    union respR7_t {
        struct {
            uint32 ECHO          : 8;
            uint32 VOLTAGE       : 4;
            uint32               : 16;
            uint32 CMD_VER       : 4;
        } bit;
        uint32 val;
    };

    union regOcr_t {
        struct {
            uint32              : 15;   // Bit 0-14:    Reserved
            uint32 VOLTAGE      : 9;    // Bit 15-23:   Voltage window
            uint32 S18A         : 1;    // Bit 24:      Switching to 1.8V accepted
            uint32              : 4;    // Bit 24-28:   Reserved
            uint32 UHSII        : 1;    // Bit 29:      UHS-II Card Status
            uint32 CCS          : 1;    // Bit 30:      Card Capacity Status (CCS), valid only if above Bit-31 indicates Ready
                                        //              CCS=0   SDSC card (addressed in 1-byte units); MMC max 2GB
                                        //              CCS=1   SDHC/SDXC card (addressed in 512-byte units); MMC > 2GB
            uint32 PWR_UP       : 1;    // Bit 31:      Card power up status bit (0=Busy, 1=Ready)
        } bit;
        uint32 val;
    };

    Exec Setup(const config_t& config);
    Exec InitBus();
    // Exec SendCmd(SdCmd cmd, uint32 arg, bool keepSsLow);
    Exec SendCmd(SdCmd cmd, uint32 arg, bool keepSsLow, respR1_t& resp);
    Exec SendCmd(SdCmd cmd, uint32 arg, bool keepSsLow, uint8 expResp, respR1_t& resp);
    Exec ReadResponseR1(respR1_t& resp, bool keepSsLow);
    Exec ReadResponseR7(respR1_t& r1, respR7_t& r7, bool keepSsLow);
    Exec ReadOcrReg(respR1_t& r1, regOcr_t& ocr);
    Exec ReadCsdReg(respR1_t& r1, sdCard::RegCSDv2& regCsd);
    Exec ReadSector(uint32 sectorNum, respR1_t& respR1, uint16& crc16, uint8* buff, Clbk_t Clbk);
    Exec WriteSector(uint32 sectorNum, uint8* buff, respR1_t& respR1, Clbk_t Clbk);
    Exec SpiRead(uint8* buff, uint16 size, bool keepSsLow);
    Status GetStatus();

private:

    static constexpr uint16 SECTOR_SIZE = 512;
    static constexpr uint8 CRC_CMD0 = 0x4A;
    static constexpr uint8 CRC_CMD8 = 0x43;
    static constexpr uint8 CRC_CMD9 = 0xFF;
    static constexpr uint8 R1_RETRY_CNT = 10;
    static constexpr uint8 TOKEN_WR_RD = 0xFE;
    static constexpr uint8 TOKEN_DATA_RESP = 0x05;
    static constexpr uint8 TOKEN_DATA_RESP_MASK = 0x1F;
    static constexpr uint8 TOKEN_CARD_BUSY = 0x00;
    static constexpr uint8 TOKEN_CARD_IDLE = 0xFF;
    static constexpr uint16 TOKEN_RETRY_CNT = 60000;
    static constexpr uint8 REG_CSD_READ_TOKEN_RETRY_CNT = 10;
    static constexpr uint8 CRC16_SIZE = 2;

    struct {
        config_t config;
        Status status = Status::NONE;
        Clbk_t ClbkEvent = nullptr;
        uint8* rxBuff_p;
        uint16* crc16_p;
    } ctx = {};

    static inline SDCARD* sdCard_p = nullptr;
    static void ClbkDummy();
    static void ClbkReadSectorDone();
    static void ClbkWriteSectorDone();
};

/******************************************************************************************************************************************
                                                                END OF FILE
******************************************************************************************************************************************/
} // End of namespace 'drv'