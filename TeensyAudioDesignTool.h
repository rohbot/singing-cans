#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveformSine   sine3;          //xy=95,258
AudioSynthWaveformDc     dc2;            //xy=97,205
AudioSynthWaveformDc     dc3;            //xy=97,307
AudioSynthWaveformSine   sine1;          //xy=98,60
AudioSynthWaveformSine   sine2;          //xy=98,160
AudioSynthWaveformDc     dc1;            //xy=99,105
AudioSynthWaveformSine   sine4;          //xy=99,357
AudioSynthWaveformDc     dc4;            //xy=99,403
AudioPlaySdWav           playSdWav1;     //xy=112,21
AudioEffectMultiply      multiply1;      //xy=235,84
AudioEffectMultiply      multiply2;      //xy=243,180
AudioEffectMultiply      multiply3;      //xy=246,284
AudioEffectMultiply      multiply4;      //xy=265,373
AudioMixer4              mixer1;         //xy=465,235
AudioMixer4              mixer2;         //xy=591,64
AudioOutputI2S           i2s1;           //xy=733,160
AudioConnection          patchCord1(sine3, 0, multiply3, 0);
AudioConnection          patchCord2(dc2, 0, multiply2, 1);
AudioConnection          patchCord3(dc3, 0, multiply3, 1);
AudioConnection          patchCord4(sine1, 0, multiply1, 0);
AudioConnection          patchCord5(sine2, 0, multiply2, 0);
AudioConnection          patchCord6(dc1, 0, multiply1, 1);
AudioConnection          patchCord7(sine4, 0, multiply4, 0);
AudioConnection          patchCord8(dc4, 0, multiply4, 1);
AudioConnection          patchCord9(playSdWav1, 0, mixer2, 0);
AudioConnection          patchCord10(playSdWav1, 1, mixer2, 1);
AudioConnection          patchCord11(multiply1, 0, mixer1, 0);
AudioConnection          patchCord12(multiply2, 0, mixer1, 1);
AudioConnection          patchCord13(multiply3, 0, mixer1, 2);
AudioConnection          patchCord14(multiply4, 0, mixer1, 3);
AudioConnection          patchCord15(mixer1, 0, mixer2, 2);
AudioConnection          patchCord16(mixer2, 0, i2s1, 0);
AudioConnection          patchCord17(mixer2, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=112,475
// GUItool: end automatically generated code

