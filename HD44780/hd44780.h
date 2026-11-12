#pragma once
#include "common.h"
#include "timer.h"

namespace drv {

class HD44780 {
public:

enum ShiftDir {
    SHIFT_R,
    SHIFT_L,
};

using SetLevel_t = void (*)(bool);
using GetLevel_t = bool (*)();
using SysTimeUs_t = uint64 (*)();

    struct config_t {
        SetLevel_t SetEN;
        SetLevel_t SetRS;
        SetLevel_t SetRW;
        SetLevel_t SetD4;
        SetLevel_t SetD5;
        SetLevel_t SetD6;
        SetLevel_t SetD7;
        GetLevel_t GetD7;
        SysTimeUs_t SysTimeUs;
    };

    Exec  Setup(const config_t& config);
    void  Cls();
    void  DispSign(char sign);
    void  DispText(const char *text_p);
    void  DispTextVA(const char* text_p...);
    Exec  DispVal(int64 int64Val, bool addSign, uint8 dotPos, uint8 digits, char* unit_p);
    void  SetShiftDir(ShiftDir dir);
    void  MoveCursor(uint8 line, uint8 position);
    void  ClrPositions(uint8 line, uint8 startPos, uint8 numOfPos);

private:

    static constexpr uint16 BF_MAX_CHECK_CNT = 1000;
    static constexpr uint16 ROW_MAX_SIGN_CNT = 40;

	// HD44780 DRIVER COMMANDS
    enum Cmd {
        CMD_CURSOR_SHIFTR   = 0b00000110,
        CMD_CURSOR_SHIFTL   = 0b00000100,
        CMD_CLS             = 0b00000001,
        CMD_SET_DDRAM_ADDR  = 0b10000000,
    };

    enum RegType {
        REG_INSTR = 0,
        REG_DATA = 1,
    };

    enum OpType {
        OP_WRITE = 0,
        OP_READ = 1,
    };

    void SendCommand(uint8 command);
    void SendByte(RegType reg, uint8 command);
    void SendByte(uint8 byte);
    bool GetBusyFlag();
    Exec WriteDataToFifo(RegType regType, OpType opType, uint8 dataSize, uint8* data_p);
    Exec GetFifoEntry();
    void DelayUs(uint16);

    struct {
        config_t config;
        Status status;
        State state;
        ShiftDir currentShiftDir;
        bool rwLevel;
        bool rsLevel;
        bool clsCmd;
    } ctx = {};
};

} // End of namespace 'drv'
