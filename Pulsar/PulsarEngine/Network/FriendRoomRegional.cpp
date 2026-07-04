#include <kamek.hpp>
#include <MarioKartWii/System/Identifiers.hpp>
#include <core/System/SystemManager.hpp>
#include <MarioKartWii/UI/Section/SectionMgr.hpp>
#include <MarioKartWii/UI/Section/Section.hpp>
#include <MarioKartWii/Race/Racedata.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <Config.hpp>
#include <PulsarSystem.hpp>
#include <Network/Network.hpp>

namespace Pulsar {
namespace Network {

static u32 REGIONID = 0x29A;
extern void ResetTrackBlockingOnRoomEnd();

static void SetRegionId(u32 regionId) {
    if (Pulsar::System::sInstance->IsContext(PULSAR_STARTOPTWW)) {
        REGIONID = 0x29A;
    }
    else if (Pulsar::System::sInstance->IsContext(PULSAR_STARTOTTWW)) {
        REGIONID = 0x29B;
    }
    else {
        REGIONID = regionId;
    }
}
static SectionLoadHook setRegionIdHook(SetRegionId);

static SectionId ConvertToRegionalSection(SectionId id) {
    switch (id) {
        case SECTION_P1_WIFI_FROOM_VS_VOTING:
        case SECTION_P1_WIFI_FROOM_TEAMVS_VOTING:
            return SECTION_P1_WIFI_VS_VOTING;
        case SECTION_P2_WIFI_FROOM_VS_VOTING:
        case SECTION_P2_WIFI_FROOM_TEAMVS_VOTING:
            return SECTION_P2_WIFI_VS_VOTING;
        case SECTION_P1_WIFI_FRIEND_VS:
        case SECTION_P1_WIFI_FRIEND_TEAMVS:
            return SECTION_P1_WIFI_VS;
        case SECTION_P2_WIFI_FRIEND_VS:
        case SECTION_P2_WIFI_FRIEND_TEAMVS:
            return SECTION_P2_WIFI_VS;
        default:
            return id;
    }
}

static bool ConvertFriendRoomStateToRegional() {
    RKNet::Controller* controller = RKNet::Controller::sInstance;
    System* system = System::sInstance;
    if (controller == nullptr || system == nullptr) {
        return false;
    }

    const bool wasHost = controller->roomType == RKNet::ROOMTYPE_FROOM_HOST;
    const bool wasNonHost = controller->roomType == RKNet::ROOMTYPE_FROOM_NONHOST;
    if (!wasHost && !wasNonHost) {
        return false;
    }

    Racedata* racedata = Racedata::sInstance;
    if (racedata == nullptr) {
        return false;
    }

    RacedataSettings& menuSettings = racedata->menusScenario.settings;
    if (menuSettings.gamemode != MODE_PRIVATE_VS) {
        return false;
    }

    menuSettings.gamemode = MODE_PUBLIC_VS;
    menuSettings.modeFlags &= ~static_cast<u32>(2);
    menuSettings.gametype = GAMETYPE_DEFAULT;

    RacedataSettings& raceSettings = racedata->racesScenario.settings;
    raceSettings.gamemode = MODE_PUBLIC_VS;
    raceSettings.modeFlags &= ~static_cast<u32>(2);
    raceSettings.gametype = GAMETYPE_DEFAULT;

    const u8 localPlayerCount = controller->subs[controller->currentSub].localPlayerCount;
    const u8 totalPlayerCount = controller->subs[controller->currentSub].playerCount;

    Network::Mgr& netMgr = system->netMgr;
    netMgr.region = REGIONID;

    controller->roomType = RKNet::ROOMTYPE_VS_REGIONAL;
    controller->localStatusData.regionId = REGIONID;

    controller->UpdateStatusDatas();
{
    Mgr& netMgr = system->netMgr;
    const u32 blockingCount = system->GetInfo().GetTrackBlocking();

    if (netMgr.lastTracks != nullptr && blockingCount > 0) {
        for (u32 i = 0; i < blockingCount; ++i) {
            netMgr.lastTracks[i] = PULSARID_NONE;
        }
        netMgr.curBlockingArrayIdx = 0;
        }
}   
    system->context = 0;
    system->UpdateContext();
    controller->StartMatching();
    return true;
}

static bool ShouldConvert(SectionId nextSectionId) {
    switch (nextSectionId) {
        case SECTION_P1_WIFI_FROOM_VS_VOTING:
        case SECTION_P1_WIFI_FROOM_TEAMVS_VOTING:
        case SECTION_P2_WIFI_FROOM_VS_VOTING:
        case SECTION_P2_WIFI_FROOM_TEAMVS_VOTING:
        case SECTION_P1_WIFI_FRIEND_VS:
        case SECTION_P1_WIFI_FRIEND_TEAMVS:
        case SECTION_P2_WIFI_FRIEND_VS:
        case SECTION_P2_WIFI_FRIEND_TEAMVS:
            return true;
        default:
            return false;
    }
}

static void ApplyNextSection(SectionMgr* sectionMgr, SectionId nextSectionId, u32 animDirection) {
    const SectionId currentNext = sectionMgr->nextSectionId;
    if (currentNext != nextSectionId) {
        const int currentPriority = sectionMgr->GetSectionPriority(currentNext);
        const int newPriority = sectionMgr->GetSectionPriority(nextSectionId);
        if (currentPriority < newPriority) {
            sectionMgr->nextSectionId = nextSectionId;
            sectionMgr->fadeAnimIdx = animDirection;
        }
    }
}

static void SetNextSectionRegionalHook(SectionMgr* sectionMgr, SectionId nextSectionId, u32 animDirection) {
    SetRegionId(REGIONID);
    bool isFroom = RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_HOST ||
                   RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_NONHOST;
    if ((Pulsar::System::sInstance->IsContext(PULSAR_STARTOPTWW) ||
         Pulsar::System::sInstance->IsContext(PULSAR_STARTOTTWW)) && isFroom) {
        static bool hasConverted = false;
        SectionId desiredSection = nextSectionId;

        if (DWC::MatchControl* matchControl = DWC::MatchControl::sInstance) {
            volatile u8* ctrlBytes = reinterpret_cast<volatile u8*>(matchControl);
            ctrlBytes[0x15] = DWC::MATCH_TYPE_ANYBODY;
        }

        RKNet::Controller* controller = RKNet::Controller::sInstance;
        if (controller != nullptr) {
            if (controller->roomType == RKNet::ROOMTYPE_FROOM_HOST ||
                controller->roomType == RKNet::ROOMTYPE_FROOM_NONHOST) {
                hasConverted = false;
                if (ShouldConvert(nextSectionId)) {
                    SectionId regionalSection = ConvertToRegionalSection(nextSectionId);
                    if (!hasConverted && ConvertFriendRoomStateToRegional()) {
                        hasConverted = true;
                        desiredSection = regionalSection;
                    }
                }
            } else if (hasConverted) {
                desiredSection = ConvertToRegionalSection(nextSectionId);
            }
        }
        ApplyNextSection(sectionMgr, desiredSection, animDirection);
    } else {
        ApplyNextSection(sectionMgr, nextSectionId, animDirection);
    }
}
kmBranch(0x80635a3c, SetNextSectionRegionalHook);


}  // namespace Network
}  // namespace Pulsar