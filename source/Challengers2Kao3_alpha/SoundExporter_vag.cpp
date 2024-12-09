#include <Challengers2Kao3_alpha/SoundExporter.h>
#include <Challengers2Kao3_alpha/WinApi.h>


////////////////////////////////////////////////////////////////
// Dekoduj bloki ADPCM formatu VAG
////////////////////////////////////////////////////////////////
int CSoundExporter::decodeVagBlocks(int32_t end_offset, int32_t &wskaznik, int32_t &loopBegin, int32_t &loopEnd)
{
	const double f[5][2] =
	{
		{ 0.0, 0.0 },
		{ 60.0 / 64.0, 0.0 },
		{ 115.0 / 64.0, -52.0 / 64.0 },
		{ 98.0 / 64.0, -55.0 / 64.0 },
		{ 122.0 / 64.0, -60.0 / 64.0 }
	};

	int32_t i = 0;
	int predict_nr = 0;
	int shift_factor = 0;
	int flags = 0;
	int d = 0;
	int s = 0;
	double s_1 = 0;
	double s_2 = 0;
	double samples[28];

	bool last_block_flag = false;

	/* Czy mamy możliwość odczytania jeszcze jednego bloku? */
	while ((przesunPlik(0) + 16) <= end_offset)
	{
		if (last_block_flag)
		{
			return 0;
		}

		czytajPlik(&predict_nr, 1);
		predict_nr &= 0xFF;
		shift_factor = predict_nr & 0x0F;
		predict_nr >>= 4;

		czytajPlik(&flags, 1);
		flags &= 0xFF;
		switch (flags)
		{
			case 0x00: case 0x02:
			{
				break;
			}
			case 0x01:
			{
				if (last_block_flag)
				{
					throw EKomunikat(L"Last block of the file (0x01) defined multiple times...");
					return 1;
				}
				else
				{
					last_block_flag = true;
				}
				break;
			}
			case 0x06:
			{
				if (loopBegin < 0)
				{
					loopBegin = wskaznik;
				}
				else
				{
					throw EKomunikat(L"Starting block of the loop (0x06) defined multiple times...");
					return 1;
				}
				break;
			}
			case 0x03:
			{
				if (loopEnd >= 0)
				{
					throw EKomunikat(L"Ending block of the loop (0x03) defined multiple times...");
					return 1;
				}
				else if (loopBegin < 0)
				{
					throw EKomunikat(L"Found ending block of the loop (0x03), but starting block (0x06) was not defined...");
					return 1;
				}
				else
				{
					loopEnd = wskaznik;

					/* Od bloku kończącego pętle mogą również być nieprzyjemne dźwięki */
					return 0;
				}
				break;
			}
			case 0x07:
			{
				return 0;
			}
			default:
			{
				wchar_t test[64];
				swprintf_s(test, 64, L"Unknown flag (0x%02X) in ADPCM block...", flags);
				throw EKomunikat(test);
				return 1;
			}
		}

		for (i = 0; i < 28; i += 2)
		{
			czytajPlik(&d, 1);
			d &= 0xFF;
			s = (d & 0x0F) << 12;
			if (s & 0x8000) s |= 0xFFFF0000;
			samples[i] = (double)(s >> shift_factor);
			s = (d & 0xF0) << 8;
			if (s & 0x8000) s |= 0xFFFF0000;
			samples[i + 1] = (double)(s >> shift_factor);
		}

		for (i = 0; i < 28; i++)
		{
			samples[i] = samples[i] + s_1 * f[predict_nr][0] + s_2 * f[predict_nr][1];
			s_2 = s_1;
			s_1 = samples[i];
			d = (int)(samples[i] + 0.5);

			if ((wskaznik + 2) >= Wav.Data.chunkSize)
			{
				throw EKomunikat(L"Data pointer went beyond the scope of Waveform data size...");
				return 1;
			}

			Wav.Data.Samples[wskaznik] = (d & 0xFF);
			Wav.Data.Samples[wskaznik + 1] = (d >> 8);
			wskaznik += 2;
		}
		
		/* W końcówce można znaleźć niepotrzebne bajty... */
		/* Najczęśćiej dotyczy to zapętlanych plików dźwiękowych */
		if (przesunPlik(0) == end_offset)
		{
			return 0;
		}
	}

	throw EKomunikat(L"File pointer went beyond the scope of ADPCM data size...");
	return 1;
}


////////////////////////////////////////////////////////////////
// Odczytuj dane z formatu VAG
// "PlayStation Compressed Sound File"
////////////////////////////////////////////////////////////////
int CSoundExporter::readVagFormat()
{
	int32_t x;
	int32_t start_offset;
	char nazwa_dzwieku[0x10];
	int32_t data_size = 0; // Waveform data size (bytes)
	int32_t wskaznik = 0; // Zawsze wcześniej niż (4 * data_size)
	int32_t loopBegin = (-1);
	int32_t loopEnd = (-1);

	// [0x00] Sprawdź nagłówek
	czytajPlik(&x, 0x04);
	if ((*(int32_t*)"VAGp") != x)
	{
		throw EKomunikat(L"Invalid file magic. Expected \"VAGp\".");
		return 1;
	}

	// [0x04] Sprawdź wersję
	czytajPlik(&x, 0x04);
	x = ((x >> 24) & 0xFF) | ((x >> 8) & 0xFF00) | ((x << 8) & 0xFF0000) | ((x << 24) & 0xFF000000);
	if (32 != x)
	{
		throw EKomunikat(L"Invalid file version. Expected 32.");
		return 1;
	}

	// [0x0C] Odczytaj rozmiar danych waveform
	przesunPlik(0x04);
	czytajPlik(&x, 0x04);
	data_size = ENDIAN_SWAP(x);

	// [0x10] Odczytaj częstotliwość próbkowania
	czytajPlik(&x, 0x04);
	Wav.Format.dwSamplesPerSec = ENDIAN_SWAP(x);
	switch (Wav.Format.dwSamplesPerSec)
	{
		case 22050: case 11025: case 16000: case 20000:
		{
			break;
		}
		default:
		{
			throw EKomunikat(L"Niepoprawna częstotliwość próbkowania.\r\nOczekiwano na 11025, 22050, 16000 lub 20000 Hz.");
			return 1;
		}
	}

	// [0x1E] Odczytaj liczbę kanałów (i pomiń jeden bajt)
	przesunPlik(0x0A);
	czytajPlik(&x, 0x02);
	x = (x & 0xFF);
	Wav.Format.wChannels = ((0 == x) || (1 == x)) ? 1 : x;
	if (1 != Wav.Format.wChannels)
	{
		throw EKomunikat(L"Niepoprawna liczba kanałów audio. Oczekiwano na 1 (Mono).");
		return 1;
	}

	// [0x20] Odczytaj nazwę dźwięku
	czytajPlik(nazwa_dzwieku, 0x10);
	if (0 != std::memcmp("tate2003", nazwa_dzwieku, 0x08))
	{
		throw EKomunikat(L"Niepoprawna nazwa dźwięku. Oczekiwano \"tate2003\".");
		return 1;
	}

	// [0x40] Początek dekodowania
	start_offset = przesunPlik(0x10);

	// Oblicz dane do zapisu
	Wav.Data.chunkSize = (4 * data_size);

	try
	{
		Wav.Data.Samples = new byte[Wav.Data.chunkSize];
	}
	catch (std::bad_alloc)
	{
		throw EKomunikat(L"Nie udało się zaalokować pamięci na Dane wyjściowe Audio...");
		return 1;
	}

	// Dekoduj bloki ADPCM
	CWindow::zmienTekstOknaLadowania((eUnicodeString(L"[") + ItemName + L"]\r\n< PlayStation compressed sound file > decoding...").getText());
	if (0 != decodeVagBlocks((start_offset + data_size), wskaznik, loopBegin, loopEnd))
	{
		return 1;
	}
	CWindow::zmienTekstOknaLadowania(L"");

	// Podmień rozmiar danych PCM
	Wav.Data.chunkSize = wskaznik;

	// Jeżeli znaleziono informacje o pętlach, to dodaj taką struktrę
	if ((loopBegin >= 0) && (loopEnd > loopBegin))
	{
		Wav.Sampler.cSampleLoops = 1;

		try
		{
			Wav.Sampler.Loops = new SWavSampleLoop[Wav.Sampler.cSampleLoops];
		}
		catch (std::bad_alloc)
		{
			throw EKomunikat(L"Nie udało się zaalokować pamięci na Pętle Audio...");
			return 1;
		}

		Wav.Sampler.Loops[0].dwBegin = loopBegin;
		Wav.Sampler.Loops[0].dwEnd = loopEnd;
	}

	return 0;
}
