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

#include "common.h"
#include "bt81xTypes.h"

/******************************************************************************************************************************************
                                                                    API
******************************************************************************************************************************************/
namespace drv {

using namespace bt81x;

class BT81x_GPU {
public:
     using SysTimeMs_t = uint64 (*)();

     struct config_t {
          uint16 dlSize;                // Display list size in words (uint32)
          uint16 hSize;                 // Display horizontal size in pixel
          uint16 vSize;                 // Display vertical size in pixel
          SysTimeMs_t GetSysTimeMs;     // Get system tick in ms callback
     };

     Exec    Setup(const config_t& config);
     Exec    StartNewScreen(color_t backgroundColor);
     Exec    EndScreen();
     void    SetTouch(const touch_t& touch);
     dlist_t& GetDisplayList();
     uint16  GetDisplayHeight();
     uint16  GetDisplayWidth();

     Exec    CmdClear(bool color, bool stencil, bool tag);
     Exec    CmdTouchTag(uint8& touchTag);
     Exec    ClearTouchTag();
     Exec    CmdCalibTouch();
     Exec    CmdDispLogo();

     Exec    ClearColor(color_t color);
     Exec    ClearColorRgb(uint8 red, uint8 green, uint8 blue);
     Exec    CmdColor(color_t color);
     Exec    CmdColorRgb(uint8 red, uint8 green, uint8 blue);
     Exec    CmdBgColor(color_t color);
     Exec    CmdBgColorRgb(uint8 red, uint8 green, uint8 blue);
     Exec    CmdFgColor(color_t color);
     Exec    CmdFgColorRgb(uint8 red, uint8 green, uint8 blue);

     Exec    CmdText(int16 xPos, int16 yPos, gpuFont_t font, gpuOpt_t option, const char* text);
     Exec    CmdText(const text_t& text);
     Exec    CmdButton(button_t& button);
     Exec    CmdToggle(toggle_t& toggle);
     Exec    CmdSlider(slider_t& slider);
     Exec    CmdKeyboard(keyboard_t& keyboard, uint8 touchTag);
     Exec    CmdGauge(gauge_t& gauge);
     Exec    CmdDial(dial_t& dial);
     Exec    CmdTrackXY(int16 x, int16 y, uint16 width, uint16 length, uint8 tag);
     Exec    CmdTrackAngle(int16 centerX, int16 centerY, uint8 tag);

     Exec    DrawLine(int16 x1, int16 y1, int16 x2, int16 y2, uint16 width, color_t color, uint8 tag);
     Exec    DrawCircle(int16 x, int16 y, uint16 radius, color_t color, uint8 tag);
     Exec    Arc(int16 x, int16 y, uint16 radius, uint8 thickness, color_t color, uint16 startDeg, uint16 endDeg);
     Exec    DrawRectangle(int16 x, int16 y, uint16 width, uint16 height, uint8 radius, align_t align, color_t color, uint8 tag);
     Exec    DrawRectangle(rectangle_t rectangle);

     void      CalcWidgetPos(widget_t type, void* widget_p, objPos_t& pos);
     void      AlignWidget(widget_t type, void* widget_p, lcdPos_t& pos);
     tRelPos_t CalcTouchRelPos(const objPos_t& objPos, uint8 margin);

     class Key {
     public:
          Key() {};
          Key(BT81x_GPU* parent_p) { ctx.gpu_p = parent_p; }

          void Setup(const key_t& key, BT81x_GPU& gpu);
          void SetParam(const key_t& key);
          void Update(const key_t& key);
          Exec Display();
          bool CheckEvent(KeyEvent event);
          key_t GetParam();
          uint8 GetTag();
          void Lock();
          void Unlock();

     private:

          struct {
               key_t param;
               Status status = Status::NONE;
               BT81x_GPU* gpu_p;
               uint8 touchTag = 0;
               KeyEvent event = KeyEvent::None;
               bool pressed;
               bool locked = false;
               uint64 sysTimeLast;
          } ctx = {};

          static inline BT81x_GPU* gpu_p;
     };

private:

     static constexpr uint8 TOUCH_TAG_RESET_VAL = 128;
     static constexpr uint8 ALIGN_OFFSET = 1;
     static constexpr float PI = 3.14159265f;

     Exec BeginPrimitive(Primitive primitive);
     Exec Vertex2F(int16 x, int16 y);
     void AppendText(const char* text_p);

     struct {
          config_t config = {};
          Status status = Status::NONE;
          dlist_t dlist;
          uint8 currentTag;
          touch_t touch = {};
     } ctx = {};
};

} // End of namespace 'bt81x'
/******************************************************************************************************************************************
                                                                END OF FILE
******************************************************************************************************************************************/