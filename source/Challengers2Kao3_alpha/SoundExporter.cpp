#include <Challengers2Kao3_alpha/SoundExporter.h>
#include <Challengers2Kao3_alpha/WinApi.h>


////////////////////////////////////////////////////////////////
// Konstruktor klasy eksportera dźwięków
////////////////////////////////////////////////////////////////
CSoundExporter::CSoundExporter(int rozpoznany_typ, HANDLE archiwum, eUnicodeString &full_path, eUnicodeString &simple_name, int32_t offset)
: CFileOperator(archiwum, false, INVALID_HANDLE_VALUE, true)
{
	Sciezka = full_path;
	ItemName = simple_name;
	KoncowyOffset = offset;
	Rodzaj = rozpoznany_typ;
}


////////////////////////////////////////////////////////////////
// Dekonstruktor klasy eksportera dźwięków
////////////////////////////////////////////////////////////////
CSoundExporter::~CSoundExporter()
{
	switch (Rodzaj)
	{
		case SOUND_RODZAJ_AT3_MONO:
		case SOUND_RODZAJ_AT3_STEREO:
		{
			LibAtrac3plus::zamknijBiblioteke();
			break;
		}
	}
}


////////////////////////////////////////////////////////////////
// Przygotuj się do eksportowania bitmapy
////////////////////////////////////////////////////////////////
void CSoundExporter::skanujPlik()
{
	try
	{
		if (0 == odczytajAudio())
		{
			eksportujAudio();
		}
	}
	catch (EKomunikat msg)
	{
		msg.pokazKomunikat(eUnicodeString(L"[Module: Sound Exporter]\r\n") + ItemName + L"\r\n\r\n");
	}
}


////////////////////////////////////////////////////////////////
// Odczytywanie wszystkich danych z dźwięku
////////////////////////////////////////////////////////////////
int CSoundExporter::odczytajAudio()
{
	switch (Rodzaj)
	{
		/*** PlayStation Compressed Sound File ***/
		case SOUND_RODZAJ_VAG:
		{
			return readVagFormat();
		}

		/*** Adaptive Transform Acoustic Coding ***/
		case SOUND_RODZAJ_AT3_MONO:
		case SOUND_RODZAJ_AT3_STEREO:
		{
			return readAt3Format();
		}

		default:
		{
			throw EKomunikat(L"Unknown sound type. Expected \"VAG\" or \"ATRAC3plus\".", 1);
			return 1;
		}
	}
}


////////////////////////////////////////////////////////////////
// Pokaż postęp w odczytywaniu sampli lub dekodowaniu ramek
////////////////////////////////////////////////////////////////
void CSoundExporter::showProgress(int32_t activity, float fraction)
{
	wchar_t percentage_text[16];
	int32_t percentage_num;
	wchar_t* activity_text = nullptr;

	switch (activity)
	{
		case 0x01:
		{
			activity_text = L"<ATRAC3+> decoding...";
			break;
		}
		case 0x02:
		{
			activity_text = L"<libvorbis> encoding...";
			break;
		}
	}

	if (fraction >= 0)
	{
		percentage_num = int32_t(100 * ((fraction <= 1.0f) ? fraction : 1.0f));
		swprintf_s(percentage_text, 16, L" (%d%%)", percentage_num);
	}
	else
	{
		percentage_text[0] = 0x00;
	}

	CWindow::zmienTekstOknaLadowania((eUnicodeString(L"[") + ItemName + L"]\r\n" + activity_text + percentage_text).getText());
}


////////////////////////////////////////////////////////////////
// Eksport pliku audio
////////////////////////////////////////////////////////////////
void CSoundExporter::eksportujAudio()
{
	/* Tworzenie podkatalogów */
	wchar_t* sciezka_text = Sciezka.getText();
	if (!tworzPodkatalogi(sciezka_text))
	{
		return;
	}

	/* Spróbuj utworzyć plik docelowy */
	if (!otworzPlikDoZapisu(sciezka_text))
	{
		return;
	}

	switch (Rodzaj)
	{
		/* Zwykły WAVEFORM PCM */
		case SOUND_RODZAJ_VAG:
		case SOUND_RODZAJ_AT3_MONO:
		{
			Wav.setUp();
			Wav.write(*this);
			
			break;
		}
		
		/* Ogg Vorbis */
		case SOUND_RODZAJ_AT3_STEREO:
		{
			Wav.setUp();
			if (0 != encodeWithVorbis())
			{
				return;
			}
		}
	}

	/* Opcjonalnie: zamknij plik */
	CloseHandle(Plik2);
	Plik2 = INVALID_HANDLE_VALUE;
}
