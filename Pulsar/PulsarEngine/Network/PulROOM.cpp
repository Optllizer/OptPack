#include <kamek.hpp>
#include <MarioKartWii/RKNet/ROOM.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <Settings/UI/SettingsPanel.hpp>
#include <Settings/Settings.hpp>
#include <Network/Network.hpp>
#include <Network/PacketExpansion.hpp>

namespace Pulsar {
namespace Network {

//Implements the ability for a host to send a message, allowing for custom host settings

//If we are in a room, we are guaranteed to be in a situation where Pul packets are being sent
//however, no reason to send the settings outside of START packets and if we are not the host, this is easily changed by just editing the check

static void ConvertROOMPacketToData(const PulROOM& packet) {
    System* system = System::sInstance;
    system->netMgr.hostContext = packet.hostSystemContext;
    system->netMgr.racesPerGP = packet.raceCount;
}

static void SyncBlockedTracksFromHost(const PulROOM& packet) {
    System* system = System::sInstance;
    if (!system) return;

    Network::Mgr& netMgr = system->netMgr;
    const u32 localBlockingCount = system->GetInfo().GetTrackBlocking();

    if (netMgr.lastTracks == nullptr || localBlockingCount == 0) return;

    const u32 copyCount = (packet.blockedTrackCount < localBlockingCount) ? packet.blockedTrackCount : localBlockingCount;
    for (u32 i = 0; i < copyCount; ++i) {
        netMgr.lastTracks[i] = static_cast<PulsarId>(packet.blockedTracks[i]);
    }
    for (u32 i = copyCount; i < localBlockingCount; ++i) {
        netMgr.lastTracks[i] = PULSARID_NONE;
    }

    netMgr.curBlockingArrayIdx = packet.curBlockingArrayIdx % localBlockingCount;
}

static void WriteBlockedTracksToPacket(PulROOM* packet) {
    System* system = System::sInstance;
    if (!system) return;

    const Network::Mgr& netMgr = system->netMgr;
    const u32 blockingCount = system->GetInfo().GetTrackBlocking();

    const u32 writeCount = (blockingCount < MAX_TRACK_BLOCKING) ? blockingCount : MAX_TRACK_BLOCKING;
    packet->blockedTrackCount = static_cast<u8>(writeCount);
    packet->curBlockingArrayIdx = netMgr.curBlockingArrayIdx;

    for (u32 i = 0; i < writeCount; ++i) {
        packet->blockedTracks[i] = (netMgr.lastTracks != nullptr) ? static_cast<u16>(netMgr.lastTracks[i]) : 0xFFFF;
    }
    for (u32 i = writeCount; i < MAX_TRACK_BLOCKING; ++i) {
        packet->blockedTracks[i] = 0xFFFF;
    }
}

static bool ApplyHostContextLocally(u32 hostContext) {
    System* system = System::sInstance;

    const bool isStartOPTWW = hostContext & (1 << PULSAR_STARTOPTWW);
    const bool isStartOTWW  = hostContext & (1 << PULSAR_STARTOTTWW);

    u32 context = (isStartOPTWW << PULSAR_STARTOPTWW) |
                  (isStartOTWW  << PULSAR_STARTOTTWW);

    system->context = context;
    return (isStartOPTWW || isStartOTWW);
}

static void BeforeROOMSend(RKNet::PacketHolder<PulROOM>* packetHolder, PulROOM* src, u32 len) {
    packetHolder->Copy(src, len); //default

    const RKNet::Controller* controller = RKNet::Controller::sInstance;
    const RKNet::ControllerSub& sub = controller->subs[controller->currentSub];
    Pulsar::System* system = Pulsar::System::sInstance;
    PulROOM* destPacket = packetHolder->packet;
    if (destPacket->messageType == 1 && sub.localAid == sub.hostAid) {
        packetHolder->packetSize += sizeof(PulROOM) - sizeof(RKNet::ROOMPacket);
        const u8 originalMessage = destPacket->message;
        if (originalMessage >= 4 && originalMessage <= 9) {
            destPacket->message = 0;
        }
 
        const Settings::Mgr& settings = Settings::Mgr::Get();

        const u8 koSetting = settings.GetSettingValue(Settings::SETTINGSTYPE_KO, SETTINGKO_ENABLED) && destPacket->message == 0;

        bool isFroomStart = destPacket->message == 0;

        const u8 ottOnline = settings.GetSettingValue(Settings::SETTINGSTYPE_OTT, SETTINGOTT_ONLINE);
        const u8 regOnly = settings.GetSettingValue(Settings::SETTINGSTYPE_HOST, SETTINGHOST_ALLOW_REGS);


        const u8 isStartOPTWW = (originalMessage == 4);
        const u8 isStartOTWW = (originalMessage == 5);

        destPacket->hostSystemContext = (ottOnline != OTTSETTING_OFFLINE_DISABLED) << PULSAR_MODE_OTT //ott
            | (ottOnline == OTTSETTING_ONLINE_FEATHER) << PULSAR_FEATHER //ott feather
            | (settings.GetSettingValue(Settings::SETTINGSTYPE_OTT, SETTINGOTT_ALLOWUMTS) ^ true) << PULSAR_UMTS //ott umts
            | koSetting << PULSAR_MODE_KO
            | (settings.GetSettingValue(Settings::SETTINGSTYPE_HOST, SETTINGHOST_ALLOW_MIIHEADS) ^ false) << PULSAR_MIIHEADS
            | settings.GetSettingValue(Settings::SETTINGSTYPE_HOST, SETTINGHOST_RADIO_HOSTWINS) << PULSAR_HAW
            //| settings.GetSettingValue(Settings::SETTINGSTYPE_HOST, SETTINGHOST_ALLOW_REGS) << PULSAR_REGS
            //| (regOnly == HOSTSETTING_ALLOW_REGONLY_ENABLED) << PULSAR_REGSONLY
            | isStartOPTWW << PULSAR_STARTOPTWW
            | isStartOTWW << PULSAR_STARTOTTWW;

        u8 raceCount;
        if (koSetting == KOSETTING_ENABLED) raceCount = 0xFE;
        else switch (settings.GetSettingValue(Settings::SETTINGSTYPE_HOST, SETTINGHOST_SCROLL_GP_RACES)) {
        case(1):
            raceCount = 4;
            break;
        case(2):
            raceCount = 5;
            break;
        case(3):
            raceCount = 6;
            break;
        case(4):
            raceCount = 7;
            break;
        case(5):
            raceCount = 8;
            break;
        case(6):
            raceCount = 9;
            break;
        case(7):
            raceCount = 10;
            break;
        case(8):
            raceCount = 11;
            break;
        case(9):
            raceCount = 15;
            break;
        case(10):
            raceCount = 23;
            break;
        case(11):
            raceCount = 33;
            break;
        case(12):
            raceCount = 0;
            break;
        case(13):
            raceCount = 1;
            break;
        case(14):
            raceCount = 2;
            break;
        default:
            raceCount = 3;
        }
        destPacket->raceCount = raceCount;
        WriteBlockedTracksToPacket(destPacket);
        ConvertROOMPacketToData(*destPacket);
        (void)ApplyHostContextLocally(destPacket->hostSystemContext);
    }
}
kmCall(0x8065b15c, BeforeROOMSend);

kmWrite32(0x8065add0, 0x60000000);
static void AfterROOMReception(const RKNet::PacketHolder<PulROOM>* packetHolder, const PulROOM& src, u32 len) {
    register RKNet::ROOMPacket* packet;
    asm(mr packet, r28;);

    const RKNet::Controller* controller = RKNet::Controller::sInstance;
    const RKNet::ControllerSub& sub = controller->subs[controller->currentSub];

    if (src.messageType == 1 && sub.localAid != sub.hostAid && packetHolder->packetSize == sizeof(PulROOM)) {
        ConvertROOMPacketToData(src);
        (void)ApplyHostContextLocally(src.hostSystemContext);

        Page* topPage = SectionMgr::sInstance->curSection->GetTopLayerPage();
        PageId topId = topPage->pageId;
        if (topId == UI::SettingsPanel::id) {
            UI::SettingsPanel* panel = static_cast<UI::SettingsPanel*>(topPage);
            panel->OnBackPress(0);
        }
    }
    memcpy(packet, &src, sizeof(RKNet::ROOMPacket));
}
kmCall(0x8065add8, AfterROOMReception);
kmCall(0x806460B8, System::GetRaceCount);
kmCall(0x8064f51c, System::GetRaceCount);
}//namespace Network
}//namespace Pulsar