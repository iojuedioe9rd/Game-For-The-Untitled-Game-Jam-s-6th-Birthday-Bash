
#define MINIAUDIO_IMPLEMENTATION
#if defined(__EMSCRIPTEN__)
#define MA_EMSCRIPTEN
#define MA_NO_JACK
#define MA_NO_DSOUND
#define MA_NO_WASAPI
#define MA_NO_ALSA
#define MA_NO_OSS
#define MA_NO_PULSEAUDIO
#define MA_NO_COREAUDIO
#define MA_NO_SNDIO
#define MA_NO_AUDIO4
#define MA_NO_WINMM
#endif
#include "miniaudio.h"
