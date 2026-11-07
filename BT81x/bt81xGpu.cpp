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
// CREATION DATE:    March 2024
// DESCRIPTION:      Bridgetek's BT81x display driver gpu functions

/******************************************************************************************************************************************
                                                                  INCLUDES
******************************************************************************************************************************************/

#include "bt81xGpu.h"
#include "utils.h"

/******************************************************************************************************************************************
                                                              TYPES & DEFINES
******************************************************************************************************************************************/
namespace drv {

#undef CODE_LABEL
#undef CODE_STATUS
#define CODE_LABEL      "BT81x GPU"
#define CODE_STATUS     ctx.status

/******************************************************************************************************************************************
                                                         PUBLIC FUNCTION DEFINITIONS
******************************************************************************************************************************************/

float ArcStepDeg(float radius, float thickness)
{
    return ((57.3f * 0.1f * thickness) / radius);
}

static void CHECK_DISP_LIST_OVF(dlist_t& dlist)
{
    ASSERT(dlist.listEnd_p <= dlist.buffEnd_p, "Display list overflow!");
}

Exec BT81x_GPU::Setup(const config_t& config)
{
    ASSERT(config.dlSize > 100, "Invalid display list size (MIN = 100)!");
    ASSERT(config.vSize > 0, "Invalid screen height!");
    ASSERT(config.hSize > 0, "Invalid screen length!");

    //-----------------------------------------------------------------------------------
    // Allocate memory for display list buffer

    ctx.dlist.buff_p = (uint32*)calloc(config.dlSize, sizeof(uint32));
    ASSERT(ctx.dlist.buff_p != nullptr, "Display list buff alloc failed!");

    ctx.dlist.buffSizeCmd = config.dlSize;
    ctx.dlist.buffSizeByte = ctx.dlist.buffSizeCmd * sizeof(uint32);
    ctx.dlist.listEnd_p = ctx.dlist.buff_p;
    ctx.dlist.buffEnd_p = ctx.dlist.buff_p + config.dlSize;

    //-----------------------------------------------------------------------------------

    ctx.config = config;
    ctx.status = Status::OK;
    return Exec::OK;
}

Exec BT81x_GPU::StartNewScreen(color_t backgroundColor)
{
    CHECK_STATUS();
    ctx.dlist.listSizeByte = 0;
    ctx.dlist.listSizeCmd = 0;
    ctx.dlist.listEnd_p = ctx.dlist.buff_p;
    *ctx.dlist.listEnd_p++ = GPU_CMD_DLSTART;
    CHECK_DISP_LIST_OVF(ctx.dlist);
    CHECK_EXEC(ClearColor(backgroundColor));
    CHECK_EXEC(CmdClear(true, true, true));
    ctx.dlist.listStartOk = true;
    ctx.dlist.listCloseOk = false;
    ctx.currentTag = TOUCH_TAG_RESET_VAL;
    return Exec::OK;
}

void BT81x_GPU::SetTouch(const touch_t& touch)
{
    ctx.touch = touch;
}

Exec BT81x_GPU::EndScreen()
{
    CHECK_STATUS();
    ASSERT(ctx.dlist.listStartOk == true, "Display list not started!");
    *ctx.dlist.listEnd_p++ = GPU_CMD_DISPLAY;
    *ctx.dlist.listEnd_p++ = GPU_CMD_SWAP;
    CHECK_DISP_LIST_OVF(ctx.dlist);
    ctx.dlist.listSizeCmd = (uint16)(ctx.dlist.listEnd_p - ctx.dlist.buff_p);
    ctx.dlist.listSizeByte = ctx.dlist.listSizeCmd * sizeof(uint32);
    if (ctx.dlist.listSizeByte > ctx.dlist.listSizeByteMax) {
        ctx.dlist.listSizeByteMax = ctx.dlist.listSizeByte;
    }
    ctx.dlist.listCloseOk = true;
    return Exec::OK;
}

dlist_t& BT81x_GPU::GetDisplayList()
{
    CHECK_STATUS();
    ASSERT(ctx.dlist.listCloseOk == true, "Display list not closed!");
    return ctx.dlist;
}

uint16  BT81x_GPU::GetDisplayHeight()
{
    return ctx.config.vSize;
}

uint16  BT81x_GPU::GetDisplayWidth()
{
    return ctx.config.hSize;
}

Exec BT81x_GPU::CmdClear(bool color, bool stencil, bool tag)
{
    CHECK_STATUS();
    *ctx.dlist.listEnd_p++ = GPU_CMD_CLEAR(color, stencil, tag);
    CHECK_DISP_LIST_OVF(ctx.dlist);
    return Exec::OK;
}

Exec BT81x_GPU::CmdTouchTag(uint8& touchTag)
{
    CHECK_STATUS();
    touchTag = ctx.currentTag++;
    *ctx.dlist.listEnd_p++ = GPU_CMD_TAG(touchTag);
    CHECK_DISP_LIST_OVF(ctx.dlist);
    return Exec::OK;
}

Exec BT81x_GPU::ClearTouchTag()
{
    CHECK_STATUS();
    *ctx.dlist.listEnd_p++ = GPU_CMD_TAG(0);
    CHECK_DISP_LIST_OVF(ctx.dlist);
    return Exec::OK;
}

Exec BT81x_GPU::CmdCalibTouch()
{
    CHECK_STATUS();
    *ctx.dlist.listEnd_p++ = GPU_CMD_CALIBRATE;
    CHECK_DISP_LIST_OVF(ctx.dlist);
    return Exec::OK;
}

Exec BT81x_GPU::CmdDispLogo()
{
    CHECK_STATUS();
    *ctx.dlist.listEnd_p++ = GPU_CMD_LOGO;
    CHECK_DISP_LIST_OVF(ctx.dlist);
    return Exec::OK;
}

Exec BT81x_GPU::ClearColor(color_t color)
{
    CHECK_STATUS();
    RET_EXEC(ClearColorRgb(color.r, color.g, color.b));
}

Exec BT81x_GPU::ClearColorRgb(uint8 red, uint8 green, uint8 blue)
{
    CHECK_STATUS();
    *ctx.dlist.listEnd_p++ = GPU_CMD_CLEAR_COLOR_RGB(red, green, blue);
    CHECK_DISP_LIST_OVF(ctx.dlist);
    return Exec::OK;
}

Exec BT81x_GPU::CmdColor(color_t color)
{
    CHECK_STATUS();
    RET_EXEC(CmdColorRgb(color.r, color.g, color.b));
}

Exec BT81x_GPU::CmdColorRgb(uint8 red, uint8 green, uint8 blue)
{
    CHECK_STATUS();
    static color_t colorLast;
    if (colorLast.r == red && colorLast.g == green && colorLast.b == blue) {
        return Exec::OK;
    }
    *ctx.dlist.listEnd_p++ = GPU_CMD_COLOR_RGB(red, green, blue);
    CHECK_DISP_LIST_OVF(ctx.dlist);
    colorLast.r = red;
    colorLast.g = green;
    colorLast.b = blue;
    return Exec::OK;
}

Exec BT81x_GPU::CmdBgColor(color_t color)
{
    CHECK_STATUS();
    RET_EXEC(CmdBgColorRgb(color.r, color.g, color.b));
}

Exec BT81x_GPU::CmdBgColorRgb(uint8 red, uint8 green, uint8 blue)
{
    CHECK_STATUS();
    *ctx.dlist.listEnd_p++ = GPU_CMD_BGCOLOR;
    *ctx.dlist.listEnd_p++ = (uint32)red << 16 | (uint32)green << 8 | blue;
    CHECK_DISP_LIST_OVF(ctx.dlist);
    return Exec::OK;
}

Exec BT81x_GPU::CmdFgColor(color_t color)
{
    CHECK_STATUS();
    RET_EXEC(CmdFgColorRgb(color.r, color.g, color.b));
}

Exec BT81x_GPU::CmdFgColorRgb(uint8 red, uint8 green, uint8 blue)
{
    CHECK_STATUS();
    *ctx.dlist.listEnd_p++ = GPU_CMD_FGCOLOR;
    *ctx.dlist.listEnd_p++ = (uint32)red << 16 | (uint32)green << 8 | blue;
    CHECK_DISP_LIST_OVF(ctx.dlist);
    return Exec::OK;
}

Exec BT81x_GPU::CmdText(int16 xPos, int16 yPos, gpuFont_t font, gpuOpt_t option, const char* text_p)
{
    CHECK_STATUS();
    uint8   romSlotNum = 0;
    *ctx.dlist.listEnd_p++ = GPU_CMD_ROMFONT;
    *ctx.dlist.listEnd_p++ = romSlotNum;
    *ctx.dlist.listEnd_p++ = font;
    *ctx.dlist.listEnd_p++ = GPU_CMD_TEXT;
    *ctx.dlist.listEnd_p++ = ((uint32)yPos << 16) | (xPos & 0xFFFF);
    *ctx.dlist.listEnd_p++ = ((uint32)option << 16) | (romSlotNum & 0xFFFF);
    CHECK_DISP_LIST_OVF(ctx.dlist);
    AppendText(text_p);
    return Exec::OK;
}

Exec BT81x_GPU::CmdText(const text_t& text)
{
    CHECK_STATUS();
    CHECK_EXEC(CmdColor(text.color));
    uint8 romSlotNum = 0;
    *ctx.dlist.listEnd_p++ = GPU_CMD_ROMFONT;
    *ctx.dlist.listEnd_p++ = romSlotNum;
    *ctx.dlist.listEnd_p++ = text.font;
    *ctx.dlist.listEnd_p++ = GPU_CMD_TEXT;
    *ctx.dlist.listEnd_p++ = ((uint32)text.yPos << 16) | (text.xPos & 0xFFFF);
    *ctx.dlist.listEnd_p++ = ((uint32)text.option << 16) | (romSlotNum & 0xFFFF);
    CHECK_DISP_LIST_OVF(ctx.dlist);
    AppendText(text.text_p);
    return Exec::OK;
}

     Exec    CmdText();

Exec BT81x_GPU::CmdButton(button_t& button)
{
    CHECK_STATUS();
    ASSERT(button.text_p != nullptr, "BUTTON CODE_LABEL POINTER");
    ASSERT(*button.text_p != 0, "BUTTON CODE_LABEL STARTS WITH NULL");
    ASSERT((button.gpuOpt == GPU_OPT_3D) || (button.gpuOpt == GPU_OPT_FLAT), "BUTTON INVALID GPU OPTION");

    //-----------------------------------------------------------------------------------

    uint8               romSlotNum = 0;
    lcdPos_t            lcdPos = {};
    drv::bt81x::color_t buttonColor;

    //-----------------------------------------------------------------------------------
    // Setting button color, text color & touch tag

    if (button.lockTouch)                                           buttonColor = button.colorOff;
    else if (ctx.touch.active && ctx.touch.tag == button.touchTag)  buttonColor = button.colorOn;
    else                                                            buttonColor = button.colorOff;

    CHECK_EXEC(CmdTouchTag(button.touchTag));
    CHECK_EXEC(CmdFgColor(buttonColor));
    CHECK_EXEC(CmdColor(button.textColor));

    //-----------------------------------------------------------------------------------
    // Adding button commands to display list

    AlignWidget(WIDGET_BUTTON, (void*)&button, lcdPos);
    *ctx.dlist.listEnd_p++ = GPU_CMD_ROMFONT;
    *ctx.dlist.listEnd_p++ = romSlotNum;
    *ctx.dlist.listEnd_p++ = button.textFont;
    *ctx.dlist.listEnd_p++ = GPU_CMD_BUTTON;
    *ctx.dlist.listEnd_p++ = ((uint32)lcdPos.y << 16) | (lcdPos.x & 0xffff);
    *ctx.dlist.listEnd_p++ = ((uint32)button.height << 16) | (button.width & 0xffff);
    *ctx.dlist.listEnd_p++ = ((uint32)button.gpuOpt << 16) | (romSlotNum & 0xffff);
    CHECK_DISP_LIST_OVF(ctx.dlist);
    AppendText(button.text_p);
    CHECK_EXEC(ClearTouchTag());

    //-----------------------------------------------------------------------------------

    return Exec::OK;
}

Exec BT81x_GPU::CmdKeyboard(keyboard_t& keyboard, uint8 touchTag)
{
    CHECK_STATUS();
    ASSERT((keyboard.gpuOpt == GPU_OPT_3D) || (keyboard.gpuOpt == GPU_OPT_FLAT), "Invalid GPU option!");

    //-----------------------------------------------------------------------------------
    // Adding keyboard GPU commands to display list

    lcdPos_t lcdPos = {};
    uint8 romSlotNum = 0;
    uint8 keyPressed = (touchTag >= '!' && touchTag <= '~') ? touchTag : 0;

    CHECK_EXEC(CmdFgColor(keyboard.keyColor));
    CHECK_EXEC(CmdColor(keyboard.textColor));
    AlignWidget(WIDGET_KEYBOARD, (void*)&keyboard, lcdPos);
    *ctx.dlist.listEnd_p++ = GPU_CMD_ROMFONT;
    *ctx.dlist.listEnd_p++ = romSlotNum;
    *ctx.dlist.listEnd_p++ = keyboard.textFont;
    *ctx.dlist.listEnd_p++ = GPU_CMD_KEYS;
    *ctx.dlist.listEnd_p++ = ((uint32)lcdPos.y << 16) | (lcdPos.x & 0xffff);
    *ctx.dlist.listEnd_p++ = ((uint32)keyboard.height << 16) | (keyboard.width & 0xffff);
    *ctx.dlist.listEnd_p++ = ((uint32)(keyPressed | keyboard.gpuOpt) << 16) | (romSlotNum & 0xffff);
    CHECK_DISP_LIST_OVF(ctx.dlist);
    AppendText(keyboard.keyText_p);
    return Exec::OK;
}

Exec BT81x_GPU::CmdToggle(toggle_t& toggle)
{
    CHECK_STATUS();
    ASSERT((toggle.gpuOpt == GPU_OPT_3D) || (toggle.gpuOpt == GPU_OPT_FLAT), "TOGGLE INVALID GPU OPTION");

    drv::bt81x::color_t barColor;

    if (toggle.state == State::ON)  barColor = toggle.barColorOn;
    else                            barColor = toggle.barColorOff;

    CHECK_EXEC(CmdFgColor(toggle.knobColor));
    CHECK_EXEC(CmdBgColor(barColor));
    CHECK_EXEC(CmdColor(toggle.textColor));
    CHECK_EXEC(CmdTouchTag(toggle.touchTag));

    uint8 romSlotNum = 0;
    lcdPos_t lcdPos = {};
    uint16 state = toggle.state == State::OFF ? 0 : UINT16_MAX;
    AlignWidget(WIDGET_TOGGLE, (void*)&toggle, lcdPos);
    *ctx.dlist.listEnd_p++ = GPU_CMD_ROMFONT;
    *ctx.dlist.listEnd_p++ = romSlotNum;
    *ctx.dlist.listEnd_p++ = toggle.textFont;
    *ctx.dlist.listEnd_p++ = GPU_CMD_TOGGLE;
    *ctx.dlist.listEnd_p++ = ((uint32)lcdPos.y << 16) | (lcdPos.x & 0xffff);
    *ctx.dlist.listEnd_p++ = ((uint32)toggle.textFont << 16) | (toggle.width & 0xffff);
    *ctx.dlist.listEnd_p++ = ((uint32)state << 16) | (toggle.gpuOpt & 0xffff);
    CHECK_DISP_LIST_OVF(ctx.dlist);
    if (toggle.text_p) AppendText(toggle.text_p);
    CHECK_EXEC(ClearTouchTag());
    return Exec::OK;
}

Exec BT81x_GPU::CmdSlider(slider_t& slider)
{
    CHECK_STATUS();
    ASSERT((slider.gpuOpt == GPU_OPT_3D) || (slider.gpuOpt == GPU_OPT_FLAT), "Invalid GPU option!");

    //-----------------------------------------------------------------------------------
    // Set slider knob kolor & bar color

    CHECK_EXEC(CmdFgColor(slider.knobColor));
    CHECK_EXEC(CmdColor(slider.barColorLeft));
    CHECK_EXEC(CmdBgColor(slider.barColorRight));

    //-----------------------------------------------------------------------------------
    // Adding slider to display list

    lcdPos_t lcdPos = {};
    AlignWidget(WIDGET_SLIDER, (void*)&slider, lcdPos);
    CHECK_EXEC(CmdTouchTag(slider.touchTag));
    *ctx.dlist.listEnd_p++ = GPU_CMD_SLIDER;
    *ctx.dlist.listEnd_p++ = ((uint32)lcdPos.y << 16) | (lcdPos.x & 0xffff);
    *ctx.dlist.listEnd_p++ = ((uint32)slider.height << 16) | (slider.length & 0xffff);
    *ctx.dlist.listEnd_p++ = ((uint32)slider.value << 16) | (slider.gpuOpt & 0xffff);
    *ctx.dlist.listEnd_p++ = ((uint32)0x0000 << 16) | (slider.maxRange & 0xffff);
    CHECK_DISP_LIST_OVF(ctx.dlist);
    CHECK_EXEC(ClearTouchTag());

    //-----------------------------------------------------------------------------------

    return Exec::OK;
}

Exec BT81x_GPU::CmdGauge(gauge_t& gauge)
{
    CHECK_STATUS();
    ASSERT((gauge.majorTickCnt >= 0) && (gauge.majorTickCnt <= 10), "MAJOR TICK COUNT (MIN=1, MAX=10)");
    ASSERT((gauge.minorTickCnt >= 0) && (gauge.minorTickCnt <= 10), "MINOR TICK COUNT (MIN=1, MAX=10)");
    ASSERT((gauge.gpuOpt == GPU_OPT_3D)         ||
           (gauge.gpuOpt == GPU_OPT_FLAT)       ||
           (gauge.gpuOpt == GPU_OPT_NOBACK)     ||
           (gauge.gpuOpt == GPU_OPT_NOTICKS)    ||
           (gauge.gpuOpt == GPU_OPT_NOPOINTER), "GAUGE INVALID GPU OPTION");
    CHECK_EXEC(CmdBgColor(gauge.bgColor));
    CHECK_EXEC(CmdColor(gauge.tickColor));

    lcdPos_t lcdPos;
    AlignWidget(WIDGET_GAUGE, (void*)&gauge, lcdPos);
    *ctx.dlist.listEnd_p++ = GPU_CMD_GAUGE;
    *ctx.dlist.listEnd_p++ = ((uint32)lcdPos.y << 16) | (lcdPos.x & 0xffff);
    *ctx.dlist.listEnd_p++ = ((uint32)gauge.gpuOpt << 16) | (gauge.radius & 0xffff);
    *ctx.dlist.listEnd_p++ = ((uint32)gauge.minorTickCnt << 16) | (gauge.majorTickCnt & 0xffff);
    *ctx.dlist.listEnd_p++ = ((uint32)gauge.maxRange << 16) | (gauge.value & 0xffff);
    CHECK_DISP_LIST_OVF(ctx.dlist);

    return Exec::OK;
}

Exec BT81x_GPU::CmdDial(dial_t& dial)
{
    CHECK_STATUS();
    ASSERT((dial.gpuOpt == GPU_OPT_3D) || (dial.gpuOpt == GPU_OPT_FLAT), "DIAL INVALID GPU OPTION");

    //-----------------------------------------------------------------------------------
    // Adding dial GPU commands to display list

    CHECK_EXEC(CmdFgColor(dial.knobColor));
    CHECK_EXEC(CmdColor(dial.markColor));

    lcdPos_t lcdPos;
    AlignWidget(WIDGET_DIAL, (void*)&dial, lcdPos);
    *ctx.dlist.listEnd_p++ = GPU_CMD_DIAL;
    *ctx.dlist.listEnd_p++ = ((uint32)lcdPos.y << 16) | (lcdPos.x & 0xffff);
    *ctx.dlist.listEnd_p++ = ((uint32)dial.gpuOpt << 16) | (dial.radius & 0xffff);
    *ctx.dlist.listEnd_p++ = dial.value & 0xffff;
    CHECK_DISP_LIST_OVF(ctx.dlist);
    return Exec::OK;
}

Exec BT81x_GPU::CmdTrackXY(int16 x, int16 y, uint16 width, uint16 height, uint8 tag)
{
    CHECK_STATUS();
    *ctx.dlist.listEnd_p++ = GPU_CMD_TRACK;
    *ctx.dlist.listEnd_p++ = ((uint32)y << 16) | (x & 0xffff);
    *ctx.dlist.listEnd_p++ = ((uint32)height << 16) | (width & 0xffff);
    *ctx.dlist.listEnd_p++ = tag;
    CHECK_DISP_LIST_OVF(ctx.dlist);
    return Exec::OK;
}

Exec BT81x_GPU::CmdTrackAngle(int16 centerX, int16 centerY, uint8 tag)
{
    CHECK_STATUS();
    RET_EXEC(CmdTrackXY(centerX, centerY, 1, 1, tag));
}

Exec BT81x_GPU::DrawLine(int16 x1, int16 y1, int16 x2, int16 y2, uint16 width, color_t color, uint8 tag)
{
    CHECK_STATUS();
    CHECK_EXEC(CmdColor(color));
    CHECK_EXEC(BeginPrimitive(Primitive::LINES));
    *ctx.dlist.listEnd_p++ = ((uint32)0x0E << 24) | (width * 16);
    CHECK_EXEC(Vertex2F(x1 * 16, y1 * 16));
    CHECK_EXEC(Vertex2F(x2 * 16, y2 * 16));
    if (tag) { CHECK_EXEC(CmdTouchTag(tag)); }
    CHECK_DISP_LIST_OVF(ctx.dlist);
    return Exec::OK;
}

Exec BT81x_GPU::DrawCircle(int16 x, int16 y, uint16 radius, color_t color, uint8 tag)
{
    CHECK_STATUS();
    CHECK_EXEC(CmdColor(color));
    CHECK_EXEC(BeginPrimitive(Primitive::POINTS));
    *ctx.dlist.listEnd_p++ = ((uint32)0x0D << 24) | (radius * 16);
    CHECK_EXEC(Vertex2F(x * 16, y * 16));
    if (tag) { CHECK_EXEC(CmdTouchTag(tag)); }
    CHECK_DISP_LIST_OVF(ctx.dlist);
    return Exec::OK;
}

Exec BT81x_GPU::Arc(int16 x, int16 y, uint16 radius, uint8 thickness, color_t color, uint16 startDeg, uint16 endDeg)
{
    CHECK_STATUS();
    // CHECK_EXEC(CmdColor(color));
    // CHECK_EXEC(BeginPrimitive(Primitive::LINE_STRIP));
    // *ctx.dlist.listEnd_p++ = (0x0E << 24) | ((thickness * 16) / 2);

    // uint16 spanDeg = (startDeg <= endDeg) ? endDeg - startDeg : startDeg - endDeg;

    // for (uint16 posDeg = 0; posDeg < spanDeg; posDeg += 2) {
    //     int16 deg = (int16)((startDeg + posDeg));
    //     deg %= 360;
    //     int32 x1 = (cosTab[deg] * radius + (x * 65536)) * 16;
    //     int32 y1 = (sinTab[deg] * radius + (y * 65536)) * 16;
    //     CHECK_EXEC(Vertex2F((int16)(x1 / 65536), (int16)(y1 / 65536)));
    // }

    return Exec::OK;
}

Exec BT81x_GPU::DrawRectangle(rectangle_t rectangle)
{
    CHECK_STATUS();
    CHECK_EXEC(CmdColor(rectangle.color));
    CHECK_EXEC(BeginPrimitive(Primitive::RECTANGLE));
    *ctx.dlist.listEnd_p++ = ((uint32)0x0E << 24) | ((rectangle.cornerR ? rectangle.cornerR : 1) * 16);
    lcdPos_t lcdPos = {};
    AlignWidget(WIDGET_RECTANGLE, (void*)&rectangle, lcdPos);
    CHECK_EXEC(Vertex2F((int16)(lcdPos.x) * 16, (int16)(lcdPos.y) * 16));
    CHECK_EXEC(Vertex2F((int16)((lcdPos.x + rectangle.width) - 2 * rectangle.cornerR) * 16, (int16)((lcdPos.y + rectangle.height) - 2 * rectangle.cornerR) * 16));
    CHECK_DISP_LIST_OVF(ctx.dlist);
    return Exec::OK;
}

Exec BT81x_GPU::DrawRectangle(int16 x, int16 y, uint16 width, uint16 height, uint8 radius, align_t align, color_t color, uint8 tag)
{
    CHECK_STATUS();
    CHECK_EXEC(CmdColor(color));
    CHECK_EXEC(BeginPrimitive(Primitive::RECTANGLE));
    *ctx.dlist.listEnd_p++ = ((uint32)0x0E << 24) | ((radius ? radius : 1) * 16);
    CHECK_EXEC(Vertex2F((int16)(x + radius) * 16, (int16)(y - radius) * 16));
    CHECK_EXEC(Vertex2F((int16)(x + width) * 16, (int16)(y - height) * 16));
    if (tag) { CHECK_EXEC(CmdTouchTag(tag)); }
    CHECK_DISP_LIST_OVF(ctx.dlist);
    return Exec::OK;
}

Exec BT81x_GPU::BeginPrimitive(Primitive primitive)
{
    *ctx.dlist.listEnd_p++ = ((uint32)0x1F << 24) | (uint8)primitive;
    CHECK_DISP_LIST_OVF(ctx.dlist);
    return Exec::OK;
}

Exec BT81x_GPU::Vertex2F(int16 x, int16 y)
{
    CHECK_STATUS();
    *ctx.dlist.listEnd_p++ = ((uint32)0x01 << 30) | ((uint32)x << 15) | y;
    CHECK_DISP_LIST_OVF(ctx.dlist);
    return Exec::OK;
}

void BT81x_GPU::AppendText(const char* text_p)
{
    uint16 textLen = (uint16)(strlen(text_p) + 1);
    uint8 wordCnt = (uint8)(textLen / sizeof(uint32));
    uint8 byteCnt = (uint8)(textLen % sizeof(uint32));

    // Copy text to display list
    memcpy((uint8*)ctx.dlist.listEnd_p, text_p, textLen);

    // Padding with zeros if text length odd
    if (byteCnt) {
        uint8* uint8_p = (uint8*)ctx.dlist.listEnd_p;
        memset(&uint8_p[textLen], 0x00, sizeof(uint32) - byteCnt);
        wordCnt++;
    }

    ctx.dlist.listEnd_p += wordCnt;
    CHECK_DISP_LIST_OVF(ctx.dlist);
}

void BT81x_GPU::AlignWidget(widget_t type, void* widget_p, lcdPos_t& pos)
{
    int16 xPos = {};
    int16 yPos = {};
    uint16 xSize = {};
    uint16 ySize = {};
    align_t align = {};
    gpuOpt_t gpuOpt = {};

    if (type == WIDGET_BUTTON) {
        button_t* button_p = (button_t*)widget_p;
        xPos = button_p->x;
        yPos = button_p->y;
        xSize = button_p->width;
        ySize = button_p->height;
        align = button_p->align;
        gpuOpt = button_p->gpuOpt;
    }
    else if (type == WIDGET_RECTANGLE) {
        rectangle_t* rectangle_p = (rectangle_t*)widget_p;
        xPos = (int16)(rectangle_p->x + rectangle_p->cornerR);
        yPos = (int16)(rectangle_p->y + rectangle_p->cornerR);
        xSize = rectangle_p->width;
        ySize = rectangle_p->height;
        align = rectangle_p->align;
    }
    else if (type == WIDGET_SLIDER) {
        slider_t* slider_p = (slider_t*)widget_p;
        xPos = slider_p->x;
        yPos = slider_p->y;
        xSize = slider_p->length;
        ySize = slider_p->height;
        align = slider_p->align;
        gpuOpt = slider_p->gpuOpt;
    }
    else if (type == WIDGET_KEYBOARD) {
        keyboard_t* keyboard_p = (keyboard_t*)widget_p;
        xPos = keyboard_p->x;
        yPos = keyboard_p->y;
        xSize = keyboard_p->width;
        ySize = keyboard_p->height;
        align = keyboard_p->align;
        gpuOpt = keyboard_p->gpuOpt;
    }
    else if (type == WIDGET_TOGGLE) {
        toggle_t* toggle_p = (toggle_t*)widget_p;
        xPos = toggle_p->x;
        yPos = toggle_p->y;
        xSize = toggle_p->width;
        ySize = 0;
        align = toggle_p->align;
        gpuOpt = toggle_p->gpuOpt;
    }
    else {
        REPORT_FAT("UNHANDLED WIDGET TYPE");
    }

    switch (align) {
        case ALIGN_NONE: {
            pos.x = xPos - (int16)(xSize / 2);
            pos.y = yPos - (int16)(ySize / 2);
            break;
        }
        case ALIGN_CENTER: {
            pos.x = xPos + (int16)((ctx.config.hSize / 2) - (xSize / 2));
            pos.y = yPos + (int16)((ctx.config.vSize / 2) - (ySize / 2));
            break;
        }
        case ALIGN_TOPL: {
            pos.x = xPos;
            pos.y = yPos;
            if (gpuOpt == GPU_OPT_3D) { pos.x -= ALIGN_OFFSET; pos.y -= ALIGN_OFFSET; }
            break;
        }
        case ALIGN_TOPR: {
            pos.x = (int16)(xPos + ctx.config.hSize - xSize);
            pos.y = yPos - ALIGN_OFFSET;
            if (gpuOpt == GPU_OPT_3D) pos.x -= ALIGN_OFFSET;
            break;
        }
        case ALIGN_BOTTOML: {
            pos.x = xPos;
            pos.y = (int16)(yPos + ctx.config.vSize - ySize);
            if (gpuOpt == GPU_OPT_3D) { pos.x -= ALIGN_OFFSET; pos.y -= ALIGN_OFFSET; }
            break;
        }
        case ALIGN_BOTTOMR: {
            pos.x = (int16)(xPos + ctx.config.hSize - xSize);
            pos.y = (int16)(yPos + ctx.config.vSize - ySize);
            if (gpuOpt == GPU_OPT_3D) { pos.x -= ALIGN_OFFSET; pos.y -= ALIGN_OFFSET; }
            break;
        }
    }
}

void BT81x_GPU::CalcWidgetPos(widget_t type, void* widget_p, objPos_t& pos)
{
    lcdPos_t lcdPos = {};
    uint16 xSize = {};
    uint16 ySize = {};

    AlignWidget(type, widget_p, lcdPos);

    if (type == WIDGET_BUTTON) {
        xSize = ((button_t*)widget_p)->width;
        ySize = ((button_t*)widget_p)->height;
    }
    else if (type == WIDGET_SLIDER) {
        xSize = ((slider_t*)widget_p)->length;
        ySize = ((slider_t*)widget_p)->height;
    }

    pos.tl.x = lcdPos.x;
    pos.tl.y = lcdPos.y;
    pos.tr.x = lcdPos.x + (int16)xSize;
    pos.tr.y = lcdPos.y;
    pos.bl.x = lcdPos.x;
    pos.bl.y = lcdPos.y + (int16)ySize;
    pos.br.x = lcdPos.x + (int16)xSize;
    pos.br.y = lcdPos.y + (int16)ySize;
}

tRelPos_t BT81x_GPU::CalcTouchRelPos(const objPos_t& objPos, uint8 margin)
{
    tRelPos_t tRelPos = {};

    if ((ctx.touch.x >= 0) && (ctx.touch.y >= 0)) {
        tRelPos.x = ctx.touch.x - objPos.tl.x;
        tRelPos.y = ctx.touch.y - objPos.tl.y;
        if ((ctx.touch.x >= (objPos.tl.x - margin))  &&
            (ctx.touch.x <= (objPos.tr.x + margin))  &&
            (ctx.touch.y >= (objPos.tl.y - margin))  &&
            (ctx.touch.y <= (objPos.bl.y + margin))) {
            tRelPos.inMargin = true;
        }
    }
    return tRelPos;
}

void BT81x_GPU::Key::Setup(const key_t& param, BT81x_GPU& gpu)
{
    SetParam(param);
    gpu_p = &gpu;
    ctx.status = Status::OK;
}

void BT81x_GPU::Key::SetParam(const key_t& param)
{
    ASSERT((param.width != 0) && (param.height != 0), "Invalid key width or height!");
    ASSERT(strlen(param.label_p) > 0, "Key label lenght is zero!");
    ASSERT((param.gpuOpt == GPU_OPT_3D) || (param.gpuOpt == GPU_OPT_FLAT), "Invalid GPU option!");
    ctx.param = param;
}

Exec BT81x_GPU::Key::Display()
{
    CHECK_STATUS();

    //-----------------------------------------------------------------------------------

    uint8 romSlotNum = 0;
    lcdPos_t lcdPos;
    drv::bt81x::color_t keyColor;
    bool touchActive;
    uint8 touchTag;
    uint32 currentSysTick;

    //-----------------------------------------------------------------------------------
    // Setting button color, text color & touch tag

    keyColor = ctx.param.color;
    touchActive = gpu_p->ctx.touch.active;
    touchTag = gpu_p->ctx.touch.tag;
    currentSysTick = gpu_p->ctx.config.GetSysTickMs();

    if (ctx.locked || !touchActive || touchTag != ctx.touchTag) {
        keyColor = ctx.param.color;
        ctx.sysTickLast = currentSysTick;
        if (ctx.pressed && !touchActive) {
            ctx.event = KeyEvent::Released;
        }
        ctx.pressed = false;
    }
    else {
        keyColor = ctx.param.colorOn;
        if ((currentSysTick - ctx.sysTickLast) > ctx.param.pressedTimeMs) {
            if (!ctx.pressed) ctx.event = KeyEvent::Pressed;
            ctx.pressed = true;
        }
    }

    CHECK_EXEC(gpu_p->CmdTouchTag(ctx.touchTag));
    CHECK_EXEC(gpu_p->CmdFgColor(keyColor));
    CHECK_EXEC(gpu_p->CmdColor(ctx.param.labelColor));

    //-----------------------------------------------------------------------------------
    // Adding key commands to display list

    gpu_p->AlignWidget(WIDGET_BUTTON, (void*)&ctx.param, lcdPos);
    *gpu_p->ctx.dlist.listEnd_p++ = GPU_CMD_ROMFONT;
    *gpu_p->ctx.dlist.listEnd_p++ = romSlotNum;
    *gpu_p->ctx.dlist.listEnd_p++ = ctx.param.labelFont;
    *gpu_p->ctx.dlist.listEnd_p++ = GPU_CMD_BUTTON;
    *gpu_p->ctx.dlist.listEnd_p++ = ((uint32)lcdPos.y << 16) | (lcdPos.x & 0xffff);
    *gpu_p->ctx.dlist.listEnd_p++ = ((uint32)ctx.param.height << 16) | (ctx.param.width & 0xffff);
    *gpu_p->ctx.dlist.listEnd_p++ = ((uint32)ctx.param.gpuOpt << 16) | (romSlotNum & 0xffff);
    CHECK_DISP_LIST_OVF(gpu_p->ctx.dlist);
    gpu_p->AppendText(ctx.param.label_p);
    CHECK_EXEC(gpu_p->ClearTouchTag());

    //-----------------------------------------------------------------------------------

    return Exec::OK;
}

bool BT81x_GPU::Key::CheckEvent(KeyEvent event)
{
    if (ctx.event == event) {
        ctx.event = KeyEvent::None;
        return true;
    }
    return false;
}

key_t BT81x_GPU::Key::GetParam()
{
    return ctx.param;
}

uint8 BT81x_GPU::Key::GetTag()
{
    return ctx.touchTag;
}

void BT81x_GPU::Key::Lock()
{
    ctx.locked = true;
    ctx.pressed = false;
    ctx.event = KeyEvent::None;
}

void BT81x_GPU::Key::Unlock()
{
    ctx.locked = false;
}

/******************************************************************************************************************************************
                                                        PRIVATE FUNCTION DECLARATIONS
******************************************************************************************************************************************/

} // END OF NAMESPACE 'drv::bt81x::gpu'
/******************************************************************************************************************************************
                                                                END OF FILE
******************************************************************************************************************************************/

