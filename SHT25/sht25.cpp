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
// CREATION DATE:    July 2023
// DESCRIPTION:      Driver for Sensirion SHT25 temperature and humidity sensor IC

/******************************************************************************************************************************************
                                                                  INCLUDES
******************************************************************************************************************************************/

#include "sht25.h"

/******************************************************************************************************************************************
                                                              TYPES & DEFINES
******************************************************************************************************************************************/
namespace drv::sht25 {

#undef CODE_LABEL
#undef CODE_STATUS
#define CODE_LABEL  ctx.config.label_p
#define CODE_STATUS ctx.status

static constexpr uint16 CRC_POLYNOMIAL = 0b100110001;          // P(x)=x^8+x^5+x^4+1
static constexpr uint8 tempConvTime[] = {11, 22, 43, 85};      // Values in ms derived from datasheet
static constexpr uint8 rhConvTime[] = {4, 9, 15, 29};          // Values in ms derived from datasheet

/******************************************************************************************************************************************
                                                         PUBLIC FUNCTION DEFINITIONS
******************************************************************************************************************************************/

Exec SHT25::Setup(const config_t& config)
{
    ASSERT(config.I2cTxPoll != nullptr, "I2C TX DATA FUNC");
    ASSERT(config.I2cRxPoll != nullptr, "I2C RX DATA FUNC");
    ASSERT(config.I2cGetStatus != nullptr, "I2C GET CODE_STATUS FUNC");
    ASSERT(config.I2cGetStatus() == Status::OK, "I2C BUS");

    ctx.config = config;
    ctx.status = Status ::INIT;

    //-----------------------------------------------------------------------------------
    // Check I2C communication -> read sensor EID

    sensorEID_t sensorEID = {};
    CHECK_EXEC(GetSensorEID(sensorEID));

    //-----------------------------------------------------------------------------------

    ctx.status = Status::OK;
    return Exec::OK;
}

Exec SHT25::GetSensorEID(sensorEID_t& sensorEID)
{
    CHECK_STATUS();

    //-----------------------------------------------------------------------------------
    // READING THE ELECTRONIC IDENTIFICATION CODE PART 1 & 2

    uint8 eidPart1[4];
    uint8 eidPart2[4];
    CHECK_EXEC(ReadData(CMD_READ_EID_PART1, eidPart1, sizeof(eidPart1)));
    CHECK_EXEC(ReadData(CMD_READ_EID_PART2, eidPart2, sizeof(eidPart2)));

    //-----------------------------------------------------------------------------------
    // SAVING THE SENSOR EID. FOR DETAIL REFER TO "SENSIRION ELECTRONIC
    // IDENTIFICATION CODE HUMIDITY SENSOR SHT2X"

    uint8* sensorEID_p = (uint8*)&sensorEID;
    sensorEID_p[5] = eidPart1[0];       // SNB_3
    sensorEID_p[4] = eidPart1[1];       // SNB_2
    sensorEID_p[3] = eidPart1[2];       // SNB_1
    sensorEID_p[2] = eidPart1[3];       // SNB_0
    sensorEID_p[1] = eidPart2[0];       // SNC_1
    sensorEID_p[0] = eidPart2[1];       // SNC_0
    sensorEID_p[7] = eidPart2[2];       // SNA_1
    sensorEID_p[6] = eidPart2[3];       // SNA_0
    return Exec::OK;
}

Exec SHT25::SoftwareReset()
{
    CHECK_STATUS();
    RET_EXEC(SendCmd(drv::sht25::CMD_SOFTWARE_RESET));
}

uint8 SHT25::GetTempConvTime(tempRes_t res)
{
    return tempConvTime[res];
}

uint8 SHT25::GetRhConvTime(rhRes_t res)
{
    return rhConvTime[res];
}

Status SHT25::GetStatus()
{
    return ctx.status;
}

bool SHT25::CheckCrc(uint8* data_p, uint8 dataCount, uint8 checksum)
{
	uint8 crc = 0;
	uint8 counter;
	uint8 bitPos;

	for (counter = 0; counter < dataCount; counter++) {
		crc ^= *data_p++;

		for (bitPos = 8; bitPos > 0; --bitPos) {
			if (crc & 0x80) crc = (uint8)((crc << 1) ^ CRC_POLYNOMIAL);
			else crc <<= 1;
		}
	}

    if (crc != checksum) {
        REPORT_WAR("CRC MISMATCH");
        return false;
    }
	return true;
}

void SHT25::ConvTempBinToDec(tempRes_t res, uint16 bin, bareOs::units::uniVal_t& temp)
{
    temp.value = (int16)((((int32)bin * 17572) / 65536) - 4685);

    if (res == TEMP_RES_14BIT) {
        temp.commaPos = bareOs::units::COMMA_POS_2;
    }
    else {
        temp.value /= 10;
        temp.commaPos = bareOs::units::COMMA_POS_1;
    }

    temp.prefix = bareOs::units::PREFIX_none;
    temp.unit = bareOs::units::UNIT_TEMPERATURE_C;
}

void SHT25::ConvRhBinToDec(rhRes_t res, uint16 bin, bareOs::units::uniVal_t& rh)
{
    rh.value = (int16)(((((uint32)bin * 12500) / 65536) - 600) / 10);
    if (res == RH_RES_8BIT) {
        rh.value /= 10;
        rh.commaPos = bareOs::units::COMMA_POS_0;
    }
    else {
        rh.commaPos = bareOs::units::COMMA_POS_1;
    }

    rh.prefix = bareOs::units::PREFIX_none;
    rh.unit = bareOs::units::UNIT_PERCENT;
}

Exec SHT25::SendCmd(cmd_t cmd)
{
    ASSERT( (cmd == CMD_TRIG_TEMP_MEAS_HM)  ||
            (cmd == CMD_TRIG_TEMP_MEAS_NHM) ||
            (cmd == CMD_TRIG_RH_MEAS_HM)    ||
            (cmd == CMD_TRIG_RH_MEAS_NHM)   ||
            (cmd == CMD_SOFTWARE_RESET), "INVALID COMMAND");

    //-----------------------------------------------------------------------------------
    // Send CMD

    RET_EXEC(ctx.config.I2cTxPoll(ctx.config.i2cAddr, (uint8*)&cmd, sizeof(uint8)));
}

Exec SHT25::ReadData(cmd_t cmd, uint8* data_p, uint8 dataSize)
{
    ASSERT(data_p != nullptr, "VARIABLE 'data_p'");
    ASSERT(dataSize > 0, "VARIABLE 'dataSize' MUST BE GRATER THAN ZERO");
    ASSERT( (cmd == CMD_READ_CONV_RESULT) ||
            (cmd == CMD_READ_USER_REG)    ||
            (cmd == CMD_READ_EID_PART1)   ||
            (cmd == CMD_READ_EID_PART2), "INVALID COMMAND");

    //-----------------------------------------------------------------------------------
    // READ DATA

    uint8 cmdSize;
    cmdSize = ((cmd == CMD_READ_EID_PART1) || (cmd == CMD_READ_EID_PART2)) ? sizeof(uint16) : sizeof(uint8);

    if (cmd != CMD_READ_CONV_RESULT) {
        CHECK_EXEC(ctx.config.I2cTxPoll(ctx.config.i2cAddr, (uint8*)&cmd, cmdSize));
    }

    RET_EXEC(ctx.config.I2cRxPoll(ctx.config.i2cAddr, data_p, dataSize));
}

Exec SHT25::WriteUserReg(userReg_t userReg)
{
    uint8 txData[2];
    txData[0] = CMD_WRITE_USER_REG;
    txData[1] = userReg.val;

    RET_EXEC(ctx.config.I2cTxPoll(ctx.config.i2cAddr, txData, sizeof(txData)));
}

/******************************************************************************************************************************************
                                                                END OF FILE
******************************************************************************************************************************************/
} // End of namespace 'drv::sht25'