#include "config.h"
#include <Arduino.h>
#include <TTGO.h>

#include <AudioFileSourcePROGMEM.h>
#include "AudioGeneratorWAV.h"
#include <AudioOutputI2S.h>
#include "ESP8266SAM.h"
#include "sound_beep.h"

TTGOClass *ttgo;
AudioGeneratorWAV *wav;
AudioFileSourcePROGMEM *file;
AudioOutputI2S *out;
ESP8266SAM *sam;

void sound_speak(const char *str)
{
    log_i("Speak text", str);
    sam->Say(out, str);
    out->stop();
}

void sound_play_beep()
{
    log_i("Sound play beep");
    file = new AudioFileSourcePROGMEM(beep_wav, beep_wav_len);
    wav->begin(file, out);
}

void sound_setup()
{
    TTGOClass *ttgo = TTGOClass::getWatch();
    ttgo->enableLDO3(); // Turn on the audio power
    sam = new ESP8266SAM();
    sam->SetVoice(sam->VOICE_SAM);
    wav = new AudioGeneratorWAV();
    out = new AudioOutputI2S();
    out->SetPinout(TWATCH_DAC_IIS_BCK, TWATCH_DAC_IIS_WS, TWATCH_DAC_IIS_DOUT);
    out->SetGain(0.1);
    sound_speak("Hi, how are you?");
}

void sound_loop()
{
    if (wav->isRunning() && !wav->loop())
    {
        log_i("stop playing wav sound");
        wav->stop();
    }
}
