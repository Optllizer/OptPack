#include <kamek.hpp>
#include <runtimeWrite.hpp>
#include <Network/WiiLink.hpp>
#include <Dolphin/DolphinIOS.hpp>
#include <MarioKartWii/RKNet/RKNetController.hpp>

namespace Pulsar {
namespace Race {

//Anti Online Item Delimiters By Ro
asmFunc GetItemDelimiterShock() {
    ASM(
        nofralloc;
loc_0x0:
  mflr r12;
  cmpwi     r7, 0x1;
  bne+ validLightning;
  addi r12, r12, 0x12C;
  mtlr r12;
  blr;
validLightning:
  mulli r29, r3, 0xF0;
  blr;
    )
}
kmCall(0x807B7C34, GetItemDelimiterShock);

asmFunc GetItemDelimiterBlooper() {
    ASM(
        nofralloc;
loc_0x0:
  mflr r12;
  cmpwi     r7, 0x1;
  bne+ validBlooper;
  addi r12, r12, 0x1A8;
  mtlr r12;
  blr;
validBlooper:
  addi r11, r1, 0x50;
  blr;
    )
}
kmCall(0x807A81C0, GetItemDelimiterBlooper);

asmFunc GetItemDelimiterPOW() {
    ASM(
        nofralloc;
loc_0x0:
mflr r12;
  cmpwi     r7, 0x1;
  bne+ validPOW;
  addi r12, r12, 0x48;
  mtlr r12;
  blr;
validPOW:
  mr r30, r3;
  blr;
    )
}
kmCall(0x807B1B44, GetItemDelimiterPOW);

//Anti Mii Crash by Ro
asmFunc AntiWiper() {
    ASM(
        nofralloc;
loc_0x0:
  cmpwi r4, 0x6;
  ble validMii;
  lhz r12, 0xE(r30);
  cmpwi r12, 0x0;
  bne validMii;
  li r31, 0x0;
  li r4, 0x6;
validMii:
  mr r29, r4;
  blr;
    )
}
kmCall(0x800CB6C0, AntiWiper);
kmWrite32(0x80526660, 0x38000001); //Credits to Ro for the last line.

//Anti Item Collission Crash by Marioiscool246
extern "C" void __ptmf_test(void*);
asmFunc AntiItemColCrash() {
    ASM(
        nofralloc;
loc_0x0:
  stwu r1, -0xC(r1);
  stw r31, 8(r1);
  mflr r31;
  addi r3, r29, 0x174;
  bl __ptmf_test;
  cmpwi r3, 0;
  bne end;
  addi r31, r31, 0x14;

end:
  mtlr r31;
  lwz r31, 8(r1);
  addi r1, r1, 0xC;
  mr r3, r29;
  blr;
    )
}
kmCall(0x807A1A54, AntiItemColCrash);

//VR System Changes by MrBean35000vr
//Multiply VR difference by 2 by Winner
asmFunc GetVRScaleWin() {
    ASM(
  li r5, 2;
  divw r3, r3, r5;
  extsh r3, r3;
    )
}
kmCall(0x8052D150, GetVRScaleWin);

//Cap VR loss from one victorious opponent between 0 and -8.
asmFunc GetCapVRLoss() {
    ASM(
  lwz       r3, 0x14(r1);
  cmpwi     r3, -8;
  bge       0f;
  li        r3, -8;
  b         1f;
  0:;
  cmpwi     r3, 0;
  ble       1f;
  li        r3, 0;
  1:;
    )
}
kmCall(0x8052D260, GetCapVRLoss);

//Cap VR gain from one defeated opponent between 2 and 12.
asmFunc GetCapVRGain() {
    ASM(
   lwz       r3, 0x14(r1);
   cmpwi     r3, 2;
   bge       0f;
   li        r3, 2;
   b         1f;
   0:;
   cmpwi     r3, 12;
   ble       1f;
   li        r3, 12;
   1:;
    )
}
kmCall(0x8052D1B0, GetCapVRGain);

//Anti Lag Start by Ro
extern "C" void sInstance__8Racedata(void*);
asmFunc AntiLagStart(){
    ASM(
loc_0x0:
  lwz r12, sInstance__8Racedata@l(r30);
  lwz r12, 0xB70(r12);
  cmpwi r12, 0x7;
  blt- loc_0x14;
  li r3, 0x1;

loc_0x14:
  cmpwi r3, 0x0;
  blr;
  )
}
kmCall(0x80533430, AntiLagStart);

// Mii Outfit C Anti-Crash
kmWrite8(0x8089089D, 0x00000062);
kmWrite8(0x808908A9, 0x00000062);
kmWrite8(0x808908E5, 0x00000062);
kmWrite8(0x808908F1, 0x00000062);
kmWrite8(0x8089092D, 0x00000062);
kmWrite8(0x80890939, 0x00000062);

// Item Spam Anti-Freeze
asmFunc ItemSpamAntiFreeze() {
    ASM(
        loc_0x0 : lbz r12, 0x1C(r27);
        add r12, r30, r12;
        cmpwi r12, 0xE0;
        blt + loc_0x18;
        li r0, 0;
        stb r0, 0x19(r27);

        loc_0x18 : lbz r0, 0x19(r27);)
}
kmCall(0x8065BBD4, ItemSpamAntiFreeze);

// Deflicker when 480p [MKW-SP]
asmFunc Deflicker() {
    ASM(
        nofralloc;
        cntlzw r0, r3;
        lwz r5, 0x14(r5);
        cmpwi r5, 0x0;
        bnelr;
        li r0, 0x0;
        blr;)
}
kmCall(0x8021A028, Deflicker);

// Play Character Icon Damage Animation When Burned Out [Ro]
asmFunc burnoutIconFix() {
    ASM(
        nofralloc;
        lwz r0, 8(r3);
        andis.r12, r0, 0x4;
        beq - loc_0x10;
        ori r0, r0, 0x1;

        loc_0x10 :;
        blr;)
}
kmCall(0x807EB38C, burnoutIconFix);

// Cancel Friend Room Joining by Pressing B [Ro]
extern "C" void ptr_inputBase(void*);
asmFunc friendRoomJoinCancel() {
    ASM(
        nofralloc;
        lis r31, ptr_inputBase @ha;
        lwz r31, ptr_inputBase @l(r31);
        lhz r31, 0x60(r31);
        andi.r31, r31, 0x2;
        beq end;

        li r3, 3;

        end :;
        cmpwi r3, 3;
        blr;)
}
kmCall(0x805DD85C, friendRoomJoinCancel);

// Prevent Crash from Invalid Camera Pointer in Broken KMP [Gab]
asmFunc InvalidCameraPointerFix() {
    ASM(
        nofralloc;
        loc_0x0 :
        cmpwi r31, 0;
        bne + loc_0x18;
        mflr r12;
        addi r12, r12, 0xA0;
        mtlr r12;
        blr;

        loc_0x18 :
        lwz r3, 0x0(r31);
        blr;)
}
kmCall(0x805ABE14, InvalidCameraPointerFix);

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

// Force player to not be penalized [B_squo]
kmWrite32(0x80549898, 0x38600000);
kmWrite32(0x8054989c, 0x4E800020);

// Online Miis have facial expressions [B_squo]
kmWrite32(0x807C7944, 0x38800000);

// Live View Icon Shadow Bug Fix [B_squo]
kmWrite32(0x807eb988, 0x807c01c0);

// Fix Mii opponents having silent / Rosalina voice Bug [B_squo]
kmWrite32(0x8086975C, 0x4082001C);

// Online Miis look at the camera when finishing in Live View [B_squo]
kmWrite32(0x80596770, 0x60000000);

//Always Show Timer on Vote Screen by Chadderz
kmWrite32(0x80650254,0x60000000);

//Allow Looking Backwards During Respawn by Ro
kmWrite32(0x805A228C,0x60000000);

//Allow pausing before the race starts by Sponge
kmWrite32(0x80856a28,0x48000050);

//Show Nametags During Countdown By Ro
kmWrite32(0x807F13F8,0x4800006C);

//Always Show Timer After Online Race Results by Sponge
kmWrite32(0x8064DB2C,0x60000000); 

//Don't Lose VR When Disconnecting by Bully
kmWrite32(0x80856560, 0x60000000);

//Disable Data Reset by Vega
kmWrite32(0x80544928, 0x7C601B78);

//Disable RR Bottom Road Respawn by Ro
kmWrite32(0x8059BE20, 0x38600000);

// Instant Voting Roulette Decide by Ro
kmWrite32(0x80643BC4, 0x60000000);
kmWrite32(0x80643C2C, 0x60000000);

// Reduce PING retry time from 700 to 80 [Wiimmfi]
kmWrite16(0x8011B47A, 80);

// Do not wait the retry time in case of successful NATNEG [Wiimmfi]
kmWrite32(0x8011B4B0, 0x60000000);

// Do not wait the idle time after a successful NATNEG [WiiLink24]
kmWrite16(0x8011BC3A, 0);

// Change the SYN-ACK timeout to 7 seconds instead of 5 seconds per node [Wiimmfi]
kmWrite32(0x800E1A58, 0x38C00000 | 7000);

// Fix the "suspend bug" where DWC stalls suspending due to ongoing NATNEG [WiiLink24, MrBean35000vr]
kmWrite32(0x800E77F8, 0x60000000);
kmWrite32(0x800E77FC, 0x60000000);

//HUD Color Modifier by Spaghetti Noppers
//kmWrite32(0x80895CC0, 0x00200086);
//kmWrite32(0x80895CC4, 0x008600FF);
//kmWrite32(0x80895CC8, 0x005200A3);
//kmWrite32(0x80895CCC, 0x00A300FF);
//kmWrite32(0x80895CD0, 0x005200A3);
//kmWrite32(0x80895CD4, 0x00A30046);
//kmWrite32(0x80895CD8, 0x00200086);
//kmWrite32(0x80895CDC, 0x008600FF);
//kmWrite32(0x80895CE0, 0x00200086);
//kmWrite32(0x80895CE4, 0x008600FF);
//kmWrite32(0x80895CE8, 0x00200086);
//kmWrite32(0x80895CEC, 0x00860046);

//No Disconnect on Countdown by _tZ
kmWrite32(0x80655578, 0x60000000);

//Change VR Limit by XeR
kmWrite16(0x8052D286, 0x00007530);
kmWrite16(0x8052D28E, 0x00007530);
kmWrite16(0x8064F6DA, 0x00007530);
kmWrite16(0x8064F6E6, 0x00007530);
kmWrite16(0x8085654E, 0x00007530);
kmWrite16(0x80856556, 0x00007530);
kmWrite16(0x8085C23E, 0x00007530);
kmWrite16(0x8085C246, 0x00007530);
kmWrite16(0x8064F76A, 0x00007530);
kmWrite16(0x8064F776, 0x00007530);
kmWrite16(0x808565BA, 0x00007530);
kmWrite16(0x808565C2, 0x00007530);
kmWrite16(0x8085C322, 0x00007530);
kmWrite16(0x8085C32A, 0x00007530);

//Allow WFC on Wiimmfi Patched ISOs
kmWrite32(0x800EE3A0, 0x2C030000);
kmWrite32(0x800ECAAC, 0x7C7E1B78);

//Disable WW Button
kmWrite16(0x8064B982, 0x00000005);
kmWrite32(0x8064BA10, 0x60000000);
kmWrite32(0x8064BA38, 0x60000000);
kmWrite32(0x8064BA50, 0x60000000);
kmWrite32(0x8064BA5C, 0x60000000);
kmWrite16(0x8064BC12, 0x00000001);
kmWrite16(0x8064BC3E, 0x00000484);
kmWrite16(0x8064BC4E, 0x000010D7);
kmWrite16(0x8064BCB6, 0x00000484);
kmWrite16(0x8064BCC2, 0x000010D7);

//Time Limit Expansion 6min
kmWrite32(0x8053F3B8,0x3C600005);
kmWrite32(0x8053F3BC,0x38837E40);
kmWrite32(0x8053F478,0x4800000C);

//3 sec TC
kmWrite32(0x80580618, 0x388000B4);
kmWrite32(0x80580674, 0x388000B4);

}//namespace Race
}//namespace Pulsar