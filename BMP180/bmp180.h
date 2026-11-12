//#########################################################################################################################################
//															  START OF FILE
//#########################################################################################################################################

#pragma once
// #include "mcu.h"
// #include "globals.h"
// #include "physicalUnits.h"
// #include "time.h"
// #include "task.h"
// #include "i2cBus.h"

// namespace drv::sensBmp180{
// //#########################################################################################################################################
// //											      PUBLIC DEFINE's, MACRO's, ENUM's & TYPE's
// //#########################################################################################################################################

// #define BMP180_ERR_CODES        \
//         ERR(NONE)               \
//         ERR(NULLPTR)            \
//         ERR(ID_MISMATCH)        \
//         ERR(AUTOMODE_TIME)      \
//         ERR(CONV_OVERRUN)       \
//         ERR(I2C_BUS)

// #define ERR(errName) errName,
// enum errCode_t : uint8 {
//     BMP180_ERR_CODES
// };
// #undef ERR

// enum status_t : uint8 {
//     STAT_IDLE,
//     STAT_CONV_RUNNING,
//     STAT_ERR,
// };

// enum convRes_t : uint8 {
//     CONV_RES_16BIT = 0b00,                                              //RESOLUTION 0.02hPa; CONVERSION TIME 8ms
//     CONV_RES_17BIT = 0b01,                                              //RESOLUTION 0.01hPa; CONVERSION TIME 14ms
//     CONV_RES_18BIT = 0b10,                                              //RESOLUTION 0.005hPa; CONVERSION TIME 26ms
//     CONV_RES_19BIT = 0b11,                                              //RESOLUTION 0.0025hPa; CONVERSION TIME 77ms
// };

// enum measMode_t : uint8 {
//     MODE_SINGLE,                                                        //EVERY CONVERSION HAS TO BE TRIGGERED
//     MODE_AUTO,                                                          //TIMED CONTINOUS MEASUREMENT MODE
// };

// using dataReadyClbk_t = void (*)(phyUnits::uniVal_t& measValue);

// struct config_t {
//     bus::i2c::I2C*      i2cBus_p;
//     measMode_t          measMode;
//     dataReadyClbk_t     dataReadyClbk;
//     bool                outputTemperature;
//     bool                outputAltitude;

//     struct {
//         convRes_t   convRes;
//         uint32      repeatTime;
//     }autoModeCfg;
// };

// //#########################################################################################################################################
// //										                        PUBLIC CLASSES
// //#########################################################################################################################################

// class BMP180 {
// //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// public:

//     //-----------------------------------------------------------------------------------
//     //FUNCTION's

// 	initStat_t  Init(const config_t& config);
//     status_t    Handler();
// 	execStat_t  TriggConv(convRes_t convRes);

// //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// private:

//     //-----------------------------------------------------------------------------------
//     //DATA TYPES's

//     enum memAddr_t {
//         ADDR_CALIB_DATA     = 0xAA,
//         ADDR_RESULT_REG     = 0xF6,
//         ADDR_CONTROL_REG    = 0xF4,
//         ADDR_SOFT_RESET     = 0xE0,
//         ADDR_CHIP_ID_REG    = 0xD0,
//         ADDR_CHIP_VER_REG   = 0xD1,
//     };

//     enum cmd_t {
//         CMD_SOFT_RESET      = 0xB6,                                     //TRIGER SOFTWARE RESET
//         CMD_TRIG_TEMP_CONV  = 0x0E,                                     //TRIGGER TEMPRATURE CONVERSION
//         CMD_TRIG_PRESS_CONV = 0x14,                                     //TRIGGER PRESSURE CONVERSION
//     };

//     union __PACKED ctrlReg_t {
//         struct {
//             uint8 meas  : 5;
//             uint8 sco   : 1;
//             uint8 oss   : 2;
//         }bit;
//         uint8 regVal;
//     };

//     //-----------------------------------------------------------------------------------
//     //FUNCTIONS's

//     execStat_t  GetCalibData();
//     execStat_t  TriggPressConv();
//     execStat_t  GetConvResult();
//     execStat_t  SendSoftRstCmd();
//     execStat_t  WriteCtrlReg(ctrlReg_t& ctrlReg_f, bool waitTillCmpl_f);
//     execStat_t  ReadData(memAddr_t memAddr, uint8* rxData, uint8 rxDataSize);
//     void        ConvTempBinToDec();
//     void        ConvPressBinToDec();
//     void        CalcAltitude(uint32 pressPascal);
//     void        ReportError(errCode_t errCode, const uint8* fileName_p, uint16 codeLineNum);

//     //-----------------------------------------------------------------------------------
//     //VARIABLE's

// 	status_t            status;
//     config_t            config;
//     errCode_t           errCode;
//     bus::i2c::trId_t    i2cTrId;
//     ctrlReg_t           ctrlReg;
//     uint8               chipId;
//     uint8               chipVer;
//     bool                autoModeRunning;
// 	task::TASK          timerAutoMode;
//     task::TASK          timerGetConvResult;

//     struct {
//         struct {
//             uint16                  binary;
//             time::unixTime_t        unixTimestampMs;
//             phyUnits::uniVal_t      uniVal;
//         }temp;

//         struct {
//             uint32                  binary;
//             convRes_t               bitRes;
//             time::unixTime_t        unixTimestampMs;
//             phyUnits::uniVal_t      uniVal;
//         }press;

//         struct {
//             time::unixTime_t        unixTimestampMs;
//             phyUnits::uniVal_t      uniVal;
//         }altitude;

//     }measData = {};

//     struct convCtrl_t {
//         struct {
//             bool convRun;
//             bool dataReady;
//         }temp;

//         struct {
//             bool convRun;
//             bool dataReady;
//             convRes_t convRes;
//         }press;
//     }convCtrl = {};


// 	//SENSOR CALIBRATION DATA -> FOR DETAIL READ BMP180 DATASHEET
// 	struct {
//         int16 AC1;
//         int16 AC2;
//         int16 AC3;
//         uint16 AC4;
//         uint16 AC5;
//         uint16 AC6;
//         int16 B1;
//         int16 B2;
//         int16 MB;
//         int16 MC;
//         int16 MD;
//     }calibData = {};
// };

// //#########################################################################################################################################
// //															    END OF FILE
// //#########################################################################################################################################
// }//END OF NAMESPACE 'drv::sensBmp180'