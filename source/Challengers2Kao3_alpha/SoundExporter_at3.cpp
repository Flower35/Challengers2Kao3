#include <Challengers2Kao3_alpha/SoundExporter.h>
#include <Challengers2Kao3_alpha/WinApi.h>


////////////////////////////////////////////////////////////////
// Odczytuj dane z formatu AT3 (ATRAC3plus)
// "Adaptive Transform Acoustic Coding"
////////////////////////////////////////////////////////////////
int CSoundExporter::readAt3Format()
{
	int32_t block_size;
	int32_t source_size;
	int32_t frames;
	byte* input_wskaznik;
	byte* output_wskaznik;

	/* SprawdŸmy coœ... */
	if (0 != LibAtrac3plus::przygotujBiblioteke())
	{
		throw EKomunikat(L"\"ATRAC3+ decoding library\" has not been initialized correctly (?!)");
		return 1;
	}

	/* Odczytaj dziwn¹ liczbê i pomiñ dziwne bajty */
	if (SOUND_RODZAJ_AT3_MONO == Rodzaj)
	{
		czytajPlik(&block_size, 0x04);
		przesunPlik(2 * block_size);
	}

	/* Odczytaj wszystkie dane z pliku [".ac3"/".at3"] */
	if (0 != Wav.read(*this))
	{
		return 1;
	}

	/* SprawdŸ ustawienia formatu */
	switch (Wav.Format.wFormatTag)
	{
		case WAVE_FORMAT_PCM:
		{
			/* Nie musimy dokonwaæ dekompresji :) */
			return 0;
		}
		case WAVE_FORMAT_EXTENSIBLE:
		{
			break;
		}
		default:
		{
			/* Niemo¿liwe, ale whatever... */
			return 1;
		}
	}

	/* SprawdŸ liczbê kana³ów */
	switch (Wav.Format.wChannels)
	{
		case 0x01: /* MONO */
		case 0x02: /* STEREO */
		{
			break;
		}
		default:
		{
			throw EKomunikat(L"Incorrect <NumChannels>. Expected mono or stereo.");
			return 1;
		}
	}

	/* SprawdŸ czêstotliwoœæ próbkowania */
	if (0xAC44 != Wav.Format.dwSamplesPerSec)
	{
		throw EKomunikat(L"Incorrect <SampleRate>. Expected 44100 Hz.");
		return 1;
	}

	/* SprawdŸ rozmiar bloku ramki */
	block_size = ((Wav.Format.ExtraBytes[2] << 8) & 0xFF00) | (Wav.Format.ExtraBytes[3] & 0xFF);
	block_size = (((block_size & 0x03FF) * 8) + 0x08);
	if (block_size != Wav.Format.wBlockAlign)
	{
		throw EKomunikat(L"Incorrect <FrameBlockSize> (does not match extensible format bytes).");
		return 1;
	}

	/* Przenieœ stare dane */
	Wav.Data.SecondaryBufor = Wav.Data.Samples;
	Wav.Data.Samples = nullptr;
	source_size = Wav.Data.chunkSize;

	/* Przygotuj siê to dekodowania */
	frames = (source_size / block_size);
	Wav.Data.chunkSize = (Wav.Format.wChannels * sizeof(int16_t) * 0x0800 * frames);
	try
	{
		Wav.Data.Samples = new byte[Wav.Data.chunkSize];
	}
	catch (std::bad_alloc)
	{
		throw EKomunikat(L"Could not allocate bytes for Wave Samples data...");
		return 1;
	}
	output_wskaznik = Wav.Data.Samples;
	input_wskaznik = Wav.Data.SecondaryBufor;

	/* Dekoduj ramki "ATRAC3+" */

	/* (--dsp--) pierwsza ramka powinna byæ pominiêta??? */
	/* zauwa¿y³em, ¿e jest ona raczej pusta (same 0x0000) */

	showProgress(0x01, (-1));
	for (int i = 0; i < frames; i++)
	{
		if (0 != LibAtrac3plus::dekodujRamke(&output_wskaznik, &input_wskaznik, block_size, Wav.Format.wChannels))
		{
			throw EKomunikat(L"ATRAC3+ decoding error (!?)");
			return 1;
		}
		else if ((output_wskaznik - Wav.Data.Samples) > Wav.Data.chunkSize)
		{
			throw EKomunikat(L"Data pointer went beyond the scope of output buffer size...");
			return 1;
		}
		else if ((input_wskaznik - Wav.Data.SecondaryBufor) > source_size)
		{
			throw EKomunikat(L"Data pointer went beyond the scope of input buffer size...");
			return 1;
		}

		showProgress(0x01, (1 + i) / float(frames));
	}
	CWindow::zmienTekstOknaLadowania(L"");

	return 0;
}
