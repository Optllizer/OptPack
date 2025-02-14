#include <kamek.hpp>
#include <PulsarSystem.hpp>
#include <Config.hpp>
#include <Settings/SettingsParam.hpp>

namespace Pulsar {

namespace Settings {

u8 Params::radioCount[Params::pageCount] ={
    5, 5, 4, 3, 2, 3 //menu, race, host, OTT, KO
    //Add user radio count here

};
u8 Params::scrollerCount[Params::pageCount] ={ 0, 1, 1, 0, 2, 0 }; //menu, race, host, OTT, KO

u8 Params::buttonsPerPagePerRow[Params::pageCount][Params::maxRadioCount] = //first row is PulsarSettingsType, 2nd is rowIdx of radio
{
    { 2, 2, 2, 2, 2, 0 }, //Menu 
    { 2, 2, 2, 2, 4, 0 }, //Race
    { 2, 4, 2, 3, 0, 0 }, //Host
    { 2, 2, 2, 0, 0, 0 }, //OTT
    { 2, 2, 0, 0, 0, 0 }, //KO
    { 2, 2, 2, 0, 0, 0 }, //Menu2
    //{}, //User
};

u8 Params::optionsPerPagePerScroller[Params::pageCount][Params::maxScrollerCount] =
{
    { 5, 7, 0, 0, 0}, //Menu 
    { 4, 0, 0, 0, 0}, //Race
    { 15, 0, 0, 0, 0}, //Host
    { 0, 0, 0, 0, 0}, //OTT
    { 4, 4, 0, 0, 0}, //KO
    { 0, 0, 0, 0, 0}, //Menu2
    //{}, //User
};

}//namespace Settings
}//namespace Pulsar



