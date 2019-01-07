#pragma once
#include "BearCore.hpp"
#ifdef BEAR_SOUND_EXPORTS
#define BEARSOUND_API __declspec(dllexport) 
#else
#define BEARSOUND_API __declspec(dllimport) 
#endif
#include "BearSound/BearSoundManager.h"
#include "BearIO/BearSoundStreamImpl.h"
#include "BearSound/BearSoundSource2D.h"
#include "BearParser/BearSoundStreamOgg.h"