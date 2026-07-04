#include <Network/RoomKey.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <PulsarSystem.hpp>
#include <Network/MatchCommand.hpp>
#include <Settings/Settings.hpp>

namespace Pulsar {
namespace Network {

static const u32 HASH_ENCODE_RKEY = 0xF3CEF1A3;

u32 HARD_CODED_ROOM_KEY = 0x14E3D38D ^ HASH_ENCODE_RKEY;

u32 GetRoomKey() {
    return HARD_CODED_ROOM_KEY ^ HASH_ENCODE_RKEY;
}

} // namespace Network
} // namespace Pulsar 