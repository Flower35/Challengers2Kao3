#include "dekoder.h"


////////////////////////////////////////////////////////////////
// Dodatkowe biblioteki
////////////////////////////////////////////////////////////////

#include <new> // `std::bad_alloc`
#include <cstring> // `std::memcpy`


////////////////////////////////////////////////////////////////
// "MaiAT3PlusDecoder"
// autor: ryuukazenomai
// website: https://sourceforge.net/projects/maiat3plusdec/
////////////////////////////////////////////////////////////////

#include "MaiAT3PlusFrameDecoder.h"


////////////////////////////////////////////////////////////////
// "LibAtrac3plus"
// Eksportowana klasa
////////////////////////////////////////////////////////////////

/* Symbol zewnÄ™trzny */
MaiAT3PlusFrameDecoder* LibAtrac3plus::Dekoder = nullptr;

/* Konstruktor */
int32_t LibAtrac3plus::przygotujBiblioteke()
{
	if (nullptr != Dekoder)
	{
		return 1;
	}

	try
	{
		Dekoder = new MaiAT3PlusFrameDecoder;
	}
	catch (std::bad_alloc)
	{
		Dekoder = nullptr;
		return 1;
	}

	return 0;
}

/* Deksontruktor */
void LibAtrac3plus::zamknijBiblioteke()
{
	if (nullptr != Dekoder)
	{
		delete(Dekoder);
		Dekoder = nullptr;
	}
}

/* Publiczna funkcja */
int32_t LibAtrac3plus::dekodujRamke(uint8_t** output, uint8_t** input, const int32_t rozmiar_bloku, const int32_t przewidywane_kanaly)
{
	int16_t* zwrotny_bufor;
	int32_t zwrotna_liczba_kanalow;
	int32_t przeskok;

	/* Czy nie przekazano przypadkiem wadilwych danych? */
	if ((nullptr == (*output)) || (nullptr == (*input)) || (nullptr == Dekoder))
	{
		return 1;
	}

	/* (1 z 2) Czy liczba kanaĹ‚Ăłw siÄ™ zgadza? */
	switch (przewidywane_kanaly)
	{
		case 0x01: /* MONO */
		{
			if (0xC0 != rozmiar_bloku)
			{
				return 1;
			}

			break;
		}

		case 0x02: /* STEREO */
		{
			if (0x0178 != rozmiar_bloku)
			{
				return 1;
			}

			break;
		}

		default:
		{
			return 1;
		}
	}

	/* Dekoduj jednÄ… ramkÄ™ */
	if (0 != Dekoder->decodeFrame((char*)(*input), rozmiar_bloku, &zwrotna_liczba_kanalow, &zwrotny_bufor))
	{
		return 1;
	}

	/* (2 z 2) Czy liczba kanaĹ‚Ăłw siÄ™ zgadza? */
	if (zwrotna_liczba_kanalow != przewidywane_kanaly)
	{
		return 1;
	}

	/* Przekopiuj nowe dane */
	przeskok = (przewidywane_kanaly * sizeof(int16_t) * 0x0800);
	std::memcpy((*output), zwrotny_bufor, przeskok);

	/* Na koniec zaaktualizuj wkaĹşniki */
	(*output) += przeskok;
	(*input) += rozmiar_bloku;

	return 0;
}
