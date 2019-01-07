#pragma once
namespace BearSound
{
	class BearSoundStreamImpl
	{
	public:
		struct SoundInfo
		{
			uint64 sampleCount;
			uint32 channelCount;
			uint32 sampleRate;
		};
		virtual const SoundInfo& getInfo() = 0;
		virtual bsize readSound(void*data, bsize countRate) =0;
		virtual void seek(bsize pos) = 0;
		virtual bsize tell() = 0;
		virtual BearSoundStreamImpl* copy()=0;
		virtual void clear() = 0;
		virtual void seekTime(double time)=0;
	};
}