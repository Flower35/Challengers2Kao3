#ifndef H_PAK_EXPORTER
#define H_PAK_EXPORTER

#include "Challengers2Kao3.h"
#include "FileOperator.h"

/*** LuŸne definicje ***/
#define ITEM_RODZAJ_ZWYKLY 0x00
#define ITEM_RODZAJ_GRAFICZNY 0x01
#define ITEM_RODZAJ_DZWIEKOWY 0x02


////////////////////////////////////////////////////////////////
// Klasa eksportera plików PAK
////////////////////////////////////////////////////////////////

class CPakExporter : public CFileOperator
{
public:

	/*** Zmienne ***/

	eUnicodeString SciezkaPlikuPak;
	eUnicodeString FolderDocelowy;

	const wchar_t* NazwaPlikuPak;
	eUnicodeString StreamName;

	int Items;
	int Langs;
	char LangNazwy[8][4];
	int Rozmiary[9];

	byte* Alokacja;

	/*** Funkcje ***/

	CPakExporter(eUnicodeString &, const wchar_t*, eUnicodeString &);
	~CPakExporter();

	eUnicodeString getErrorMessageHeader();

	int skanujBlokItems(int, int &);
	bool sprawdzStreamSize(int);
	int odczytajNaglowki();
	int beginExtracting();

	bool czyItemPasujeDoWzorca(eUnicodeString &, const wchar_t*, const wchar_t*, const wchar_t*);
	int eksportujItem(int, eUnicodeString &);
	int eksportujItemSubfunkcja(int, int, eUnicodeString &);
	void opracujKoncowaNazweItema(int32_t &, int32_t &, eUnicodeString &);
	
	bool isItemSkipable(eUnicodeString &);
	bool isItemUpgradable(eUnicodeString &);
};

#endif
