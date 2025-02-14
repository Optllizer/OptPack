#include <UI/CtrlRaceBase/InputDisplay.hpp>

namespace Pulsar {
namespace UI {

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