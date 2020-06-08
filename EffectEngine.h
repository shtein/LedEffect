#ifndef __EFFECTENGINE_H
#define __EFFECTENGINE_H

#include "EffectEngineCtx.h"

class Effect;

//Data limits
#define MAX_EFFECTS 15
#define MAX_MODES   5

//Engine flags
#define EFF_RANDOM_START_MODE     0x01
#define EFF_RANDOM_START_EFFECT   0x02

/////////////////////////////////
// EffectEngine
class EffectEngine{
  public:
    EffectEngine();
    ~EffectEngine();

    void init(CRGB *leds, int maxLeds, uint8_t flags = 0);
    void addMode(Effect **effects); //add mode
    void addEffect(Effect *effect); //add effect to current mode
    
    void loop(const struct CtrlQueueItem &itm);    
    
  protected:
    void showStrip();

    //Command handling
    void onModeChange(const struct CtrlQueueData &data);
    void onNumLedsChange(const struct CtrlQueueData &data);
    void onEffectChange(const struct CtrlQueueData &data); 
    

    //Internal routines
    void setMode(uint8_t mode);
    void setEffect(uint8_t effectNum);
    Effect *getEffect() const;

    //Reading/writing config from/to EEPROM
    void readConfig();
    void writeConfig(); 
    void preSaveConfig();
    
  protected:
    //Effects for modes
    struct EFFECT_MODE{
      Effect     **effects;  //Effects
      uint8_t      numEffects:4;          //Total number of effects
      uint8_t      effectNum:4;           //Index of the current or last effect from _effects
    } _modes[MAX_MODES]; 

    uint8_t    _numModes:4;               //Total number of modes
    uint8_t    _modeNum:4;                //Current mode

    CRGB       *_leds;                    //Leds
    uint16_t   _maxLeds:12;               //Max number of leds    
    uint16_t   _numLeds:12;               //Actual number of leds

    uint16_t   _flags:8;                  //Flags 
   
    unsigned long _millis;                //Processing
    unsigned long _millisToSaveCfg;       //When to safe config
};

#endif //__EFFECTENGINE_H
