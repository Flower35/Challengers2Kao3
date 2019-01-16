#ifndef H_SOUND_EXPORTER
#define H_SOUND_EXPORTER

#include "SoundExporter_wav.h"

/* Moje definicje */
#define SOUND_RODZAJ_VAG 0x01
#define SOUND_RODZAJ_AT3_MONO 0x02
#define SOUND_RODZAJ_AT3_STEREO 0x03


////////////////////////////////////////////////////////////////
// Dodatkowe biblioteki statyczne
////////////////////////////////////////////////////////////////

#include <MaiAT3PlusDecoder/dekoder.h>

#pragma comment(lib, "libvorbis")
#include <vorbis/vorbisenc.h>
#pragma comment(lib, "libogg")
#include <ogg/ogg.h>


////////////////////////////////////////////////////////////////
// Klasa eksportera dŸwiêków
////////////////////////////////////////////////////////////////

class CSoundExporter : public CFileOperator
{
public:

	/*** Zmienne ***/

	eUnicodeString Sciezka;
	eUnicodeString ItemName;
	int32_t KoncowyOffset;

	int Rodzaj;

	SWavRiffChunk Wav;

	/*** Funkcje ***/

	CSoundExporter(int, HANDLE, eUnicodeString &, eUnicodeString &, int32_t);
	~CSoundExporter();

	void skanujPlik();
	int odczytajAudio();
	void eksportujAudio();

	int readVagFormat();
	int decodeVagBlocks(int32_t, int32_t &, int32_t &, int32_t &);

	int readAt3Format();

	int encodeWithVorbis();
	void showProgress(int32_t, float);
};

#endif
