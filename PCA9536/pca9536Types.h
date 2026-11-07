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
// CREATION DATE:    October 2024
// DESCRIPTION:      Driver for PCA9536 GPIO expander IC

/******************************************************************************************************************************************
                                                                  INCLUDES
******************************************************************************************************************************************/

#pragma once
#include "common.h"

namespace drv::pca9536 {

static constexpr uint16 DEVICE_ID = 0x0117;
static constexpr uint8 I2C_DEFAULT_ADDR = 0b1000001;

enum RegAddr : uint8 {
    REG_INPUT       = 0x00,     // Input Port register
    REG_OUTPUT      = 0x01,     // Output Port register
    REG_POLARITY    = 0x02,     // Polarity Inversion register
    REG_CONFIG      = 0x03,     // Configuration register
};

enum GpioCnt : uint8 {
    GPIO_CNT_4,
    GPIO_CNT_8,
};

// Reflects logic levels of pins defined as inputs
union regIn_t {
    struct {
        uint8 IN0 : 1;
        uint8 IN1 : 1;
        uint8 IN2 : 1;
        uint8 IN3 : 1;
        uint8 IN4 : 1;
        uint8 IN5 : 1;
        uint8 IN6 : 1;
        uint8 IN7 : 1;
    } bit;
    uint8 val;
};

// Reflects outgoing logic levels of pins defined as outputs
union regOut_t {
    struct {
        uint8 OUT0 : 1;
        uint8 OUT1 : 1;
        uint8 OUT2 : 1;
        uint8 OUT3 : 1;
        uint8 OUT4 : 1;
        uint8 OUT5 : 1;
        uint8 OUT6 : 1;
        uint8 OUT7 : 1;
    } bit;
    uint8 val;
};

// Inverts polarity of Input Port register data
// 0 = Input Port register data retained (default value)
// 1 = Input Port register data inverted
union regInv_t {
    struct {
        uint8 INV0 : 1;
        uint8 INV1 : 1;
        uint8 INV2 : 1;
        uint8 INV3 : 1;
        uint8 INV4 : 1;
        uint8 INV5 : 1;
        uint8 INV6 : 1;
        uint8 INV7 : 1;
    } bit;
    uint8 val;
};

// Configures the directions of the I/O pins
// 0 = corresponding port pin enabled as an output
// 1 = corresponding port pin configured as input (default value)
union regConfig_t {
    struct {
        uint8 DIR0 : 1;
        uint8 DIR1 : 1;
        uint8 DIR2 : 1;
        uint8 DIR3 : 1;
        uint8 DIR4 : 1;
        uint8 DIR5 : 1;
        uint8 DIR6 : 1;
        uint8 DIR7 : 1;
    } bit;
    uint8 val;
};

} // End of namespace "drv::pca9536"
