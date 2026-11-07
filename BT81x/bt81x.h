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
// DESCRIPTION:      Bridgetek BT81x display driver

/******************************************************************************************************************************************
                                                                  INCLUDES
******************************************************************************************************************************************/

#pragma once
#include "common.h"
#include "bt81xGpu.h"
#include "bt81xTypes.h"

/******************************************************************************************************************************************
                                                                   TYPES
******************************************************************************************************************************************/
namespace drv::bt81x {

using Clbk_t = void(*)();
using SpiTxPoll_t = Exec (*)(uint8* txData, uint16 txSize, bool keepSsLow);
using SpiTxClbk_t = Exec (*)(uint8* txData, uint16 txSize, bool keepSsLow, Clbk_t Clbk);
using SpiTxRxPoll_t = Exec (*)(uint8* txSrc, uint16 txSize, uint8* rxDest, uint16 rxSize, bool keepSsLow);
using SpiTxRxClbk_t = Exec (*)(uint8* txSrc, uint16 txSize, uint8* rxDest, uint16 rxSize, bool keepSsLow, Clbk_t Clbk);
using SpiStatus_t = Status (*)();

struct config_t {
    SpiTxPoll_t     SpiTxPoll = nullptr;
    SpiTxClbk_t     SpiTxClbk = nullptr;
    SpiTxRxPoll_t   SpiTxRxPoll = nullptr;
    SpiTxRxClbk_t   SpiTxRxClbk = nullptr;
    SpiStatus_t     SpiStatus = nullptr;
    const char*     label_p = nullptr;
};

/******************************************************************************************************************************************
                                                                    API
******************************************************************************************************************************************/

class BT81x {
public:

    Exec  Setup(const config_t& config);
    Exec  ReadReg(regAddr_t regAddr, regWidth_t regSize, uint8 *regData_p);
    Exec  WriteReg(regAddr_t regAddr, regWidth_t regSize, uint32 data);
    Exec  ReadMem(uint32 addr, uint8* data, uint16 size, Clbk_t Clbk);
    Exec  WriteMemPoll(uint32 addr, uint8* data, uint16 size);
    Exec  WriteMemClbk(uint32 addr, uint8* data, uint16 size, Clbk_t Clbk);
    Exec  SendSysCmd(systemCmd_t cmd, uint8 param);
    Exec  ReadCmdWrite(uint16& ramCmdPos);
    Exec  WriteCmdWrite(uint16 ramCmdPos);
    Exec  ReadCmdRead(uint16& ramCmdPos);
    Exec  SetBacklight(uint8 lumination);
    Exec  SoftwareReset();
    Status  GetStatus();

private:

    static constexpr uint32 BL_MAX_LUM_VAL = 128;      // Backlight lumination max register value -> 0 - 128
    static constexpr uint8 MEM_ADDR_SIZE_WR = 3;       // Memory addres size in bytes for write operations
    static constexpr uint8 MEM_ADDR_SIZE_RD = 4;       // Memory addres size in bytes for read operations

    struct {
        config_t config;
        Status status = Status::NONE;
    } ctx = {};

    static void SpiDoneClbkDummy();
};

/******************************************************************************************************************************************
                                                                END OF FILE
******************************************************************************************************************************************/
} // END OF NAMESPACE 'drv::bt81x'
