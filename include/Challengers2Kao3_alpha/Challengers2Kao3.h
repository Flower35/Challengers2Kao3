#ifndef H_CHALLENGERS2KAO3_MAIN
#define H_CHALLENGERS2KAO3_MAIN

#define TEKST_LAST_UPDATE L"last update: 2019-01-17, 00:52"


////////////////////////////////////////////////////////////////
// BIBLIOTEKI PROGRAMU
////////////////////////////////////////////////////////////////

#include <Windows.h>
#include <cwchar> // swprintf_s
#include <cstdint> // int16_t, int23_t
#include <new> // catch std::bad_alloc
#include <CommCtrl.h> // WC_STATIC, WC_BUTTON, WC_LISTBOX

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment (lib, "Winmm")


////////////////////////////////////////////////////////////////
// Moje dodatkowe klasy
////////////////////////////////////////////////////////////////

#include "eString.h"


////////////////////////////////////////////////////////////////
// Zastrzeżenie:
////////////////////////////////////////////////////////////////

class EKomunikat
{
private:
	eUnicodeString Tekst;
	int Rodzaj;
public:
	EKomunikat(const wchar_t *a, int b = 0);
	void pokazKomunikat(eUnicodeString a);
};

#define KOMUNIKAT_TYTUL L"<Challengers2Kao3>"


////////////////////////////////////////////////////////////////
// Nazwy poziomów z Kao Challengers
////////////////////////////////////////////////////////////////

struct StIniLevel
{
	const wchar_t* pak;
	const wchar_t* name;

	StIniLevel(const wchar_t*, const wchar_t*);
};

struct StIniWorld
{
	const wchar_t* name;
	int num;
	const StIniLevel* levels;

	StIniWorld(const wchar_t*, const int, const StIniLevel*);
};

#define NUM_WORLDS 9
extern const StIniWorld KaoChallengersLevels[NUM_WORLDS];
#define WORLD1_NUM_LEVELS 2
#define WORLD2_NUM_LEVELS 5
#define WORLD3_NUM_LEVELS 5
#define WORLD4_NUM_LEVELS 4
#define WORLD5_NUM_LEVELS 4
#define WORLD6_NUM_LEVELS 4
#define WORLD7_NUM_LEVELS 3
#define WORLD8_NUM_LEVELS 5
#define WORLD9_NUM_LEVELS 15
#define NUM_LEVELS (WORLD1_NUM_LEVELS + WORLD2_NUM_LEVELS + WORLD3_NUM_LEVELS + WORLD4_NUM_LEVELS + WORLD5_NUM_LEVELS + WORLD6_NUM_LEVELS + WORLD7_NUM_LEVELS + WORLD8_NUM_LEVELS + WORLD9_NUM_LEVELS)


////////////////////////////////////////////////////////////////
// Makro do konwersji BIG ENDIAN na LITTLE ENDIAN
////////////////////////////////////////////////////////////////

#define ENDIAN_SWAP(X) ((X >> 24) & 0xFF) | ((X >> 8) & 0xFF00) | ((X << 8) & 0xFF0000) | ((X << 24) & 0xFF000000)


////////////////////////////////////////////////////////////////
// Koniec pliku "KAO23_PORT.h"
////////////////////////////////////////////////////////////////

#endif
