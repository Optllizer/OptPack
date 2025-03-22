#include <kamek.hpp>
#include <MarioKartWii/UI/Page/RaceMenu/RaceMenu.hpp>
#include <MarioKartWii/UI/Page/Other/TTSplits.hpp>
#include <MarioKartWii/UI/Page/Menu/MultiDriftSelect.hpp>
#include <MarioKartWii/UI/Page/Other/VR.hpp>
#include <MarioKartWii/GlobalFunctions.hpp>
#include <PulsarSystem.hpp>
#include <Gamemodes/OnlineTT/OnlineTT.hpp>
#include <Gamemodes/KO/KOMgr.hpp>
#include <Gamemodes/KO/KORaceEndPage.hpp>
#include <Network/PulSELECT.hpp>
#include <Network/PacketExpansion.hpp>
#include <Settings/Settings.hpp>
#include <SlotExpansion/CupsConfig.hpp>
#include <SlotExpansion/UI/ExpansionUIMisc.hpp>
#include <Gamemodes/OnlineTT/OTTRegional.hpp>


namespace Pulsar {
//For hooks which are shared by different things
namespace UI {

PageId TTSplitsGetNextPage(const Pages::TTSplits& splits) {
    const bool isOTTF = System::sInstance->IsContext(PULSAR_MODE_OTT) &&
    (RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_HOST) || 
    (RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_FROOM_NONHOST);
    const bool isOTTW = System::sInstance->IsContext(PULSAR_MODE_OTT) && 
    (RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_VS_REGIONAL) || 
    (RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_JOINING_REGIONAL);

    const SectionId sectionId = SectionMgr::sInstance->curSection->sectionId;
    if (isOTTW) return PAGE_WW_LEADERBOARDS_UPDATE;
    else if (isOTTF || sectionId == SECTION_GP) return PAGE_GPVS_LEADERBOARD_UPDATE;
    else if(sectionId == SECTION_TT || sectionId == SECTION_GHOST_RACE_1 || sectionId == SECTION_GHOST_RACE_2) return PAGE_TT_LEADERBOARDS;
    else if(sectionId >= SECTION_WATCH_GHOST_FROM_CHANNEL && sectionId <= SECTION_WATCH_GHOST_FROM_MENU) return PAGE_GHOST_REPLAY_PAUSE_MENU; //Enhanced Replay
    return PAGE_NONE;
}
kmBranch(0x808561dc, TTSplitsGetNextPage);

void LoadCorrectPageAfterMultiDrift(Pages::MultiDriftSelect* page, u32 animDirection, float animLength) {
    page->EndStateAnimated(animLength, animDirection);
    System* system = System::sInstance;
    SectionMgr* sectionMgr = SectionMgr::sInstance;
    if(system->ottVoteState == OTT::COMBO_SELECTION) {
        system->ottVoteState = OTT::COMBO_SELECTED;
        Network::ExpSELECTHandler& handler = Network::ExpSELECTHandler::Get();
        for(int i = 0; i < 2; ++i) {
            handler.toSendPacket.playersData[i].character = sectionMgr->sectionParams->characters[i];
            handler.toSendPacket.playersData[i].kart = sectionMgr->sectionParams->karts[i];
        }
        handler.toSendPacket.allowChangeComboStatus = Network::SELECT_COMBO_SELECTED;
    }
    else if(sectionMgr->curSection->Get<Pages::VR>() != nullptr && page->nextPageId == static_cast<PageId>(PULPAGE_SETTINGS)) {
        ExpSection::GetSection()->CreateAndInitPage(*ExpSection::GetSection(), PULPAGE_SETTINGS);
        return;
    }
    else if(sectionMgr->curSection->Get<Pages::VR>() != nullptr) {
        page->nextPageId = IsBattle() ? PAGE_BATTLE_CUP_SELECT : PAGE_CUP_SELECT;
    }
}
kmCall(0x8084b68c, LoadCorrectPageAfterMultiDrift);

//if more custom racemenus added in pulsar, make the func virtual
void RaceMenuExtraControls(Pages::RaceMenu& page, u32 gameControlCount) {

    const SectionId curSectionId = SectionMgr::sInstance->curSection->sectionId;
    if(curSectionId >= SECTION_P1_WIFI_FRIEND_VS || curSectionId >= SECTION_P2_WIFI_FRIEND_COIN) {
    if(page.pageId == KO::RaceEndPage::fakeId) return static_cast<KO::RaceEndPage&>(page).InitExtraControls(gameControlCount);
    }
    page.InitControlGroup(gameControlCount);
}
kmCall(0x80858ebc, RaceMenuExtraControls);

void CorrectRoomStartButton(Pages::Globe::MessageWindow& control, u32 bmgId, Text::Info* info) {
    //Network::SetGlobeMsgColor(control, -1);
    if (bmgId == BMG_PLAY_GP || bmgId == BMG_PLAY_TEAM_GP) {
        const u32 hostContext = System::sInstance->netMgr.hostContext;
        const bool isOTT = hostContext & (1 << PULSAR_MODE_OTT);
        const bool isKO = hostContext & (1 << PULSAR_MODE_KO);
        if (isOTT || isKO) {
            const bool isTeam = bmgId == BMG_PLAY_TEAM_GP;
            bmgId = (BMG_PLAY_OTT - 1) + isOTT + isKO * 2 + isTeam * 3;
        }
    }
    control.SetMessage(bmgId, info);
}
kmCall(0x805e4df4, CorrectRoomStartButton);

}//namespace UI
}//namespace Pulsar