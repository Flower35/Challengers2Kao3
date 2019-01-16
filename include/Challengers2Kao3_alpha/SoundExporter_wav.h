#ifndef H_SOUND_EXPORTER_WAV
#define H_SOUND_EXPORTER_WAV

#include "Challengers2Kao3.h"
#include "FileOperator.h"


/* Luüne definicje dla formatu WAV */
//// (redefinicja) #define WAVE_FORMAT_PCM 0x0001
#define WAVE_FORMAT_EXTENSIBLE (-2) // 0xFFFE
#define MIDI_UNITY_NOTE 60 // düwiÍk "middle C"
#define WAV_ATRAC3PLUS_GUID "\xBF\xAA\x23\xE9\x58\xCB\x71\x44\xA1\x19\xFF\xFA\x01\xE4\xCE\x62"


////////////////////////////////////////////////////////////////
// Struktury RIFF WAV
////////////////////////////////////////////////////////////////

struct SWavChunk
{
	int32_t chunkID;
	int32_t chunkSize;

	SWavChunk();
	virtual void setUp();
	virtual int32_t getChunkSize();
	virtual int write(CFileOperator &);
	virtual int read(CFileOperator &);
};

struct SWavSampleLoop
{
	int32_t dwIdentifier;
	int32_t dwType;
	int32_t dwBegin;
	int32_t dwEnd;
	int32_t dwFraction;
	int32_t dwPlayCount;

	SWavSampleLoop();
	void write(CFileOperator &);
	void read(CFileOperator &);
};

struct SWavSamplerChunk : SWavChunk
{
	int32_t dwManufacturer;
	int32_t dwProduct;
	int32_t dwSamplePeriod;
	int32_t dwMIDIUnityNote; // zawsze "Middle C"
	int32_t dwMIDIPitchFraction; // zawsze zero
	int32_t dwSMPTEFormat; // zawsze zero
	int32_t dwSMPTEOffset; // zawsze zero
	int32_t cSampleLoops;
	int32_t cbSamplerData; // zawsze zero

	SWavSampleLoop* Loops;

	SWavSamplerChunk();
	~SWavSamplerChunk();
	void setUp();
	int32_t getChunkSize();
	int write(CFileOperator &);
	int read(CFileOperator &);
};

struct SWavFactChunk : SWavChunk
{
	int32_t dwSampleLength;
	int32_t dwSampleSkip;

	SWavFactChunk();
	void setUp();
	int32_t getChunkSize();
	int write(CFileOperator &);
	int read(CFileOperator &);
};

struct SWavFormatChunk : SWavChunk
{
	int16_t wFormatTag; // tylko PCM lub EXTENSIBLE
	uint16_t wChannels; // tylko MONO lub STEREO
	uint32_t dwSamplesPerSec; // SampleRate, np. 22050 Hz
	uint32_t dwAvgBytesPerSec; // ByteRate
	uint16_t wBlockAlign;
	uint16_t wBitsPerSample; // tylko 16

	int16_t cbSize;
	int16_t wSamplesPerBlock;
	int32_t dwChannelMask;
	byte SubFormat[16]; // tylko ATRAC3plus
	byte ExtraBytes[16];

	SWavFormatChunk();
	void setUp();
	int32_t getChunkSize();
	int write(CFileOperator &);
	int read(CFileOperator &);
};

struct SWavDataChunk : SWavChunk
{
	byte* Samples;
	byte* SecondaryBufor;

	SWavDataChunk();
	~SWavDataChunk();
	int write(CFileOperator &);
	int read(CFileOperator &);
};

struct SWavRiffChunk : SWavChunk
{
	SWavFormatChunk Format;
	SWavDataChunk Data;
	SWavSamplerChunk Sampler;
	SWavFactChunk Fact;

	SWavRiffChunk();
	int32_t getChunkSize();
	void setUp();
	int write(CFileOperator &);
	int read(CFileOperator &);
};


#endif
