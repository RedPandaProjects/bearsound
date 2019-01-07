#include "BearSound.hpp"
#include "openal\al.h"
#include "openal\alc.h"

extern ALCdevice*  audioDevice ;
extern ALCcontext* audioContext ;


using namespace BearCore;
using namespace BearSound;
int getFormatFromChannelCount(unsigned int channelCount)
{
	int format = 0;
	switch (channelCount)
	{
	case 1:  format = AL_FORMAT_MONO16;                    break;
	case 2:  format = AL_FORMAT_STEREO16;                  break;
	case 4:  format = alGetEnumValue("AL_FORMAT_QUAD16");  break;
	case 6:  format = alGetEnumValue("AL_FORMAT_51CHN16"); break;
	case 7:  format = alGetEnumValue("AL_FORMAT_61CHN16"); break;
	case 8:  format = alGetEnumValue("AL_FORMAT_71CHN16"); break;
	default:
		BEAR_FATALERROR(false, TEXT("BearSound не подерживает больше 8 каналов звука channelCount[%u]"), channelCount);
	}

	return format;
}


BearSoundSource2D::BearSoundSource2D()
{
	bear_fill(m_buffers);
	m_source = 0;
	m_stream = 0; m_update = false; m_format = 0; m_size_buffer = 0; m_data = 0; m_pos_snd = 0; m_loop = false;

}



BearSoundSource2D::~BearSoundSource2D()
{
	clear();
}

void BearSound::BearSoundSource2D::setSoundStream(BearSoundStreamImpl & stream, double time_start , bool full_load)
{
	clear();
	m_stream = stream.copy();
	m_update = !full_load;
	int32	 ulFormat = getFormatFromChannelCount(stream.getInfo().channelCount);
	m_format = ulFormat;
	if (full_load)
	{
		stream.seekTime(time_start);
		bsize buffer_size =static_cast<bsize> (stream.getInfo().sampleCount * 2 * stream.getInfo().channelCount);
		uint8*bufferr = bear_alloc<uint8>(buffer_size);
		alGenBuffers(1, m_buffers);
		alGenSources(1, &m_source);

		if (stream.readSound(bufferr, buffer_size))
		{
			alBufferData(m_buffers[0], ulFormat, bufferr, static_cast<ALsizei>(buffer_size), stream.getInfo().sampleRate);
			alSourceQueueBuffers(m_source, 1, &m_buffers[0]);
		}
		bear_free(bufferr);
	}
	else
	{
		stream.seekTime(time_start);
		uint32 buffer_size = (stream.getInfo().sampleRate )*stream.getInfo().channelCount*2;
		buffer_size -= buffer_size % (stream.getInfo().channelCount * 2);
		alGenBuffers(3, m_buffers);
		alGenSources(1, &m_source);
		m_size_buffer = buffer_size;
		m_data = bear_alloc<uint8>(buffer_size);
		for (uint8 i = 0; i < 3; i++)
		{
			bsize size = stream.readSound(m_data, buffer_size);
			if (size)
			{
				alBufferData(m_buffers[i], ulFormat, m_data, static_cast<ALsizei>( size), stream.getInfo().sampleRate);
				alSourceQueueBuffers(m_source, 1, &m_buffers[i]);
			}
			if (size != buffer_size) {
				
				m_update = false;
				break;
			}
		}
		m_pos_snd = stream.tell();

	}
}


void BearSoundSource2D::play(bool loop) const
{
	if (!m_source)return;
	alSourcePlay(m_source);
	if (m_update)
		m_loop = loop;
	else
	alSourcei(m_source, AL_LOOPING, loop);
}

void BearSoundSource2D::pause() const
{
	if (!m_source)return;
	alSourcePause(m_source);
}

void BearSoundSource2D::stop() const
{
	if (!m_source)return;
		alSourceStop(m_source);
}

void BearSoundSource2D::setPlayingOffset(BearTime & Time) 
{
	if (!m_source)return;
	if (m_update)
	{
		auto stream = m_stream;
		m_stream = 0;
		setSoundStream(*stream, Time.asseconds(), true);
		bear_free(stream);

	}
	else
	{
		alSourcef(m_source, AL_SEC_OFFSET, Time.asseconds());
	}
}
BearTime BearSoundSource2D::getPlayingOffset()const 
{
	if (!m_source)return BearTime();
	ALfloat secs = 0.f;
	alGetSourcef(m_source, AL_SEC_OFFSET, &secs);
	BearTime times;
	times.append(secs);
	return times;
}

bool BearSoundSource2D::getLoop()const
{
	if (!m_source)return 0 ;
	ALint loop;
	alGetSourcei(m_source, AL_LOOPING, &loop);

	return loop != 0;
}

void BearSound::BearSoundSource2D::update()
{
	if (!m_update||!m_source)return;

	ALint iBuffersProcessed = 0;
	alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &iBuffersProcessed);
	while (iBuffersProcessed)
	{
		ALuint buffer = 0;
		alSourceUnqueueBuffers(m_source, 1, &buffer); 
		m_stream->seek(m_pos_snd);
		bsize size = m_stream->readSound(m_data,m_size_buffer);
		if (size)
		{
			alBufferData(buffer, m_format, m_data, static_cast<ALsizei>(size), m_stream->getInfo().sampleRate);
			alSourceQueueBuffers(m_source, 1, &buffer);
		}
		m_pos_snd = m_stream->tell();
		if (size != m_size_buffer) {
			m_update = m_loop; m_pos_snd = 0; if(!m_loop)break;
		}

		iBuffersProcessed--;
	}


}

void BearSoundSource2D::setVolume(float volume)const
{
	if (!m_source)return;
	alSourcef(m_source, AL_GAIN, volume );
}

float BearSoundSource2D::getVolune()const
{
	if (!m_source)return 0;
	ALfloat gain;
	alGetSourcef(m_source, AL_GAIN, &gain);

	return gain;
}

BearTime BearSoundSource2D::getDuration()const
{
	return BearTime();
//	alSourceQueueBuffers(
	/*BearTime times;
	float sec = (float)info.sampleCount / info.sampleRate / info.channelCount;
	times.assign(sec);
	return times;*/
}

BearSoundSource2D::status BearSoundSource2D::getStatus()const
{
	if (!m_source)return status::stoped;
	ALint status;
	alGetSourcei(m_source, AL_SOURCE_STATE, &status);
	switch (status)
	{
	case AL_INITIAL:
	case AL_STOPPED: return status::stoped;
	case AL_PAUSED:  return status::paused;
	case AL_PLAYING: return status::playing;
	}

	return status::stoped;
}

void BearSoundSource2D::clear()
{
	if (m_stream)
	{
		m_stream->clear();
	}
	bear_free(m_stream);
	bear_free(m_data);
	m_update = false;
	if (m_source) {
		stop();
		alDeleteSources(1, &m_source);
	}
	uint32 num_b = 0;
	for (; m_buffers[num_b] && num_b < 4; num_b++);
	alDeleteBuffers(num_b, m_buffers);
	bear_fill(m_buffers);
	m_source = 0;
	m_format = 0;
	m_pos_snd = 0;
	m_size_buffer = 0;
	m_loop = 0;
	

}

void BearSound::BearSoundSource2D::swap(BearSoundSource2D & t)
{
	bear_swap(m_buffers[0], t.m_buffers[0]);
	bear_swap(m_buffers[1], t.m_buffers[1]);
	bear_swap(m_buffers[2], t.m_buffers[2]);

	bear_swap(m_source, t.m_source);
	bear_swap(m_stream, t.m_stream);
	bear_swap(m_update, t.m_update);
	bear_swap(m_data, t.m_data);
	bear_swap(m_format, t.m_format);
	bear_swap(m_size_buffer, t.m_size_buffer);
	bear_swap(m_pos_snd, t.m_pos_snd);
	bear_swap(m_loop, t.m_loop);
	
}
void Swap(short &s1, short &s2)
{
	short sTemp = s1;
	s1 = s2;
	s2 = sTemp;
}