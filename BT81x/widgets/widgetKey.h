#pragma once
#include "common.h"
#include "bt81xGpu.h"

namespace drv::bt81x::widget {

// Widget key-button
struct keyParam_t {
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
    uint16          pressedTimeMs = 0;  // Time in ms to get the KeyEvent::Pressed event
};

class Key {
public:
     void Setup(const keyParam_t& key, BT81x_GPU& gpu);
     void SetParam(const keyParam_t& key);
     Exec Display();
     bool CheckEvent(KeyEvent event);
     keyParam_t GetParam();
     uint8 GetTag();
     void Lock();
     void Unlock();

private:

     struct {
          keyParam_t param;
          Status status = Status::NONE;
          BT81x_GPU* gpu_p;
          uint8 touchTag = 0;
          KeyEvent event = KeyEvent::None;
          bool pressed;
          bool locked = false;
          uint64 sysTimeLast;
     } ctx = {};
};

// Widget key matrix
struct keyMatrixParam_t {
    uint16          width;              // Matrix width in pixel
    uint16          height;             // Matrix height in pixel
    uint8           spacing;            // Spacing between keeys in pixel
    uint8           countX;             // Key count horizontally
    uint8           countY;             // Key counnt vertically
    int16           x;                  // Position or alignment offset x axis in pixel
    int16           y;                  // Position or alignment offset y axis in pixel
    align_t         align;              // Align option
    gpuOpt_t        gpuOpt;             // Gpu option
    color_t         color;              // Key color
    color_t         colorOn;            // Key color when pressed
    const char**    labelList_p;        // Key label text list
    gpuFont_t       labelFont;          // Key label text font
    color_t         labelColor;         // Key label text color
    uint16          pressedTimeMs = 0;  // Time in ms to get the KeyEvent::Pressed event
    uint16          repeatAfterMs = 0;
    uint16          repeatIntervalMs = 0;
};

class KeyMatrix {
public:
     KeyMatrix() {};
     KeyMatrix(BT81x_GPU* parent_p) { ctx.gpu_p = parent_p; }

     void Setup(const keyMatrixParam_t& param, BT81x_GPU& gpu);
     void UpdateParam(const keyMatrixParam_t& param);
     Exec Display();
     bool CheckEvent(uint8 keyID, KeyEvent event);
     keyMatrixParam_t GetParam();
     uint8 GetTag(uint8 keyID);
     uint8 GetKeyCount();
     void Lock(uint8 keyID);
     void Unlock(uint8 keyID);
     void LockAll();
     void UnlockAll();

private:

     struct KeyParam {
          int16 x;
          int16 y;
          const char* label_p = nullptr;
          uint8 touchTag = 0;
          KeyEvent event = KeyEvent::None;
          bool pressed = false;
          bool repeat;
          bool locked = false;
          uint64 sysTimeLast;
          uint64 repeatSysTimeLast;
     };

     struct {
          keyMatrixParam_t param;
          Status status = Status::NONE;
          BT81x_GPU* gpu_p;
          Key* keyList_p;
          uint16 keyWidth;
          uint16 keyHeight;
          uint8 keyCnt;
          KeyParam* keyListNew_p;
     } ctx = {};
};

} // End of namespace 'drv::bt81x::widget'