
namespace BearSound
{
	class BEARSOUND_API BearSoundSource2D
	{
	public:
		enum status
		{
			playing,
			stoped,
			paused,
		};
		BearSoundSource2D();
		~BearSoundSource2D();
		void setSoundStream(BearSoundStreamImpl&stream,bool full_load=false);
		void play(bool loop) const;
		void pause() const;
		void stop() const;
		void setPlayingOffset(BearTime& Time) const;
		BearTime getPlayingOffset() const;
		bool getLoop() const;
		void update();
		void setVolume(float volume) const;
		float getVolune() const;
		BearTime getDuration() const;
		status getStatus() const;
		void clear();
		BearSoundSource2D&operator=(const BearSoundSource2D&t);
		void swap(const BearSoundSource2D&t);
	private:
		bsize		    m_buffers[3];
		bsize		    m_source;
		const BearSoundStreamImpl*m_stream;
		bool m_update;
	};
}