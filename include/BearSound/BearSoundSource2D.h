namespace BearSound
{
	class BEARSOUND_API BearSoundSource2D
	{
		BEAR_CLASS_NO_COPY(BearSoundSource2D)
	public:
		enum status
		{
			playing,
			stoped,
			paused,
		};
		BearSoundSource2D();
		void swap(BearSoundSource2D&right);

		~BearSoundSource2D();
		void setSoundStream(BearSoundStreamImpl&stream, double time_start=0.0,bool full_load = false);
		void play(bool loop) const;
		void pause() const;
		void stop() const;
		void setPlayingOffset(BearCore::BearTime& Time);
		BearCore::BearTime getPlayingOffset() const;
		bool getLoop() const;
		void update();
		void setVolume(float volume) const;
		float getVolune() const;
		BearCore::BearTime getDuration() const;
		status getStatus() const;
		void clear();

	private:
		 uint32		    m_buffers[3];
		 uint8*m_data;
		 int32 m_format;
		 uint32 m_size_buffer;
		 uint32		    m_source;
		 BearSoundStreamImpl*m_stream;
		 bool m_update;
		 bsize m_pos_snd;
		 mutable bool m_loop;
	};
}