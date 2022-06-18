/*******************************************************************************

 Bare Conductive Touch MP3 player: for the tentacle (with edits by Helen Leigh)
 ------------------------------

 Take the tentacle in your arms and let it caress you. 
 Stroke the vein of silver stitched through the innermost part 
 of the soft, pillowy limb. The creature will respond.

 Be gentle and take comfort.

 ------------------------------
 
 Maderia HC range conductive thread (silver) machine emboidered on a white cotton 
 tentacle stuffed with polyester wadding. Thread connected to a Bare Conductive Touch 
 Board. Solder a wire to the board and connect the other end of the wire to a metal 
 press stud. sew the other side of the press stud on to your tentacle, and close the
 press stud. Disconnect the press stud for reprogramming board or washing tentacle.

 Replace the preprogrammed sound files on your SD card with the noise(s) you want for 
 each electrode.

 ------------------------------

 Original based on code by Jim Lindblom and plenty of inspiration from the Freescale
 Semiconductor datasheets and application notes.

 Bare Conductive code written by Stefan Dzisiewski-Smith, Peter Krige, Pascal
 Loose

 This work is licensed under a MIT license https://opensource.org/licenses/MIT

 Copyright (c) 2016, Bare Conductive

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

*******************************************************************************/

// touch includes
#include <MPR121.h>
#include <MPR121_Datastream.h>
#include <Wire.h>

// MP3 includes
#include <SPI.h>
#include <SdFat.h>
#include <FreeStack.h>
#include <SFEMP3Shield.h>

// touch constants
const uint32_t BAUD_RATE = 115200;
const uint8_t MPR121_ADDR = 0x5C;
const uint8_t MPR121_INT = 4;

// serial monitor behaviour constants
const bool WAIT_FOR_SERIAL = false;

// MPR121 datastream behaviour constants
const bool MPR121_DATASTREAM_ENABLE = false;

// MP3 variables
SFEMP3Shield MP3player;
uint8_t result;
uint8_t lastPlayed = 0;

// MP3 behaviour constants
const bool REPLAY_MODE = false;  // By default, touching an electrode repeatedly will
                                // play the track again from the start each time
                                //
                                // If you set this to false, repeatedly touching an
                                // electrode will stop the track if it is already
                                // playing, or play it from the start if it is not

// SD card instantiation
SdFat sd;

void setup() {
  Serial.begin(BAUD_RATE);

  pinMode(LED_BUILTIN, OUTPUT);

  if (WAIT_FOR_SERIAL) {
    while (!Serial);
  }

  if (!sd.begin(SD_SEL, SPI_HALF_SPEED)) {
    sd.initErrorHalt();
  }

  if (!MPR121.begin(MPR121_ADDR)) {
    Serial.println("error setting up MPR121");
    switch (MPR121.getError()) {
      case NO_ERROR:
        Serial.println("no error");
        break;
      case ADDRESS_UNKNOWN:
        Serial.println("incorrect address");
        break;
      case READBACK_FAIL:
        Serial.println("readback failure");
        break;
      case OVERCURRENT_FLAG:
        Serial.println("overcurrent on REXT pin");
        break;
      case OUT_OF_RANGE:
        Serial.println("electrode out of range");
        break;
      case NOT_INITED:
        Serial.println("not initialised");
        break;
      default:
        Serial.println("unknown error");
        break;
    }
    while (1);
  }

  MPR121.setInterruptPin(MPR121_INT);

  if (MPR121_DATASTREAM_ENABLE) {
    MPR121.restoreSavedThresholds();
    MPR121_Datastream.begin(&Serial);
  } else {
    MPR121.setTouchThreshold(40);
    MPR121.setReleaseThreshold(20);
  }

  MPR121.setFFI(FFI_10);
  MPR121.setSFI(SFI_10);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);  // switch on user LED while auto calibrating electrodes

  MPR121.setGlobalCDT(CDT_4US);  // reasonable for larger capacitances at the end of long cables when using Interactive Wall Kit
  MPR121.autoSetElectrodeCDC();  // autoset all electrode settings

  digitalWrite(LED_BUILTIN, LOW);

  result = MP3player.begin();
  MP3player.setVolume(10, 10);

  if (result != 0) {
    Serial.print("Error code: ");
    Serial.print(result);
    Serial.println(" when trying to start MP3 player");
  }
}

void loop() {
  MPR121.updateAll();

  // only make an action if we have one or fewer pins touched
  // ignore multiple touches
  if (MPR121.getNumTouches() <= 1) {
    for (int i=0; i < 12; i++) {  // Check which electrodes were pressed
      if (MPR121.isNewTouch(i)) {
          if (!MPR121_DATASTREAM_ENABLE) {
            Serial.print("pin ");
            Serial.print(i);
            Serial.println(" was just touched");
          }

          digitalWrite(LED_BUILTIN, HIGH);
          MP3player.playTrack(i-0);


  if (MPR121_DATASTREAM_ENABLE) {
    MPR121_Datastream.update();
  }
 }
}
