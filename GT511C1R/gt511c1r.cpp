#include "gt511c1r.h"

namespace drv {
namespace gt511c1r {

GT511C1R* GT511C1R::activeInstance_p = nullptr;

static uint16 Read16(const uint8* p)
{
    return static_cast<uint16>(static_cast<uint16>(p[0]) |
           (static_cast<uint16>(p[1]) << 8));
}

static uint32 Read32(const uint8* p)
{
    return static_cast<uint32>(p[0]) |
           (static_cast<uint32>(p[1]) << 8) |
           (static_cast<uint32>(p[2]) << 16) |
           (static_cast<uint32>(p[3]) << 24);
}

Exec GT511C1R::Setup(const config_t& config)
{
    if (initialized || activeInstance_p != nullptr ||
        config.UartTxPoll == nullptr) {
        return Exec::ERR;
    }
    ctx = config;
    rxIndex = 0;
    responseReady = false;
    waitingResponse = false;
    activeInstance_p = this;
    initialized = true;
    //ctx.UartSetRxCallback(&GT511C1R::RxTrampoline);
    return Exec::OK;
}

Exec GT511C1R::Deinit()
{
    if (!initialized || waitingResponse) return Exec::ERR;
    //ctx.UartSetRxCallback(nullptr);
    activeInstance_p = nullptr;
    initialized = false;
    return Exec::OK;
}

Exec GT511C1R::SendCommand(Command command, uint32 parameter)
{
    if (!initialized) return Exec::ERR;
    // A previous command must be completed/consumed before sending another.
    //if (waitingResponse || responseReady) return Exec::WAR;

    txPacket[0] = 0x55;
    txPacket[1] = 0xAA;
    txPacket[2] = static_cast<uint8>(ctx.deviceId);
    txPacket[3] = static_cast<uint8>(ctx.deviceId >> 8);
    for (uint8 i = 0; i < 4; ++i) {
        txPacket[4 + i] = static_cast<uint8>(parameter >> (8U * i));
    }
    const uint16 cmd = static_cast<uint16>(command);
    txPacket[8] = static_cast<uint8>(cmd);
    txPacket[9] = static_cast<uint8>(cmd >> 8);
    uint16 sum = 0;
    for (uint8 i = 0; i < 10; ++i) sum = static_cast<uint16>(sum + txPacket[i]);
    txPacket[10] = static_cast<uint8>(sum);
    txPacket[11] = static_cast<uint8>(sum >> 8);

    // ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    //     rxIndex = 0;
    //     responseReady = false;
    //     waitingResponse = true;
    // }
    const Exec result = ctx.UartTxPoll(txPacket, PACKET_SIZE);
    // if (result != Exec::OK) {
    //     ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { waitingResponse = false; rxIndex = 0; }
    // }
    return result;
}

void GT511C1R::RxTrampoline(uint8 data)
{
    if (activeInstance_p != nullptr) activeInstance_p->OnRxByte(data);
}

void GT511C1R::OnRxByte(uint8 data)
{
    if (!waitingResponse || responseReady) return;
    if (rxIndex == 0 && data != 0x55) return;
    if (rxIndex == 1 && data != 0xAA) {
        rxIndex = (data == 0x55) ? 1 : 0;
        return;
    }
    rxPacket[rxIndex] = data;
    rxIndex += 1;
    if (rxIndex != PACKET_SIZE) return;
    rxIndex = 0;
    uint8 bytes[PACKET_SIZE];
    for (uint8 i = 0; i < PACKET_SIZE; ++i) bytes[i] = rxPacket[i];
    uint16 sum = 0;
    for (uint8 i = 0; i < 10; ++i) sum = static_cast<uint16>(sum + bytes[i]);
    if (Read16(&bytes[10]) != sum || Read16(&bytes[2]) != ctx.deviceId) return;
    const uint16 code = Read16(&bytes[8]);
    if (code != ACK && code != NACK) return;
    response.parameter = Read32(&bytes[4]);
    response.responseCode = code;
    response.ack = (code == ACK);
    responseReady = true;
    waitingResponse = false;
}

} // namespace gt511c1r
} // namespace drv
