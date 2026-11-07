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
// DESCRIPTION:      FTDI BT81x display driver types lib

/******************************************************************************************************************************************
                                                                  INCLUDES
******************************************************************************************************************************************/

#pragma once

/******************************************************************************************************************************************
                                                                   TYPES
******************************************************************************************************************************************/

namespace drv::bt81x {

constexpr uint16 CHIP_ID = 0x7C;
constexpr uint16 RAM_DL_SIZE = 8192;
constexpr uint16 RAM_CMD_SIZE = 4096;
constexpr uint32 EXT_OSC_FREQ_Hz = 12000000;

enum memMap_t : uint32 {
    MEM_RAM_G_START = 0x000000,                 // GENERAL PUPRPOSE GRAPHICS RAM
    MEM_RAM_G_END = 0x0FFFFF,
    MEM_ROM_FONT_START = 0x1E0000,              // FONT TABLE & BITMAP
    MEM_ROM_FONT_END = 0x2FFFFB,
    MEM_ROM_FONTROOT_START = 0x2FFFFC,          // FONT TABLE POINTER ADDRESS
    MEM_ROM_FONTROOT_END = 0x2FFFFF,
    MEM_RAM_DL_START = 0x300000,                // RAM DISPLAY LIST
    MEM_RAM_DL_END = 0x301FFF,
    MEM_RAM_REG_START = 0x302000,               // REGISTERS
    MEM_RAM_REG_END = 0x302FFF,
    MEM_RAM_CMD_START = 0x308000,               // COMMAND BUFFER
    MEM_RAM_CMD_END = 0x308FFF,
    MEM_RAM_ERR_REPORT_START = 0x309800,        // COPROCESSOR FAULT REPORT AREA
    MEM_RAM_ERR_REPORT_END = 0x3098FF,
};

enum systemCmd_t : uint8 {
    SYSCMD_ACTIVE = 0x00,
    SYSCMD_STANDBY = 0x41,
    SYSCMD_SLEEP = 0x42,
    SYSCMD_PWRDOWN = 0x43,
    SYSCMD_CLKEXT = 0x44,
    SYSCMD_CLKINT = 0x48,
    SYSCMD_CLKSEL = 0x61,
    SYSCMD_RST_PULSE = 0x68,
    SYSCMD_PINDRIVE = 0x70,
    SYSCMD_PINSTATE_PD = 0x71,
};

enum regWidth_t : uint8 {
    REG_8BIT = 1,
    REG_16BIT = 2,
    REG_32BIT = 4,
};

enum blCtrl_t : uint8 {
    BLCTRL_NORMAL,
    BLCTRL_INVERTED,
};

enum touchSamp_t : uint8 {
    TOUCH_OFF = 0,
    TOUCH_CONTINOUS = 3,
};

enum touchMode_t : uint8 {
    TOUCH_EXTENDED = 0,
    TOUCH_SINGLE = 1,
};

enum regAddr_t : uint32 {
    REG_ID = 0x302000,
    REG_FRAMES = 0x302004,
    REG_CLOCK = 0x302008,
    REG_FREQUENCY = 0x30200C,
    REG_RENDERMODE = 0x302010,
    REG_SNAPY = 0x302014,
    REG_SNAPSHOT = 0x302018,
    REG_SNAPFORMAT = 0x30201C,
    REG_CPURESET = 0x302020,
    REG_TAP_CRC = 0x302024,
    REG_TAP_MASK = 0x302028,
    REG_HCYCLE = 0x30202C,
    REG_HOFFSET = 0x302030,
    REG_HSIZE = 0x302034,
    REG_HSYNC0 = 0x302038,
    REG_HSYNC1 = 0x30203C,
    REG_VCYCLE = 0x302040,
    REG_VOFFSET = 0x302044,
    REG_VSIZE = 0x302048,
    REG_VSYNC0 = 0x30204C,
    REG_VSYNC1 = 0x302050,
    REG_DLSWAP = 0x302054,
    REG_ROTATE = 0x302058,
    REG_OUTBITS = 0x30205C,
    REG_DITHER = 0x302060,
    REG_SWIZZLE = 0x302064,
    REG_CSPREAD = 0x302068,
    REG_PCLK_POL = 0x30206C,
    REG_PCLK = 0x302070,
    REG_TAG_X = 0x302074,
    REG_TAG_Y = 0x302078,
    REG_TAG = 0x30207C,
    REG_VOL_PB = 0x302080,
    REG_VOL_SOUND = 0x302084,
    REG_SOUND = 0x302088,
    REG_PLAY = 0x30208C,
    REG_GPIO_DIR = 0x302090,
    REG_GPIO = 0x302094,
    REG_GPIOX_DIR = 0x302098,
    REG_GPIOX = 0x30209C,

    REG_INT_FLAGS = 0x3020A8,
    REG_INT_EN = 0x3020AC,
    REG_INT_MASK = 0x3020B0,

    REG_PLAYBACK_START = 0x3020B4,
    REG_PLAYBACK_LENGTH = 0x3020B8,
    REG_PLAYBACK_READPTR = 0x3020BC,
    REG_PLAYBACK_FREQ = 0x3020C0,
    REG_PLAYBACK_FORMAT = 0x3020C4,
    REG_PLAYBACK_LOOP = 0x3020C8,
    REG_PLAYBACK_PLAY = 0x3020CC,

    REG_PWM_HZ = 0x3020D0,
    REG_PWM_DUTY = 0x3020D4,

    REG_MACRO_0 = 0x3020D8,
    REG_MACRO_1 = 0x3020DC,

    REG_CMD_READ = 0x3020F8,
    REG_CMD_WRITE = 0x3020FC,
    REG_CMD_DL = 0x302100,

    REG_TOUCH_MODE = 0x302104,

    REG_TOUCH_ADC_MODE = 0x302108,
    REG_CTOUCH_EXTENDED = 0x302108,

    REG_TOUCH_CHARGE = 0x30210C,
    REG_TOUCH_SETTLE = 0x302110,
    REG_TOUCH_OVERSAMPLE = 0x302114,
    REG_TOUCH_RZTHRESH = 0x302118,

    REG_TOUCH_RAW_XY = 0x30211C,
    REG_CTOUCH_TOUCH1_XY = 0x30211C,

    REG_TOUCH_RZ = 0x302120,
    REG_CTOUCH_TOUCH4_Y = 0x302120,

    REG_TOUCH_SCREEN_XY = 0x302124,
    REG_CTOUCH_TOUCH0_XY = 0x302124,

    REG_TOUCH_TAG_XY = 0x302128,
    REG_TOUCH_TAG = 0x30212C,

    REG_TOUCH_TAG1_XY = 0x302130,
    REG_TOUCH_TAG1 = 0x302134,

    REG_TOUCH_TAG2_XY = 0x302138,
    REG_TOUCH_TAG2 = 0x30213C,

    REG_TOUCH_TAG3_XY = 0x302140,
    REG_TOUCH_TAG3 = 0x302144,

    REG_TOUCH_TAG4_XY = 0x302148,
    REG_TOUCH_TAG4 = 0x30214C,

    REG_TOUCH_TRANSFORM_A = 0x302150,
    REG_TOUCH_TRANSFORM_B = 0x302154,
    REG_TOUCH_TRANSFORM_C = 0x302158,
    REG_TOUCH_TRANSFORM_D = 0x30215C,
    REG_TOUCH_TRANSFORM_E = 0x302160,
    REG_TOUCH_TRANSFORM_F = 0x302164,

    REG_TOUCH_CONFIG = 0x302168,

    REG_CTOUCH_TOUCH4_X = 0x30216C,

    REG_EHOST_TOUCH_ACK = 0x302170,

    REG_BIST_EN = 0x302174,

    REG_TRIM = 0x302180,
    REG_ANA_COMP = 0x302184,
    REG_SPI_WIDTH = 0x302188,

    REG_TOUCH_DIRECT_XY = 0x30218C,
    REG_CTOUCH_TOUCH2_XY = 0x30218C,

    REG_TOUCH_DIRECT_Z1Z2 = 0x302190,
    REG_CTOUCH_TOUCH3_XY = 0x302190,

    REG_DATESTAMP = 0x302564,

    REG_CMDB_SPACE = 0x302574,
    REG_CMDB_WRITE = 0x302578,
    REG_ADAPTIVE_FRAMERATE = 0x30257C,

    REG_PLAYBACK_PAUSE = 0x3025EC,
    REG_FLASH_STATUS = 0x3025F0,

    REG_UNDERRUN = 0x30260C,
    REG_AH_HCYCLE_MAX = 0x302610,
    REG_PCLK_FREQ = 0x302614,
    REG_PCLK_2X = 0x302618,

    REG_TRACKER = 0x309000,
    REG_TRACKER_1 = 0x309004,
    REG_TRACKER_2 = 0x309008,
    REG_TRACKER_3 = 0x30900C,
    REG_TRACKER_4 = 0x309010,

    REG_MEDIAFIFO_READ = 0x309014,
    REG_MEDIAFIFO_WRITE = 0x309018,

    REG_FLASH_SIZE = 0x309024,

    REG_ANIM_ACTIVE = 0x30902C,

    REG_PLAY_CONTROL = 0x30914E,
};

enum gpuFont_t : uint8 {
    FONT16 = 16,
    FONT18 = 18,
    FONT20 = 20,
    FONT21 = 21,
    FONT22 = 22,
    FONT23 = 23,
    FONT24 = 24,
    FONT25 = 25,
    FONT26 = 26,
    FONT27 = 27,
    FONT28 = 28,
    FONT29 = 29,
    FONT30 = 30,
    FONT31 = 31,
    FONT32 = 32,
    FONT33 = 33,
    FONT34 = 34,
};

enum gpuOpt_t : uint16 {
    GPU_OPT_3D = 0,
    GPU_OPT_MONO = 1,
    GPU_OPT_NODL = 2,
    GPU_OPT_NOTEAR = 4,
    GPU_OPT_FULLSCREEN = 8,
    GPU_OPT_MEDIAFIFO = 16,
    GPU_OPT_SOUND = 32,
    GPU_OPT_FLASH = 64,
    GPU_OPT_FLAT = 256,
    GPU_OPT_SIGNED = 256,
    GPU_OPT_DITHER = 256,
    GPU_OPT_CENTERX = 512,
    GPU_OPT_CENTERY = 1024,
    GPU_OPT_CENTER = 1536,
    GPU_OPT_RIGHTX = 2048,
    GPU_OPT_NOBACK = 4096,
    GPU_OPT_FORMAT = 4096,
    GPU_OPT_FILL = 8192,
    GPU_OPT_NOTICKS = 8192,
    GPU_OPT_NOHM = 16384,
    GPU_OPT_NOPOINTER = 16384,
    GPU_OPT_NOSECS = 32768,
    GPU_OPT_NOHANDS = 49152,
};

struct touch_t {
    uint8 tag = 255;
    int16 x = INT16_MIN;
    int16 y = INT16_MIN;
    bool active;
};

enum widget_t : uint8 {
    WIDGET_BUTTON,
    WIDGET_TOGGLE,
    WIDGET_SLIDER,
    WIDGET_GAUGE,
    WIDGET_DIAL,
    WIDGET_KEYBOARD,
    WIDGET_RECTANGLE,
};

enum align_t : uint8 {
    ALIGN_NONE,
    ALIGN_CENTER,   // Center of screen
    ALIGN_TOPL,     // Top left corner
    ALIGN_TOPR,     // Top right corner
    ALIGN_BOTTOML,  // Bottom left corner
    ALIGN_BOTTOMR,  // Bottom right corner
};

enum class Primitive : uint8 {
    BITMAPS = 1,
    POINTS,
    LINES,
    LINE_STRIP,
    EDGE_STRIP_R,
    EDGE_STRIP_L,
    EDGE_STRIP_A,
    EDGE_STRIP_B,
    RECTANGLE,
};

constexpr uint32 GPU_CMD_DISPLAY = 0;
constexpr uint32 GPU_CMD_APPEND = 4294967070;
constexpr uint32 GPU_CMD_BGCOLOR = 4294967049;
constexpr uint32 GPU_CMD_BITMAP_TRANSFORM = 4294967073;
constexpr uint32 GPU_CMD_BUTTON = 4294967053;
constexpr uint32 GPU_CMD_CALIBRATE = 4294967061;
constexpr uint32 GPU_CMD_CLOCK = 4294967060;
constexpr uint32 GPU_CMD_COLDSTART = 4294967090;
constexpr uint32 GPU_CMD_CRC = 4294967043;
constexpr uint32 GPU_CMD_CSKETCH = 4294967093;
constexpr uint32 GPU_CMD_DIAL = 4294967085;
constexpr uint32 GPU_CMD_DLSTART = 4294967040;
constexpr uint32 GPU_CMD_EXECUTE = 4294967047;
constexpr uint32 GPU_CMD_FGCOLOR = 4294967050;
constexpr uint32 GPU_CMD_GAUGE = 4294967059;
constexpr uint32 GPU_CMD_GETMATRIX = 4294967091;
constexpr uint32 GPU_CMD_GETPOINT = 4294967048;
constexpr uint32 GPU_CMD_GETPROPS = 4294967077;
constexpr uint32 GPU_CMD_GETPTR = 4294967075;
constexpr uint32 GPU_CMD_GRADCOLOR = 4294967092;
constexpr uint32 GPU_CMD_GRADIENT = 4294967051;
constexpr uint32 GPU_CMD_HAMMERAUX = 4294967044;
constexpr uint32 GPU_CMD_IDCT_DELETED = 4294967046;
constexpr uint32 GPU_CMD_INFLATE = 4294967074;
constexpr uint32 GPU_CMD_INTERRUPT = 4294967042;
constexpr uint32 GPU_CMD_INT_RAMSHARED = 4294967101;
constexpr uint32 GPU_CMD_INT_SWLOADIMAGE = 4294967102;
constexpr uint32 GPU_CMD_KEYS = 4294967054;
constexpr uint32 GPU_CMD_LOADIDENTITY = 4294967078;
constexpr uint32 GPU_CMD_LOADIMAGE = 4294967076;
constexpr uint32 GPU_CMD_LOGO = 4294967089;
constexpr uint32 GPU_CMD_MARCH = 4294967045;
constexpr uint32 GPU_CMD_MEDIAFIFO = 4294967097;
constexpr uint32 GPU_CMD_MEMCPY = 4294967069;
constexpr uint32 GPU_CMD_MEMCRC = 4294967064;
constexpr uint32 GPU_CMD_MEMSET = 4294967067;
constexpr uint32 GPU_CMD_MEMWRITE = 4294967066;
constexpr uint32 GPU_CMD_MEMZERO = 4294967068;
constexpr uint32 GPU_CMD_NUMBER = 4294967086;
constexpr uint32 GPU_CMD_PLAYVIDEO = 4294967098;
constexpr uint32 GPU_CMD_PROGRESS = 4294967055;
constexpr uint32 GPU_CMD_REGREAD = 4294967065;
constexpr uint32 GPU_CMD_ROMFONT = 4294967103;
constexpr uint32 GPU_CMD_ROTATE = 4294967081;
constexpr uint32 GPU_CMD_SCALE = 4294967080;
constexpr uint32 GPU_CMD_SCREENSAVER = 4294967087;
constexpr uint32 GPU_CMD_SCROLLBAR = 4294967057;
constexpr uint32 GPU_CMD_SETBASE = 4294967096;
constexpr uint32 GPU_CMD_SETBITMAP = 4294967107;
constexpr uint32 GPU_CMD_SETFONT = 4294967083;
constexpr uint32 GPU_CMD_SETFONT2 = 4294967099;
constexpr uint32 GPU_CMD_SETMATRIX = 4294967082;
constexpr uint32 GPU_CMD_SETROTATE = 4294967094;
constexpr uint32 GPU_CMD_SETSCRATCH = 4294967100;
constexpr uint32 GPU_CMD_SKETCH = 4294967088;
constexpr uint32 GPU_CMD_SLIDER = 4294967056;
constexpr uint32 GPU_CMD_SNAPSHOT = 4294967071;
constexpr uint32 GPU_CMD_SNAPSHOT2 = 4294967095;
constexpr uint32 GPU_CMD_SPINNER = 4294967062;
constexpr uint32 GPU_CMD_STOP = 4294967063;
constexpr uint32 GPU_CMD_SWAP = 4294967041;
constexpr uint32 GPU_CMD_NOP = 0x2D000000UL;
constexpr uint32 GPU_CMD_SYNC = 4294967106;
constexpr uint32 GPU_CMD_TEXT = 4294967052;
constexpr uint32 GPU_CMD_TOGGLE = 4294967058;
constexpr uint32 GPU_CMD_TOUCH_TRANSFORM = 4294967072;
constexpr uint32 GPU_CMD_TRACK = 4294967084;
constexpr uint32 GPU_CMD_TRANSLATE = 4294967079;
constexpr uint32 GPU_CMD_VIDEOFRAME = 4294967105;
constexpr uint32 GPU_CMD_VIDEOSTART = 4294967104;
constexpr uint32 GPU_CMD_CLEAR_COLOR_RGB(uint8 red, uint8 green, uint8 blue) { return ((2UL << 24) | (((red) & 255UL) << 16) | (((green) & 255UL) << 8) | (((blue) & 255UL) << 0)); }
constexpr uint32 GPU_CMD_TAG(uint8 tagNum) {return ((3UL << 24) | (((tagNum) & 255UL) << 0)); }
constexpr uint32 GPU_CMD_COLOR_RGB(uint8 red, uint8 green, uint8 blue) { return ((4UL << 24) | (((red) & 255UL) << 16) | (((green) & 255UL) << 8) | (((blue) & 255UL) << 0)); }
constexpr uint32 GPU_CMD_CLEAR(uint8 color, uint8 stencil, uint8 tag) { return ((38UL << 24) | (((color) & 1UL) << 2) | (((stencil) & 1UL) << 1) | (((tag) & 1UL) << 0)); }

union regClkSel_t {
    struct {
        uint8 clkFreq   : 6;
        uint8 pllRange  : 2;
    } bit;
    uint8 val;
};

union regCpuReset_t {
    struct {
        uint8 GRAPHICS  : 1;
        uint8 TOUCH     : 1;
        uint8 AUDIO     : 1;
        uint8           : 5;
    } bit;
    uint8 val;
};

// Irq's REG_INT_FLAGS & REG_INT_MASK
union regIrq_t {
    struct {
        uint16 SWAP             : 1;    // Display list swap occurred
        uint16 TOUCH            : 1;    // Touch detected
        uint16 TAG              : 1;    // Touch-screen tag value change
        uint16 SOUND            : 1;    // Sound effect ended
        uint16 PLAYBACK         : 1;    // Audio playback ended
        uint16 CMDEMPTY         : 1;    // Command FIFO empty
        uint16 CMDFLAG          : 1;    // Command FIFO flag
        uint16 CONV_COMPLETE    : 1;    // Touch-screen conversions completed
        uint16 UNDERRUN         : 1;    // Graphics pipeline underrun
        uint16                  : 7;    // Reserved
    } bit;
    uint16 val;
};

// REG_TOUCH_CONFIG
union regTouchConfig_t {
    struct {
        uint16 SAMPLCLK : 2;    // Sampling clocks(for resistive touch screen only)
        uint16 SUPPRESS : 1;    // Suppress 300ms startup (for FocalTech only)
        uint16          : 1;
        uint16 I2CADDR  : 7;
        uint16 ENLP     : 1;    // Enable low-power mode(for FocalTech only)
        uint16 IGNSHRT  : 1;    // Ignore short-circuit protection. For resistive touch screen only
        uint16          : 1;
        uint16 HOSTMODE : 1;    // 1: Enable the host mode, 0: Normal mode
        uint16 WORKMODE : 1;    // Working mode of touch engine -> 0: capacitive 1: resistive
    } bit;
    uint16 val;
};

// REG_TRACKER
union regTracker_t {
    struct {
        uint32 tag : 8;
        uint32 : 8;
        uint32 value : 16;
    } bit;
    uint32 val;
};

// Display list
struct dlist_t {
    uint32* buff_p = nullptr;
    uint32* buffEnd_p = nullptr;
    uint16 buffSizeCmd = 0;
    uint16 buffSizeByte = 0;
    uint32* listEnd_p = nullptr;
    uint16 listSizeByte = 0;
    uint16 listSizeByteMax = 0;
    uint16 listSizeCmd = 0;
    bool listStartOk = false;
    bool listCloseOk = false;
};

// Widget-object coordinates on LCD display
struct lcdPos_t {
    int16 x;
    int16 y;
};

struct objPos_t {
    lcdPos_t tl;    // Top left corner xy position on display
    lcdPos_t tr;    // Top right corner xy position on display
    lcdPos_t bl;    // Bottom left corner xy position on display
    lcdPos_t br;    // Bottom right corner xy position on display
};

// Touch relative position
struct tRelPos_t {
    int16 x = 0;            // Touch x position inside widget-object
    int16 y = 0;            // Touch y position inside widget-object
    bool inMargin = false;  // Touch position is inside declared margin
};

// Color RGB
struct color_t {
    uint8 r;
    uint8 g;
    uint8 b;
};

// Widget text
struct text_t {
    const char* text_p;
    int16 xPos;
    int16 yPos;
    gpuFont_t font;
    color_t color;
    gpuOpt_t option;
};

// Widget button
struct button_t {
    int16           x;              // Position or alignment offset x axis in pixel
    int16           y;              // Position or alignment offset y axis in pixel
    uint16          width;          // Button width in pixel
    uint16          height;         // Button height in pixel
    align_t         align;          // Align option
    gpuOpt_t        gpuOpt;         // Gpu option
    color_t         colorOff;       // Button when not pressed
    color_t         colorOn;        // Button color when pressed
    const char*     text_p;         // Button text-label
    gpuFont_t       textFont;       // Text font selection
    color_t         textColor;      // Text color
    uint8           touchTag = 0;   // Touch tag will be assigned automatically
    bool            lockTouch = false;   // Lock button touch function
};

enum class KeyEvent {
    None,
    Pressed,
    Released,
};

// Widget key-button
struct key_t {
    int16           x;                  // Position or alignment offset x axis in pixel
    int16           y;                  // Position or alignment offset y axis in pixel
    uint16          width;              // Key width in pixel
    uint16          height;             // Key height in pixel
    align_t         align;              // Align option
    gpuOpt_t        gpuOpt;             // Gpu option
    color_t         color;              // Key color
    color_t         colorOn;            // Key color when pressed
    const char*     label_p;            // Key label text
    gpuFont_t       labelFont;          // Key label text font
    color_t         labelColor;         // Key label text color
    uint16          pressedTimeMs = 0;  // Time in ms to get the KeyEvent::Pressed
};

// Widget rectangle
struct rectangle_t {
    int16           x;              // Position or alignment offset x axis in pixel
    int16           y;              // Position or alignment offset y axis in pixel
    uint16          width;          // Button width in pixel
    uint16          height;         // Button height in pixel
    uint16          cornerR;        // Corner radius in pixel
    align_t         align;          // Align option
    color_t         color;          // Button when not pressed
};

struct arc_t {
    int16 x;
    int16 y;
    uint16 radius;
    uint8 thickness;
    color_t color;
    uint16 startDeg;
    uint16 endDeg;
};

// Widget toggle
struct toggle_t {
    uint16          x;
    uint16          y;
    uint16          width;
    align_t         align;
    gpuOpt_t        gpuOpt;
    color_t         knobColor;
    color_t         barColorOff;
    color_t         barColorOn;
    color_t         textColor;
    const char*     text_p;
    gpuFont_t       textFont;
    State           state;
    uint8           touchTag = 0;   // Touch tag will be assigned automatically
};

// Widget slider with knob
struct slider_t {
    uint16          x;
    uint16          y;
    uint16          height;
    uint16          length;
    align_t         align;
    bool            vertical;       // Set to true for vertical slider
    color_t         knobColor;
    color_t         barColorLeft;   // Slider bar color left side of knob
    color_t         barColorRight;  // Slider bar color right side of knob
    gpuOpt_t        gpuOpt;
    uint16          value;
    uint16          maxRange;
    uint8           touchTag;
};

struct gauge_t {
    int16           x;
    int16           y;
    uint16          radius;
    uint8           majorTickCnt;   // Number of major subdivisions on the dial, 1-10
    uint8           minorTickCnt;   // Number of minor subdivisions on the dial, 1-10
    gpuOpt_t        gpuOpt;
    color_t         bgColor;        // Gauge background color
    color_t         tickColor;
    color_t         ptrColor;
    uint16          value;          // Pointer value
    uint16          maxRange;       // Gauge max range
    uint8           touchTag;
};

struct dial_t {
    int16           x;
    int16           y;
    uint16          radius;
    gpuOpt_t        gpuOpt;
    color_t         knobColor;
    color_t         markColor;
    uint16          value;          // Dila marker position, range 0-65535
    uint8           touchTag;
};

struct keyboard_t {
    int16           x;
    int16           y;
    uint16          width;
    uint16          height;
    align_t         align;
    gpuOpt_t        gpuOpt;
    const char*     keyText_p;
    gpuFont_t       textFont;
    color_t         textColor;
    color_t         keyColor;
};

constexpr color_t COLOR_WHITE    = {255, 255, 255};
constexpr color_t COLOR_BLACK    = {0, 0, 0};
constexpr color_t COLOR_RED      = {255, 0, 0};
constexpr color_t COLOR_GREEN    = {0, 255, 0};
constexpr color_t COLOR_BLUE     = {0, 0, 255};

} //END OF NAMESPACE 'drv::bt81x'
