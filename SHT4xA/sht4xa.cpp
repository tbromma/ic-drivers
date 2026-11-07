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
// DESCRIPTION:      Driver for Sensirion SHT4xA temperature and humidity sensor IC family

/******************************************************************************************************************************************
                                                                  INCLUDES
******************************************************************************************************************************************/

#include "sht4xa.h"

/******************************************************************************************************************************************
                                                              TYPES & DEFINES
******************************************************************************************************************************************/
namespace drv {

#undef CODE_LABEL
#undef CODE_STATUS
#define CODE_LABEL  ctx.config.label_p
#define CODE_STATUS ctx.status

using namespace sht4xa;
using namespace bareOs;

/******************************************************************************************************************************************
                                                         PUBLIC FUNCTION DEFINITIONS
******************************************************************************************************************************************/

Exec SHT4xA::Setup(const config_t& config)
{
    ASSERT(ctx.status == Status::NONE, "PROBABLY EXECUTING 'SETUP' AGAIN");
    ASSERT(config.I2cTxPoll != nullptr, "I2C TX DATA FUNC");
    ASSERT(config.I2cRxPoll != nullptr, "I2C RX DATA FUNC");
    ASSERT(config.I2cGetStatus != nullptr, "I2C GET CODE_STATUS FUNC");
    ASSERT(config.I2cGetStatus() == Status::OK, "I2C BUS");

    ctx.config = config;
    ctx.status = Status ::INIT;

    //-----------------------------------------------------------------------------------
    // Check I2C communication reading sensor serial number

    serialNum_t serial;
    CHECK_EXEC(ReadSensorSn(serial));

    //-----------------------------------------------------------------------------------

    ctx.status = Status::OK;
    return Exec::OK;
}

Exec SHT4xA::ReadSensorSn(serialNum_t& serial)
{
    CHECK_STATUS();

    //-----------------------------------------------------------------------------------
    // Reading & saving sensor unique serial number

    serialData_t data;
    uint8 txData = (uint8)Command::READ_SERIAL_NUMBER;

    CHECK_EXEC(ctx.config.I2cTxPoll(ctx.config.i2cAddr, &txData, sizeof(txData)));
    bareOs::Timer::DelayMs(1);
    CHECK_EXEC(ctx.config.I2cRxPoll(ctx.config.i2cAddr, (uint8*)&data, sizeof(data)));

    serial = (data.highWord << 16) | data.lowWord;

    //-----------------------------------------------------------------------------------

    return Exec::OK;
}

Exec SHT4xA::TriggConversion(sht4xa::Resolution res)
{
    CHECK_STATUS();
    Command convCmd;
    if (res == Resolution::LOW)         convCmd = Command::MEAS_TEMP_RH_LOW_RES;
    else if (res == Resolution::MEDIUM) convCmd = Command::MEAS_TEMP_RH_MEDIUM_RES;
    else                                convCmd = Command::MEAS_TEMP_RH_HIGH_RES;
    RET_EXEC(ctx.config.I2cTxPoll(ctx.config.i2cAddr, (uint8*)&convCmd, sizeof(Command)));
}

Exec SHT4xA::SoftwareReset()
{
    CHECK_STATUS();
    Command cmdRst = Command::SOFT_RESET;
    RET_EXEC(ctx.config.I2cTxPoll(ctx.config.i2cAddr, (uint8*)&cmdRst, sizeof(Command)));
}

Exec SHT4xA::ReadConvResult(convData_t& data)
{
    CHECK_STATUS();
    CHECK_EXEC(ctx.config.I2cRxPoll(ctx.config.i2cAddr, (uint8*)&data, sizeof(data)));
    utils::RotateBytewise((uint8*)&data.tempBin, sizeof(uint16), (uint8*)&data.tempBin);
    utils::RotateBytewise((uint8*)&data.rhBin, sizeof(uint16), (uint8*)&data.rhBin);
    return Exec::OK;
}

uint8 SHT4xA::GetConvTimeMs(Resolution res)
{
    return convTimeMs[(uint8)res];
}

void SHT4xA::ConvTempBinToDec(sht4xa::Resolution res, uint16 bin, units::uniVal_t& decimal)
{
    decimal.value = (int16)((((int32)bin * 17500) / 65535) - 4500);

    if (res == Resolution::HIGH) {
        decimal.commaPos = units::COMMA_POS_2;
    }
    else {
        decimal.value /= 10;
        decimal.commaPos = units::COMMA_POS_1;
    }

    decimal.prefix = units::PREFIX_none;
    decimal.unit = units::UNIT_TEMPERATURE_C;
    decimal.error = false;
    decimal.valid = true;
}

void SHT4xA::ConvRhBinToDec(sht4xa::Resolution res, uint16 bin, units::uniVal_t& decimal)
{
    decimal.value = (int16)(((((uint32)bin * 12500) / 65535) - 600) / 10);
    if (res == Resolution::HIGH) {
        decimal.commaPos = units::COMMA_POS_1;
    }
    else {
        decimal.value /= 10;
        decimal.commaPos = units::COMMA_POS_0;
    }
    decimal.prefix = units::PREFIX_none;
    decimal.unit = units::UNIT_PERCENT;
    decimal.error = false;
    decimal.valid = true;
}

bool SHT4xA::CheckCrc(uint8* data_p, uint8 dataCount, uint8 checksum)
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

    if (crc != checksum) return false;
	return true;
}

} // End of namespace 'drv'
/******************************************************************************************************************************************
                                                                END OF FILE
******************************************************************************************************************************************/
