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
// DESCRIPTION:      Semtechs SX126x radio types

/******************************************************************************************************************************************
                                                                  INCLUDES
******************************************************************************************************************************************/
// namespace drv {

// class SX126x {
// public:

enum opCmd_t : uint8 {
    CMD_WRITE_REGISTER              = 0x0D,
    CMD_READ_REGISTER               = 0x1D,
    CMD_WRITE_BUFFER                = 0x0E,
    CMD_READ_BUFFER                 = 0x1E,
    CMD_RESET_STATS                 = 0x00,
    CMD_CFG_DIOIRQ                  = 0x08,
    CMD_CLR_IRQSTATUS               = 0x02,
    CMD_CLR_DEV_ERRORS              = 0x07,
    CMD_CALIBRATE                   = 0x89,
    CMD_CALIBRATEIMAGE              = 0x98,

    CMD_SET_SLEEP                   = 0x84,
    CMD_SET_STANDBY                 = 0x80,
    CMD_SET_FS                      = 0xC1,
    CMD_SET_TX                      = 0x83,
    CMD_SET_RX                      = 0x82,
    CMD_SET_RXDUTYCYCLE             = 0x94,
    CMD_SET_CAD                     = 0xC5,
    CMD_SET_TXCONTINUOUSWAVE        = 0xD1,
    CMD_SET_TXCONTINUOUSPREAMBLE    = 0xD2,
    CMD_SET_PACKET_TYPE             = 0x8A,
    CMD_SET_RF_FREQUENCY            = 0x86,
    CMD_SET_TXPARAMS                = 0x8E,
    CMD_SET_PACONFIG                = 0x95,
    CMD_SET_CADPARAMS               = 0x88,
    CMD_SET_BUFFER_BASE_ADDR        = 0x8F,
    CMD_SET_MODULATIONPARAMS        = 0x8B,
    CMD_SET_PACKETPARAMS            = 0x8C,
    CMD_SET_TCXOMODE                = 0x97,
    CMD_SET_TXFALLBACKMODE          = 0x93,
    CMD_SET_RFSWITCHMODE            = 0x9D,
    CMD_SET_STOPRXTIMERONPREAMBL    = 0x9F,
    CMD_SET_LORASYMBTIMEOUT         = 0xA0,

    CMD_GET_STATUS                  = 0xC0,
    CMD_GET_PACKET_TYPE             = 0x11,
    CMD_GET_RXBUFFERSTATUS          = 0x13,
    CMD_GET_PACKETSTATUS            = 0x14,
    CMD_GET_RSSIINST                = 0x15,
    CMD_GET_STATS                   = 0x10,
    CMD_SET_REGULATORMODE           = 0x96,
    CMD_GET_IRQSTATUS               = 0x12,
    CMD_GET_DEV_ERRORS              = 0x17,
};

enum regAddr_t : uint16 {
    REG_LoRaSyncWordMSB = 0x0740,
    REG_LoRaSyncWordLSB = 0x0741,
    REG_TxModulation    = 0x0889,
    REG_RxGain          = 0x08AC,
    REG_TxClampConfig   = 0x08D8,
    REG_RtcControl      = 0x0902,
};

enum packetType_t : uint8 {
    PACKET_TYPE_GFSK = 0x00,
    PACKET_TYPE_LORA = 0x01,
    PACKET_TYPE_LR_FHSS = 0x03,
};

// Radio standby mode types
enum stdbyMode_t {
    STDBY_RC,
    STDBY_XOSC,
};

enum chipMode_t {
    MODE_UNDEF0 = 0b000,            // Undefined and reserved for future use
    MODE_UNDEF1 = 0b001,            // Undefined and reserved for future use
    MODE_STBY_RC = 0b010,
    MODE_STBY_OSC = 0b011,
    MODE_FS = 0b100,
    MODE_RX = 0b101,
    MODE_TX = 0b110,
};

enum cmdStat_t {
    CMDSTAT_UNDEF0 = 0b000,         // Undefined and reserved for future use
    CMDSTAT_UNDEF1 = 0b001,         // Undefined and reserved for future use
    CMDSTAT_NEW_DATA = 0b010,       // A packet has been successfully received and data can be retrieved
    CMDSTAT_TIMEOUT = 0b011,        // Transaction from host took too long to complete and triggered an internal watchdog
    CMDSTAT_PROCESS_ERR = 0b100,    // Processor was unable to process command either because of an invalid opcode or incorrect number of parameters
    CMDSTAT_EXEC_ERR = 0b101,       // The command was successfully processed, however the chip could not execute the command
    CMDSTAT_TX_DONE = 0b110,        // The transmission of the current packet has terminated
};

enum rampTime_t : uint8 {
    RAMP_TIME_10us = 0x00,
    RAMP_TIME_20us = 0x01,
    RAMP_TIME_40us = 0x02,
    RAMP_TIME_80us = 0x03,
    RAMP_TIME_200us = 0x04,
    RAMP_TIME_800us = 0x05,
    RAMP_TIME_1700us = 0x06,
    RAMP_TIME_3400us = 0x07,
};

enum loraSf_t : int8 {
    LORA_SF_none = -1,
    LORA_SF5 = 5,
    LORA_SF6 = 6,
    LORA_SF7 = 7,
    LORA_SF8 = 8,
    LORA_SF9 = 9,
    LORA_SF10 = 10,
    LORA_SF11 = 11,
    LORA_SF12 = 12,
};

enum loraBw_t : int8 {
    LORA_BW_none = -1,
    LORA_BW_500 = 6,
    LORA_BW_250 = 5,
    LORA_BW_125 = 4,
    LORA_BW_062 = 3,
    LORA_BW_041 = 10,
    LORA_BW_031 = 2,
    LORA_BW_020 = 9,
    LORA_BW_015 = 1,
    LORA_BW_010 = 8,
    LORA_BW_007 = 0,
};

enum loraCr_t : int8 {
    LORA_CR_none = -1,
    LORA_CR_4_5 = 1,
    LORA_CR_4_6 = 2,
    LORA_CR_4_7 = 3,
    LORA_CR_4_8 = 4,
};

enum loraLowDrOpt_t : uint8 {
    LORA_LOWDROPT_OFF = 0,
    LORA_LOWDROPT_ON = 1,
};

enum loraHeader_t : uint8 {
    LORA_HEADER_EXPLICIT = 0,
    LORA_HEADER_IMPLICIT = 1,
};

enum loraCrc_t : uint8 {
    LORA_CRC_OFF = 0,
    LORA_CRC_ON = 1,
};

enum loraIq_t : uint8 {
    LORA_IQ_STD = 0,
    LORA_IQ_INV = 1,
};

union irqMask_t {
    struct {
        uint16 txDone           : 1;
        uint16 rxDone           : 1;
        uint16 preambDet        : 1;
        uint16 syncWordValid    : 1;
        uint16 headerValid      : 1;
        uint16 headerErr        : 1;
        uint16 crcErr           : 1;
        uint16 cadDone          : 1;
        uint16 cadDetected      : 1;
        uint16 timeout          : 1;
        uint16                  : 4;
        uint16 lrFhssHop        : 1;
        uint16                  : 1;
    } bit;
    uint16 val;
};

union chipStat_t {
    struct {
        uint8               : 1;
        cmdStat_t cmdStat   : 3;
        chipMode_t chipMode : 3;
        uint8               : 1;
    } bit;
    uint8 val;
};

union devErr_t {
    struct {
        uint16 RC64K_CALIB_ERR  : 1;
        uint16 RC13M_CALIB_ERR  : 1;
        uint16 PLL_CALIB_ERR    : 1;
        uint16 ADC_CALIB_ERR    : 1;
        uint16 IMG_CALIB_ERR    : 1;
        uint16 XOSC_START_ERR   : 1;
        uint16 PLL_LOCK_ERR     : 1;
        uint16                  : 1;
        uint16 PA_RAMP_ERR      : 1;
        uint16                  : 7;
    } bit;
    uint16 val;
};

union sleepCfg_t {
    struct {
        uint8 wake  : 1;        // RTC wake: 0 -> disable, 1 -> enable
        uint8       : 1;
        uint8 start : 1;        // 0 -> cold start, 1 -> warm start (device configuration in retention)
        uint8       : 5;
    } bit;
    uint8 val;
};

struct __packed rxBuffStat_t {
    chipStat_t  chipStat;
    uint8       payloadLen;
    uint8       dataStartAddr;
};

struct __packed rxPacketStat_t {
    chipStat_t  chipStat;
    uint8       avgRssi;
    int8        snrPacket;
    uint8       signalRssi;
};

struct __packed readReg_t {
    chipStat_t  status;
    uint8       value;
};

// }

// } // End of namespace 'drv'
