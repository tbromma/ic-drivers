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
    PDO_none = 0b0000,
    PDO_5V = 0b0001,
    PDO_9V = 0b0010,
    PDO_12V = 0b0011,
    PDO_15V = 0b1000,
    PDO_18V = 0b1001,
    PDO_20V = 0b1010,
};

enum Command {
    CMD_REQ_PDO_SELECT = 0b00001,
    CMD_GET_SRC_CAP = 0b00100,
    CMD_SEND_HARD_RST = 0b10000,
};

enum class PdVoltage : uint8 {
    PD_none = 0b0000,
    PD_5V   = 0b0001,
    PD_9V   = 0b0010,
    PD_12V  = 0b0011,
    PD_15V  = 0b0100,
    PD_18V  = 0b0101,
    PD_20V  = 0b0110,
};

inline constexpr const char* pdVoltageStr[] {
    "none",
    "5V",
    "9V",
    "12V",
    "15V",
    "18V",
    "20V",
};

constexpr float pdVoltageFloat[] {
    0.0f,
    5.0f,
    9.0f,
    12.0f,
    15.0f,
    18.0f,
    20.0f,
};

enum class PdCurrent : uint8 {
    PD_0p5A  = 0b0000,
    PD_0p7A  = 0b0001,
    PD_1A    = 0b0010,
    PD_1p25A = 0b0011,
    PD_1p5A  = 0b0100,
    PD_1p75A = 0b0101,
    PD_2A    = 0b0110,
    PD_2p25A = 0b0111,
    PD_2p5A  = 0b1000,
    PD_2p75A = 0b1001,
    PD_3A    = 0b1010,
    PD_3p25A = 0b1011,
    PD_3p5A  = 0b1100,
    PD_4A    = 0b1101,
    PD_4p5A  = 0b1110,
    PD_5A    = 0b1111,
};

inline constexpr const char* pdCurrentStr[] = {
    "0.5A",
    "0.7A",
    "1.0A",
    "1.25A",
    "1.5A",
    "1.75A",
    "2.0A",
    "2.25A",
    "2.5A",
    "2.75A",
    "3.0A",
    "3.25A",
    "3.5A",
    "4.0A",
    "4.5A",
    "5.0A",
};

constexpr float pdCurrentFloat[] {
    0.5f,
    0.7f,
    1.0f,
    1.25f,
    1.5f,
    1.75f,
    2.0f,
    2.25f,
    2.5f,
    2.75f,
    3.0f,
    3.25f,
    3.5f,
    4.0f,
    4.5f,
    5.0f,
};

union RegPdStatus0 {
    struct {
        PdCurrent PD_CURRENT : 4;
        PdVoltage PD_VOLTAGE : 4;
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
        PdCurrent CURRENT   : 4;
        uint8               : 3;
        uint8 DETECT        : 1;
    } bit;
    uint8 val;
};

union RegSrcPdo9V {
    struct {
        PdCurrent CURRENT   : 4;
        uint8               : 3;
        uint8 DETECT        : 1;
    } bit;
    uint8 val;
};

union RegSrcPdo12V {
    struct {
        PdCurrent CURRENT   : 4;
        uint8               : 3;
        uint8 DETECT        : 1;
    } bit;
    uint8 val;
};

union RegSrcPdo15V {
    struct {
        PdCurrent CURRENT   : 4;
        uint8               : 3;
        uint8 DETECT        : 1;
    } bit;
    uint8 val;
};

union RegSrcPdo18V {
    struct {
        PdCurrent CURRENT   : 4;
        uint8               : 3;
        uint8 DETECT        : 1;
    } bit;
    uint8 val;
};

union RegSrcPdo20V {
    struct {
        PdCurrent CURRENT   : 4;
        uint8               : 3;
        uint8 DETECT        : 1;
    } bit;
    uint8 val;
};

union RegSrcPdo {
    struct {
        uint8                   : 4;
        PdoSelect PDO_SELECT    : 4;
    } bit;
    uint8 val;
};

union RegGoCommand {
    struct {
        Command COMMAND_FUNC    : 5;
        uint8                   : 3;
    } bit;
    uint8 val;
};

struct PdCapability {
    bool        dataValid = false;
    bool        pd5VDetected;
    PdCurrent   pd5VCurrent;
    bool        pd9VDetected;
    PdCurrent   pd9VCurrent;
    bool        pd12VDetected;
    PdCurrent   pd12VCurrent;
    bool        pd15VDetected;
    PdCurrent   pd15VCurrent;
    bool        pd18VDetected;
    PdCurrent   pd18VCurrent;
    bool        pd20VDetected;
    PdCurrent   pd20VCurrent;
};

struct PdSelected {
    bool valid;
    PdVoltage voltage;
    PdCurrent current;
    float wattage;
};

} // End of namespace "drv::husb238"
