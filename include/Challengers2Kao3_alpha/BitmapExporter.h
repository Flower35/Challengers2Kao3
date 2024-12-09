#ifndef H_BITMAP_EXPORTER
#define H_BITMAP_EXPORTER

#include "Challengers2Kao3.h"
#include "FileOperator.h"


/* Definicje rodzajów Bitmap z KAO2 */
#define RGBA8 0x00
#define RGB8 0x01
#define PAL8_RGBA8 0x02
#define PAL8_RGBX8 0x03
#define RGBX8 0x04
#define PAL8_RGBA8_IDTEX8 0x05
#define PAL8_RGBX8_IDTEX8 0x06
#define RGBA8_SWIZZLED 0x2C
#define RGBX8_SWIZZLED 0x63
#define DXT1 0x4E


////////////////////////////////////////////////////////////////
// Klasa eksportera tekstur
////////////////////////////////////////////////////////////////

class CBitmapExporter : public CFileOperator {
public:

	/*** Zmienne ***/

	eUnicodeString Folder;
	eUnicodeString ItemName;
	eUnicodeString PelnaSciezka;

	int Wersja;

	byte* Piksele1;
	byte* Piksele2;
	uint32_t Paleta[256];

	int Rodzaj;
	int SizeX;
	int SizeY;

	eString BitmapName;
	bool BmpExt;

	/*** Funkcje ***/

	CBitmapExporter(HANDLE, eUnicodeString &, eUnicodeString &);
	~CBitmapExporter();

	bool wykonajZadanie(int);
	bool przygotujTeksture();
	bool przygotujNazwe();
	void eksportujTeksture();

	float getBytesPerPixel();
	bool isUsingPalette();
	const wchar_t* getRodzajAsName();
	int getBytesPerPixelNew();
	void DecompressBlockBC1(unsigned int x, unsigned int y, unsigned int stride, const unsigned char* blockStorage, unsigned char* image);
};

#endif
