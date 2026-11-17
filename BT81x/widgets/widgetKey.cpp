#include "widgetKey.h"

#undef CODE_STATUS
#undef ACTION_ON_ERR
#define CODE_STATUS     ctx.status
#define ACTION_ON_ERR   ctx.status = Status::ERR

namespace drv::bt81x::widget {

void Key::Setup(const keyParam_t& param, BT81x_GPU& gpu)
{
    ASSERT(gpu.GetStatus() == Status::OK, "GPU invalid status!");
    ctx.status = Status::INIT;
    SetParam(param);
    ctx.gpu_p = &gpu;
    ctx.status = Status::OK;
}

void Key::SetParam(const keyParam_t& param)
{
    CHECK_STATUS();
    ASSERT((param.width != 0) && (param.height != 0), "Invalid key width or height!");
    ASSERT(strlen(param.label_p) > 0, "Key label lenght is zero!");
    ASSERT((param.gpuOpt == GPU_OPT_3D) || (param.gpuOpt == GPU_OPT_FLAT), "Invalid GPU option!");
    ctx.param = param;
}

Exec Key::Display()
{
    CHECK_STATUS();

    uint8 romSlotNum = 0;
    lcdPos_t lcdPos;
    drv::bt81x::color_t keyColor;
    bool touchActive;
    uint8 touchTag;
    uint64 currentSysTime;

    keyColor = ctx.param.color;
    touchActive = ctx.gpu_p->ctx.touch.active;
    touchTag = ctx.gpu_p->ctx.touch.tag;
    currentSysTime = ctx.gpu_p->ctx.config.GetSysTimeMs();

    if (ctx.locked || !touchActive || touchTag != ctx.touchTag) {
        keyColor = ctx.param.color;
        ctx.sysTimeLast = currentSysTime;
        if (ctx.pressed && !touchActive) {
            ctx.event = KeyEvent::Released;
        }
        ctx.pressed = false;
    }
    else {
        keyColor = ctx.param.colorOn;
        if ((currentSysTime - ctx.sysTimeLast) > ctx.param.pressedTimeMs) {
            if (!ctx.pressed) ctx.event = KeyEvent::Pressed;
            ctx.pressed = true;
        }
    }

    CHECK_EXEC(ctx.gpu_p->CmdTouchTag(ctx.touchTag));
    CHECK_EXEC(ctx.gpu_p->CmdFgColor(keyColor));
    CHECK_EXEC(ctx.gpu_p->CmdColor(ctx.param.labelColor));

    //-----------------------------------------------------------------------------------
    // Adding key commands to display list

    ctx.gpu_p->AlignWidget(WIDGET_KEY, (void*)&ctx.param, lcdPos);
    *ctx.gpu_p->ctx.dlist.listEnd_p++ = GPU_CMD_ROMFONT;
    *ctx.gpu_p->ctx.dlist.listEnd_p++ = romSlotNum;
    *ctx.gpu_p->ctx.dlist.listEnd_p++ = ctx.param.labelFont;
    *ctx.gpu_p->ctx.dlist.listEnd_p++ = GPU_CMD_BUTTON;
    *ctx.gpu_p->ctx.dlist.listEnd_p++ = ((uint32)lcdPos.y << 16) | (lcdPos.x & 0xffff);
    *ctx.gpu_p->ctx.dlist.listEnd_p++ = ((uint32)ctx.param.height << 16) | (ctx.param.width & 0xffff);
    *ctx.gpu_p->ctx.dlist.listEnd_p++ = ((uint32)ctx.param.gpuOpt << 16) | (romSlotNum & 0xffff);
    ctx.gpu_p->CHECK_DISP_LIST_OVF(ctx.gpu_p->ctx.dlist);
    ctx.gpu_p->AppendText(ctx.param.label_p);
    CHECK_EXEC(ctx.gpu_p->ClearTouchTag());

    //-----------------------------------------------------------------------------------

    return Exec::OK;
}

bool Key::CheckEvent(KeyEvent event)
{
    CHECK_STATUS();
    if (ctx.event == event) {
        ctx.event = KeyEvent::None;
        return true;
    }
    return false;
}

keyParam_t Key::GetParam()
{
    CHECK_STATUS();
    return ctx.param;
}

uint8 Key::GetTag()
{
    CHECK_STATUS();
    return ctx.touchTag;
}

void Key::Lock()
{
    CHECK_STATUS();
    ctx.locked = true;
    ctx.pressed = false;
    ctx.event = KeyEvent::None;
}

void Key::Unlock()
{
    CHECK_STATUS();
    ctx.locked = false;
}

//---------------------------------------------------------------------------------------
// Key matrix

void KeyMatrix::Setup(const keyMatrixParam_t& param, BT81x_GPU& gpu)
{
    ASSERT(gpu.GetStatus() == Status::OK, "GPU invalid status!");
    ASSERT((param.width != 0) && (param.height != 0), "Invalid key matrix width or height!");
    ASSERT((param.gpuOpt == GPU_OPT_3D) || (param.gpuOpt == GPU_OPT_FLAT), "Invalid GPU option!");

    ctx.status = Status::INIT;
    ctx.keyListNew_p = (KeyParam*)calloc(param.countX * param.countY, sizeof(KeyParam));
    ASSERT(ctx.keyListNew_p != nullptr, "Memory allocation failed!");
    ctx.keyCnt = param.countX * param.countY;

    uint8 gapCntX = param.countX - 1;
    uint8 gapCntY = param.countY - 1;
    ctx.keyWidth = (uint16)((param.width - gapCntX * param.spacing) / param.countX);
    ctx.keyHeight = (uint16)((param.height - gapCntY * param.spacing) / param.countY);
    const char** labelList_p = param.labelList_p;
    KeyParam* key_p = ctx.keyListNew_p;
    int16 xPos;
    int16 yPos;
    lcdPos_t lcdPos;

    gpu.AlignWidget(WIDGET_KEY_MATRIX, (void*)&param, lcdPos);
    xPos = lcdPos.x;
    yPos = lcdPos.y;

    for (uint8 y = 0; y < param.countY; y++) {
        for (uint8 x = 0; x < param.countX; x++) {
            ASSERT(strlen(*labelList_p) > 0, "Key label lenght is zero!");
            key_p->x = xPos;
            key_p->y = yPos;
            key_p->label_p = *labelList_p;
            key_p++;
            labelList_p++;
            xPos += (int16)(ctx.keyWidth + param.spacing);
        }
        xPos = lcdPos.x;
        yPos += (int16)(ctx.keyHeight + param.spacing);
    }

    ctx.gpu_p = &gpu;
    ctx.param = param;
    ctx.status = Status::OK;
}

Exec KeyMatrix::Display()
{
    CHECK_STATUS();

    uint8 romSlotNum = 0;
    drv::bt81x::color_t keyColor;
    bool touchActive;
    uint8 touchTag;
    uint64 currentSysTime = 0;
    KeyParam* key_p = ctx.keyListNew_p;

    keyColor = ctx.param.color;
    touchActive = ctx.gpu_p->ctx.touch.active;
    touchTag = ctx.gpu_p->ctx.touch.tag;
    currentSysTime = ctx.gpu_p->ctx.config.GetSysTimeMs();

    for (uint8 keyID = 0; keyID < ctx.keyCnt; keyID++) {
        if (key_p->locked || !touchActive || touchTag != key_p->touchTag) {
            keyColor = ctx.param.color;
            key_p->sysTimeLast = currentSysTime;
            if (key_p->pressed && !touchActive) {
                key_p->event = KeyEvent::Released;
            }
            key_p->pressed = false;
            key_p->repeat = false;
        }
        else {
            keyColor = ctx.param.colorOn;
            if ((currentSysTime - key_p->sysTimeLast) > ctx.param.pressedTimeMs) {
                if (!key_p->pressed) key_p->event = KeyEvent::Pressed;
                key_p->pressed = true;
            }
            if (!key_p->repeat && ctx.param.repeatAfterMs) {
                if ((currentSysTime - key_p->sysTimeLast) > (ctx.param.pressedTimeMs + ctx.param.repeatAfterMs)) {
                    key_p->repeat = true;
                    key_p->repeatSysTimeLast = currentSysTime;
                    key_p->event = KeyEvent::Repeat;
                }
            }
            else if (key_p->repeat) {
                if ((currentSysTime - key_p->repeatSysTimeLast) > ctx.param.repeatIntervalMs) {
                    key_p->repeatSysTimeLast = currentSysTime;
                    key_p->event = KeyEvent::Repeat;
                }
            }
        }

        CHECK_EXEC(ctx.gpu_p->CmdTouchTag(key_p->touchTag));
        CHECK_EXEC(ctx.gpu_p->CmdFgColor(keyColor));
        CHECK_EXEC(ctx.gpu_p->CmdColor(ctx.param.labelColor));
        *ctx.gpu_p->ctx.dlist.listEnd_p++ = GPU_CMD_ROMFONT;
        *ctx.gpu_p->ctx.dlist.listEnd_p++ = romSlotNum;
        *ctx.gpu_p->ctx.dlist.listEnd_p++ = ctx.param.labelFont;
        *ctx.gpu_p->ctx.dlist.listEnd_p++ = GPU_CMD_BUTTON;
        *ctx.gpu_p->ctx.dlist.listEnd_p++ = ((uint32)key_p->y << 16) | (key_p->x & 0xffff);
        *ctx.gpu_p->ctx.dlist.listEnd_p++ = ((uint32)ctx.keyHeight << 16) | (ctx.keyWidth & 0xffff);
        *ctx.gpu_p->ctx.dlist.listEnd_p++ = ((uint32)ctx.param.gpuOpt << 16) | (romSlotNum & 0xffff);
        ctx.gpu_p->CHECK_DISP_LIST_OVF(ctx.gpu_p->ctx.dlist);
        ctx.gpu_p->AppendText(key_p->label_p);
        CHECK_EXEC(ctx.gpu_p->ClearTouchTag());
        key_p++;
    }

    return Exec::OK;
}

bool KeyMatrix::CheckEvent(uint8 keyID, KeyEvent event)
{
    CHECK_STATUS();
    RET_WAR_COND(keyID == ctx.keyCnt, "Invalid key ID!");
    RET_WAR_COND(keyID >= ctx.keyCnt, "Invalid key ID!");
    if (ctx.keyListNew_p[keyID].event == event) {
        ctx.keyListNew_p[keyID].event = KeyEvent::None;
        return true;
    }
    return false;
}

keyMatrixParam_t KeyMatrix::GetParam()
{
    CHECK_STATUS();
    return ctx.param;
}

uint8 KeyMatrix::GetTag(uint8 keyID)
{
    CHECK_STATUS();
    return ctx.keyListNew_p[keyID].touchTag;
}

uint8 KeyMatrix::GetKeyCount()
{
    return ctx.keyCnt;
}

void KeyMatrix::Lock(uint8 keyID)
{
    CHECK_STATUS();
    ctx.keyListNew_p[keyID].locked = true;
    ctx.keyListNew_p[keyID].pressed = false;
    ctx.keyListNew_p[keyID].repeat = false;
    ctx.keyListNew_p[keyID].event = KeyEvent::None;
}

void KeyMatrix::Unlock(uint8 keyID)
{
    CHECK_STATUS();
    ctx.keyListNew_p[keyID].locked = false;
}

void KeyMatrix::LockAll()
{
    for (uint8 keyID = 0; keyID < ctx.keyCnt; keyID++) {
        Lock(keyID);
    }
}

void KeyMatrix::UnlockAll()
{
    for (uint8 keyID = 0; keyID < ctx.keyCnt; keyID++) {
        Unlock(keyID);
    }
}

} // End of namespace 'drv::bt81x::widget'