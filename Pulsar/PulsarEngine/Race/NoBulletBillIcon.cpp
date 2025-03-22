#include <kamek.hpp>
#include <MarioKartWii/Item/ItemPlayer.hpp>
#include <SlotExpansion/CupsConfig.hpp>
#include <PulsarSystem.hpp>

namespace Pulsar {
namespace Race {

//No Bullet Biil Icon by Gabriela. 
void NoBulletBillIcon(Item::PlayerRoulette *roulette, ItemId item) {
  if(!System::sInstance->IsContext(PULSAR_CT)) roulette->unknown_0x24 = (u32) item;
}
kmCall(0x807a9b28, NoBulletBillIcon);
}// namespace Item
}// namespace OptPack