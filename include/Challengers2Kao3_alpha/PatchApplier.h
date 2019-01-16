#ifndef H_PATCH_APPLIER
#define H_PATCH_APPLIER

#include "Challengers2Kao3.h"
#include "FileOperator.h"

#define KAO3_FILESIZE 0x00385038
#define KAO3_CRC32_CHECKSUM 0x36FBC9A9
#define CRC32_POLY 0xEDB88320 // "ISO 3309"


////////////////////////////////////////////////////////////////
// Klasa wstrzykuj¹ca ³atki ASM
////////////////////////////////////////////////////////////////

class CPatchApplier : public CFileOperator
{
public:

	/*** Zmienne ***/

	eUnicodeString Sciezka;
	byte* Bufor;

	/*** Funkcje ***/

	CPatchApplier(const wchar_t *);
	~CPatchApplier();

	void aplikujKod(uint32_t, uint32_t, const char*);
	int aplikujLatke(int);
	void aplikujCommonChanges();

	bool testujPlikWykonywalny();
};

#endif
