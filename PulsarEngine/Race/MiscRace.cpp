#include <kamek.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <MarioKartWii/UI/Ctrl/CtrlRace/CtrlRaceBalloon.hpp>
#include <MarioKartWii/UI/Ctrl/CtrlRace/CtrlRaceResult.hpp>
#include <MarioKartWii/GlobalFunctions.hpp>
#include <MarioKartWii/Driver/DriverManager.hpp>
#include <Settings/Settings.hpp>
#include <Info.hpp>
#include <MarioKartWii/Item/ItemSlot.hpp>
#include <MarioKartWii/Race/Raceinfo/Raceinfo.hpp>
#include <PulsarSystem.hpp>
#include <Race/MiscRace.hpp>

namespace Pulsar {
namespace Race {

static void NonGhostPlayerCount(RacedataScenario& scenario, u8* playerCount, u8* screenCount, u8* localPlayerCount) {
    scenario.ComputePlayerCounts(playerCount, screenCount, localPlayerCount);
    System* system = System::sInstance;
    u8 realPlayers = *playerCount;
    if (scenario.settings.gamemode != MODE_TIME_TRIAL) for (int i = 0; i < 12; ++i) if (scenario.players[i].playerType == PLAYER_GHOST) --realPlayers;
    system->nonTTGhostPlayersCount = realPlayers;
}
kmCall(0x8052fc78, NonGhostPlayerCount);

kmWrite32(0x807997e0, 0x60000000);
//Starting item for OTT and TT, id is TRIPLE_MUSHROOM by default
static void SetStartingItem(Item::PlayerInventory& inventory, ItemId id, bool isItemForcedDueToCapacity) {
    register u32 playerId;
    asm(mr playerId, r29;);
    if (Racedata::sInstance->racesScenario.players[playerId].playerType == PLAYER_CPU) return;
    const System* system = System::sInstance;
    const bool isTT = DriverMgr::isTT;
    if (isTT || system->IsContext(PULSAR_MODE_OTT)) {
        bool isFeather;
        if (isTT) {
            const TTMode mode = system->ttMode;
            isFeather = (mode == TTMODE_150_FEATHER || mode == TTMODE_200_FEATHER);
        }
        else 
            isFeather = system->IsContext(PULSAR_FEATHER);
        
    const bool isOTTW = RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_VS_REGIONAL || 
                        RKNet::Controller::sInstance->roomType == RKNet::ROOMTYPE_JOINING_REGIONAL;    
        if (isFeather && !isOTTW) 
        id = BLOOPER;
        inventory.SetItem(id, isItemForcedDueToCapacity);
        if (isFeather) inventory.currentItemCount = 3;
    }
}
kmCall(0x80799808, SetStartingItem);

//From JoshuaMK, ported to C++ by Brawlbox and adapted as a setting
static int MiiHeads(Racedata* racedata, u32 unused, u32 unused2, u8 id) {
    CharacterId charId = racedata->racesScenario.players[id].characterId;
    const u32 gamemode = Racedata::sInstance->racesScenario.settings.gamemode;

    if (System::sInstance->IsContext(PULSAR_MIIHEADS)) {
        if (charId < MII_M) {
            if (id == 0) {
                charId = MII_M; // Player 1 (host)
            } else if (RKNet::Controller::sInstance->connectionState != 0) {
                charId = MII_M; // Non-host player or other players
            }
        }
    }
    if (gamemode == MODE_TIME_TRIAL) {
        charId = racedata->racesScenario.players[id].characterId;
    }

    return charId;
}

kmCall(0x807eb154, MiiHeads);
kmWrite32(0x807eb15c, 0x60000000);
kmWrite32(0x807eb160, 0x88de01b4);

//credit to XeR for finding the float address
static void BattleGlitchEnable() {
    const u8 val = Settings::Mgr::Get().GetSettingValue(Settings::SETTINGSTYPE_RACE, SETTINGRACE_RADIO_BATTLE);
    float maxDistance = 7500.0f;
    if (val == RACESETTING_BATTLE_GLITCH_ENABLED) maxDistance = 75000.0f;
    System* system = System::sInstance;
    if (system->IsContext(PULSAR_MODE_OTT)) {
        const Input::RealControllerHolder* controllerHolder = SectionMgr::sInstance->pad.padInfos[0].controllerHolder;
        const ControllerType controllerType = controllerHolder->curController->GetType();
        const u16 inputs = controllerHolder->inputStates[0].buttonRaw;
        const u16 newInputs = (inputs & ~controllerHolder->inputStates[1].buttonRaw);
        u32 toggleInput = PAD::PAD_BUTTON_Y;
        switch (controllerType) {
        case NUNCHUCK:
            toggleInput = WPAD::WPAD_BUTTON_DOWN;
            break;
        case WHEEL:
            toggleInput = WPAD::WPAD_BUTTON_MINUS;
            break;
        case CLASSIC:
            toggleInput = WPAD::WPAD_CL_TRIGGER_ZL;
            break;
        }
        if ((newInputs & toggleInput) == toggleInput) system->ottHideNames = !system->ottHideNames;
        if (system->ottHideNames) maxDistance -= maxDistance;
    }
    RaceBalloons::maxDistanceNames = maxDistance;
}
RaceFrameHook BattleGlitch(BattleGlitchEnable);

kmWrite32(0x8085C914, 0x38000000); //times at the end of races in VS
/*
static void DisplayTimesInsteadOfNames(CtrlRaceResult& result, u8 id) {

    if(Settings::Mgr::Get().GetSettingValue(Settings::SETTINGSTYPE_MENU, SETTINGMENU_RADIO_TIMES) == MENUSETTING_TIMES_DISABLED)
    result.FillName(id);
    if(Settings::Mgr::Get().GetSettingValue(Settings::SETTINGSTYPE_MENU, SETTINGMENU_RADIO_TIMES) == MENUSETTING_TIMES_ENABLED || System::sInstance->IsContext(PULSAR_MODE_OTT))
    result.FillFinishTime(id);

    bool isOTTWW = false;
    isOTTWW = (RKNet::Controller::sInstance->roomType = RKNet::ROOMTYPE_VS_REGIONAL) && 
              (RKNet::Controller::sInstance->roomType = RKNet::ROOMTYPE_JOINING_REGIONAL);

    if (System::sInstance->IsContext(PULSAR_MODE_OTT) && isOTTWW)
    result.FillFinishTime(id);
}
kmCall(0x8085d460, DisplayTimesInsteadOfNames); //for WWs*/

//don't hide position tracker (MrBean35000vr)
kmWrite32(0x807F4DB8, 0x38000001);

//Draggable blue shells
static void DraggableBlueShells(Item::PlayerObj& sub) {
    if(!System::sInstance->IsContext(PULSAR_CT)) {
        sub.isNotDragged = true;
    }
}
kmBranch(0x807ae8ac, DraggableBlueShells);

//Coloured Minimap
kmWrite32(0x807DFC24, 0x60000000);

/*
//No Team Invincibility
kmWrite32(0x8056fd24, 0x38000000); //KartCollision::CheckKartCollision()
kmWrite32(0x80572618, 0x38000000); //KartCollision::CheckItemCollision()
kmWrite32(0x80573290, 0x38000000); //KartCollision::HandleFIBCollision()
kmWrite32(0x8068e2d0, 0x38000000); //PlayerEffects ctor
kmWrite32(0x8068e314, 0x38000000); //PlayerEffects ctor
kmWrite32(0x807a7f6c, 0x38c00000); //FIB are always red
kmWrite32(0x807b0bd4, 0x38000000); //pass TC to teammate
kmWrite32(0x807bd2bc, 0x38000000); //RaceGlobals
kmWrite32(0x807f18c8, 0x38000000); //TC alert*/

//Accurate Explosion Damage (MrBean, CLF)
kmWrite16(0x80572690, 0x4800);
kmWrite16(0x80569F68, 0x4800);

//CtrlItemWindow
kmWrite24(0x808A9C16, 'PUL'); //item_window_new -> item_window_PUL

const char* ChangeItemWindowPane(ItemId id, u32 itemCount) {
    const bool feather = System::sInstance->IsContext(PULSAR_FEATHER);
    const bool megaTC = System::sInstance->IsContext(PULSAR_MEGATC);
    const char* paneName;
    if (id == BLOOPER && feather) {
        if (itemCount == 2) paneName = "feather_2";
        else if (itemCount == 3) paneName = "feather_3";
        else paneName = "feather";
    }
    //else if (id == THUNDER_CLOUD && megaTC) paneName = "megaTC";
    else paneName = GetItemIconPaneName(id, itemCount);
    return paneName;
}
kmCall(0x807f3648, ChangeItemWindowPane);
kmCall(0x807ef168, ChangeItemWindowPane);
kmCall(0x807ef3e0, ChangeItemWindowPane);
kmCall(0x807ef444, ChangeItemWindowPane);

kmWrite24(0x808A9FF3, 'PUL');

//Accurate Item Roulette Roulette by BrawlBox
static int AccurateItemRoulette(Item::ItemSlotData *itemSlotData, u16 itemBoxType, u8 position, ItemId prevRandomItem, bool r7){
        if (!IsBattle()){
            const u8 playerId = Raceinfo::sInstance->playerIdInEachPosition[position-1];
            Item::Player *itemPlayer = &Item::Manager::sInstance->players[playerId];
            return itemSlotData->DecideItem(itemBoxType, position, itemPlayer->isHuman, 0x1, itemPlayer);
            }
            return itemSlotData->DecideRouletteItem(itemBoxType, position, prevRandomItem, r7);
}
kmCall(0x807ba1e4, AccurateItemRoulette);
kmCall(0x807ba428, AccurateItemRoulette);
kmCall(0x807ba598, AccurateItemRoulette);

//Deflicker by Toadette Hack Fan and Optllizer
void DeFlicker() {
Blurry = 0x41820040;
if(Settings::Mgr::Get().GetSettingValue(Settings::SETTINGSTYPE_MENU2, SETTINGMENU2_RADIO_FLICKER) == MENUSETTING2_FLICKER_ENABLED) {
Blurry = 0x48000040;
}
}
static RaceLoadHook Deflick(DeFlicker);

//Force 30FPS by vabold
kmWrite32(0x80554224, 0x3C808000);
kmWrite32(0x80554228, 0x88841204);
kmWrite32(0x8055422C, 0x48000044);

 void FPSPatch() {
    FPSPatchHook = 0x00;
    if (Settings::Mgr::Get().GetSettingValue(Settings::SETTINGSTYPE_MENU2, SETTINGMENU2_RADIO_FPS) == 1) {
        FPSPatchHook = 0x00FF0100;
    }
    System::CacheInvalidateAddress(FPSPatchHook);
 }
static PageLoadHook PatchFPS(FPSPatch);

//Remove Background Blur by TheGamingBram
void Remove_Background_Blur(){
    U32_RBBG_HOOK_PT1 = 0x03000000;
    U32_RBBG_HOOK_PT2 = 0x3f000000;

    if(Settings::Mgr::Get().GetSettingValue(Settings::SETTINGSTYPE_MENU2, SETTINGMENU2_RADIO_BLUR) == 1){
        U32_RBBG_HOOK_PT1 = 0x03000000;
        U32_RBBG_HOOK_PT2 = 0x30000000;
    }
    System::CacheInvalidateAddress(U32_RBBG_HOOK_PT1);
    System::CacheInvalidateAddress(U32_RBBG_HOOK_PT2);
}
static PageLoadHook RBBG_Hook(Remove_Background_Blur);

//Always Show Timer on Vote Screen by Chadderz
kmWrite32(0x80650254,0x60000000);

//Allow Looking Backwards During Respawn by Ro
kmWrite32(0x805A228C,0x60000000);

//Allow pausing before the race starts by Sponge
kmWrite32(0x80856a28,0x48000050);

//Show Nametags During Countdown By Ro
kmWrite32(0x807F13F0,0x38600001);

//Always Show Timer After Online Race Results by Sponge
kmWrite32(0x8064DB2C,0x60000000); 

//Don't Lose VR When Disconnecting by Bully
kmWrite32(0x80856560, 0x60000000);

//HUD Color Modifier by Spaghetti Noppers
kmWrite32(0x80895CC0, 0x00200086); 
kmWrite32(0x80895CC4, 0x008600FF); 
kmWrite32(0x80895CC8, 0x005200A3);
kmWrite32(0x80895CCC, 0x00A300FF); 
kmWrite32(0x80895CD0, 0x005200A3); 
kmWrite32(0x80895CD4, 0x00A30046); 
kmWrite32(0x80895CE8, 0x00200086);
kmWrite32(0x80895CEC, 0x00860046); 
kmWrite32(0x80895CD8, 0x00200086);
kmWrite32(0x80895CDC, 0x008600FF); 
kmWrite32(0x80895CE0, 0x00200086); 
kmWrite32(0x80895CE4, 0x008600FF); 

//AntiFlicker by Riidefi
asmFunc AntiFlicker() {
    ASM(
        nofralloc;
loc_0x0:
  lwz r4, 0(r3);
  lbz r5, 16(r4);
  andi. r5, r5, 0x40;
  bne- loc_0x6C;
  lwz r5, 40(r4);
  li r6, 0x20;
  add r5, r4, r5;
  b loc_0x2C;

loc_0x20:
  addi r6, r6, 0x1;
  cmpwi r6, 0x80;
  bge- loc_0x6C;

loc_0x2C:
  mr r7, r5;
  lbzux r8, r7, r6;
  cmplwi r8, 0;
  beq+ loc_0x20;
  cmplwi r8, 97;
  bne- loc_0x6C;
  lwz r7, 1(r7);
  not r8, r7;
  rlwinm r7, r7, 8, 24, 31;
  rlwinm. r8, r8, 0, 8, 31;
  cmpwi cr1, r7, 0x27;
  crorc 20, 2, 6;
  bge- cr5, loc_0x74;
  addi r6, r6, 0x5;
  cmpwi r6, 0x80;
  blt+ loc_0x2C;

loc_0x6C:
  lwz r3, 0(r3);
  b end;

loc_0x74:
  lbz r5, 23(r4);
  cmplwi r5, 0;
  bne- loc_0x88;
  li r5, 0x1;
  stb r5, 23(r4);

loc_0x88:
  lwz r5, 60(r4);
  add r4, r4, r5;
  lbzu r5, 170(r4);
  lbz r6, 5(r4);
  cmpwi r5, 0x0;
  lbz r5, 10(r4);
  beq- loc_0xB4;
  cmpwi r6, 0x0;
  beq- loc_0xB4;
  cmpwi r5, 0x0;
  bne+ loc_0x6C;

loc_0xB4:
  lis r5, 0x800;
  stw r5, 11(r4);
  lis r5, 0x9800;
  ori r5, r5, 0x6108;
  stw r5, 8(r4);
  lis r5, 0x3361;
  ori r5, r5, 0x7D9;
  stw r5, 4(r4);
  lis r5, 0x6106;
  ori r5, r5, 0x8003;
  stw r5, 0(r4);
  lwz r3, 0(r3);
  end:
  blr;
    )
}
kmBranch(0x80052190, AntiFlicker);
kmPatchExitPoint(AntiFlicker, 0x80052194);
}//namespace Race
}//namespace Pulsar