//Written by Henrik Helmius in 2015
//Using FastLED library v3.1

#include <FastLED.h>
//Led data
#define NUM_LEDS 150
#define DATA_PIN 6
//Strip takes data in this order
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
//msgeq data
#define MSGEQ7_STROBE_PIN 2
#define MSGEQ7_RESET_PIN  3
#define AUDIO_PIN  0
//controll pins
#define AUDIO_LOW_PIN 1
#define AUDIO_HIGH_PIN 2
#define POT1_PIN 3
#define POT2_PIN 4
#define POT3_PIN 5
#define FBUTTON_PIN 7
#define BBUTTON_PIN 8
#define RGBBUTTON_PIN 9
//Audio levels variable
int levels[7];
//Knows the current animation
int animation;
int nbrOfAnimations = 6;



void setup() {
  //Setup the leds
  FastLED.addLeds<WS2812B, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  //Setup the msgeq
  initMSGEQ7();
  animation = 0;
}

void loop() {
  //Runs through the different animations. They should return if the button is pressed.
  //The delay is needeed to make sure that the user has enough time to let go of the button
  switch (animation) {
    case 0:
      audiobars();
      break;
    case 1:
      rainbow();
      break;
    case 2:
      fade();
      break;
    case 3:
      stars();
      break;
    case 4:
      rgbFill();
      break;
    case 5:
      morse();
      break;
  }
  //Check wich button was pressed
 
  //Forward button
  if (digitalRead(FBUTTON_PIN) == HIGH) {
    animation++;
    //Check if the animationnumber is max
    if (animation == nbrOfAnimations) {
      animation = 0;
    }
  }
  //Backwards button
  else if (digitalRead(BBUTTON_PIN) == HIGH) {
    animation--;
    if (animation < 0) {
      //Set to last animation
      //Index 0 => -1
      animation = nbrOfAnimations - 1;
    }
  }
  else if (digitalRead(RGBBUTTON_PIN) == HIGH) {
    strobe();
  }
}




//Animations

//Produces lightshow with two blocks on the ends representing the audio level in the two highest bands of the msgeq7. Also features a block in the middle representing the two lowest bands of the msgeq7
void audiobars() {
  int cutoffLevel;
  int sensitivity;
  while (buttonNotPressed()) {
    ReadAudio();
    FastLED.clear();
    cutoffLevel = analogRead(AUDIO_LOW_PIN);
    sensitivity = analogRead(AUDIO_HIGH_PIN);
    //calculate the bass levels
    int lowLevel = map(constrain(levels[0] + levels[1], cutoffLevel, sensitivity * 2), cutoffLevel, sensitivity * 2, 0, NUM_LEDS);
    //draw the low level bar
    fill_solid(leds + (NUM_LEDS - lowLevel) / 2, lowLevel, CHSV(map(analogRead(POT1_PIN), 0, 1023, 0, 255) + lowLevel * 1.5, 255, 255));
    //High levels
    int highLevel = map(constrain(levels[5] + levels[6], cutoffLevel, sensitivity * 2), cutoffLevel, sensitivity * 2, 0, NUM_LEDS);
    //Calculate high level colors
    CHSV highColor = CHSV(map(analogRead(POT2_PIN), 0, 1023, 0, 255) - highLevel, 255, 255);
    //fill beginning
    fill_solid(leds, highLevel / 2, highColor);
    //fill end of strip
    fill_solid(leds + NUM_LEDS - highLevel / 2, highLevel / 2, highColor);
    FastLED.show();
  }
}
//Scrolls a rainbow across the strip
void rainbow() {
  //Timing variables
  int interval;
  unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  //moves the rainbow
  uint8_t startValue = 0;
  while (buttonNotPressed()) {
    FastLED.clear();
    //Draw the rainbow
    //Maps value to 65 since the rainbow effect goes missing after that
    fill_rainbow(leds, NUM_LEDS, startValue, map(analogRead(POT2_PIN), 0, 1023, 0, 65));
    //Serial.println(map(analogRead(POT2_PIN), 0, 1023, 0, 100));
    FastLED.show();
    //Read the chosen speed
    //Map value to max 100, no visible change otherwise
    interval = map(analogRead(POT1_PIN), 0, 1023, 100, 0);
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      //It's time to move the rainbow
      // save the last time you changed the position
      previousMillis = currentMillis;
      //Move the rainbow
      startValue = startValue + 2;
    }

  }
}
//Fades through the color palette
void fade() {
  //Timing variables
  int interval;
  unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  //Holds the current color
  uint8_t color = 0;
  while (buttonNotPressed()) {
    FastLED.clear();
    //Fill the led with a single color
    //Map pot 2 to the brightness. To make max brightness when the pot is turned all the way to the left, take 255-valueofpot
    //color is the color, it cycles through the hsv colorspace
    fill_solid(leds, NUM_LEDS, CHSV(color, 255, map(analogRead(POT2_PIN), 0, 1023, 0, 255)));
    FastLED.show();
    //Read the color change speed
    interval = map(analogRead(POT1_PIN), 0, 1023, 100, 0);
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      //It's time to change the color
      // save the last time you changed the position
      previousMillis = currentMillis;
      //Change the color
      color++;
    }
  }
}
//Animates twinkeling leds that fade away
void stars() {
  //Supposed to be black on startup
  //Do not want to clear every time in the loop, use fade instead
  FastLED.clear();
  int interval;
  unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  uint8_t randColor = 0;
  while (buttonNotPressed()) {
    fadeToBlackBy(leds, NUM_LEDS, map(analogRead(POT2_PIN), 0, 1023, 0, 255));
    //Read the timing of the next star
    interval = 1023-analogRead(POT1_PIN);
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      //It's time to create new star
      //save the last time you changed the position
      previousMillis = currentMillis;
      //position of next star
      int pos = random8(NUM_LEDS);
      //Read the color
      uint8_t color = map(analogRead(POT3_PIN), 0, 1023, 0, 255);
      if (color == 255) {
        //The value is 255, make the led a random color instead
        leds[pos] = CHSV(randColor, 255, 255);
        randColor=randColor+21;
      }
      else {
        //Create new "star"
        leds[pos] = CHSV(color, 255, 255);
      }
    }
    FastLED.show();
  }
}

void strobe() {
  int interval;
  boolean ledState = LOW;
  unsigned long previousMillis = 0;
  while (digitalRead(RGBBUTTON_PIN)==HIGH) {
    FastLED.clear();
    interval = analogRead(POT3_PIN);
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      // save the last time you blinked the LED
      previousMillis = currentMillis;
      // if the LED is off turn it on and vice-versa:
      if (ledState == LOW) {
        ledState = HIGH;
        fill_solid(leds, NUM_LEDS, CRGB::White);
      } else {
        ledState = LOW;
        //No need to change the leds, the clear comand in the beginning of the loop takes care of that
      }
    }
    FastLED.show();
  }
}
//Scrolls F6 in morse-code
void morse() {
  int interval;
  int startPosition = 0;
  unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  //F6 in morse
  boolean F6[23] = {true, false, true, false, true, false, true, false, true, true, true, false, false, false, true, false, true, true, true, false, true, false, true};
  boolean  forward = true;
  while (buttonNotPressed()) {
    FastLED.clear();
    for (int i = 0; i < sizeof(F6); i++) {
      //check if value should be drawn
      if (F6[i]) {
        //Divide the strip into morse pieces and fill the ones with true with the chosen color.
        //TODO change color to rgb from pot
        fill_solid(leds + NUM_LEDS / sizeof(F6)*i + startPosition, NUM_LEDS / sizeof(F6), CHSV(map(analogRead(POT1_PIN), 0, 1023, 0, 255), 255, 255));
      }
    }
    FastLED.show();
    interval = analogRead(POT2_PIN);
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      // save the last time you changed the position
      previousMillis = currentMillis;
      if (forward) {
        startPosition++;
      }
      else {
        startPosition--;
      }
      //Horrible hadcoded stuff
      //If the morse code reaches the end of the strip, the direction is set to reverse untill the beginning is reached again
      if (NUM_LEDS / sizeof(F6) * 23 + startPosition == NUM_LEDS) {
        forward = false;
      }
      if (startPosition == 0) {
        forward = true;
      }
    }
  }
}
void rgbFill() {
  while (buttonNotPressed()) {
    fill_solid(leds, NUM_LEDS, CRGB(map(analogRead(POT1_PIN), 0, 1023, 0, 255), map(analogRead(POT2_PIN), 0, 1023, 0, 255), map(analogRead(POT3_PIN), 0, 1023, 0, 255)));
    FastLED.show();
  }
}


//Setupfunctions
//Initialize the msgeq7
void initMSGEQ7() { 
  pinMode(MSGEQ7_RESET_PIN, OUTPUT);
  pinMode(MSGEQ7_STROBE_PIN, OUTPUT);
  digitalWrite(MSGEQ7_RESET_PIN, LOW);
  digitalWrite(MSGEQ7_STROBE_PIN, HIGH);
}
//Read audio levels from msgeq7
void ReadAudio() {
  digitalWrite(MSGEQ7_RESET_PIN, HIGH);
  digitalWrite(MSGEQ7_RESET_PIN, LOW);
  for (byte band = 0; band < 7; band++) {
    digitalWrite(MSGEQ7_STROBE_PIN, LOW);
    delayMicroseconds(30);
    levels[band] = analogRead(AUDIO_PIN);
    digitalWrite(MSGEQ7_STROBE_PIN, HIGH);
  }
}
//Returns when the button is released
void hold() {
  pinMode(13, OUTPUT);
  while (!buttonNotPressed()) {
    digitalWrite(13, HIGH);
  }
  digitalWrite(13, LOW);

}
boolean buttonNotPressed() {
  //Return false if one of the buttons are pressed down
  return (digitalRead(FBUTTON_PIN) == LOW && digitalRead(BBUTTON_PIN) == LOW && digitalRead(RGBBUTTON_PIN) == LOW);
}
