#include "FastLED.h"
#include "Mode.h"
#define NUM_LEDS 45
#define DATA_PIN 7

#include "Time.h"
CRGB leds[NUM_LEDS]; //has to be here to be available in all modes
uint8_t buffer[NUM_LEDS]; //a shared buffer that can be used by every mode to do whatever it wants with it

void waitForBytes(const uint8_t num)
{
  //wait for data
  while(Serial.available() < num)
  {
    delay(1);
  }
}

#include "Strobe.h"
#include "ColorFade.h"

//type of the parse functions
typedef void (*ParsePtr)(void);
typedef void (*ExecutePtr)(void);
typedef void (*InitPtr)(void);

ParsePtr parse[NUM_MODES];//the parse functions of all the modes
ExecutePtr execute[NUM_MODES];
InitPtr initialize[NUM_MODES];
Mode currentMode = FADE;


void setup() { 
  Serial.begin(9600);
  
  
  pinMode(11, INPUT_PULLUP);
  
  initialize[FADE] = colorFadeInit;
  parse[FADE] = colorFadeParseSerial;
  execute[FADE] = colorFadeUpdate;
  
  initialize[STROBE] = strobeInit;
  parse[STROBE] = strobeParseSerial;
  execute[STROBE] = strobeUpdate;  
  
  FastLED.addLeds<WS2811, DATA_PIN>(leds, NUM_LEDS);
  
  //blink
  for(int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i]= CRGB::Red;
  }
  FastLED.show();
  delay(500);
  for(int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i]= CRGB::Black;
  }
  FastLED.show();  
  delay(500);
  
  
  initialize[currentMode]();
    
}

void loop() { 

  if(Serial.available() > 0)
  {
    const uint8_t mode = Serial.read();
    if(mode != currentMode)
    {
      currentMode = (Mode)mode;
      initialize[currentMode](); 
    }
    parse[currentMode]();
  }
  execute[currentMode]();
  FastLED.show();
}

