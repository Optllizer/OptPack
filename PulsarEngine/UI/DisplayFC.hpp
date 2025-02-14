#ifndef DISPLAY_FC
#define DISPLAY_FC
#include <kamek.hpp>
#include <MarioKartWii/UI/Ctrl//CtrlRace/CtrlRaceResult.hpp>
#include <MarioKartWii/UI/Page/Leaderboard/GPVSLeaderboardUpdate.hpp>
#include <MarioKartWii/UI/Page/Leaderboard/GPVSLeaderboardTotal.hpp>
#include <MarioKartWii/Race/RaceInfo/RaceInfo.hpp>
#include <MarioKartWii/UI/Page/Leaderboard/WWLeaderboardUpdate.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <MarioKartWii/RKNet/USER.hpp>
#include <Settings/Settings.hpp>
#include <UI/UI.hpp>

namespace Pulsar {
namespace UI {
class ExpGPVSLeaderboardTotal : public Pages::GPVSLeaderboardTotal  {
public:
    void OnUpdate() override;
};

class ExpGPVSLeaderboardUpdate : public Pages::GPVSLeaderboardUpdate {
public:
    void OnUpdate() override;
    void BeforeEntranceAnimations() override;
};

class ExpWWLeaderboardUpdate : public Pages::WWLeaderboardUpdate {
public:
    void OnUpdate() override;
};
enum LeaderboardDisplayType {
    LEADERBOARD_DISPLAY_NAMES,
    LEADERBOARD_DISPLAY_TIMES,
    LEADERBOARD_DISPLAY_FC
};

void setLeaderboardDisplayType(LeaderboardDisplayType type);
LeaderboardDisplayType getLeaderboardDisplayType();

void nextLeaderboardDisplayType();
void fillLeaderboardResults(int count, CtrlRaceResult** results);

bool checkLeaderboardDisplaySwapInputs();

const u32 WIIMOTE_DPAD_BUTTONS = WPAD::WPAD_BUTTON_LEFT | WPAD::WPAD_BUTTON_RIGHT | WPAD::WPAD_BUTTON_DOWN | WPAD::WPAD_BUTTON_UP;
const u32 CLASSIC_DPAD_BUTTONS = WPAD::WPAD_CL_BUTTON_UP | WPAD::WPAD_CL_BUTTON_LEFT | WPAD::WPAD_CL_BUTTON_DOWN | WPAD::WPAD_CL_BUTTON_RIGHT;
const u32 GC_DPAD_BUTTONS = PAD::PAD_BUTTON_LEFT | PAD::PAD_BUTTON_RIGHT | PAD::PAD_BUTTON_DOWN | PAD::PAD_BUTTON_UP;
}//namespace UI
}//namespace Pulsar
#endif