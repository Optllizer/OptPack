#ifndef INPUT_DISPLAY
#define INPUT_DISPLAY
#include <kamek.hpp>
#include <UI/CtrlRaceBase/CustomCtrlRaceBase.hpp>
#include <MarioKartWii/Kart/KartManager.hpp>
#include <Settings/Settings.hpp>
#include <UI/UI.hpp>

namespace Pulsar {
namespace UI {

typedef float f32;

class CtrlRaceInputViewer : public CtrlRaceBase {
    enum DpadState {
        DpadState_Off,
        DpadState_Up,
        DpadState_Down,
        DpadState_Left,
        DpadState_Right,
        DpadState_Count // Invalid 
    };
    enum AccelState {
        AccelState_Off,
        AccelState_Pressed,
        AccelState_Count // Invalid 
    };
    enum Trigger {
        Trigger_L,
        Trigger_R,
        Trigger_BD, // Brake drift
        Trigger_Count // Invalid 
    };
    enum TriggerState {
        TriggerState_Off,
        TriggerState_Pressed,
        TriggerState_Count // Invalid 
    };
public:
    CtrlRaceInputViewer() {
        m_dpadState = DpadState_Off;
        m_dpadTimer = 0;
        m_accelState = AccelState_Off;
        m_triggerStates[Trigger_L] = TriggerState_Off;
        m_triggerStates[Trigger_R] = TriggerState_Off;
        m_triggerStates[Trigger_BD] = TriggerState_Off;
        m_stickState = Vec2(0.0f, 0.0f);
    }
    void Init() override;
    void OnUpdate() override;
    static u32 Count();
    static void Create(Page& page, u32 index, u32 count);
private:
    void Load(const char* variant, u8 id);
    void setDpad(DpadState state);
    void setAccel(AccelState state);
    void setTrigger(Trigger trigger, TriggerState state);
    void setStick(Vec2 state);
private:
    nw4r::lyt::Pane* m_dpadPanes[(int)DpadState_Count];
    nw4r::lyt::Pane* m_accelPanes[(int)AccelState_Count];
    nw4r::lyt::Pane* m_triggerPanes[(int)Trigger_Count][(int)TriggerState_Count];
    nw4r::lyt::Pane *m_stickPane;
    nw4r::math::VEC3 m_stickOrigin;
    DpadState m_dpadState;
    s8 m_dpadTimer; // Hold the DPAD press for DPAD_HOLD_FOR_N_FRAMES frames (?????)
    AccelState m_accelState;
    TriggerState m_triggerStates[(int)Trigger_Count];
    Vec2 m_stickState;
    u32 m_playerId;
    static const s8 DPAD_HOLD_FOR_N_FRAMES;
private:
    static const char* DpadStateToName(DpadState state) {
        switch (state) {
        case DpadState_Up: return "Up";
        case DpadState_Down: return "Down";
        case DpadState_Left: return "Left";
        case DpadState_Right: return "Right";
        default: return "Off";
        }
    }
    static const char* AccelStateToName(AccelState state) {
        switch (state) {
        case AccelState_Pressed: return "Pressed";
        default: return "Off";
        }
    }
    static const char* TriggerToName(Trigger trigger) {
        switch (trigger) {
        case Trigger_L: return "L";
        case Trigger_R: return "R";
        case Trigger_BD: return "BD";
        default: return "Off";
        }
    }
    static const char* TriggerStateToName(TriggerState state) {
        switch (state) {
        case TriggerState_Pressed: return "Pressed";
        default: return "Off";
        }
    }
};

}//namespace UI
}//namespace Pulsar
#endif