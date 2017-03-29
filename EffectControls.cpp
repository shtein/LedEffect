#include "precomp.h"
#include "EffectControls.h"
#include "EffectEngineCtx.h"
#include "pins.h"

EffectControls::EffectControls():  _remote(REMOTE_PIN) {
  _maxEffects = 0;
  _effectNum  = 0;
  _speedDelay = 0;
  _numLeds    = 0;
}

EffectControls::~EffectControls(){
}

void EffectControls::init(struct EffectEngineCtx &ctx){    

  ctx.cf      = EEMC_MODE | EEMC_NUMLEDS;
  ctx.mode    = EEM_EFFECT;

   //This is where to read from EEPROM
  _maxEffects = ctx.maxEffects;
  _effectNum  = ctx.effectNum;
  _numLeds    = ctx.numLeds;
  _speedDelay = ctx.speedDelay;
  _mode       = ctx.mode;
}

void EffectControls::loop(struct EffectEngineCtx &ctx){
  //Read controls
  readControls();
  
  //First see if we need to cleanup anything
  if(ctx.cf != EEMC_NONE){
     //Proceed with cleanup, remember what's to be remembered

     //Effect was changed, remember current speed and color 
     if(ctx.cf & (EEMC_EFFECT | EEMC_MODE)){
        setSpeedDelay(ctx.speedDelay);
        setHSV(ctx.hsv);
     }

     //Reset flag
     ctx.cf = EEMC_NONE;
  }

  _mode = getMode();
  if(_mode != ctx.mode){
    ctx.cf  |= EEMC_MODE;
    ctx.mode = _mode;
  }
  
 
  //Current effect
  _effectNum = getCurrentEffect();
  if(_effectNum != ctx.effectNum){
    ctx.cf       |= EEMC_EFFECT; 
    ctx.effectNum = _effectNum;
  }

  //Speed Delay
  _speedDelay = getSpeedDelay();
  if(_speedDelay != ctx.speedDelay){
    ctx.cf         |= EEMC_SPEED;
    ctx.speedDelay  = _speedDelay; 
  }

  //Color
  _hsv = getHSV();
  if(_hsv != ctx.hsv){
    ctx.cf    |= EEMC_COLOR;
    ctx.hsv  = _hsv; 
  }

  //Number of leds
  _numLeds = getNumLeds();
  if(_numLeds != ctx.numLeds){
    ctx.cf      |= EEMC_NUMLEDS;
    ctx.numLeds = _numLeds;
  }
  
}


int powInt(int x, int y, int limit){
  int n = 1;

  if(y > limit)
    y = limit;
  
  for(int i = 0; i < y; i++){
    n = n * x;
  }

  return n; 
}


int EffectControls::getRemotePushedDec(int value, int limit, unsigned long key, int repeatLimit) const{
  
   if(value <= limit){
      return limit;
   }
   
   int pushed = _remote.pushed(key);
 
  if(pushed != 0){
    value -= powInt(2, pushed - 1, repeatLimit);      
    if(value < limit){
      value = limit;
    }
  }

  return value;
}

int EffectControls::getRemotePushedInc(int value, int limit, unsigned long key, int repeatLimit) const{
  
   if(value >= limit){
      return limit;
   }
   
  int pushed = _remote.pushed(key);
 
  if(pushed != 0){
    value += powInt(2, pushed - 1, repeatLimit);      

    if(value > limit){
      value = limit;
    }
  }

  return value;
}

int EffectControls::getRemotePushedValue(int value, 
                                         int limitDec, int limitInc, 
                                         unsigned long keyDec, unsigned long keyInc, 
                                         int repeatLimit ) const{

  int newVal = getRemotePushedDec(value, limitDec, keyDec, repeatLimit);
   //Changed ?
   if(newVal == value){
      newVal = getRemotePushedInc(value, limitInc, keyInc, repeatLimit);
   }
 
   return newVal;
}


#define SPEED_STEP_MAX 5

int EffectControls::getSpeedDelay() const{
  //Not effect mode - nothing to change
   if(_mode != EEM_EFFECT){
      return _speedDelay; 
   }
   
   return getRemotePushedValue(_speedDelay, SPEED_DELAY_MIN, SPEED_DELAY_MAX, RKEY_RIGHT, RKEY_LEFT, SPEED_STEP_MAX);
}

void EffectControls::setSpeedDelay(int speedDelay){
   _speedDelay = speedDelay;
}


int EffectControls::getCurrentEffect() const{
  //Not effect mode - nothing to change
   if(_mode != EEM_EFFECT){
      return _effectNum; 
   }

   int effectNum = _effectNum;
   
   if(_remote.pushed(RKEY_DOWN) == 1){
      effectNum = (effectNum + _maxEffects - 1) % _maxEffects;
   }
  
   if(_remote.pushed(RKEY_UP) == 1){
      effectNum = (effectNum + 1) % _maxEffects;
   }

   return effectNum;
}

void EffectControls::setCurrentEffect(int curEffect){
  _effectNum = curEffect;
}


#define COLOR_STEP_MAX 5

CHSV EffectControls::getHSV() const{
  //Check mode
  if(_mode == EEM_OFF || _mode == EEM_SETUP){
      return _hsv; 
   }
  
  CHSV hsv(_hsv);

  //Keys 1 and 4 - hue
  //Keys 2 and 5 - suturation
  //Keys 3 and 6 - valuee
  unsigned long controls[3][2] = { {RKEY_4, RKEY_1}, {RKEY_5, RKEY_2}, {RKEY_6, RKEY_3} };

/*
  Somehow code below does work with CHSV, it works with CRGB
  for(int i = 0; i < 3; i++){
     hsv.raw[i] = (uint8_t)getRemotePushedValue((int)hsv.raw[i], 0, 255, controls[i][0], controls[i][1], COLOR_STEP_MAX);
  }
*/

   hsv.h = (uint8_t)getRemotePushedValue(hsv.h, 0, 255, controls[0][0], controls[0][1], COLOR_STEP_MAX);
   hsv.s = (uint8_t)getRemotePushedValue(hsv.s, 0, 255, controls[1][0], controls[1][1], COLOR_STEP_MAX);
   hsv.v = (uint8_t)getRemotePushedValue(hsv.v, 0, 255, controls[2][0], controls[2][1], COLOR_STEP_MAX);

  return hsv;
}

void EffectControls::setHSV(const CHSV &hsv){
  _hsv = hsv;
}


int EffectControls::getNumLeds() const {
  return _numLeds;
}

void EffectControls::setNumLeds(int numLeds)  {
  _numLeds = numLeds;
}

int EffectControls::getMode() const{
  //Single push only
  return _remote.pushed(RKEY_OK) == 1 ? (_mode + 1) % EEM_MODEMAX : _mode;
}

void EffectControls::readControls(){
  _remote.read();
}

