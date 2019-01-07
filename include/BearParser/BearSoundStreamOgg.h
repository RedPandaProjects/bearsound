#pragma once
struct OggVorbis_File;
struct vorbis_info;
namespace BearSound
{
	
	class BEARSOUND_API BearSoundStreamOGG :public BearSoundStreamImpl
	{
	public:
		BearSoundStreamOGG( BearSoundStreamOGG&&right);
		BearSoundStreamOGG(const BearSoundStreamOGG&right);
		BearSoundStreamOGG&operator=(const BearSoundStreamOGG&right);
		void copy(const BearSoundStreamOGG &right);

		void swap(BearSoundStreamOGG&right);
		BearSoundStreamOGG&operator=(BearSoundStreamOGG&&right);
		BearSoundStreamOGG();
		~BearSoundStreamOGG();
		bool loadFromFile(const bchar*file);
		bool loadFromStream(BearCore::BearStreamRef<BearCore::BearInputStream> & m_stream);
		bool loadFromBuffer(BearCore::BearStreamRef<BearCore::BearBufferedReader> & m_stream);
		virtual void clear();
	private:
		virtual BearSoundStreamImpl* copy();
		virtual const SoundInfo& getInfo();
		virtual void seek(bsize sec) ;
		virtual bsize tell();
		virtual bsize readSound(void*data, bsize countRate);
		virtual void seekTime(double time);
		struct data
		{
			~data();
			SoundInfo m_info;
			OggVorbis_File	*m_OggVorbisFile;
			vorbis_info		*m_VorbisInfo;
			BearCore::BearStreamRef< BearCore::BearInputStream > m_stream;
		};
		BearCore::BearMemoryMultiRef<data> m_data;
	};
}