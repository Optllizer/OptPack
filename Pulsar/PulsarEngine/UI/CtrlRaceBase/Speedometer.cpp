#include <MarioKartWii/Kart/KartManager.hpp>
#include <UI/CtrlRaceBase/Speedometer.hpp>
#include <Settings/Settings.hpp>

namespace Pulsar {
namespace UI {
u32 CtrlRaceSpeedo::Count() {
    const RacedataScenario& scenario = Racedata::sInstance->racesScenario;
    u32 localPlayerCount = scenario.localPlayerCount;
    const SectionId sectionId = SectionMgr::sInstance->curSection->sectionId;
    if(sectionId >= SECTION_WATCH_GHOST_FROM_CHANNEL && sectionId <= SECTION_WATCH_GHOST_FROM_MENU) localPlayerCount += 1;
    if(localPlayerCount == 0 && (scenario.settings.gametype & GAMETYPE_ONLINE_SPECTATOR)) localPlayerCount = 1;
    return localPlayerCount;
}
void CtrlRaceSpeedo::Create(Page& page, u32 index, u32 count) {
    u8 speedoType = (count == 3) ? 4 : count;
    for(int i = 0; i < count; ++i) {
        CtrlRaceSpeedo* som = new(CtrlRaceSpeedo);
        page.AddControl(index + i, *som, 0);
        char variant[0x20];
        int pos = i;
        if(count == 1 && Settings::Mgr::Get().GetSettingValue(Settings::SETTINGSTYPE_RACE, SETTINGRACE_RADIO_SOM) == RACESETTING_SOM_CLASSIC) {
            pos = 1;
            snprintf(variant, 0x20, "Speedo_%1d_%1d", 1, 0);
        } else if(count == 1 && Settings::Mgr::Get().GetSettingValue(Settings::SETTINGSTYPE_RACE, SETTINGRACE_RADIO_SOM) == RACESETTING_SOM_LEFT) {
            pos = 1;
            snprintf(variant, 0x20, "Speedo2_%1d_%1d", 1, 0);
        } else if(count == 1 && Settings::Mgr::Get().GetSettingValue(Settings::SETTINGSTYPE_RACE, SETTINGRACE_RADIO_SOM) == RACESETTING_SOM_RIGHT) {
            pos = 1; 
            snprintf(variant, 0x20, "Speedo4_%1d_%1d", 4, 3);
        } else if(count == 1 && Settings::Mgr::Get().GetSettingValue(Settings::SETTINGSTYPE_RACE, SETTINGRACE_RADIO_SOM) == RACESETTING_SOM_DISABLED){
            snprintf(variant, 0x20, "Speedo3_%1d_%1d", 4, 2);
        } else {
            pos = 1;
            snprintf(variant, 0x20, "Speedo_%1d_%1d", 1, 0);
        }
        som->Load(variant, i);
    }
}
static CustomCtrlBuilder SOM(CtrlRaceSpeedo::Count, CtrlRaceSpeedo::Create);

void CtrlRaceSpeedo::Load(const char* variant, u8 id) {
    this->hudSlotId = id;
    ControlLoader loader(this);
    const char* anims[] ={
        "Hundreds", "Hundreds", nullptr,
        "Tens", "Tens", nullptr,
        "Units", "Units", nullptr,
        "Dot", "Dot",nullptr,
        "Tenths", "Tenths", nullptr,
        "Hundredths", "Hundredths", nullptr,
        "Thousandths", "Thousandths", nullptr,
        nullptr
    };
    if(id == 0 && Settings::Mgr::Get().GetSettingValue(Settings::SETTINGSTYPE_RACE, SETTINGRACE_RADIO_SOM) == RACESETTING_SOM_CLASSIC) {
        loader.Load(UI::raceFolder, "PULSpeedo", variant, anims);
    } else if(id == 0 && Settings::Mgr::Get().GetSettingValue(Settings::SETTINGSTYPE_RACE, SETTINGRACE_RADIO_SOM) == RACESETTING_SOM_LEFT) {
        loader.Load(UI::raceFolder, "PULSpeedo2", variant, anims);
    } else if(id == 0 && Settings::Mgr::Get().GetSettingValue(Settings::SETTINGSTYPE_RACE, SETTINGRACE_RADIO_SOM) == RACESETTING_SOM_RIGHT) {
        loader.Load(UI::raceFolder, "PULSpeedo4", variant, anims);
    } else if (id == 0 && Settings::Mgr::Get().GetSettingValue(Settings::SETTINGSTYPE_RACE, SETTINGRACE_RADIO_SOM) == RACESETTING_SOM_DISABLED) {
        loader.Load(UI::raceFolder, "PULSpeedo3", variant, anims);
    } else {
      loader.Load(UI::raceFolder, "PULSpeedo", variant, anims);
    }
    this->Animate();
    return;
}

void CtrlRaceSpeedo::Init() {
    this->HudSlotColorEnable("speed0", true);
    this->HudSlotColorEnable("speed1", true);
    this->HudSlotColorEnable("speed2", true);
    this->HudSlotColorEnable("speed3", true);
    this->HudSlotColorEnable("speed4", true);
    this->HudSlotColorEnable("speed5", true);
    this->HudSlotColorEnable("speed6", true);
    this->HudSlotColorEnable("kmh", true);
    this->HudSlotColorEnable("optpack", true);
    LayoutUIControl::Init();
    return;
}

void CtrlRaceSpeedo::OnUpdate() {
    this->UpdatePausePosition();
    const u8 digits = Settings::Mgr::Get().GetSettingValue(Settings::SETTINGSTYPE_RACE, SETTINGRACE_SCROLL_SOM);
    const Kart::Pointers& pointers = Kart::Manager::sInstance->players[this->GetPlayerId()]->pointers;
    const Kart::Physics* physics = pointers.kartBody->kartPhysicsHolder->physics;

    Vec3 sum;
    MTX::PSVECAdd(&physics->engineSpeed, &physics->speed2, &sum);
    MTX::PSVECAdd(&physics->speed3, &sum, &sum);
    float speed = MTX::PSVECMag(&sum);
    float speedCap = pointers.kartMovement->hardSpeedLimit;
    if (speed > speedCap) speed = speedCap;

    const u32 speedValue = static_cast<u32>(speed * 1000.0f);

    // 10 means empty, 11 dot
    u32 hundreds = speedValue % 1000000 / 100000;
    u32 tens = speedValue % 100000 / 10000;
    u32 units = speedValue % 10000 / 1000;

    // Adjust handling of dot and decimals based on `digits` value
    u32 dot, tenths, hundredths, thousandths;

    if (digits == 0) {
        dot = 11; // show dot
        tenths = speedValue % 1000 / 100;
        hundredths = 10;
        thousandths = 10;
    } else if (digits == 1) {
        dot = 11; // show dot
        tenths = speedValue % 1000 / 100;
        hundredths = speedValue % 100 / 10;
        thousandths = 10;
    } else if (digits == 2) {
        dot = 11; // show dot
        tenths = speedValue % 1000 / 100;
        hundredths = speedValue % 100 / 10;
        thousandths = speedValue % 10;
    } else {
        dot = 10; // no dot
        tenths = 10;
        hundredths = 10;
        thousandths = 10;
    }

    // Adjust positioning for smaller speed values
    if (speedValue < 10000) { // shift everything by 2 to the left
        hundreds = units;
        tens = dot;
        units = tenths;
        dot = hundredths;
        tenths = thousandths;
        hundredths = 10;
        thousandths = 10;
    }
    else if (speedValue < 100000) { // shift by 1 to the left
        hundreds = tens;
        tens = units;
        units = dot;
        dot = tenths;
        tenths = hundredths;
        hundredths = thousandths;
        thousandths = 10;
    }

    SpeedArg args(hundreds, tens, units, dot, tenths, hundredths, thousandths);
    this->Animate(&args);
    return;
}

void CtrlRaceSpeedo::Animate(const SpeedArg* args) {
    for(int i = 0; i < 7; ++i) {
        AnimationGroup& group = this->animator.GetAnimationGroupById(i);
        float frame = 0.0f;
        if(args != nullptr) frame = static_cast<float>(args->values[i]);
        group.PlayAnimationAtFrameAndDisable(0, frame);
    }
}
}//namespace UI
}//namespace Pulsar