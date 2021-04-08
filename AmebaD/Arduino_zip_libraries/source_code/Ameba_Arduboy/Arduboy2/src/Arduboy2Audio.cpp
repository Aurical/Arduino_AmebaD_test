/**
 * @file Arduboy2Audio.cpp
 * \brief
 * The Arduboy2Audio class for speaker and sound control.
 */

#include "Arduboy2.h"

bool Arduboy2Audio::audio_enabled = false;

void Arduboy2Audio::on()
{
  // fire up audio pins by seting them as outputs
  pinMode(PIN_SPEAKER_1, OUTPUT);
  pinMode(PIN_SPEAKER_2, OUTPUT);

  audio_enabled = true;
}

void Arduboy2Audio::off()
{
  audio_enabled = false;
  // shut off audio pins by setting them as inputs
  pinMode(PIN_SPEAKER_1, INPUT);
  pinMode(PIN_SPEAKER_2, INPUT);
}

void Arduboy2Audio::toggle()
{
  if (audio_enabled)
    off();
  else
    on();
}

void Arduboy2Audio::saveOnOff()
{
  EEPROM.update(Arduboy2Base::eepromAudioOnOff, audio_enabled);
}

void Arduboy2Audio::begin()
{
  if (EEPROM.read(Arduboy2Base::eepromAudioOnOff))
    on();
  else
    off();
}

boolean Arduboy2Audio::enabled()
{
  return audio_enabled;
}
