#include "BearSound.hpp"
#include "openal\al.h"
#include "openal\alc.h"
ALCdevice*  audioDevice = NULL;
ALCcontext* audioContext = NULL;
using namespace BearCore;
BearVector3<float> listenerPosition(0.f, 0.f, 0.f);
BearVector3<float> listenerDirection(0.f, 0.f, -1.f);
BearVector3<float> listenerUpVector(0.f, 1.f, 0.f);
float listenerVolume =100.f;
bool BearSound::BearSoundManager::Initialization()
{
	if (!audioDevice&&!audioContext)
	{
		audioDevice = alcOpenDevice(NULL);
		if (audioDevice)
		{
			audioContext = alcCreateContext(audioDevice, NULL);
			if (audioContext)
			{
				BearLog::Printf(TEXT("BearSound:Intializing..."));
				alcMakeContextCurrent(audioContext);
				float orientation[] = { listenerDirection.x,
					listenerDirection.y,
					listenerDirection.z,
					listenerUpVector.x,
					listenerUpVector.y,
					listenerUpVector.z };
				alListenerf(AL_GAIN, listenerVolume * 0.01f);
				alListener3f(AL_POSITION, listenerPosition.x, listenerPosition.y, listenerPosition.z);
				alListenerfv(AL_ORIENTATION, orientation);
				return true;

			}
			else
			{
				alcCloseDevice(audioDevice);
	
			}
			alcDestroyContext(audioContext);
			 audioDevice = NULL;
			audioContext = NULL;
			return false;
		}
		return false;
	}
	return false;
}

void BearSound::BearSoundManager::Close()
{
	alcMakeContextCurrent(NULL);
	if (audioContext)
		alcDestroyContext(audioContext);

	if (audioDevice)
		alcCloseDevice(audioDevice);
	audioDevice = NULL;
	audioContext = NULL;
}

void BearSound::BearSoundManager::setGlobalVolume(float volume)
{
	if (audioContext)
		alListenerf(AL_GAIN, volume );

	listenerVolume = volume;
}

float BearSound::BearSoundManager::getGlobalVolume()
{
	return listenerVolume;
}

