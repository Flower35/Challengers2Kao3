#ifndef H_FILE_OPERATOR
#define H_FILE_OPERATOR

#include "Challengers2Kao3.h"

/*** Testowa struktura ***/

struct StTestowyPlik
{
	int world_id;
	int level_id;
	wchar_t* nazwa;
};


////////////////////////////////////////////////////////////////
// Klasa operująca na plikach (odczyt, zapis, sprawdzanie)
////////////////////////////////////////////////////////////////

class CFileOperator
{
public:

	/*** Zmienne ***/

	HANDLE Plik1;
	HANDLE Plik2;
	bool ShouldClosePlik1;
	bool ShouldClosePlik2;

	/*** Funkcje ***/

	CFileOperator(HANDLE, bool, HANDLE, bool);
	~CFileOperator();

	int przesunPlik(int);
	void czytajPlik(void* gdzie, int ile);
	void zapiszPlik(void* co, int ile);

	bool otworzPlikDoOdczytu(const wchar_t* nazwa);
	bool otworzPlikDoZapisu(const wchar_t* nazwa, int tryb = 0);
	bool tworzPodkatalogi(wchar_t*);
	bool czyPlikIstnieje(const wchar_t*);

	void kopiujWaznePliki(int* pliki_skopiowane, eUnicodeString* katalog);
	bool czyNalezySkopiowacWaznyPlik(const StTestowyPlik &plik);
};

#endif
