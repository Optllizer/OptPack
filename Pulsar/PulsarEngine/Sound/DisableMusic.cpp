#include <kamek.hpp>
#include <MarioKartWii/Audio/Other/AudioValues.hpp>
#include <Settings/Settings.hpp>
#include <Settings/SettingsParam.hpp>
#include <PulsarSystem.hpp>

/*OptPack Note: Code by Brawlbox. Realistically, I could switch to Pulsars default, but on OptPack release music didnt disable wifi music and
this just works and 99.9% also want the menu music disabled.*/


namespace Pulsar{
namespace Sound {
    void SetMusicState(){
        if (Pulsar::Settings::Mgr::Get().GetSettingValue(Pulsar::Settings::SETTINGSTYPE_MENU,SETTINGMENU_RADIO_DISABLEMUSIC)){
           Audio::PlayersVolumeMgr::sInstance->volumes[0].curValue = 0;
        }
    }
    void SetMusicStateSettings(){
        Audio::PlayersVolumeMgr::sInstance->volumes[0].curValue = 1;
        SetMusicState();
    }
    kmBranch(0x80710ba8, SetMusicState);//menu music
    kmBranch(0x80715c38, SetMusicState);//menu music
    kmBranch(0x80715e60, SetMusicState);//ingame music
    kmBranch(0x8070f598, SetMusicState);//wifi-music
    kmBranch(0x80717d64, SetMusicState);//replay music
    Pulsar::Settings::Hook PatchMusicInstantly(SetMusicStateSettings);
} // namespace Sound
} // namespace OptPack
