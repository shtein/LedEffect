#include "precomp.h"
#include "effect.h"
#include "effectenginectx.h"

/////////////////////////////////
// Effect
Effect::Effect(){
  _speedDelay = 25;
}

Effect::~Effect(){
}


void Effect::init(CRGB *leds, int numLeds){  

  setAll(leds, numLeds, CRGB::Black);
  
  reset();
}


void Effect::loop(CRGB *leds, int numLeds){  
  
  //proceed
  proceed(leds, numLeds);
 
}


CRGB Effect::getColor() const{
  return getHSV();
}
    
void Effect::setRandomColor(){
  setHSV(CHSV(random(256), 0xFF, 0xFF));
}

void Effect::setSpeedDelay(uint16_t speedDelay){
  //Scale down
  _speedDelay = (uint8_t)map(speedDelay, SPEED_DELAY_MIN, SPEED_DELAY_MAX, 0, 255);
}

uint16_t Effect::getSpeedDelay() const{
  //Scale Up
  return (uint16_t)map(_speedDelay, 0, 255, SPEED_DELAY_MIN, SPEED_DELAY_MAX);
}

void Effect::setPixel(CRGB &led,  byte red, byte green, byte blue) {
   led.r = red;
   led.g = green;
   led.b = blue;
}

void Effect::setPixel(CRGB &led, const CRGB &color) {
   led = color;
}

void Effect::setAll(CRGB *leds, int numLeds, const CRGB &color) {
  fill_solid(leds, numLeds, color);
}

void Effect::setAll(CRGB *leds, int numLeds, byte red, byte green, byte blue) {  
  fill_solid(leds, numLeds, CRGB (red, green, blue));
}


const CHSV &Effect::getHSV() const{
  return _hsv;
}

void Effect::setHSV(const CHSV &hsv){
  _hsv   = hsv;
}



//////////////////////////////////////
// EffectPaletteTransform

CRGBPalette16 EffectPaletteTransform::_palCurrent;
CRGBPalette16 EffectPaletteTransform::_palTarget;
    
int EffectPaletteTransform::_step;


EffectPaletteTransform::EffectPaletteTransform() {
  setSpeedDelay(25);  
}

EffectPaletteTransform::~EffectPaletteTransform(){
  
}

void EffectPaletteTransform::reset(){
  _palCurrent = getNewPal();
  _palTarget  = getNewPal();
  
  _step       = getMaxStep();
}

int EffectPaletteTransform::getMaxStep() const{
  return CHANGE_PAL_STEP;
}


int EffectPaletteTransform::getPalClrIndex(int /*ledIndex*/, int /*numLeds*/) const{
  //First index by default - need to experiment with this
  return 0;
}

bool EffectPaletteTransform::isReadyToBlendPal() const{
  return true;
}

bool EffectPaletteTransform::isReadyToChangePal() const{
  return _step == 0;
}

bool EffectPaletteTransform::isReadyToUpdateLeds() const{
  return true;
}

CRGBPalette16 EffectPaletteTransform::getNewPal() const{
  //Random palette
  return CRGBPalette16(CHSV(random8(), 255, random8(128,255)), 
                       CHSV(random8(), 255, random8(128,255)), 
                       CHSV(random8(), 192, random8(128,255)), 
                       CHSV(random8(), 255, random8(128,255))
                      );
}

uint8_t EffectPaletteTransform::getMaxPaxPalChanges() const{
  return MAX_PAL_CHANGES;
}

void EffectPaletteTransform::updateColors(){
  //Change target palette
  _palTarget = getNewPal();
}

void EffectPaletteTransform::updateLeds(CRGB *leds, int numLeds){  
  for(int i = 0; i < numLeds; i++){                                    
    leds[i] = ColorFromPalette(_palCurrent, getPalClrIndex(i, numLeds), 255, LINEARBLEND);
  }
}

void EffectPaletteTransform::proceed(CRGB *leds, int numLeds){
  
  //Check if it is to update target palette
  if(isReadyToChangePal()){
    //Update palette
    updateColors();

    //Reset step
    _step = getMaxStep();       
  }
  
  
  //Proceed with palette transtion
  if(isReadyToBlendPal()){
    nblendPaletteTowardPalette(_palCurrent, _palTarget, getMaxPaxPalChanges()); 
  }


  //Set colors
  if(isReadyToUpdateLeds()){
    updateLeds(leds, numLeds);
  }

  //Prepare for the next move                                        
  _step--;
}


