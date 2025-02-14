#ifndef _PUL_FROOMPAGE_
#define _PUL_FROOMPAGE_
#include <kamek.hpp>
#include <MarioKartWii/UI/Page/Other/FriendRoom.hpp>
#include <MarioKartWii/UI/Page/Other/Message.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>
#include <Settings/UI/SettingsPanel.hpp>

/*Extends FriendRoom to add a settings and a change teams button; contrary to the other two pages where the settings are present,
here they are added as a layer. That is because the FriendRoom still needs to run in the background as it is what checks when players join/leave,
when the host starts a gamemode etc...
Consequently, most controls present on the FriendRoom (whether its own or other Pages') are hidden when these buttons are pressed
to prevent ugly Z-fighting
*/
namespace Pulsar {
namespace UI {
class ExpFroom : public Pages::FriendRoom {
public:
    ExpFroom();
    void OnInit() override;
    void AfterControlUpdate() override;
    void OnResume() override;
    void OnMessageBoxClick(Pages::MessageBoxTransparent* msgPage);
    void OnActivate() override;

private:
    void ExtOnButtonSelect(PushButton& button, u32 hudSlotId);
    void OnSettingsButtonClick(PushButton& button, u32 hudSlotId);
    void OnTeamsButtonClick(PushButton& button, u32 hudSlotId);
    void OnKickButtonClick(PushButton& button, u32 hudSlotId);
    PtmfHolder_2A<ExpFroom, void, PushButton&, u32> onSettingsClickHandler;
    PtmfHolder_2A<ExpFroom, void, PushButton&, u32> onTeamsClickHandler;
    PtmfHolder_2A<ExpFroom, void, PushButton&, u32> onKickClickHandler;
    PushButton settingsButton;
    PushButton teamsButton;
    PushButton kickButton;
    bool areControlsHidden;
public:
    PulPageId topSettingsPage;
};
}//namespace UI
}//namespace Pulsar
#endif