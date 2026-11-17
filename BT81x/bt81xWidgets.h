#pragma once
#include "common.h"
#include "bt81xTypes.h"
#include "widgetKey.h"

namespace drv::bt81x::widget {

struct keyboardParam_t {
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

// Widget rectangle
struct rectangleParam_t {
    int16           x;              // Position or alignment offset x axis in pixel
    int16           y;              // Position or alignment offset y axis in pixel
    uint16          width;          // Button width in pixel
    uint16          height;         // Button height in pixel
    uint16          cornerR;        // Corner radius in pixel
    align_t         align;          // Align option
    color_t         color;          // Button when not pressed
};

struct arcParam_t {
    int16 x;
    int16 y;
    uint16 radius;
    uint8 thickness;
    color_t color;
    uint16 startDeg;
    uint16 endDeg;
};

// Widget toggle
struct toggleParam_t {
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
struct sliderParam_t {
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

struct gaugeParam_t {
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

struct dialParam_t {
    int16           x;
    int16           y;
    uint16          radius;
    gpuOpt_t        gpuOpt;
    color_t         knobColor;
    color_t         markColor;
    uint16          value;          // Dila marker position, range 0-65535
    uint8           touchTag;
};

}