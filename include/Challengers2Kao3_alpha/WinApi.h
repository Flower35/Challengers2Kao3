#ifndef H_WINAPI_MAIN
#define H_WINAPI_MAIN

#include "Challengers2Kao3.h"

/* LuŸne definicje opcjonalne */

#define OKNO_SZEROKOSC 640
#define OKNO_WYSOKOSC 360
#define BITMAPA1_SZEROKOSC 320
#define BITMAPA1_WYSOKOSC 128
#define KLASA_OKNA_1 L"Challengers2Kao3 - Okno g³ówne"
#define KLASA_OKNA_2 L"Challengers2Kao3 - Okno drugie"
#define KLASA_OKNA_3 L"Challengers2Kao3 - Okno trzecie"
#define KLASA_OKNA_4 L"Challengers2Kao3 - Okno czwarte"

/* Definicje ID dla przycisków */

#define ID_PRZYCISK (0xC9)
#define ID_PRZYCISKI_SWIATY (0xC9 + 5)
#define NUM_LATKI 2

#define OPCJA_ZAAWANSOWANA_DECOMPRESS_SFX 0x00
#define OPCJA_ZAAWANSOWANA_DECOMPILE_GFX 0x01
#define OPCJA_ZAAWANSOWANA_UPGRADABLES 0x02
#define OPCJA_ZAAWANSOWANA_SKIPABLES 0x03
#define OPCJA_ZAAWANSOWANA_DONT_OVERWRITE 0x04
#define NUM_OPCJE_ZAAWANSOWANE 0x05

/* Inne definicje (ikonki i bitmapy) */

#include "Challengers2Kao3_rc.h"

#define GLOS_KAO_PROBLEMY 0
#define GLOS_KAO_ONIE 1
#define GLOS_KAO_SUPER 2
#define GLOS_KAO_ZROBIEWSZYSTKO 3
#define GLOS_KAO_PROSZE 4


////////////////////////////////////////////////////////////////
// Globalna klasa okna:
////////////////////////////////////////////////////////////////

/*static*/ class CWindow {
public:
	static HINSTANCE Instancja;

	static HBITMAP Bitmapa;
	static HFONT Czcionka[2];
	static OPENFILENAME Ofn;
	static wchar_t NazwaPlikuOtwieranego[256];

	static HWND Okna[4];
	static HWND LoadingTekst;
	static HWND EditBoxKatalogi[3];
	static HWND CheckBoxPoziomy[NUM_LEVELS];
	static HWND PrzyciskiSwiaty[NUM_WORLDS];
	static HWND RadioButtonLatki[NUM_LATKI];
	static HWND CheckBoxZaawansowane[NUM_OPCJE_ZAAWANSOWANE];

	CWindow();
	~CWindow();

	static LRESULT CALLBACK procedura(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK procedura_druga(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK procedura_trzecia(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK procedura_czwarta(HWND, UINT, WPARAM, LPARAM);
	static bool utworzOkienka();
	static int petlaProgramu();

	static void zmienTekstOknaLadowania(const wchar_t*);
	static bool czyJestOpcjaZaawansowana(int);
	static bool sprawdzPoprawnoscSciezek(wchar_t*, wchar_t*, eUnicodeString*);

	static void odtworzDzwiek(int);
};


#endif