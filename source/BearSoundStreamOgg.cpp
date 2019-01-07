#include "BearSound.hpp"
#include "vorbis\codec.h"
#include "vorbis\vorbisenc.h"
#include "vorbis\vorbisfile.h"




using namespace BearCore;
size_t ov_read_func(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	BearInputStream* stream = static_cast<BearInputStream*>(datasource);
	return static_cast<size_t>(stream->Read(ptr, size*nmemb));
}


int ov_seek_func(void *datasource, ogg_int64_t offset, int whence)
{
	auto Aefile = ((BearInputStream*)datasource);
	switch (whence)
	{
	case SEEK_SET:
		break;
	case SEEK_CUR:
		offset += Aefile->Tell();
		break;
	case SEEK_END:
		offset = Aefile->Size() - static_cast<bsize>(offset);
	}
	return (int)Aefile->Seek(static_cast<bsize>(offset));
}

int ov_close_func(void *datasource)
{
	return 0;
}
long ov_tell_func(void *datasource)
{

	return  static_cast<long>(((BearInputStream*)datasource)->Tell());
}
static ov_callbacks callbacks = { &ov_read_func, &ov_seek_func, NULL, &ov_tell_func };

BearSound::BearSoundStreamOGG::BearSoundStreamOGG( BearSoundStreamOGG && cl)
{
	swap(cl);
}

BearSound::BearSoundStreamOGG::BearSoundStreamOGG(const BearSoundStreamOGG & right)
{
	copy(right);
}

BearSound::BearSoundStreamOGG & BearSound::BearSoundStreamOGG::operator=(const BearSoundStreamOGG & right)
{
	copy(right);
	return*this;
}

void BearSound::BearSoundStreamOGG::copy(const BearSoundStreamOGG & cl)
{
	m_data.copy(cl.m_data);
}

void BearSound::BearSoundStreamOGG::swap(BearSoundStreamOGG & cl)
{
	m_data.swap(cl.m_data);
}

BearSound::BearSoundStreamOGG & BearSound::BearSoundStreamOGG::operator=( BearSoundStreamOGG && cl)
{
	swap(cl);
	return *this;
}

BearSound::BearSoundStreamOGG::BearSoundStreamOGG()
{

}

BearSound::BearSoundStreamOGG::~BearSoundStreamOGG()
{
	clear();
}

bool BearSound::BearSoundStreamOGG::loadFromFile(const bchar * file)
{
	clear();
	BearFileStream *file_stream= bear_new<BearFileStream>();
	if (!file_stream->Open(file))
	{
		bear_delete(file_stream);
		return false;
	}
	
	return loadFromStream(BearStreamRef< BearInputStream>(file_stream));;
}

bool BearSound::BearSoundStreamOGG::loadFromStream(BearCore::BearStreamRef<BearCore::BearInputStream> & stream)
{
	clear();
	m_data.get()->m_OggVorbisFile = bear_new<::OggVorbis_File>();
	m_data.get()->m_stream = stream;
	int status = ov_open_callbacks(*m_data.get()->m_stream, m_data.get()->m_OggVorbisFile, NULL, 0, callbacks);
	if (status < 0)
	{
		clear();
		return  false;
	}
	m_data.get()->m_VorbisInfo = ov_info(m_data.get()->m_OggVorbisFile, -1);
	m_data.get()->m_info.channelCount = m_data.get()->m_VorbisInfo->channels;
	m_data.get()->m_info.sampleRate = m_data.get()->m_VorbisInfo->rate;
	m_data.get()->m_info.sampleCount = ov_pcm_total(m_data.get()->m_OggVorbisFile, -1) ;
	return true;
}

bool BearSound::BearSoundStreamOGG::loadFromBuffer(BearCore::BearStreamRef<BearCore::BearBufferedReader>& m_stream)
{
	bool res=loadFromStream(BearStreamRef< BearInputStream>(*m_stream));
	m_stream.clear_no_free();
	return res;
}

void BearSound::BearSoundStreamOGG::clear()
{
	m_data.clear();

}

BearSound::BearSoundStreamImpl * BearSound::BearSoundStreamOGG::copy()
{
	return bear_new<BearSoundStreamOGG>(*this);
}

const  BearSound::BearSoundStreamOGG::SoundInfo & BearSound::BearSoundStreamOGG::getInfo()
{
	return m_data.get()->m_info;
}

void BearSound::BearSoundStreamOGG::seek(bsize sec)
{
	if (!m_data.get()->m_OggVorbisFile)return;
	ov_pcm_seek(m_data.get()->m_OggVorbisFile,sec);
}

bsize BearSound::BearSoundStreamOGG::tell()
{
	if (!m_data.get()->m_OggVorbisFile)return 0;
	return  static_cast<bsize>(ov_pcm_tell(m_data.get()->m_OggVorbisFile));
	
}

bsize BearSound::BearSoundStreamOGG::readSound(void*data, bsize size)
{
	if (!m_data.get()->m_OggVorbisFile)return false;
	int current_section;
	long lDecodeSize;
	unsigned long ulSamples;
	short *pSamples;
	BEAR_ASSERT(!( size % (2 * m_data.get()->m_info.channelCount)));
	unsigned long ulBytesDone = 0;
	while (1)
	{
		lDecodeSize = ov_read(m_data.get()->m_OggVorbisFile, (char*)data + ulBytesDone,static_cast<int>( size - ulBytesDone), 0, 2, 1, &current_section);

		if (lDecodeSize > 0)
		{
			ulBytesDone += lDecodeSize;

			if (ulBytesDone == static_cast<unsigned long>(size))
				break;
			BEAR_ASSERT(ulBytesDone <= static_cast<unsigned long>(size));
		}
		else
		{
			BEAR_ASSERT(lDecodeSize==0);
			break;
		}
	}


	if (m_data.get()->m_info.channelCount == 6)
	{
		pSamples = (short*)data;
		for (ulSamples = 0; ulSamples < (static_cast<unsigned long>(size) >> 1); ulSamples += 6)
		{
			// WAVEFORMATEXTENSIBLE Order : FL, FR, FC, LFE, RL, RR
			// OggVorbis Order            : FL, FC, FR,  RL, RR, LFE
			bear_swap(pSamples[ulSamples + 1], pSamples[ulSamples + 2]);
			bear_swap(pSamples[ulSamples + 3], pSamples[ulSamples + 5]);
			bear_swap(pSamples[ulSamples + 4], pSamples[ulSamples + 5]);
		}
	}
	return ulBytesDone;
	
}


void BearSound::BearSoundStreamOGG::seekTime(double time)
{
	if (m_data.get()->m_OggVorbisFile)
	{
		ov_time_seek(m_data.get()->m_OggVorbisFile, time);
	}
}

BearSound::BearSoundStreamOGG::data::~data()
{
	ov_clear(m_OggVorbisFile);
	bear_free(m_OggVorbisFile);
}