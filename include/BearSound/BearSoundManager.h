#pragma once
namespace BearSound
{
	class BEARSOUND_API BearSoundManager
	{
	public:
		static bool Initialization();
		static void Close();
		static void setGlobalVolume(float volume);
		static float getGlobalVolume();
	private:
		
	};
}