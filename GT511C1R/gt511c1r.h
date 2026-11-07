#pragma once
#include "common.h"

namespace drv {
namespace gt511c1r {

constexpr uint8 PACKET_SIZE = 12;
constexpr uint16 DEFAULT_DEVICE_ID = 0x0001;
constexpr uint16 ACK = 0x0030;
constexpr uint16 NACK = 0x0031;

enum class Command : uint16 {
    OPEN             = 0x0001,
    CLOSE            = 0x0002,
    USB_INTERNAL_CHECK = 0x0003,
    CHANGE_BAUDRATE  = 0x0004,
    SET_LED          = 0x0012,
    GET_ENROLL_COUNT = 0x0020,
    CHECK_ENROLLED   = 0x0021,
    ENROLL_START     = 0x0022,
    ENROLL_1         = 0x0023,
    ENROLL_2         = 0x0024,
    ENROLL_3         = 0x0025,
    IS_PRESS_FINGER  = 0x0026,
    DELETE_ID        = 0x0040,
    DELETE_ALL       = 0x0041,
    VERIFY           = 0x0050,
    IDENTIFY         = 0x0051,
    CAPTURE_FINGER   = 0x0060,
    MAKE_TEMPLATE    = 0x0061,
    GET_IMAGE        = 0x0062,
    GET_RAW_IMAGE    = 0x0063,
    GET_TEMPLATE     = 0x0070,
    SET_TEMPLATE     = 0x0071,
};

using RxCallback_t = void (*)(uint8 data);
using UartTxPoll_t = Exec (*)(uint8* data_p, uint8 size);
using UartSetRxCallback_t = void (*)(RxCallback_t callback);

struct config_t {
    UartTxPoll_t UartTxPoll = nullptr;
    uint16 deviceId = DEFAULT_DEVICE_ID;
};

// Command-only driver: sends 12-byte packets and receives 12-byte ACK/NACK packets.
// A received response is made available by GetResponse(); no blocking waits in RX ISR.
class GT511C1R {
public:
    Exec Setup(const config_t& config);
    Exec Deinit();
    Exec SendCommand(Command command, uint32 parameter = 0);

    Exec Open()                 { return SendCommand(Command::OPEN); }
    Exec Close()                { return SendCommand(Command::CLOSE); }
    Exec SetLed(bool on)        { return SendCommand(Command::SET_LED, on ? 1UL : 0UL); }
    Exec GetEnrollCount()       { return SendCommand(Command::GET_ENROLL_COUNT); }
    Exec CheckEnrolled(uint16 id) { return SendCommand(Command::CHECK_ENROLLED, id); }
    Exec EnrollStart(uint16 id) { return SendCommand(Command::ENROLL_START, id); }
    Exec Enroll1()              { return SendCommand(Command::ENROLL_1); }
    Exec Enroll2()              { return SendCommand(Command::ENROLL_2); }
    Exec Enroll3()              { return SendCommand(Command::ENROLL_3); }
    Exec IsPressFinger()        { return SendCommand(Command::IS_PRESS_FINGER); }
    Exec DeleteID(uint16 id)   { return SendCommand(Command::DELETE_ID, id); }
    Exec DeleteAll()            { return SendCommand(Command::DELETE_ALL); }
    Exec Verify(uint16 id)     { return SendCommand(Command::VERIFY, id); }
    Exec Identify()             { return SendCommand(Command::IDENTIFY); }
    Exec CaptureFinger(bool bestImage = true) { return SendCommand(Command::CAPTURE_FINGER, bestImage ? 1UL : 0UL); }
    Exec ChangeBaudrate(uint32 baudrate) { return SendCommand(Command::CHANGE_BAUDRATE, baudrate); }

    struct response_t {
        uint32 parameter = 0;
        uint16 responseCode = 0;
        bool ack = false;
    };

    // Returns true once for each completed valid response.
    bool GetResponse(response_t& response);
    bool IsWaitingResponse() const { return waitingResponse; }

private:
    static GT511C1R* activeInstance_p;
    static void RxTrampoline(uint8 data);
    void OnRxByte(uint8 data);

    config_t ctx = {};
    uint8 txPacket[PACKET_SIZE] = {};
    volatile uint8 rxPacket[PACKET_SIZE] = {};
    volatile uint8 rxIndex = 0;
    volatile bool responseReady = false;
    volatile bool waitingResponse = false;
    response_t response = {};
    bool initialized = false;
};

} // namespace gt511c1r
} // namespace drv
