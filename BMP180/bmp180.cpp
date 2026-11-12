//#########################################################################################################################################
//															  START OF FILE
//#########################################################################################################################################

#include "bmp180.h"
//#include "usefulfunctions.h"

// namespace drv::sensBmp180 {
// //#########################################################################################################################################
// //												 PRIVATE DEFINE's, MACRO's, ENUM's & TYPEDEF's
// //#########################################################################################################################################

// constexpr uint8 BMP180_I2C_ADDRESS = 0b1110111;
// constexpr uint8 tempConvTime = 6;
// constexpr uint8 pressConvTime[] = {8, 14, 26, 77};

// //#########################################################################################################################################
// //														   MODULE FUNCTION DEFINITIONS
// //#########################################################################################################################################

// initStat_t BMP180::Init(const config_t& config_f)
// {
// 	//-----------------------------------------------------------------------------------
// 	//CHECK CONFIG

//     if(config_f.i2cBus_p == nullptr) {
//         REPORT_ERROR(NULLPTR);
//         return INIT_ERR;
//     }

//     config = config_f;

//     //-----------------------------------------------------------------------------------
// 	//INITIALIZING VARIABLES

// 	status = STAT_IDLE;
//     autoModeRunning = false;
// 	errCode = NONE;

//     //-----------------------------------------------------------------------------------
// 	//SENSOR SOFTWARE RESET

//     if(SendSoftRstCmd() == EXEC_ERR) return INIT_ERR;
//     task::DelayMs(20);

// 	//-----------------------------------------------------------------------------------
// 	//READING SENSOR ID & SENSOR VERSION

//     if(ReadData(ADDR_CHIP_ID_REG, &chipId, sizeof(chipId)) == EXEC_ERR) return INIT_ERR;

// 	if(chipId != 0x55) {
// 		REPORT_ERROR(ID_MISMATCH);
// 		return INIT_ERR;
// 	}

//     if(ReadData(ADDR_CHIP_VER_REG, &chipVer, sizeof(chipVer)) == EXEC_ERR) return INIT_ERR;

// 	//-----------------------------------------------------------------------------------
// 	//READING SENSOR CALIBRATION DATA

//     if(GetCalibData() == EXEC_ERR) return INIT_ERR;

//     //-----------------------------------------------------------------------------------

// 	return INIT_OK;
// }

// status_t BMP180::Handler(void)
// {
//     if(status == STAT_ERR) return status;

// 	//-----------------------------------------------------------------------------------
// 	//CHECK CONVERSION MODE

//     if((config.measMode == MODE_AUTO) && (autoModeRunning == false)) {
//         uint16 totalConvTime;
//         totalConvTime = tempConvTime;
//         totalConvTime += pressConvTime[config.autoModeCfg.convRes];
//         totalConvTime *= 125;   //ADD 25% TO CONVERSION TIME
//         totalConvTime /= 100;

//         if(totalConvTime > config.autoModeCfg.repeatTime) {
//             REPORT_ERROR(AUTOMODE_TIME);
//             config.autoModeCfg.repeatTime = totalConvTime;
//         }

//         timerAutoMode.SetTaskTime(config.autoModeCfg.repeatTime);
//         timerAutoMode.SetMode(task::MODE_REPEATED);
//         timerAutoMode.Enable(true, true);
//         autoModeRunning = true;
//     }

//     else if((config.measMode == MODE_SINGLE) && (autoModeRunning == true)) {
//         timerAutoMode.Disable();
//         autoModeRunning = false;
//     }

// 	//-----------------------------------------------------------------------------------
// 	//IF AUTOMODE TRIGG NEW CONVERSION

// 	if((timerAutoMode.GetState() == task::ENABLED) && timerAutoMode.Check()) {
//         TriggConv(config.autoModeCfg.convRes);
//     }

//     //-----------------------------------------------------------------------------------
//     //GET CONVERSION RESULT IF TIME ELAPSED

//     if((timerGetConvResult.GetState() == task::ENABLED) && timerGetConvResult.Check()) {
//         timerGetConvResult.Disable();
//         GetConvResult();
//     };

// 	//-----------------------------------------------------------------------------------

//     return status;
// }

// execStat_t BMP180::TriggConv(convRes_t convRes_f)
// {
//     if(status == STAT_ERR) return EXEC_ERR;

//     //-----------------------------------------------------------------------------------
//     //CHECK FOR CONVERSION OVERRUN

//     if(timerGetConvResult.GetState() == task::ENABLED) {
//         REPORT_ERROR(CONV_OVERRUN);
//         return EXEC_ERR;
//     }

//     //-----------------------------------------------------------------------------------
//     //TRIGGER TEMPERATURE CONVERSION & ENABLE CONVERSION TIMER

//     ctrlReg.bit.meas = CMD_TRIG_TEMP_CONV;
//     ctrlReg.bit.sco = true;
//     ctrlReg.bit.oss = 0;

//     if(WriteCtrlReg(ctrlReg, false) == EXEC_ERR) return EXEC_ERR;

//     timerGetConvResult.SetTaskTime(tempConvTime);
//     timerGetConvResult.SetMode(task::MODE_ONE_SHOT);
//     timerGetConvResult.Enable(true, false);
//     convCtrl.press.convRes = convRes_f;
//     convCtrl.temp.dataReady = false;
//     convCtrl.temp.convRun = true;
//     status = STAT_CONV_RUNNING;

// 	return EXEC_OK;
// }

// //#########################################################################################################################################
// //												              PRIVATE FUNCTION's
// //#########################################################################################################################################

// execStat_t  BMP180::GetCalibData()
// {
//     uint8 rxData[22];
//     uint16* data16_p = (uint16*)rxData;

//     if(ReadData(ADDR_CALIB_DATA, rxData, sizeof(rxData)) == EXEC_ERR) return EXEC_ERR;

//     //CALIB DATA STORED IN SENSOR MEMORY AS BIG ENDIAN!
//     calibData.AC1 = uFunc::RotateBytewise(*data16_p++, sizeof(uint16));
//     calibData.AC2 = uFunc::RotateBytewise(*data16_p++, sizeof(uint16));
//     calibData.AC3 = uFunc::RotateBytewise(*data16_p++, sizeof(uint16));
// 	calibData.AC4 = uFunc::RotateBytewise(*data16_p++, sizeof(uint16));
// 	calibData.AC5 = uFunc::RotateBytewise(*data16_p++, sizeof(uint16));
// 	calibData.AC6 = uFunc::RotateBytewise(*data16_p++, sizeof(uint16));
// 	calibData.B1 = uFunc::RotateBytewise(*data16_p++, sizeof(uint16));
// 	calibData.B2 = uFunc::RotateBytewise(*data16_p++, sizeof(uint16));
// 	calibData.MB = uFunc::RotateBytewise(*data16_p++, sizeof(uint16));
// 	calibData.MC = uFunc::RotateBytewise(*data16_p++, sizeof(uint16));
// 	calibData.MD = uFunc::RotateBytewise(*data16_p, sizeof(uint16));

//     return EXEC_OK;
// }

// execStat_t BMP180::TriggPressConv()
// {
//     if(status == STAT_ERR) return EXEC_ERR;

//     //-----------------------------------------------------------------------------------
//     //CHECK FOR CONVERSION OVERRUN

//     if(timerGetConvResult.GetState() == task::ENABLED) {
//         REPORT_ERROR(CONV_OVERRUN);
//         return EXEC_ERR;
//     }

//     //-----------------------------------------------------------------------------------
//     //TRIGGER PRESSURE CONVERSION & ENABLE CONVERSION TIMER

//     ctrlReg.bit.meas = CMD_TRIG_PRESS_CONV;
//     ctrlReg.bit.sco = true;
//     ctrlReg.bit.oss = convCtrl.press.convRes;

//     if(WriteCtrlReg(ctrlReg, false) == EXEC_ERR) return EXEC_ERR;

//     timerGetConvResult.SetTaskTime(pressConvTime[convCtrl.press.convRes]);
//     timerGetConvResult.SetMode(task::MODE_ONE_SHOT);
//     timerGetConvResult.Enable(true, false);
//     convCtrl.press.dataReady = false;
//     convCtrl.press.convRun = true;
// 	return EXEC_OK;
// }

// execStat_t BMP180::GetConvResult()
// {
//     uint8 convResult[3] = {};

//     //-----------------------------------------------------------------------------------
//     //GET CONVERSION RESULT BINARY & CHECK CRC

//     if(ReadData(ADDR_RESULT_REG, convResult, sizeof(convResult)) == EXEC_ERR) return EXEC_ERR;

//     //-----------------------------------------------------------------------------------
//     //CALCULATE DECIMAL VALUES

// 	if(convCtrl.temp.convRun == true) {
//         convCtrl.temp.convRun = false;
//         measData.temp.binary = (uint16)convResult[0] << 8;
//         measData.temp.binary |= convResult[1];
//         measData.temp.unixTimestampMs = time::RtcGetUnixTime(time::UNIX_TIME_RES_ms);
//         convCtrl.temp.dataReady = true;
//         TriggPressConv();
// 	}

// 	else if(convCtrl.press.convRun == true) {
//         convCtrl.press.convRun = false;
//         measData.press.binary = (uint32)convResult[0] << 16;
//         measData.press.binary |= (uint32)convResult[1] << 8;
//         measData.press.binary |= convResult[2];
//         measData.press.binary >>= 8 - ctrlReg.bit.oss;
//         measData.press.bitRes = convCtrl.press.convRes;
//         measData.press.unixTimestampMs = time::RtcGetUnixTime(time::UNIX_TIME_RES_ms);
//         convCtrl.press.dataReady = true;
//     }

//     //-----------------------------------------------------------------------------------
//     //DATA READY HANDLING

//     if(convCtrl.temp.dataReady == true) {
//         convCtrl.temp.dataReady = false;
//         ConvTempBinToDec();
//         if(config.outputTemperature == true) config.dataReadyClbk(measData.temp.uniVal);
//     }

//     if(convCtrl.press.dataReady == true) {
//         ConvPressBinToDec();
//         CalcAltitude(measData.press.uniVal.value);
//         config.dataReadyClbk(measData.press.uniVal);

//         if(config.outputAltitude == true) config.dataReadyClbk(measData.altitude.uniVal);
//     }

//     //-----------------------------------------------------------------------------------

//     return EXEC_OK;
// }

// void BMP180::ConvTempBinToDec()
// {
// 	int64 X1;
// 	int64 X2;
//     int64 B5;
//     int32 tempDecVal;

// 	//-----------------------------------------------------------------------------------
// 	//CALCULATING TEMPERATURE PHYSICAL VALUE. FOR DETAILS REFER TO THE BOSCH BMP180 DATASHEET, PAGE 15

//     X1 = (((int64)measData.temp.binary - calibData.AC6) * calibData.AC5) / 32768;
//     X2 = ((int64)calibData.MC * 2048) / (X1 + calibData.MD);
//     B5 = X1 + X2;

//     tempDecVal = (B5 + 8) / 16;

// 	//-----------------------------------------------------------------------------------
// 	//SAVING MEASUREMENT DATA

//     measData.temp.uniVal.value = (tempDecVal < 0) ? tempDecVal * -1 : tempDecVal;
//     measData.temp.uniVal.sign = (tempDecVal < 0) ? phyUnits::NEGATIVE : phyUnits::POSITIVE;
// 	measData.temp.uniVal.commaPos = phyUnits::COMMA_POS_1;
//     measData.temp.uniVal.prefix = phyUnits::PREFIX_NONE;
//     measData.temp.uniVal.unit = phyUnits::UNIT_TEMPERATURE_C;
//     measData.temp.uniVal.unixTimestampMs = time::RtcGetUnixTime(time::UNIX_TIME_RES_ms);
// }

// void BMP180::ConvPressBinToDec()
// {
// 	int64 B3;
// 	uint64 B4;
//     int64 B5;
// 	int64 B6;
// 	uint64 B7;
// 	int64 X1;
// 	int64 X2;
// 	int64 X3;
//     int64 tempVal;
//     uint32 pressDecVal;

// 	//-----------------------------------------------------------------------------------
// 	//CALCULATING PRESSURE PHYSICAL VALUE. FOR DETAILS REFER TO THE BOSCH BMP180 DATASHEET, PAGE 15

// 	X1 = ((measData.temp.binary - calibData.AC6) * calibData.AC5) / 32768;
// 	X2 = (calibData.MC * 2048) / (X1 + calibData.MD);
// 	B5 = X1 + X2;

// 	B6 = B5 - 4000;

// 	X1 = ((int64)calibData.B2 * B6 * B6) / 8388608;
// 	X2 = ((int64)calibData.AC2 * B6) / 2048;
// 	X3 = X1 + X2;

//     B3 = (((((int64)calibData.AC1 * 4) + X3) << ctrlReg.bit.oss) + 2) / 4;

// 	X1 = ((int64)calibData.AC3 * B6) / 8192;
// 	X2 = (calibData.B1 * B6 * B6) / 268435456;
// 	X3 = (X1 + X2 + 2) / 4;

// 	B4 = ((int64)calibData.AC4 * (X3 + 32768)) / 32768;
// 	B7 = ((uint64)measData.press.binary - B3) * (50000 >> ctrlReg.bit.oss);

// 	if(B7 < 0x80000000) tempVal = (B7 * 2) / B4;
// 	else				tempVal = (B7 / B4) * 2;

// 	X1 = (tempVal * tempVal) / 65536;
// 	X1 = (X1 * 3038) / 65536;
// 	X2 = ((int64)-7357 * tempVal) / 65536;
//     pressDecVal = tempVal + ((X1 + X2 + 3791) / 16);

// 	//-----------------------------------------------------------------------------------
// 	//SAVING MEASUREMENT DATA

//     measData.press.uniVal.value = pressDecVal;
//     measData.press.uniVal.sign = phyUnits::POSITIVE;
//     measData.press.uniVal.commaPos = phyUnits::COMMA_POS_0;
//     measData.press.uniVal.prefix = phyUnits::PREFIX_NONE;
//     measData.press.uniVal.unit = phyUnits::UNIT_PRESSURE;
//     measData.press.uniVal.unixTimestampMs = time::RtcGetUnixTime(time::UNIX_TIME_RES_ms);
// }

// void BMP180::CalcAltitude(uint32 pressPascal)
// {
//     int64 a = 39;
//     int64 b = -16201721;
//     int64 c = 1241320110943;
//     int64 x1 = pressPascal;
//     int64 x2 = x1 * x1;
//     int64 altitude;

// 	//-----------------------------------------------------------------------------------
// 	//CALCULATING ALTITUDE IN [m]; FORMULA DERIVED WITH EXCEL FROM FORMULA IN THE BMP180 DATASHEET ON PAGE 16

//     altitude = (a * x2 + b * x1 + c) / (uint32)1e8;

// 	//-----------------------------------------------------------------------------------
// 	//SAVING MEASUREMENT DATA

//     measData.altitude.uniVal.value = altitude;
//     measData.altitude.uniVal.sign = (altitude < 0) ? phyUnits::NEGATIVE : phyUnits::POSITIVE;
//     measData.altitude.uniVal.commaPos = phyUnits::COMMA_POS_0;
//     measData.altitude.uniVal.prefix = phyUnits::PREFIX_NONE;
//     measData.altitude.uniVal.unit = phyUnits::UNIT_LENGTH;
//     measData.altitude.uniVal.unixTimestampMs = time::RtcGetUnixTime(time::UNIX_TIME_RES_ms);
// }

// execStat_t BMP180::SendSoftRstCmd()
// {
//     if(status == STAT_ERR) return EXEC_ERR;

//     //-----------------------------------------------------------------------------------
//     //SEND SOFTWARE RESET CMD

//     uint8 cmdData[2] = {ADDR_SOFT_RESET, CMD_SOFT_RESET};

//     if(config.i2cBus_p->TxData(BMP180_I2C_ADDRESS, cmdData, sizeof(cmdData), true, i2cTrId) == EXEC_ERR) {
//         REPORT_ERROR(I2C_BUS);
//         return EXEC_ERR;
//     }

//     //-----------------------------------------------------------------------------------

//     return EXEC_OK;
// }

// execStat_t BMP180::WriteCtrlReg(ctrlReg_t& ctrlReg_f, bool waitTillCmpl_f)
// {
//     if(status == STAT_ERR) return EXEC_ERR;

//     uint8 txData[2] = {ADDR_CONTROL_REG, ctrlReg_f.regVal};

//     if(config.i2cBus_p->TxData(BMP180_I2C_ADDRESS, txData, sizeof(txData), waitTillCmpl_f, i2cTrId) == EXEC_ERR) {
//         REPORT_ERROR(I2C_BUS);
//         return EXEC_ERR;
//     }

//     return EXEC_OK;
// }

// //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

// execStat_t BMP180::ReadData(memAddr_t memAddr_f, uint8* rxData_fp, uint8 rxDataSize_f)
// {
//     if(status == STAT_ERR) return EXEC_ERR;

//     //-----------------------------------------------------------------------------------
//     //READ DATA

//     if(config.i2cBus_p->TxData(BMP180_I2C_ADDRESS, (uint8*)&memAddr_f, sizeof(memAddr_t), true, i2cTrId) == EXEC_ERR) {
//         REPORT_ERROR(I2C_BUS);
//         return EXEC_ERR;
//     }

// 	if(config.i2cBus_p->RxData(BMP180_I2C_ADDRESS, rxData_fp, rxDataSize_f, true, i2cTrId) == EXEC_ERR) {
//         REPORT_ERROR(I2C_BUS);
//         return EXEC_ERR;
//     }

//     //-----------------------------------------------------------------------------------

//     return EXEC_OK;
// }

// //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

// void BMP180::ReportError(errCode_t errCode_f, const uint8* fileName_fp, uint16 codeLineNum_f)
// {
//     const uint8 moduleName[MODULE_NAME_MAX_LENGTH] = "SENS BMP180 DRV";

//     #define ERR(errName) #errName,
//     const char* errCodeStr[] = {
//         BMP180_ERR_CODES
//     };
//     #undef ERR

//     //-----------------------------------------------------------------------------------
//     //SET ERROR STATE

//     convCtrl = {};
//     status = STAT_ERR;
//     errCode = errCode_f;

//     //-----------------------------------------------------------------------------------
//     //REPORT ERROR

//     errHand::errData_t errData;
//     errData.softModuleName_p    = (uint8*)moduleName;
//     errData.errCode             = (uint8)errCode_f;
//     errData.errCodeStr_p        = (uint8*)errCodeStr[errCode_f];
//     errData.fileName            = (uint8*)fileName_fp;
//     errData.codeLineNum         = codeLineNum_f;
//     errHand::ReportError(errData);
// }

// //#########################################################################################################################################
// //															    END OF FILE
// //#########################################################################################################################################
// }//END OF NAMESPACE 'drv::sensBmp180'