/******************************************************************************************************************************************
                                                               START OF FILE
******************************************************************************************************************************************/

#include "hd44780.h"
#include "utils.h"

#undef CODE_STATUS
#define CODE_STATUS ctx.status

namespace drv {

#define RS_PIN_HIGH		{ ctx.config.SetRS(1); ctx.rsLevel = 1; }
#define RS_PIN_LOW      { ctx.config.SetRS(0); ctx.rsLevel = 0; }

#define RW_PIN_HIGH		{ ctx.config.SetRW(1); ctx.rwLevel = 1; }
#define RW_PIN_LOW      { ctx.config.SetRW(0); ctx.rwLevel = 0; }

#define EN_PIN_HIGH		ctx.config.SetEN(1)
#define EN_PIN_LOW      ctx.config.SetEN(0)

#define D4_PIN_HIGH		ctx.config.SetD4(1)
#define D4_PIN_LOW      ctx.config.SetD4(0)

#define D5_PIN_HIGH		ctx.config.SetD5(1)
#define D5_PIN_LOW      ctx.config.SetD5(0)

#define D6_PIN_HIGH		ctx.config.SetD6(1)
#define D6_PIN_LOW      ctx.config.SetD6(0)

#define D7_PIN_HIGH		ctx.config.SetD7(1)
#define D7_PIN_LOW      ctx.config.SetD7(0)
#define D7_PIN_GET      ctx.config.GetD7()

#define BUSY_FLAG_GET	D7_PIN_GET

Exec HD44780::Setup(const config_t& config)
{
    ASSERT(config.SetEN != nullptr, "Set EN line not configured!");
    ASSERT(config.SetRS != nullptr, "Set RS line not configured!");
    ASSERT(config.SetRW != nullptr, "Set RW line not configured!");
    ASSERT(config.SetD4 != nullptr, "Set D4 line not configured!");
    ASSERT(config.SetD5 != nullptr, "Set D5 line not configured!");
    ASSERT(config.SetD6 != nullptr, "Set D6 line not configured!");
    ASSERT(config.SetD7 != nullptr, "Set D7 line not configured!");
    ASSERT(config.SysTimeUs != nullptr, "'SysTimeUs' not configured!");

    ctx.config = config;
    ctx.status = Status::INIT;

    //-----------------------------------------------------------------------------------
    // CLEARING ALL DATA PINS

    RS_PIN_LOW;
    RW_PIN_LOW;
    EN_PIN_LOW;
    D4_PIN_LOW;
    D5_PIN_LOW;
    D6_PIN_LOW;
    D7_PIN_LOW;

    //-----------------------------------------------------------------------------------
    // INITIALIZING THE HD44780 DRIVER ACCORDING TO DATASHEET PAGE 46

    EN_PIN_HIGH;
    D4_PIN_HIGH;
    D5_PIN_HIGH;
    EN_PIN_LOW;

    DelayUs(5000);

    EN_PIN_HIGH;
    D4_PIN_HIGH;
    D5_PIN_HIGH;
    EN_PIN_LOW;

    DelayUs(100);

    EN_PIN_HIGH;
    D4_PIN_HIGH;
    D5_PIN_HIGH;
    EN_PIN_LOW;

    EN_PIN_HIGH;
    D5_PIN_HIGH;
    D4_PIN_LOW;
    EN_PIN_LOW;

    DelayUs(1000);

    //-----------------------------------------------------------------------------------
    // STANDARD SETTINGS

    SendCommand(0b00101000);    // 4BIT-INTERFACE, TWO LINE DISPLAY, SIGNS 5X8 POINT
    SendCommand(0b00001100);    // LCD ON, CURSOR OFF, CURSOR BLINKING OFF
    SendCommand(CMD_CLS);
    SetShiftDir(SHIFT_R);

    //-----------------------------------------------------------------------------------

    ctx.status = Status::OK;
    return Exec::OK;
}

void HD44780::Cls(void)
{
    SendCommand(CMD_CLS);
}

void HD44780::DispSign(char sign)
{
    SendByte(REG_DATA, sign);
}

void HD44780::DispText(const char *text_p)
{
    char* alloc_p = nullptr;
    uint8 textLen = (uint8)strlen(text_p);
    if (ctx.currentShiftDir == SHIFT_L) {
        alloc_p = (char*)calloc(textLen + 1, sizeof(char));
        ASSERT(alloc_p != nullptr, "Memory allocation failed!");
        utils::RotateStr((char*)text_p, alloc_p);
    }
    char* str_p = alloc_p ? alloc_p : (char*)text_p;
    while (textLen--) SendByte(REG_DATA, *str_p++);
    if (alloc_p) free(alloc_p);
}

void HD44780::DispTextVA(const char *text_p...)
{
    char str[ROW_MAX_SIGN_CNT + 1];
    va_list args;
    va_start(args, text_p);
    vsnprintf(str, sizeof(str), text_p, args);
    if (ctx.currentShiftDir == SHIFT_L) utils::RotateStr(str, (char*)str);
    uint8 textLen = (uint8)strlen(str);
    const char* str_p = str;
    while (textLen--) SendByte(REG_DATA, *str_p++);
}

Exec HD44780::DispVal(int64 int64Val, bool addSign, uint8 dotPos, uint8 digits, char* unit_p)
{
    char valStr[20];
    CHECK_EXEC(utils::ValToStr(int64Val, addSign, dotPos, digits, unit_p, valStr));
    DispText(valStr);
    return Exec::OK;
}

void HD44780::SetShiftDir(ShiftDir dir)
{
    if (ctx.currentShiftDir != dir) {
        uint8 shiftDirCmd;
        if(dir == SHIFT_R)  shiftDirCmd = CMD_CURSOR_SHIFTR;
        else                shiftDirCmd = CMD_CURSOR_SHIFTL;
        SendCommand(shiftDirCmd);
        ctx.currentShiftDir = dir;
    }
}

void HD44780::MoveCursor(uint8 line, uint8 position)
{
    if (line == 0) {
        REPORT_ERR("Line must be equel/greater 1");
        line = 1;
    }
    if (position == 0 || position > ROW_MAX_SIGN_CNT) {
        REPORT_ERR("Line position out off range -> %d (min=1, max=%d)!", position, ROW_MAX_SIGN_CNT);
        if (position == 0) position = 1;
        else               position = ROW_MAX_SIGN_CNT;
    }

    uint8 cmd = 0;

    switch(line) {
        case 1:	cmd = CMD_SET_DDRAM_ADDR | (position - 0x01); break;
        case 2: cmd = CMD_SET_DDRAM_ADDR | (position + 0x3F); break;
        case 3:	cmd = CMD_SET_DDRAM_ADDR | (position + 0x13); break;
        case 4:	cmd = CMD_SET_DDRAM_ADDR | (position + 0x53);
    }
    SendCommand(cmd);
}

void HD44780::ClrPositions(uint8 line, uint8 startPos, uint8 numOfPos)
{
    if (line == 0) {
        REPORT_ERR("Line must be equel/greater 1");
        line = 1;
    }
    if (startPos == 0 || startPos > ROW_MAX_SIGN_CNT) {
        REPORT_ERR("Line position out off range -> %d (min=1, max=%d)!", startPos, ROW_MAX_SIGN_CNT);
        if (startPos == 0) startPos = 1;
        else               startPos = ROW_MAX_SIGN_CNT;
    }

    uint8 singCnt = numOfPos;
    MoveCursor(line, ctx.currentShiftDir == SHIFT_R ? startPos : (uint8)(startPos + numOfPos - 1));
    while(singCnt--) SendByte(REG_DATA, ' ');
}

/******************************************************************************************************************************************
                                                              PRIVATE FUNCTIONS
******************************************************************************************************************************************/

void HD44780::SendCommand(uint8 command)
{
    SendByte(REG_INSTR, command);
}

void HD44780::SendByte(RegType reg, uint8 byte)
{
    //-----------------------------------------------------------------------------------
    // Check if lcd busy flag

    if (GetBusyFlag()) {
        uint8 busyCnt = BF_MAX_CHECK_CNT;
        while (GetBusyFlag()) {
            if (!busyCnt--) {
                REPORT_ERR("Busy flag timeout!");
                return;
            }
        }
    }

    //-----------------------------------------------------------------------------------
    // Set control lines

    if (reg == REG_INSTR)   { RS_PIN_LOW; }
    else                    { RS_PIN_HIGH; }
    RW_PIN_LOW;

    //-----------------------------------------------------------------------------------
    // WRITING THE UPPER NIBBLE

    uint8 upperNible = byte >> 4;

    EN_PIN_HIGH;
    if (upperNible & 1)			D4_PIN_HIGH;
    else						D4_PIN_LOW;
    if (upperNible & (1 << 1))	D5_PIN_HIGH;
    else						D5_PIN_LOW;
    if (upperNible & (1 << 2))	D6_PIN_HIGH;
    else                        D6_PIN_LOW;
    if (upperNible & (1 << 3))  D7_PIN_HIGH;
    else                        D7_PIN_LOW;
    EN_PIN_LOW;

    //-----------------------------------------------------------------------------------
    // WRITING THE LOWER NIBBLE

    EN_PIN_HIGH;
    if (byte & 1)           D4_PIN_HIGH;
    else                    D4_PIN_LOW;
    if (byte & (1 << 1))    D5_PIN_HIGH;
    else                    D5_PIN_LOW;
    if (byte & (1 << 2))    D6_PIN_HIGH;
    else                    D6_PIN_LOW;
    if (byte & (1 << 3))    D7_PIN_HIGH;
    else                    D7_PIN_LOW;
    EN_PIN_LOW;
}

bool HD44780::GetBusyFlag()
{
    bool busyFlag;
    RS_PIN_LOW;
    RW_PIN_HIGH;
	EN_PIN_HIGH;
    busyFlag = BUSY_FLAG_GET;
    EN_PIN_LOW;
	EN_PIN_HIGH;
    EN_PIN_LOW;
    return busyFlag;
}

void HD44780::DelayUs(uint16 us)
{
    uint64 tickCntTarget = ctx.config.SysTimeUs() + us;
    while (ctx.config.SysTimeUs() < tickCntTarget);
}

} // End of namespace 'drv'
/******************************************************************************************************************************************
                                                                END OF FILE
******************************************************************************************************************************************/
