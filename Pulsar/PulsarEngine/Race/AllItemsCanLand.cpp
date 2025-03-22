#include <kamek.hpp>
#include <MarioKartWii/Kart/KartCollision.hpp>
#include <MarioKartWii/Item/ItemManager.hpp>
#include <MarioKartWii/Item/ItemBehaviour.hpp>
#include <MarioKartWii/Item/ItemPlayer.hpp>
#include <MarioKartWii/Item/Obj/ObjProperties.hpp>
#include <MarioKartWii/Race/RaceData.hpp>
#include <PulsarSystem.hpp>

//All Items Can Land by Brawlbox, Gabriela, Optllizer

namespace Pulsar {
namespace Race {
using Item::ObjProperties;
int UseItem(Kart::Collision *kartCollision, ItemId id){
    u8 playerId = kartCollision->GetPlayerIdx();
    Item::Manager::sInstance->players[playerId].inventory.currentItemCount++;
    Item::Behavior::behaviourTable[id].useFunction(Item::Manager::sInstance->players[playerId]);
    return -1;
}

int AllShocksCanLand(Kart::Collision *kartCollision){
    if (System::sInstance->IsContext(PULSAR_CT)) return UseItem(kartCollision, LIGHTNING);
    return -1;
}

int AllMegasCanLand(Kart::Collision *kartCollision){
    if (System::sInstance->IsContext(PULSAR_CT)) return UseItem(kartCollision, MEGA_MUSHROOM);
    return -1;
}

int AllFeathersCanLand(Kart::Collision *kartCollision){
    if (System::sInstance->IsContext(PULSAR_CT)) return UseItem(kartCollision, BLOOPER);
    return -1;
}

int AllPOWsCanLand(Kart::Collision *kartCollision){
    if (System::sInstance->IsContext(PULSAR_CT)) return UseItem(kartCollision, POW_BLOCK);
    return -1;
}

int AllGoldensCanLand(Kart::Collision *kartCollision){
    if (System::sInstance->IsContext(PULSAR_CT)) return UseItem(kartCollision, MUSHROOM);
    return -1;
}

int AllBulletsCanLand(Kart::Collision *kartCollision){
    if (System::sInstance->IsContext(PULSAR_CT)) return UseItem(kartCollision, BULLET_BILL);
    return -1;
}

//Enable Item Falling by Gabriela. Ported to Pulsar by Optllizer
//https://github.com/Gabriela-Orzechowska/MKW-Cosmos/blob/main/code/Race/AllItemsCanLand.cpp#L96
void EnableItemFalling(ObjProperties& dest, ObjProperties& src){
    if(!System::sInstance->IsContext(PULSAR_CT)) return;
    dest.canFallOnTheGround = true;
    dest.canFallOnTheGround2 = true;
    if(!System::sInstance->IsContext(PULSAR_MODE_OTT)) return;
    dest.canFallOnTheGround = false;
    dest.canFallOnTheGround2 = false;
    }
kmBranch(0x80790fb4, EnableItemFalling);

kmWritePointer(0x808b54b8, AllShocksCanLand);
kmWritePointer(0x808b54d0, AllMegasCanLand);
kmWritePointer(0x808b54f4, AllPOWsCanLand);
kmWritePointer(0x808b5500, AllGoldensCanLand);
kmWritePointer(0x808b550c, AllBulletsCanLand);
//kmWritePointer(0x808b54e8, AllFeathersCanLand); Already done in Pulsar
} // namespace Race
} // namespace Pulsar
