#ifndef H_ATRAC3PLUS_DECODER
#define H_ATRAC3PLUS_DECODER

#ifndef API_ATRAC3PLUS_DECODER
#define API_ATRAC3PLUS_DECODER __declspec(dllexport)
#endif


////////////////////////////////////////////////////////////////
// Biblioteki definiujÄ…ce typy zmiennych
////////////////////////////////////////////////////////////////

#include <cstdint>


////////////////////////////////////////////////////////////////
// "MaiAT3PlusDecoder"
// autor: ryuukazenomai
// website: https://sourceforge.net/projects/maiat3plusdec/
////////////////////////////////////////////////////////////////

class MaiAT3PlusFrameDecoder;


////////////////////////////////////////////////////////////////
// "LibAtrac3plus"
////////////////////////////////////////////////////////////////

extern "C"
/*static*/ class LibAtrac3plus
{
public:

	API_ATRAC3PLUS_DECODER static MaiAT3PlusFrameDecoder* Dekoder;

	API_ATRAC3PLUS_DECODER static int32_t przygotujBiblioteke();
	API_ATRAC3PLUS_DECODER static void zamknijBiblioteke();

	API_ATRAC3PLUS_DECODER static int32_t dekodujRamke(uint8_t** output, uint8_t** input, const int32_t rozmiar_bloku, const int32_t przewidywane_kanaly);
};

#endif
