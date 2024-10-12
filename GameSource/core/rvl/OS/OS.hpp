#ifndef _OS_
#define _OS_
#include <types.hpp>
#include <core/rvl/gx/GX.hpp>

//it's not namespaced in the actual game but this makes it more readable
#define OS_BUS_CLOCK (*(u32 *)0x800000f8)
#define OS_TIMER_CLOCK (OS_BUS_CLOCK / 4)

#define OSNanosecondsToTicks(nsec) ((nsec) / (1000000000 / OS_TIMER_CLOCK))
#define OSTicksToSeconds(ticks) ((ticks) / OS_TIMER_CLOCK)
#define OSTicksToNanoseconds(ticks) ((ticks) * (1000000000 / OS_TIMER_CLOCK))
#define OSTicksToMilliseconds(ticks) ((ticks) / (OS_TIMER_CLOCK / 1000))
namespace OS {

struct CalendarTime {
    int sec, min, hour, mday, mon, year, wday, yday, msec, usec;
};
void TicksToCalendarTime(u64 time, CalendarTime* result);

void Report(const char* format, ...);
u64 GetTime();
u32 GetTick();
void Fatal(GX::Color foreground, GX::Color background, const char* message);
void LaunchTitle(UnkType r3, UnkType r4);
int CalcCRC32(void* data, u32 length);
int DisableInterrupts(); //801a65ac
int EnableInterrupts(); //801a65c0
int RestoreInterrupts(int level); //801a65d4
int DisableScheduler();
void Restart(u32 resetCode);

void* OSGetArenaLo(); //801a10cc

}//namespace OS

#endif
