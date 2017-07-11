
//These libraries are included by the Teensy Audio System Design Tool
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
//#include <SD.h>//not using the SD library at this time
#include <SerialFlash.h>
#include <Adafruit_NeoPixel.h>

#include "note_frequency.h"
#include "scales.h"
#include "TeensyAudioDesignTool.h"

#define NUM_NOTES 4
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

#define TOUCH_PIN_RED 33
#define TOUCH_PIN_BLUE 32
#define TOUCH_PIN_GREEN 17
#define TOUCH_PIN_ORANGE 16

#define TOUCH_PIN_MASTER 25

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            24

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      4

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

enum {GAME, FREESTYLE};

//an array to define each of the eight touch sense pins,
//all other touch sense pins are used by the Teensy Audio Shield (16,17,18,19)
int pinTouch[] = {33, 32, 17, 16, 25};

int mode = GAME;

int corrent_count = 0;

int scale_index = 0;//var to keep track fo which scale is being used

int dcVal = 100;//value to control the decay of each note


bool debug = 0;//Set to 1 for Serial debugging

bool master_touch = 0;

int last_note = NUM_NOTES + 1;

int tune[NUM_NOTES];

int sequence[NUM_NOTES];

int seq_pos = 0;

int last_touch = -1;

unsigned long last_touched = 0;

unsigned long freestyle_time = 0;


////////////////////////////////////////////////////////
void setup()
{
  //initialize buttons
  //initialize Serial
  Serial.begin(115200);
  pixels.begin();

  //set aside audio memory
  AudioMemory(64);

  //initialize audio settings
  sine1.amplitude(0.125);
  sine1.frequency(440);
  sine2.amplitude(0.125);
  sine2.frequency(440);
  sine3.amplitude(0.125);
  sine3.frequency(440);
  sine4.amplitude(0.125);
  sine4.frequency(440);

  dc1.amplitude(0);
  dc2.amplitude(0);
  dc3.amplitude(0);
  dc4.amplitude(0);

  //initialize volume
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.8);
  //enable line out for troubleshooting on O-scope
  //sgtl5000_1.unmuteLineout();


  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
  playFile("GAME.WAV");
  //playFile("FREE.WAV");

  tune[0] = 0;
  tune[1] = 1;
  tune[2] = 2;
  tune[3] = 3;

  clearSequence();
  playTune();
}
//////////////////////////////////////////////
void loop()
{
  // volumeCheck();//check the volume knob

  touchCheck();//check if any of the capacitive pads have been touched

  // dcValCheck();//check the decay knob

  //  buttonCheck();//check for button presses to change the scale

}

void clearSequence() {
  for (int i = 0; i < NUM_NOTES; i++) {
    sequence[0] = -1;
  }
  seq_pos = 0;
  last_touch = -1;
  Serial.println("Sequence Reset");
  last_touched = millis();
}

int pickNote() {
  int new_note = random(0, NUM_NOTES - 1);
  if (new_note == last_note) {
    return pickNote();
  }
  last_note = new_note;
  return new_note;
}

void newTune() {
  last_note = -1;
  tune[0] = pickNote();
  tune[1] = pickNote();
  tune[2] = pickNote();
  tune[3] = pickNote();

}

void playTune() {

  dc1.amplitude(0);
  dc2.amplitude(0);
  dc3.amplitude(0);
  dc4.amplitude(0);
  playTone(tune[0], 500, tune[0]);
  playTone(tune[1], 500, tune[1]);
  playTone(tune[2], 500, tune[2]);
  playTone(tune[3], 500, tune[3]);

}

//extra add on play tune with lightOn 
void lightOn(int num) {
  if (num == 0) { // RED
    pixels.setPixelColor(0, 255, 0, 0); //red on
  }
  if (num == 1) { // BLUE
    pixels.setPixelColor(1, 0, 0, 255); //blue on
  }
  if (num == 2) { // ORANGE
    pixels.setPixelColor(2, 255, 100, 0); //orange on
  }
  if (num == 3) { // GREEN
    pixels.setPixelColor(3, 0, 255, 0); //green on
  }
  pixels.show();
}

void lightOff(int num) {
  pixels.setPixelColor(num, 0, 0, 0); //all off 
  pixels.show();
}

void playTone(int tone, int duration, int light) {

  sine1.frequency(note_frequency[scale[scale_index][tone]]);
  dc1.amplitude(1.0, 5);
  lightOn(light);
  delay(duration);
  dc1.amplitude(0, dcVal);
  lightOff(light);

}


void playFile(const char *filename)
{

  dc1.amplitude(0);
  dc2.amplitude(0);
  dc3.amplitude(0);
  dc4.amplitude(0);

  Serial.print("Playing file: ");
  Serial.println(filename);
  sgtl5000_1.volume(0.4);

  // Start playing the file.  This sketch continues to
  // run while the file plays.
  playSdWav1.play(filename);

  // A brief delay for the library read WAV info
  delay(5);

  // Simply wait for the file to finish playing.
  while (playSdWav1.isPlaying()) {
    // uncomment these lines if you audio shield
    // has the optional volume pot soldered
    //float vol = analogRead(15);
    //vol = vol / 1024;
    // sgtl5000_1.volume(vol);
  }
  sgtl5000_1.volume(0.8);

}


bool sameSequence() {

  for (int i = 0; i < NUM_NOTES; i++) {
    if (sequence[0] != tune[0])
      return false;
  }
  return true;

}

/////////////////////////////////////////////////////
void touchCheck()
{
  //Each capacitive touch pad will vary based on the size and material it is made of
  //The value necessary to trigger each note will require some trial and error to get the
  //sensitivity just right. Try adjusting these values to get the best response.
  int new_touch = -1;

  if (touchRead(TOUCH_PIN_RED) > 3000)
  {
    //once a pad is touched, a value from the note frquency froma table is looked up via a 2D table
    //with x corresponding to a scale and y corresponding to one of the eight notes on the drum.

    sine1.frequency(note_frequency[scale[scale_index][0]]);
    dc1.amplitude(1.0, 5);
    new_touch = 0;
    Serial.print("RED touch \t");
    Serial.println(touchRead(TOUCH_PIN_RED));
    pixels.setPixelColor(0, 255, 0, 0); //red on
  } else {
    pixels.setPixelColor(0, 0, 0, 0); //red off
  }

  if (touchRead(TOUCH_PIN_RED) <= 3000)
  {
    //one the pad is released, the note fades out with a decay val set by the dcVal knob
    dc1.amplitude(0, dcVal);
  }


  if (touchRead(TOUCH_PIN_BLUE) > 3000)
  {
    sine2.frequency(note_frequency[scale[scale_index][1]]);
    dc2.amplitude(1.0, 5);
    new_touch = 1;
    Serial.print("BLUE touch\t");
    Serial.println(touchRead(TOUCH_PIN_BLUE));
    pixels.setPixelColor(1, 0, 0, 255); //blue on
  } else {
    pixels.setPixelColor(1, 0, 0, 0); //blue off

  }
  if (touchRead(TOUCH_PIN_BLUE) <= 3000)
  {
    dc2.amplitude(0, dcVal);
  }


  if (touchRead(TOUCH_PIN_ORANGE) > 3000)
  {
    sine3.frequency(note_frequency[scale[scale_index][2]]);
    dc3.amplitude(1.0, 5);
    new_touch = 2;
    Serial.print("ORANGE touch\t");
    Serial.println(touchRead(TOUCH_PIN_ORANGE));
    pixels.setPixelColor(2, 255, 100, 0); //orange on
  } else {
    pixels.setPixelColor(2, 0, 0, 0); //orange off
  }
  if (touchRead(TOUCH_PIN_ORANGE) <= 3000)
  {
    dc3.amplitude(0, dcVal);
  }


  if (touchRead(TOUCH_PIN_GREEN) > 3500)
  {
    sine4.frequency(note_frequency[scale[scale_index][3]]);
    dc4.amplitude(1.0, 5);
    new_touch = 3;
    Serial.print("GREEN touch\t");
    Serial.println(touchRead(TOUCH_PIN_GREEN));
    pixels.setPixelColor(3, 0, 255, 0); //green on
  } else {
    pixels.setPixelColor(3, 0, 0, 0); //green off
  }
  if (touchRead(TOUCH_PIN_GREEN) <= 3000)
  {
    dc4.amplitude(0, dcVal);
  }

  if (touchRead(TOUCH_PIN_MASTER) > 3000)
  {
    //   sine5.frequency(note_frequency[scale[scale_index][4]]);
    // dc5.amplitude(1.0, 5);
    if (!master_touch && mode == GAME) {
      master_touch = true;
      playTune();
    }
    //new_touch = -1;
  }
  if (touchRead(TOUCH_PIN_MASTER) <= 3000)
  {
    master_touch = false;
    //play_song(false);
    //dc5.amplitude(0, dcVal);
  }

  if (last_touch != new_touch && new_touch >= 0 && mode == GAME) {
    Serial.print(new_touch);
    Serial.print("\t");
    Serial.println(last_touch);
    last_touched = millis();
    last_touch = new_touch;
    sequence[seq_pos] = new_touch;
    seq_pos++;
    if (seq_pos == NUM_NOTES) {

      if (sameSequence()) {
        Serial.println("Correct");
        corrent_count++;
        delay(500);
        playFile("CORRECT.WAV");
        if (corrent_count == 3) {
          mode = FREESTYLE;
          freestyle_time = millis();
          delay(500);
          playFile("FREE.WAV");
          delay(500);
        } else {
          newTune();
          playTune();

        }

        //play correct
        // new tune

      } else {

        Serial.println("Wrong!");
        delay(500);

        playFile("WRONG.WAV");        // incorrect
        delay(500);
        playTune();
        // play sequece again
      }
      clearSequence();

    }
  }

  if (mode == GAME && millis() - last_touched > 3000) {

    clearSequence();

  }

  if ( mode == FREESTYLE && millis() - freestyle_time > 60000) {

    mode = GAME;
    playFile("GAME.WAV");
    corrent_count = 0;
    newTune();
    playTune();
    clearSequence();


  }

  pixels.show();
  delay(50);
}



