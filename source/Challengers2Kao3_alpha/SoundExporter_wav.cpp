#include <Challengers2Kao3_alpha/SoundExporter_wav.h>


////////////////////////////////////////////////////////////////
// Abstakcyjna struktura "WavChunk"
////////////////////////////////////////////////////////////////

SWavChunk::SWavChunk()
{
	chunkID = 0;
	chunkSize = 0;
}

void SWavChunk::setUp() {}

int32_t SWavChunk::getChunkSize()
{
	return 0;
}

int SWavChunk::write(CFileOperator &file)
{
	file.zapiszPlik(&chunkID, 0x04);
	file.zapiszPlik(&chunkSize, 0x04);
	
	return 0;
}

int SWavChunk::read(CFileOperator &file)
{
	file.czytajPlik(&chunkID, 0x04);
	file.czytajPlik(&chunkSize, 0x04);

	return 0;
}


////////////////////////////////////////////////////////////////
// Podstruktura "SampleLoop"
////////////////////////////////////////////////////////////////

SWavSampleLoop::SWavSampleLoop()
{
	dwIdentifier = 0x01;
	dwType = 0;
	dwBegin = (-1);
	dwEnd = (-1);
	dwFraction = 0;
	dwPlayCount = 0;
}

void SWavSampleLoop::write(CFileOperator &file)
{
	file.zapiszPlik(&dwIdentifier, 0x04);
	file.zapiszPlik(&dwType, 0x04);
	file.zapiszPlik(&dwBegin, 0x04);
	file.zapiszPlik(&dwEnd, 0x04);
	file.zapiszPlik(&dwFraction, 0x04);
	file.zapiszPlik(&dwPlayCount, 0x04);
}

void SWavSampleLoop::read(CFileOperator &file)
{
	file.czytajPlik(&dwIdentifier, 0x04);
	file.czytajPlik(&dwType, 0x04);
	file.czytajPlik(&dwBegin, 0x04);
	file.czytajPlik(&dwEnd, 0x04);
	file.czytajPlik(&dwFraction, 0x04);
	file.czytajPlik(&dwPlayCount, 0x04);
}


////////////////////////////////////////////////////////////////
// Struktura "SamplerChunk"
////////////////////////////////////////////////////////////////

SWavSamplerChunk::SWavSamplerChunk()
: SWavChunk()
{
	chunkID = *(int32_t*)"smpl";

	dwManufacturer = 0;
	dwProduct = 0;
	dwSamplePeriod = 0;
	dwMIDIUnityNote = MIDI_UNITY_NOTE;
	dwMIDIPitchFraction = 0;
	dwSMPTEFormat = 0;
	dwSMPTEOffset = 0;
	cSampleLoops = 0;
	cbSamplerData = 0;

	Loops = nullptr;
}

SWavSamplerChunk::~SWavSamplerChunk()
{
	if (nullptr != Loops)
	{
		delete[](Loops);
	}
}

void SWavSamplerChunk::setUp()
{
	cbSamplerData = 0;

	chunkSize = getChunkSize();
}

int32_t SWavSamplerChunk::getChunkSize()
{
	return (9 * 0x04) + (cSampleLoops * 6 * 0x04);
}

int SWavSamplerChunk::write(CFileOperator &file)
{
	::SWavChunk::write(file);

	file.zapiszPlik(&dwManufacturer, 0x04);
	file.zapiszPlik(&dwProduct, 0x04);
	file.zapiszPlik(&dwSamplePeriod, 0x04);
	file.zapiszPlik(&dwMIDIUnityNote, 0x04);
	file.zapiszPlik(&dwMIDIPitchFraction, 0x04);
	file.zapiszPlik(&dwSMPTEFormat, 0x04);
	file.zapiszPlik(&dwSMPTEOffset, 0x04);
	file.zapiszPlik(&cSampleLoops, 0x04);
	file.zapiszPlik(&cbSamplerData, 0x04);

	if (nullptr != Loops)
	{
		for (int i = 0; i < cSampleLoops; i++)
		{
			Loops[i].write(file);
		}
	}

	return 0;
}

int SWavSamplerChunk::read(CFileOperator &file)
{
	/* Czy jest minimalny rozmiar dla 0 sampli? */
	if (chunkSize < getChunkSize())
	{
		throw EKomunikat(L"Incorrect <smpl> chunk size.");
		return 1;
	}

	file.czytajPlik(&dwManufacturer, 0x04);
	file.czytajPlik(&dwProduct, 0x04);
	file.czytajPlik(&dwSamplePeriod, 0x04);
	file.czytajPlik(&dwMIDIUnityNote, 0x04);
	file.czytajPlik(&dwMIDIPitchFraction, 0x04);
	file.czytajPlik(&dwSMPTEFormat, 0x04);
	file.czytajPlik(&dwSMPTEOffset, 0x04);
	file.czytajPlik(&cSampleLoops, 0x04);
	file.czytajPlik(&cbSamplerData, 0x04);

	/* Sprawdź jeszcze raz po odczytaniu ilości loopów */
	if (getChunkSize() != chunkSize)
	{
		throw EKomunikat(L"Incorrect <smpl> chunk size.");
		return 1;
	}

	if (nullptr != Loops)
	{
		delete[](Loops);
		Loops = nullptr;
	}
	
	if (cSampleLoops > 0)
	{
		Loops = new SWavSampleLoop[cSampleLoops];

		for (int i = 0; i < cSampleLoops; i++)
		{
			Loops[i].read(file);
		}
	}

	return 0;
}


////////////////////////////////////////////////////////////////
// Struktura "FactChunk"
////////////////////////////////////////////////////////////////

SWavFactChunk::SWavFactChunk()
: SWavChunk()
{
	chunkID = *(int32_t*)"fact";

	dwSampleLength = 0;
	dwSampleSkip = 0;
}

void SWavFactChunk::setUp()
{
	chunkSize = getChunkSize();
}

int32_t SWavFactChunk::getChunkSize()
{
	return (2 * 0x04);
}

int SWavFactChunk::write(CFileOperator &file)
{
	::SWavChunk::write(file);

	file.zapiszPlik(&dwSampleLength, 0x04);
	file.zapiszPlik(&dwSampleSkip, 0x04);

	return 0;
}

int SWavFactChunk::read(CFileOperator &file)
{
	if (getChunkSize() != chunkSize)
	{
		throw EKomunikat(L"Incorrect <fact> chunk size.");
		return 1;
	}

	file.czytajPlik(&dwSampleLength, 0x04);
	file.czytajPlik(&dwSampleSkip, 0x04);

	return 0;
}


////////////////////////////////////////////////////////////////
// Struktura "FormatChunk"
////////////////////////////////////////////////////////////////

SWavFormatChunk::SWavFormatChunk()
: SWavChunk()
{
	chunkID = *(int32_t*)"fmt ";

	wFormatTag = WAVE_FORMAT_PCM;
	wChannels = 1;
	dwSamplesPerSec = 22050;
	dwAvgBytesPerSec = 0;
	wBlockAlign = 0;
	wBitsPerSample = 16;

	cbSize = 0;
	wSamplesPerBlock = 0;
	dwChannelMask = 0;
	std::memset(SubFormat, 0x00, 16);
	std::memset(ExtraBytes, 0x00, 16);
}

void SWavFormatChunk::setUp()
{
	wFormatTag = WAVE_FORMAT_PCM;
	wBitsPerSample = 16;
	wBlockAlign = wChannels * wBitsPerSample / 8;
	dwAvgBytesPerSec = wBlockAlign * dwSamplesPerSec;
	
	//// opcjonalnie, bo i tak nie sprawdzam
	//// cbSize = 0;

	chunkSize = getChunkSize();
}

int32_t SWavFormatChunk::getChunkSize()
{
	return (4 * 0x02) + (2 * 0x04)
		+ ((WAVE_FORMAT_EXTENSIBLE == wFormatTag) ? (0x02 + cbSize) : 0);
}

int SWavFormatChunk::write(CFileOperator &file)
{
	if (WAVE_FORMAT_PCM != wFormatTag)
	{
		throw EKomunikat(L"Only WAVE_FORMAT_PCM is supported for writing.");
		return 1;
	}

	::SWavChunk::write(file);

	file.zapiszPlik(&wFormatTag, 0x02);
	file.zapiszPlik(&wChannels, 0x02);
	file.zapiszPlik(&dwSamplesPerSec, 0x04);
	file.zapiszPlik(&dwAvgBytesPerSec, 0x04);
	file.zapiszPlik(&wBlockAlign, 0x02);
	file.zapiszPlik(&wBitsPerSample, 0x02);

	return 0;
}

int SWavFormatChunk::read(CFileOperator &file)
{
	/* Czy jest minimalny rozmiar dla bloku formatu? */
	if (chunkSize < getChunkSize())
	{
		throw EKomunikat(L"Incorrect <fmt> chunk size.");
		return 1;
	}

	file.czytajPlik(&wFormatTag, 0x02);
	file.czytajPlik(&wChannels, 0x02);
	file.czytajPlik(&dwSamplesPerSec, 0x04);
	file.czytajPlik(&dwAvgBytesPerSec, 0x04);
	file.czytajPlik(&wBlockAlign, 0x02);
	file.czytajPlik(&wBitsPerSample, 0x02);

	switch (wFormatTag)
	{
		case WAVE_FORMAT_PCM:
		{
			return 0;
		}
		case WAVE_FORMAT_EXTENSIBLE:
		{
			file.czytajPlik(&cbSize, 0x02);
			if ((cbSize < 22) || (cbSize > 38))
			{
				throw EKomunikat(L"Incorrect <cbSize> in <fmt> chunk.");
				return 1;
			}
			/* Sprawdź jeszcze raz po odczytaniu extra size */
			if (getChunkSize() != chunkSize)
			{
				throw EKomunikat(L"Incorrect <fmt> chunk size.");
				return 1;
			}

			file.czytajPlik(&wSamplesPerBlock, 0x02);
			file.czytajPlik(&dwChannelMask, 0x04);
			file.czytajPlik(SubFormat, 0x10);
			if (0 != std::memcmp(SubFormat, WAV_ATRAC3PLUS_GUID, 0x10))
			{
				throw EKomunikat(L"Unsupported <GUID> in <SubFormat> chunk. Expected <ATRAC3plus>. ");
				return 1;
			}

			file.czytajPlik(ExtraBytes, (cbSize - 22));
			return 0;
		}
		default:
		{
			throw EKomunikat(L"Unsupported <FormatTag> in <fmt> chunk.");
			return 1;
		}
	}
}


////////////////////////////////////////////////////////////////
// Struktura "DataChunk"
////////////////////////////////////////////////////////////////

SWavDataChunk::SWavDataChunk()
: SWavChunk()
{
	chunkID = *(int32_t*)"data";

	Samples = nullptr;
	SecondaryBufor = nullptr;
}

SWavDataChunk::~SWavDataChunk()
{
	if (nullptr != Samples)
	{
		delete[](Samples);
	}

	if (nullptr != SecondaryBufor)
	{
		delete[](SecondaryBufor);
	}
}

int SWavDataChunk::write(CFileOperator &file)
{
	::SWavChunk::write(file);

	if (nullptr != Samples)
	{
		file.zapiszPlik(Samples, chunkSize);
	}
	
	return 0;
}

int SWavDataChunk::read(CFileOperator &file)
{
	if (nullptr != Samples)
	{
		delete[](Samples);
		Samples = nullptr;
	}
	
	Samples = new byte[chunkSize];

	file.czytajPlik(Samples, chunkSize);

	return 0;
}


////////////////////////////////////////////////////////////////
// Struktura "RiffChunk"
////////////////////////////////////////////////////////////////

SWavRiffChunk::SWavRiffChunk()
: SWavChunk()
{
	chunkID = *(int32_t*)"RIFF";
}

void SWavRiffChunk::setUp()
{
	Format.setUp();

	Data.setUp();

	if (Sampler.cSampleLoops > 0)
	{
		/* Trochę głupio, ale muszę przenieść dane z innej struktury :P */
		Sampler.dwSamplePeriod = int32_t(1e+9f / Format.dwSamplesPerSec);
		Sampler.setUp();
	}

	/* Nie zapisujemy bloku <fact>, bo nasz format nie jest skompresowany */

	chunkSize = getChunkSize();
}

int32_t SWavRiffChunk::getChunkSize()
{
	return (0x04)
		+ (0x08 + Format.chunkSize)
		+ (0x08 + Data.chunkSize)
		+ ((Sampler.cSampleLoops > 0) ? (0x08 + Sampler.chunkSize) : 0);
}

int SWavRiffChunk::write(CFileOperator &file)
{
	::SWavChunk::write(file);
	file.zapiszPlik("WAVE", 0x04);

	if (0 != Format.write(file))
	{
		return 1;
	}

	/* Nie zapisujemy bloku <fact>, bo nasz format nie jest skompresowany */

	if (0 != Data.write(file))
	{
		return 1;
	}

	if (Sampler.cSampleLoops > 0)
	{
		if (0 != Sampler.write(file))
		{
			return 1;
		}
	}

	return 0;
}

int SWavRiffChunk::read(CFileOperator &file)
{
	int32_t x;
	bool fmt_chunk = false;
	bool data_chunk = false;
	bool smpl_chunk = false;
	bool fact_chunk = false;
	SWavChunk dummy;

	if (0 != ::SWavChunk::read(file))
	{
		return 1;
	}
	if ((*(int*)"RIFF") != chunkID)
	{
		throw EKomunikat(L"Expected <RIFF> <WAVE> at the beginning of the file...");
		return 1;
	}

	file.czytajPlik(&x, 0x04);
	if ((*(int*)"WAVE") != x)
	{
		throw EKomunikat(L"Expected <RIFF> <WAVE> at the beginning of the file...");
		return 1;
	}

	/* Pętla odczytywania chunków */
	x = 0x04;
	while (x < chunkSize)
	{
		if (0 != dummy.read(file))
		{
			return 1;
		}
		x += 0x08;

		switch (dummy.chunkID)
		{
			case 0x20746D66: // "fmt "
			{
				if (fmt_chunk)
				{
					throw EKomunikat(L"<fmt> chunk defined multiple times...");
					return 1;
				}
				else
				{
					fmt_chunk = true;
					Format.chunkSize = dummy.chunkSize;
					if (0 != Format.read(file))
					{
						return 1;
					}
					x += Format.chunkSize;
				}

				break;
			}

			case 0x6C706D73: // "smpl"
			{
				if (!fmt_chunk)
				{
					throw EKomunikat(L"<smpl> chunk reached, but <fmt> chunk was not defined yet...");
					return 1;
				}
				else if (smpl_chunk)
				{
					throw EKomunikat(L"<smpl> chunk defined multiple times...");
					return 1;
				}
				else
				{
					smpl_chunk = true;
					Sampler.chunkSize = dummy.chunkSize;
					if (0 != Sampler.read(file))
					{
						return 1;
					}
					x += Sampler.chunkSize;
				}

				break;
			}

			case 0x74636166: // "fact"
			{
				if (!fmt_chunk)
				{
					throw EKomunikat(L"<fact> chunk reached, but <fmt> chunk was not defined yet...");
					return 1;
				}
				else if (fact_chunk)
				{
					throw EKomunikat(L"<fact> chunk defined multiple times...");
					return 1;
				}
				else
				{
					fact_chunk = true;
					Fact.chunkSize = dummy.chunkSize;
					if (0 != Fact.read(file))
					{
						return 1;
					}
					x += Fact.chunkSize;
				}

				break;
			}

			case 0x61746164: // "data"
			{
				if (!fmt_chunk)
				{
					throw EKomunikat(L"<data> chunk reached, but <fmt> chunk was not defined yet...");
					return 1;
				}
				else if (data_chunk)
				{
					throw EKomunikat(L"<data> chunk defined multiple times...");
					return 1;
				}
				else
				{
					data_chunk = true;
					Data.chunkSize = dummy.chunkSize;
					if (0 != Data.read(file))
					{
						return 1;
					}
					x += Data.chunkSize;
				}

				break;
			}

			default:
			{
				wchar_t test1[64];
				swprintf_s(test1, 64, L"Unsupported chunk <0x%08X> at offset 0x%08X.", dummy.chunkID, file.przesunPlik(-4));
				throw EKomunikat(test1);
				return 1;
			}
		}
	}

	/* Sprawdzenie, czy nic się nie zepsuło */
	if (x > chunkSize)
	{
		throw EKomunikat(L"File pointer went beyond the scope of <RIFF> chunk size...");
		return 1;
	}

	/* Sprawdzenie, czy odczytano ważne chunki */
	if (!fmt_chunk)
	{
		throw EKomunikat(L"<fmt> chunk not found!");
		return 1;
	}
	if (!data_chunk)
	{
		throw EKomunikat(L"<data> chunk not found!");
		return 1;
	}
	if ((WAVE_FORMAT_EXTENSIBLE == Format.wFormatTag) && (!fact_chunk))
	{
		throw EKomunikat(L"<fact> chunk not found!");
		return 1;
	}

	return 0;
}
