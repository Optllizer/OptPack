#include <MarioKartWii/UI/Page/RaceHUD/RaceHUD.hpp>
#include <UI/UI.hpp>
#include <PulsarSystem.hpp>

//Expanded Pages:
#include <Ghost/UI/ExpGhostSelect.hpp>
#include <SlotExpansion/UI/ExpCupSelect.hpp>
#include <Settings/UI/ExpFroomPage.hpp>
#include <Settings/UI/ExpOptionsPage.hpp>
#include <Settings/UI/ExpWFCMainPage.hpp>
#include <UI/ChangeCombo/ChangeCombo.hpp>

//Pulsar Custom Pages:
#include <UI/TeamSelect/TeamSelect.hpp>
#include <Gamemodes/KO/KORaceEndPage.hpp>
#include <Gamemodes/KO/KOMgr.hpp>
#include <Gamemodes/KO/KOWinnerPage.hpp>
#include <Settings/UI/SettingsPanel.hpp>

namespace Pulsar {
namespace UI {

//ExpSection
static ExpSection* CreateSection() {
    return new ExpSection;
}
kmCall(0x8063504c, CreateSection);
kmWrite32(0x80635058, 0x60000000);

void ExpSection::CreatePages(ExpSection& self, SectionId id) {
    const System* system = System::sInstance;
    self.CreateSectionPages(id);
    memset(&self.pulPages, 0, sizeof(Page*) * PULPAGE_MAX);
    self.CreatePulPages();
}
kmCall(0x80622088, ExpSection::CreatePages);

void ExpSection::CreatePulPages() {

    const System* system = System::sInstance;
    switch(this->sectionId) {


        case SECTION_GP:                          //0x1e
        case SECTION_TT:                          //0x1f
        case SECTION_P1VS:                        //0x20
        case SECTION_P2VS:                        //0x21
        case SECTION_P3VS:                        //0x22
        case SECTION_P4VS:                        //0x23
        case SECTION_P1TEAM_VS:                   //0x24
        case SECTION_P2TEAM_VS:                   //0x25
        case SECTION_P3TEAM_VS:                   //0x26
        case SECTION_P4TEAM_VS:                   //0x27
            if(system->IsContext(PULSAR_MODE_OTT)) {
                this->CreateAndInitPage(*this, PAGE_TT_SPLITS);
                Pages::RaceHUD::sInstance->nextPageId = PAGE_TT_SPLITS;
            }
            break;
            //case SECTION_P1_WIFI_FROOM_VS_VOTING:      //0x60
            //case SECTION_P1_WIFI_FROOM_TEAMVS_VOTING:  //0x61
            //case SECTION_P1_WIFI_FROOM_BALLOON_VOTING: //0x62
            //case SECTION_P1_WIFI_FROOM_COIN_VOTING:    //0x63
            //case SECTION_P2_WIFI_FROOM_VS_VOTING:      //0x64
            //case SECTION_P2_WIFI_FROOM_TEAMVS_VOTING:  //0x65
            //case SECTION_P2_WIFI_FROOM_BALLOON_VOTING: //0x66
            //case SECTION_P2_WIFI_FROOM_COIN_VOTING:    //0x67

        case SECTION_P1_WIFI_VS: //0x68
        case SECTION_P2_WIFI_VS: //0x69
        case SECTION_P1_WIFI_FRIEND_VS:       //0x70
        case SECTION_P1_WIFI_FRIEND_TEAMVS:   //0x71
        case SECTION_P2_WIFI_FRIEND_VS:       //0x74
        case SECTION_P2_WIFI_FRIEND_TEAMVS:   //0x75
            if(system->IsContext(PULSAR_MODE_OTT)) {
                this->CreateAndInitPage(*this, PAGE_TT_SPLITS);
                Pages::RaceHUD::sInstance->nextPageId = PAGE_TT_SPLITS;
            }
            if(system->IsContext(PULSAR_MODE_KO)) {
                this->CreateAndInitPage(*this, KO::RaceEndPage::id);
                this->CreateAndInitPage(*this, KO::WinnerPage::id);
            }
            break;
        case SECTION_P1_WIFI_FRIEND_BALLOON:  //0x72
        case SECTION_P1_WIFI_FRIEND_COIN:     //0x73
        case SECTION_P2_WIFI_FRIEND_BALLOON:  //0x76
        case SECTION_P2_WIFI_FRIEND_COIN:     //0x77
        case SECTION_SINGLE_P_FROM_MENU:         //0x48
        case SECTION_SINGLE_P_TT_CHANGE_CHARA:   //0x49
        case SECTION_SINGLE_P_TT_CHANGE_COURSE:  //0x4a
        case SECTION_SINGLE_P_VS_NEXT_RACE:      //0x4b
        case SECTION_SINGLE_P_BT_NEXT_BATTLE:    //0x4c
        case SECTION_SINGLE_P_MR_CHOOSE_MISSION: //0x4d
        case SECTION_SINGLE_P_CHAN_RACE_GHOST:   //0x4e
        case SECTION_SINGLE_P_LIST_RACE_GHOST:   //0x50
        case SECTION_P1_WIFI:                    //0x55
        case SECTION_P1_WIFI_FROM_FROOM_RACE:    //0x56
        case SECTION_P1_WIFI_FROM_FIND_FRIEND:   //0x57
        case SECTION_P2_WIFI:                    //0x5b
        case SECTION_P2_WIFI_FROM_FROOM_RACE:    //0x5c
        case SECTION_OPTIONS:                    //0x8c
            this->CreateAndInitPage(*this, SettingsPanel::id);
    }
    if(this->hasAutoVote) {
        this->CreateAndInitPage(*this, PAGE_AUTO_ENDING2);
        this->CreateAndInitPage(*this, PAGE_MESSAGEBOX);
        this->CreateAndInitPage(*this, PAGE_SELECT_STAGE_MGR);
    }
    if(this->Get<ExpFroom>() != nullptr) this->CreateAndInitPage(*this, PULPAGE_TEAMSELECT); //can also put it as part of the case froom of createandinitpage
}

void ExpSection::CreateAndInitPage(ExpSection& self, u32 id) {


    Page* page;
    PageId initId = static_cast<PageId>(id); //in case a pulpage wants a specific init id
    switch(id) {
        case PAGE_CUP_SELECT:
            page = new ExpCupSelect;
            break;
        case PAGE_GHOST_SELECT:
            page = new ExpGhostSelect;
            break;
        case PAGE_FRIEND_ROOM:
            page = new ExpFroom;
            break;
            /* not needed since only static vars
        case PAGE_FRIEND_ROOM_MESSAGES:
            page = new ExpFroomMessages;
            break;
            */
        case PAGE_OPTIONS:
            page = new ExpOptions;
            break;
            /* not needed since only static vars
        case PAGE_SINGLE_PLAYER_MENU:
            page = new ExpSinglePlayer;
            break;
            */
        case PAGE_WFC_MAIN:
            page = new ExpWFCMain;
            break;
        case PAGE_WFC_MODE_SELECT:
            page = new ExpWFCModeSel;
            break;
        case PAGE_VR:
            page = new ExpVR;
            break;
        case PAGE_CHARACTER_SELECT:
            page = new ExpCharacterSelect;
            break;
        case PAGE_KART_SELECT:
            page = new ExpKartSelect;
            break;
        case PAGE_BATTLE_KART_SELECT:
            page = new ExpBattleKartSelect;
            break;
        case PAGE_MULTIPLAYER_KART_SELECT:
            page = new ExpMultiKartSelect;
            break;
        case PAGE_GPVS_LEADERBOARD_UPDATE:
            page = new ExpGPVSLeaderboardUpdate;
            break;
        case PAGE_GPVS_TOTAL_LEADERBOARDS:
            page = new ExpGPVSLeaderboardTotal;
            break;
        case PAGE_WW_LEADERBOARDS_UPDATE:
            page = new ExpWWLeaderboardUpdate;
            break;

            //PULPAGES
        case TeamSelect::id:
            page = new TeamSelect;
            break;
        case KO::RaceEndPage::id:
            initId = KO::RaceEndPage::fakeId;
            page = new KO::RaceEndPage;
            break;
        case KO::WinnerPage::id:
            page = new KO::WinnerPage;
            break;
        case SettingsPanel::id:
            page = new SettingsPanel;
            break;
        default:
            page = self.CreatePageById(initId);


    }
    if(id < PULPAGE_INITIAL) self.Set(page, initId);
    else self.SetPulPage(page, static_cast<PulPageId>(id));
    page->Init(initId);

}
kmBranch(0x80622d08, ExpSection::CreateAndInitPage);

void ExpSection::DisposePulPages(SectionPad& pad, bool enablePointer) {
    pad.EnablePointer(enablePointer); //default
    register ExpSection* section;
    asm(mr section, r31;);
    for(int pulPageId = 0; pulPageId < PULPAGE_MAX; ++pulPageId) {
        Page* page = section->pulPages[pulPageId];
        if(page != nullptr) {
            page->Dispose();
            delete page;
        }
    }
}
kmCall(0x80622268, ExpSection::DisposePulPages);

void ExpSection::AddPageLayer(ExpSection& self, u32 id) {
    AddPageLayerAnimatedReturnTopLayer(self, id, 0xff);
}
kmBranch(0x80622da0, ExpSection::AddPageLayer);

Page* ExpSection::AddPageLayerAnimatedReturnTopLayer(ExpSection& self, u32 id, u32 animDirection) {

    if(animDirection == 0xff) animDirection = self.animDirection;
    Page* page;
    if(id < PULPAGE_INITIAL) {
        page = self.pages[id];
    }
    else page = self.pulPages[id - PULPAGE_INITIAL];

    self.activePages[++self.layerCount] = page;
    if(animDirection != 0xffffffff) page->animationDirection = animDirection; //inlined Page::SetAnimDirection
    page->Activate();
    return page;
}
kmBranch(0x80622e00, ExpSection::AddPageLayerAnimatedReturnTopLayer);


kmWrite32(0x80623128, 0x48000020); //skip the usual activate
kmWrite32(0x80623140, 0x60000000); //nop layerCount increase, as AddPageLayer will do it for us
kmWrite32(0x80623144, 0x60000000); //nop setanimdirection as r29 is faulty

void ExpSection::SetNextPage(u32 id, u32 animDirection) {
    register ExpSection* self;
    asm(mr self, r28;);
    AddPageLayerAnimatedReturnTopLayer(*self, id, animDirection);
}
kmCall(0x8062314c, ExpSection::SetNextPage);

//Various Util funcs
void ChangeImage(LayoutUIControl& control, const char* paneName, const char* tplName) {
    TPLPalettePtr tplRes = static_cast<TPLPalettePtr>(control.layout.resources->multiArcResourceAccessor.GetResource(lyt::res::RESOURCETYPE_TEXTURE, tplName));
    if(tplRes != nullptr) control.layout.GetPaneByName(paneName)->GetMaterial()->GetTexMapAry()->ReplaceImage(tplRes);
};

//Implements the use of Pulsar's BMGHolder when needed
enum BMGType {
    BMG_NORMAL,
    CUSTOM_BMG,
    //CUP_TEXT
};
BMGType isCustom;

static int GetMsgIdxByBmgId(const BMGHolder& bmg, s32 bmgId) {
    const BMGMessageIds& msgIds = *bmg.messageIds;
    int ret = -1;
    for(int i = 0; i < msgIds.msgCount; ++i) {
        int curBmgId = msgIds.messageIds[i];
        if(curBmgId == bmgId) {
            ret = i;
            break;
        }
        else if(curBmgId > bmgId) break;
    }
    return ret;
}

static int GetMsgIdxById(const BMGHolder& normalHolder, s32 bmgId) {

    int ret = GetMsgIdxByBmgId(System::sInstance->GetBMG(), bmgId);
    if(ret < 0) {
        isCustom = BMG_NORMAL;
        ret = GetMsgIdxByBmgId(normalHolder, bmgId);
    }
    else isCustom = CUSTOM_BMG;
    return ret;
    //}
}
kmBranch(0x805f8c88, GetMsgIdxById);

wchar_t* GetMsgByMsgIdx(const BMGHolder& bmg, s32 msgIdx) {
    const BMGInfo& info = *bmg.info;
    if(msgIdx < 0 || msgIdx >= info.msgCount) return nullptr;
    const u32 offset = info.entries[msgIdx].dat1Offset & 0xFFFFFFFE;
    const BMGData& data = *bmg.data;
    return reinterpret_cast<wchar_t*>((u8*)&data + offset);
}

wchar_t* GetMsg(const BMGHolder& normalHolder, s32 msgIdx) {
    /*
    wchar_t* ret = nullptr;
    if(isCustom) ret = GetMsgByMsgIdx(System::sInstance->GetBMG(), msgIdx);
    if(ret == nullptr) ret = GetMsgByMsgIdx(normalHolder, msgIdx);
    return ret;
    */

    /*if(isCustom == CUP_TEXT) {
        CupsConfig* config = CupsConfig::sInstance;
        u32 idx = isCustom & 0xFFFF;
        switch((isCustom & ~0xFFFF) >> 12) {
            case(1):
                return config->GetCupName(static_cast<PulsarCupId>(idx));
            case(2):
                return config->GetTrackName(static_cast<PulsarId>(idx));
            case(3):
                return config->GetTrackAuthor(static_cast<PulsarId>(idx));
        }
    }
    else {*/
    wchar_t* ret = nullptr;
    if(isCustom == CUSTOM_BMG) ret = GetMsgByMsgIdx(System::sInstance->GetBMG(), msgIdx);
    if(ret == nullptr) ret = GetMsgByMsgIdx(normalHolder, msgIdx);
    return ret;
    //}
}
kmBranch(0x805f8cf0, GetMsg);

const u8* GetFontIndex(const BMGHolder& bmg, s32 msgIdx) {
    const BMGInfo& info = *bmg.info;
    if(msgIdx < 0 || msgIdx >= info.msgCount) return nullptr;
    return &info.entries[msgIdx].font;
};

const u8* GetFont(const BMGHolder& normalHolder, s32 msgIdx) {
    const u8* ret = nullptr;
    //if(isCustom == CUP_TEXT) msgIdx = 0;
    if(isCustom == CUSTOM_BMG) ret = GetFontIndex(System::sInstance->GetBMG(), msgIdx);
    if(ret == nullptr) ret = GetFontIndex(normalHolder, msgIdx);
    return ret;
}
kmBranch(0x805f8d2c, GetFont);

const wchar_t* GetCustomMsg(s32 bmgId) {
    const BMGHolder& bmg = System::sInstance->GetBMG();
    return GetMsgByMsgIdx(bmg, GetMsgIdxById(bmg, bmgId));
}
void ResetMatColor(lyt::Pane* pane, u32 color) {
    lyt::Material* mat = pane->material;
    ut::Color colors(color);
    mat->tevColours[0].r = colors.r;
    mat->tevColours[0].g = colors.g;
    mat->tevColours[0].b = colors.b;
    mat->tevColours[0].a = colors.a;
    mat->tevColours[1].r = 0xff;
    mat->tevColours[1].g = 0xff;
    mat->tevColours[1].b = 0xff;
    mat->tevColours[1].a = 0xff;
}
void UnbindRLMC(lyt::Material* mat) {
    for(ut::LinkList<lyt::AnimationLink, offsetof(lyt::AnimationLink, link)>::Iterator it = mat->animLinkList.GetBeginIter();
        it != mat->animLinkList.GetEndIter(); ++it) {
        if(!it->disable) {
            lyt::AnimTransform* anim = it->animTrans;
            u32 idx = it->idx;
            const lyt::res::AnimationBlock* res = anim->resource;
            u32 animOffsets = ut::ConvertOffsToPtr<u32>(res, res->animOffsetToAnimOffsetsArray)[idx];
            const lyt::res::AnimationContent* animContent = ut::ConvertOffsToPtr<lyt::res::AnimationContent>(res, animOffsets);

            const u32* animInfoOffsets = ut::ConvertOffsToPtr<u32>(animContent, sizeof(*animContent));
            for(int i = 0; i < animContent->infoCount; ++i) {
                const lyt::res::AnimationInfo* animInfo = ut::ConvertOffsToPtr<lyt::res::AnimationInfo>(animContent, animInfoOffsets[i]);
                if(animInfo->kind == lyt::res::ANIMATIONTYPE_RLMC) mat->UnbindAnimation(anim);
            }
        }
        if(mat->animLinkList.GetSize() == 0) break;
    }
}

void FasterRoulette(Pages::Vote& vote) { 
    vote.remainingRoulette = 0.0f;
    vote.remainingRouletteDuration = 0;
}
//kmBranch(0x806446B4, FasterRoulette);
//kmPatchExitPoint(FasterRoulette,0x806446BC);

//kmWrite32(0x80643BC4,0x60000000);
//kmWrite32(0x80643C2C,0x60000000);

//Switch between Names, Times, FCs by Rambo
void ExpGPVSLeaderboardTotal::OnUpdate() {
    if (checkLeaderboardDisplaySwapInputs()) {
        nextLeaderboardDisplayType();
        fillLeaderboardResults(GetRowCount(), this->results);
    }
    this->results[0]->SetTextBoxMessage("info", UI::BMG_RESULT_INFO);
}

void ExpGPVSLeaderboardUpdate::OnUpdate() {
    if (checkLeaderboardDisplaySwapInputs()) {
        nextLeaderboardDisplayType();
        fillLeaderboardResults(GetRowCount(), this->results);
    }
    this->results[0]->SetTextBoxMessage("info", UI::BMG_RESULT_INFO);
}

// Apply old toggle logic, set default display type
void ExpGPVSLeaderboardUpdate::BeforeEntranceAnimations() {
    if(System::sInstance->IsContext(PULSAR_MODE_OTT)) {
        setLeaderboardDisplayType(LEADERBOARD_DISPLAY_TIMES);
    } else {
        setLeaderboardDisplayType(LEADERBOARD_DISPLAY_NAMES);
    }

    fillLeaderboardResults(GetRowCount(), this->results);
}

void ExpWWLeaderboardUpdate::OnUpdate() {
    if (checkLeaderboardDisplaySwapInputs()) {
        nextLeaderboardDisplayType();
        fillLeaderboardResults(GetRowCount(), this->results);
    }
    this->results[0]->SetTextBoxMessage("info", UI::BMG_RESULT_INFO);
}
static LeaderboardDisplayType displayLeaderboardType = LEADERBOARD_DISPLAY_NAMES;

void setLeaderboardDisplayType(LeaderboardDisplayType type) {
    displayLeaderboardType = type;
}

LeaderboardDisplayType getLeaderboardDisplayType() {
    return displayLeaderboardType;
}

// In case we want to add more display types (FC, flags, whatever..)
void nextLeaderboardDisplayType() {
    if (displayLeaderboardType == LEADERBOARD_DISPLAY_NAMES) {
        displayLeaderboardType = LEADERBOARD_DISPLAY_TIMES;
    } else if (displayLeaderboardType == LEADERBOARD_DISPLAY_TIMES) {
        if (RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_NONE) {
            displayLeaderboardType = LEADERBOARD_DISPLAY_NAMES;
        } else {
            displayLeaderboardType = LEADERBOARD_DISPLAY_FC;
        }
    } else if (displayLeaderboardType == LEADERBOARD_DISPLAY_FC) {
        displayLeaderboardType = LEADERBOARD_DISPLAY_NAMES;
    }
}

bool isSectionSpectatorLiveView(SectionId id) {
    return id == SECTION_P1_WIFI_VS_LIVEVIEW || id == SECTION_P2_WIFI_VS_LIVEVIEW || id == SECTION_P1_WIFI_BT_LIVEVIEW || id == SECTION_P2_WIFI_BT_LIVEVIEW;
}

void fillLeaderboardResults(int count, CtrlRaceResult** results) {
    for(int i = 0; i < (count & 0xff); ++i) {
        const int position = (i + 1) & 0xff;
        const u8 playerId = Raceinfo::sInstance->playerIdInEachPosition[position - 1];

        if(displayLeaderboardType == LEADERBOARD_DISPLAY_TIMES) {
            results[i]->FillFinishTime(playerId);
        } else if(displayLeaderboardType == LEADERBOARD_DISPLAY_NAMES) {
            results[i]->FillName(playerId);
        } else if(displayLeaderboardType == LEADERBOARD_DISPLAY_FC) {
            if (playerId < 12) {
                u32 hudSlot = Racedata::sInstance->GetHudSlotId(playerId);
                u64 fc = RKNet::USERHandler::sInstance->receivedPackets[playerId].fc;

                // This is the local player, so we need to get the FC from the packet we send.
                if (hudSlot < 2 && !isSectionSpectatorLiveView(SectionMgr::sInstance->curSection->sectionId)) {
                    fc = RKNet::USERHandler::sInstance->toSendPacket.fc;
                }

                u32 fcParts[3];
                for (int j = 0; j < 3; ++j) {
                    fcParts[j] = fc % 10000;
                    fc /= 10000;
                }

                wchar_t fcText[16];
                swprintf(fcText, 16, L"%04d-%04d-%04d", fcParts[2], fcParts[1], fcParts[0]);

                if (wcscmp(fcText, L"0000-0000-0000") == 0) {
                    wcsncpy(fcText, L"----", 5);
                    continue;
                }

                Text::Info textInfo;
                textInfo.strings[0] = fcText;

                results[i]->SetTextBoxMessage("player_name", UI::BMG_TEXT, &textInfo);
                results[i]->ResetTextBoxMessage("time");
            }
        }
    }
}

bool checkLeaderboardDisplaySwapInputs() {
    const Input::RealControllerHolder* controllerHolder = SectionMgr::sInstance->pad.padInfos[0].controllerHolder;
    const ControllerType controllerType = controllerHolder->curController->GetType();
    const u16 inputs = controllerHolder->inputStates[0].buttonRaw;
    const u16 newInputs = (inputs & ~controllerHolder->inputStates[1].buttonRaw);

    bool swapDisplayType = false;
    switch (controllerType) {
    case NUNCHUCK:
    case WHEEL:
        swapDisplayType = (newInputs & WIIMOTE_DPAD_BUTTONS) != 0;
        break;
    case CLASSIC:
        swapDisplayType = (newInputs & (WPAD::WPAD_CL_TRIGGER_L | WPAD::WPAD_CL_TRIGGER_R | CLASSIC_DPAD_BUTTONS)) != 0;
        break;
    default:
        swapDisplayType = (newInputs & (PAD::PAD_BUTTON_L | PAD::PAD_BUTTON_R | GC_DPAD_BUTTONS)) != 0;
        break;
    }

    return swapDisplayType;
}

//Input Display by MKW-SP Team ported by Rambo
const s8 CtrlRaceInputViewer::DPAD_HOLD_FOR_N_FRAMES = 10;
void CtrlRaceInputViewer::Init() {
    char name[32];
    bool isBrakedriftToggled = System::sInstance->IsContext(PULSAR_200);
    RacedataScenario& raceScenario = Racedata::sInstance->racesScenario;
    
    for (int i = 0; i < (int)DpadState_Count; ++i) {
        DpadState state = static_cast<DpadState>(i);
        const char* stateName = CtrlRaceInputViewer::DpadStateToName(state);
        
        snprintf(name, 32, "Dpad%.*s", strlen(stateName), stateName);
        nw4r::lyt::Pane* pane = this->layout.GetPaneByName(name);
        this->SetPaneVisibility(name, state == DpadState_Off);
        this->m_dpadPanes[i] = pane;
        
        this->HudSlotColorEnable(name, true);
    }
    
    for (int i = 0; i < (int)AccelState_Count; ++i) {
        AccelState state = static_cast<AccelState>(i);
        const char* stateName = CtrlRaceInputViewer::AccelStateToName(state);
        
        snprintf(name, 32, "Accel%.*s", strlen(stateName), stateName);
        nw4r::lyt::Pane* pane = this->layout.GetPaneByName(name);
        this->SetPaneVisibility(name, state == AccelState_Off);
        if (isBrakedriftToggled) {
            pane->trans.x += pane->scale.x * 15.0f;
            pane->trans.y += pane->scale.z * 15.0f;
        }
        this->m_accelPanes[i] = pane;
        
        this->HudSlotColorEnable(name, true);
    }
    
    for (int i = 0; i < (int)Trigger_Count; ++i) {
        Trigger trigger = static_cast<Trigger>(i);
        const char* triggerName = CtrlRaceInputViewer::TriggerToName(trigger);
        
        for (int j = 0; j < (int)TriggerState_Count; ++j) {
            TriggerState state = static_cast<TriggerState>(j);
            const char* stateName = CtrlRaceInputViewer::TriggerStateToName(state);
            
            snprintf(name, 32, "Trigger%.*s%.*s", strlen(triggerName), triggerName, strlen(stateName), stateName);
            nw4r::lyt::Pane* pane = this->layout.GetPaneByName(name);
            this->SetPaneVisibility(name, state == TriggerState_Off);
            if (!isBrakedriftToggled && trigger == Trigger_BD) {
                this->SetPaneVisibility(name, false);
            }
            this->m_triggerPanes[i][j] = pane;
            
            this->HudSlotColorEnable(name, true);
        }
    }
    
    this->m_stickPane = this->layout.GetPaneByName("Stick");
    this->m_stickOrigin = this->m_stickPane->trans;
    this->m_playerId = this->GetPlayerId();
    this->HudSlotColorEnable("Stick", true);
    this->HudSlotColorEnable("StickBackdrop", true);

    LayoutUIControl::Init();
}

void CtrlRaceInputViewer::OnUpdate() {
    this->UpdatePausePosition();
    u8 playerId = this->GetPlayerId();
    if (playerId != m_playerId) {
        m_dpadTimer = 0;
        m_playerId = playerId;
    }
    RacedataScenario& raceScenario = Racedata::sInstance->racesScenario;
    if (playerId < raceScenario.playerCount) {
        RaceinfoPlayer* player = Raceinfo::sInstance->players[playerId];
        if (player) {
            Input::State* input = &player->realControllerHolder->inputStates[0];
            DpadState dpadState = (DpadState)input->motionControlFlick;
            Vec2 stick = input->stick;
            // Check mirror mode
            if (raceScenario.settings.modeFlags & 1) {
                stick.x = -stick.x;
                if (input->motionControlFlick == DpadState_Left) {
                    dpadState = DpadState_Right;
                } else if (input->motionControlFlick == DpadState_Right) {
                    dpadState = DpadState_Left;
                }
            }
            
            bool accel = input->buttonActions & 0x1;
            bool L = input->buttonActions & 0x4;
            bool R = (input->buttonActions & 0x8) || (input->buttonActions & 0x2);
            setDpad(dpadState);
            setAccel(accel ? AccelState_Pressed : AccelState_Off);
            setTrigger(Trigger_L, L ? TriggerState_Pressed : TriggerState_Off);
            setTrigger(Trigger_R, R ? TriggerState_Pressed : TriggerState_Off);
            setStick(stick);
            bool isBrakedriftToggled = System::sInstance->IsContext(PULSAR_200);
            if (isBrakedriftToggled) {
                bool BD = input->buttonActions & 0x2;
                setTrigger(Trigger_BD, BD ? TriggerState_Pressed : TriggerState_Off);
            }
        }
    }
}
u32 CtrlRaceInputViewer::Count() {
    if(Settings::Mgr::Get().GetSettingValue(Settings::SETTINGSTYPE_MENU, SETTINGMENU_RADIO_INPUT) == MENUSETTING_INPUT_DISABLED)
        return 0;
    else if(Settings::Mgr::Get().GetSettingValue(Settings::SETTINGSTYPE_MENU, SETTINGMENU_RADIO_INPUT) == MENUSETTING_INPUT_ENABLED) {
        // Declare and initialize scenario here
        const RacedataScenario& scenario = Racedata::sInstance->racesScenario;
        u32 localPlayerCount = scenario.localPlayerCount;
        const SectionId sectionId = SectionMgr::sInstance->curSection->sectionId;
        if(sectionId >= SECTION_WATCH_GHOST_FROM_CHANNEL && sectionId <= SECTION_WATCH_GHOST_FROM_MENU) 
            localPlayerCount += 1;
        if(localPlayerCount == 0 && (scenario.settings.gametype & GAMETYPE_ONLINE_SPECTATOR)) 
            localPlayerCount = 1;
        return localPlayerCount;
    }
    return 0; 
}
void CtrlRaceInputViewer::Create(Page& page, u32 index, u32 count) {
    u8 variantId = (count == 3) ? 4 : count;
    for(int i = 0; i < count; ++i) {
        CtrlRaceInputViewer* inputViewer = new CtrlRaceInputViewer;
        page.AddControl(index + i, *inputViewer, 0);
        char variant[0x20];
        int pos = i;
        snprintf(variant, 0x20, "InputDisplay_%u_%u", variantId, pos);
        inputViewer->Load(variant, i);
    }
}
static CustomCtrlBuilder INPUTVIEWER(CtrlRaceInputViewer::Count, CtrlRaceInputViewer::Create);
void CtrlRaceInputViewer::Load(const char* variant, u8 id) {
    this->hudSlotId = id;
    ControlLoader loader(this);
    const char* groups[] = { nullptr, nullptr };
    loader.Load(UI::raceFolder, "PULInputViewer", variant, groups);
}
void CtrlRaceInputViewer::setDpad(DpadState state) {
    if (state == m_dpadState) {
        return;
    }
    // Only hold for off press
    if (state == DpadState_Off && m_dpadTimer != 0 && --m_dpadTimer) {
        return;
    }
    m_dpadPanes[static_cast<u32>(m_dpadState)]->flag &= ~1;
    m_dpadPanes[static_cast<u32>(state)]->flag |= 1;
    m_dpadState = state;
    m_dpadTimer = DPAD_HOLD_FOR_N_FRAMES;
}
void CtrlRaceInputViewer::setAccel(AccelState state) {
    if (state == m_accelState) {
        return;
    }
    m_accelPanes[static_cast<u32>(m_accelState)]->flag &= ~1;
    m_accelPanes[static_cast<u32>(state)]->flag |= 1;
    m_accelState = state;
}
void CtrlRaceInputViewer::setTrigger(Trigger trigger, TriggerState state) {
    u32 t = static_cast<u32>(trigger);
    if (state == m_triggerStates[t]) {
        return;
    }
    m_triggerPanes[t][static_cast<u32>(m_triggerStates[t])]->flag &= ~1;
    m_triggerPanes[t][static_cast<u32>(state)]->flag |= 1;
    m_triggerStates[t] = state;
}
void CtrlRaceInputViewer::setStick(Vec2 state) {
    if (state.x == m_stickState.x && state.z == m_stickState.z) {
        return;
    }
    // Map range [-1, 1] -> [-width * 5 / 19, width * 5 / 19]
    f32 scale = 5.0f / 19.0f;
    m_stickPane->trans.x =
            m_stickOrigin.x + scale * state.x * m_stickPane->scale.x * m_stickPane->size.x;
    m_stickPane->trans.y =
            m_stickOrigin.y + scale * state.z * m_stickPane->scale.z * m_stickPane->size.z;
    m_stickState = state;
}
}//namespace UI
}//namespace Pulsar