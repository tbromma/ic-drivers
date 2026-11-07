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

#pragma once
#include "common.h"

namespace drv::husb238 {

static constexpr uint8 I2C_ADDR = 0b0001000;

enum RegAddr : uint8 {
    REG_PD_STATUS0  = 0x00,
    REG_PD_STATUS1  = 0x01,
    REG_SRC_PDO_5V  = 0x02,
    REG_SRC_PDO_9V  = 0x03,
    REG_SRC_PDO_12V = 0x04,
    REG_SRC_PDO_15V = 0x05,
    REG_SRC_PDO_18V = 0x06,
    REG_SRC_PDO_20V = 0x07,
    REG_SRC_PDO     = 0x08,
    REG_GO_COMMAND0 = 0x09,
};

enum PdoSelect {
    SRC_PDO_5V = 0b0001,
    SRC_PDO_9V = 0b0010,
    SRC_PDO_12V = 0b0011,
    SRC_PDO_15V = 0b1000,
    SRC_PDO_18V = 0b1001,
    SRC_PDO_20V = 0b1010,
};

enum Command {
    CMD_REQ_PDO_SELECT = 0b00001,
    CMD_GET_SRC_CAP = 0b00100,
    CMD_SEND_HARD_RST = 0b10000,
};

union RegPdStatus0 {
    struct {
        uint8 PD_SRC_CURRENT : 4;
        uint8 PD_SRC_VOLTAGE : 4;
    } bit;
    uint8 val;
};

union RegPdStatus1 {
    struct {
        uint8 _5V_CURRENT   : 2;
        uint8 _5V_VOLTAGE   : 1;
        uint8 PD_RESPONSE   : 3;
        uint8 ATTACH        : 1;
        uint8 CC_DIR        : 1;
    } bit;
    uint8 val;
};

union RegSrcPdo5V {
    struct {
        uint8 SRC_5V_CURRENT    : 4;
        uint8                   : 3;
        uint8 SRC_5V_DETECT     : 1;
    } bit;
    uint8 val;
};

union RegSrcPdo9V {
    struct {
        uint8 SRC_9V_CURRENT    : 4;
        uint8                   : 3;
        uint8 SRC_9V_DETECT     : 1;
    } bit;
    uint8 val;
};

union RegSrcPdo12V {
    struct {
        uint8 SRC_12V_CURRENT   : 4;
        uint8                   : 3;
        uint8 SRC_12V_DETECT    : 1;
    } bit;
    uint8 val;
};

union RegSrcPdo15V {
    struct {
        uint8 SRC_15V_CURRENT   : 4;
        uint8                   : 3;
        uint8 SRC_15V_DETECT    : 1;
    } bit;
    uint8 val;
};

union RegSrcPdo18V {
    struct {
        uint8 SRC_18V_CURRENT   : 4;
        uint8                   : 3;
        uint8 SRC_18V_DETECT    : 1;
    } bit;
    uint8 val;
};

union RegSrcPdo20V {
    struct {
        uint8 SRC_20V_CURRENT   : 4;
        uint8                   : 3;
        uint8 SRC_20V_DETECT    : 1;
    } bit;
    uint8 val;
};

union RegSrcPdo {
    struct {
        uint8               : 4;
        uint8 PDO_SELECT    : 4;
    } bit;
    uint8 val;
};

union RegGoCommand {
    struct {
        uint8 COMMAND_FUNC  : 5;
        uint8               : 3;
    } bit;
    uint8 val;
};

} // End of namespace "drv::husb238"
