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
                                                                    CODE
******************************************************************************************************************************************/

#pragma once

namespace drv::rv3028 {

constexpr uint8 I2C_ADDR = 0x52;
constexpr uint8 REGISTER_CNT = 40;

enum RegAddr : uint8 {
    REG_SEC             = 0x00,
    REG_MIN             = 0x01,
    REG_HOUR            = 0x02,
    REG_WEEKDAY         = 0x03,
    REG_DAY             = 0x04,
    REG_MONTH           = 0x05,
    REG_YEAR            = 0x06,

    REG_ALARM_MIN       = 0x07,
    REG_ALARM_HOUR      = 0x08,
    REG_ALARM_DAY       = 0x09,
    REG_ALARM_WEEKDAY   = 0x0A,

    REG_TIMER_VAL       = 0x0B,
    REG_TIMER_STAT      = 0x0C,
    REG_TIMER_CTRL      = 0x0D,

    REG_STATUS          = 0x0E,
    REG_CTRL1           = 0x0F,
    REG_CTRL2           = 0x10,

    REG_GP_FLAGS        = 0x11,
    REG_CLK_INT_MASK    = 0x12,
    REG_EVT_CTRL        = 0x13,

    REG_TS_COUNT        = 0x14,
    REG_TS_SEC          = 0x15,
    REG_TS_MIN          = 0x16,
    REG_TS_HOUR         = 0x17,
    REG_TS_DAY          = 0x18,
    REG_TS_MONTH        = 0x19,
    REG_TS_YEAR         = 0x1A,

    REG_UNIX0           = 0x1B,
    REG_UNIX1           = 0x1C,
    REG_UNIX2           = 0x1D,
    REG_UNIX3           = 0x1E,

    REG_USER_RAM1       = 0x1F,
    REG_USER_RAM2       = 0x20,

    REG_PASSWORD1       = 0x21,
    REG_PASSWORD2       = 0x22,
    REG_PASSWORD3       = 0x23,
    REG_PASSWORD4       = 0x24,

    REG_EEPROM_ADDR     = 0x25,
    REG_EEPROM_DATA     = 0x26,
    REG_EEPROM_CMD      = 0x27,

    REG_ID              = 0x28,

    REG_EEPROM_CLKOUT   = 0x35,
    REG_EEPROM_OFFSET   = 0x36,
    REG_EEPROM_BACKUP   = 0x37,
};

union RegSec {
    struct {
        uint8 bcd1     : 4;
        uint8 bcd10    : 3;
        uint8 reserved : 1;
    } bit;
    uint8 val;
};

union RegMin {
    struct {
        uint8 bcd1     : 4;
        uint8 bcd10    : 3;
        uint8          : 1;
    } bit;
    uint8 val;
};

union RegHour {
    struct {
        uint8 bcd1     : 4;
        uint8 bcd10    : 2;
        uint8 mode12   : 1;
        uint8          : 1;
    } bit;
    uint8 val;
};

union RegWeekday {
    struct {
        uint8 weekday  : 3;
        uint8          : 5;
    } bit;
    uint8 val;
};

union RegDay {
    struct {
        uint8 bcd1     : 4;
        uint8 bcd10    : 2;
        uint8          : 2;
    } bit;
    uint8 val;
};

union RegMonth {
    struct {
        uint8 bcd1     : 4;
        uint8 bcd10    : 1;
        uint8          : 3;
    } bit;
    uint8 val;
};

union RegYear {
    struct {
        uint8 bcd1     : 4;
        uint8 bcd10    : 4;
    } bit;
    uint8 val;
};

// Alarm registers
union RegAlarmMin {
    struct {
        uint8 bcd1     : 4;
        uint8 bcd10    : 3;
        uint8 AE_M     : 1; // Alarm enable minute
    } bit;
    uint8 val;
};

union RegAlarmHour {
    struct {
        uint8 bcd1     : 4;
        uint8 bcd10    : 2;
        uint8 AE_H     : 1; // Alarm enable hour
        uint8          : 1;
    } bit;
    uint8 val;
};

union RegAlarmDay {
    struct {
        uint8 bcd1     : 4;
        uint8 bcd10    : 2;
        uint8 AE_D     : 1; // Alarm enable day
        uint8          : 1;
    } bit;
    uint8 val;
};

union RegAlarmWeekday {
    struct {
        uint8 weekday  : 3;
        uint8          : 4;
        uint8 AE_WD    : 1; // Alarm enable weekday
    } bit;
    uint8 val;
};

// Timer
union RegTimerVal {
    struct {
        uint8 value    : 8;
    } bit;
    uint8 val;
};

union RegTimerStat {
    struct {
        uint8 running  : 1;
        uint8 expired  : 1;
        uint8          : 6;
    } bit;
    uint8 val;
};

union RegTimerCtrl {
    struct {
        uint8 TE       : 1; // Timer enable
        uint8 TD       : 2; // Timer clock source
        uint8          : 5;
    } bit;
    uint8 val;
};

// Status & Control
union RegStatus {
    struct {
        uint8 PORF    : 1;
        uint8 EVF     : 1;
        uint8 AF      : 1;
        uint8 TF      : 1;
        uint8 UF      : 1;
        uint8 BSF     : 1;
        uint8 CLKF    : 1;
        uint8 EEBUSY  : 1;
    } bit;
    uint8 val;
};

union RegCtrl1 {
    struct {
        uint8 TRPT    : 1;
        uint8         : 2;
        uint8 EERD    : 1;
        uint8         : 1;
        uint8 WADA    : 1;
        uint8         : 2;
    } bit;
    uint8 val;
};

union RegCtrl2 {
    struct {
        uint8 RESET   : 1;
        uint8 HRMODE  : 1;
        uint8 EIE     : 1;
        uint8 AIE     : 1;
        uint8 TIE     : 1;
        uint8 UIE     : 1;
        uint8         : 1;
        uint8 TSE     : 1;
    } bit;
    uint8 val;
};

union RegGpBits {
    struct {
        uint8 GP0   : 1;
        uint8 GP1   : 1;
        uint8 GP2   : 1;
        uint8 GP3   : 1;
        uint8 GP4   : 1;
        uint8 GP5   : 1;
        uint8 GP6   : 1;
        uint8       : 1;
    } bit;
    uint8 val;
};

// Clock output
union RegClkout {
    struct {
        uint8 FD      : 3;
        uint8         : 1;
        uint8         : 2;
        uint8 CLKSY   : 1;
        uint8 CLKOE   : 1;
    } bit;
    uint8 val;
};

// Offset
union RegOffset {
    struct {
        uint8 offset  : 8;
    } bit;
    uint8 val;
};

// Backup
union RegBackup {
    struct {
        uint8 TCR       : 2;
        uint8 BSM       : 2;
        uint8 FEDE      : 1;
        uint8 TCE       : 1;
        uint8 BSIE      : 1;
        uint8 EEOFFSET  : 1;
    } bit;
    uint8 val;
};

// RAM registers
union RegUserRam {
    struct {
        uint8 data    : 8;
    } bit;
    uint8 val;
};

// EEPROM command
union RegEepromCmd {
    struct {
        uint8 cmd     : 4;
        uint8         : 4;
    } bit;
    uint8 val;
};

} // End of namespace 'drv::rv3028'
