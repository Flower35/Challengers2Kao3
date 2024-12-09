#include <Challengers2Kao3_alpha/FileOperator.h>
#include <Challengers2Kao3_alpha/WinApi.h>


////////////////////////////////////////////////////////////////
// Konstruktor klasy operacji na plikach
////////////////////////////////////////////////////////////////
CFileOperator::CFileOperator(HANDLE input, bool prawda1, HANDLE output, bool prawda2)
{
	Plik1 = input;
	ShouldClosePlik1 = prawda1;
	Plik2 = output;
	ShouldClosePlik2 = prawda2;
}


////////////////////////////////////////////////////////////////
// Dekonstruktor klasy opracji na plikach
////////////////////////////////////////////////////////////////
CFileOperator::~CFileOperator()
{
	if (ShouldClosePlik1 && (INVALID_HANDLE_VALUE != Plik1))
	{
		CloseHandle(Plik1);
		Plik1 = INVALID_HANDLE_VALUE;
	}

	if (ShouldClosePlik2 && (INVALID_HANDLE_VALUE != Plik2))
	{
		CloseHandle(Plik2);
		Plik2 = INVALID_HANDLE_VALUE;
	}
}


////////////////////////////////////////////////////////////////
// Odczytaj bajty z pierwszego pliku
////////////////////////////////////////////////////////////////
void CFileOperator::czytajPlik(void* gdzie, int ile)
{
	DWORD test;
	ReadFile(Plik1, gdzie, ile, &test, 0);
	if (test != ile)
	{
		wchar_t test1[64];
		swprintf_s(test1, 64, L"Error reading data from file! (%d bytes)", ile);
		throw EKomunikat(test1);
	}
}


////////////////////////////////////////////////////////////////
// Zapisz bajty do drugiego pliku
////////////////////////////////////////////////////////////////
void CFileOperator::zapiszPlik(void* co, int ile)
{
	DWORD test;
	WriteFile(Plik2, co, ile, &test, 0);
	if (test != ile)
	{
		wchar_t test1[64];
		swprintf_s(test1, 64, L"Error writing data to file! (%d bytes)", ile);
		throw EKomunikat(test1);
	}
}


////////////////////////////////////////////////////////////////
// Przesuń pozycję w pierwszym pliku
////////////////////////////////////////////////////////////////
int CFileOperator::przesunPlik(int ile)
{
	return SetFilePointer(Plik1, ile, 0, FILE_CURRENT);
}


////////////////////////////////////////////////////////////////
// Otwórz nowy plik do odczytywania
////////////////////////////////////////////////////////////////
bool CFileOperator::otworzPlikDoOdczytu(const wchar_t* nazwa)
{
	Plik1 = CreateFile(nazwa, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	if (INVALID_HANDLE_VALUE == Plik1)
	{
		wchar_t test1[256];
		swprintf_s(test1, 256, L"Could not open file \"%s\"!", nazwa);
		throw EKomunikat(test1);
		return false;
	}

	return true;
}


////////////////////////////////////////////////////////////////
// Utwórz nowy plik do zapisywania
////////////////////////////////////////////////////////////////
bool CFileOperator::otworzPlikDoZapisu(const wchar_t* nazwa, int tryb)
{
	DWORD dwDesiredAccess;
	DWORD dwCreationDisposition;

	/* Ustal tryb otwierania */
	switch (tryb)
	{
		case 1:
		{
			dwDesiredAccess = (GENERIC_WRITE | GENERIC_READ);
			dwCreationDisposition = OPEN_EXISTING;
			break;
		}
		default:
		{
			dwDesiredAccess = GENERIC_WRITE;
			dwCreationDisposition = CREATE_ALWAYS;
		}
	}

	Plik2 = CreateFile(nazwa, dwDesiredAccess, 0, 0, dwCreationDisposition, 0, 0);
	if (INVALID_HANDLE_VALUE == Plik2)
	{
		wchar_t test1[256];
		swprintf_s(test1, 256, L"Could not open file \"%s\"!", nazwa);
		throw EKomunikat(test1);
		return false;
	}

	return true;
}


////////////////////////////////////////////////////////////////
// Twórz kolejne podkatalogi przed utworzeniem pliku
////////////////////////////////////////////////////////////////
bool CFileOperator::tworzPodkatalogi(wchar_t* pelna_sciezka)
{
	wchar_t last_slash;

	for (wchar_t* p = pelna_sciezka; (0x00 != (*p)); p++)
	{
		if ((*p == '/') || (*p == '\\'))
		{
			last_slash = *p;
			*p = 0x00;

			if (0 == CreateDirectory(pelna_sciezka, 0))
			{
				if (ERROR_ALREADY_EXISTS != GetLastError())
				{
					wchar_t test1[256];
					swprintf_s(test1, 256, L"Could not create path \"%s\"!", pelna_sciezka);
					throw EKomunikat(test1);
					return false;
				}
			}

			*p = last_slash;
		}
	}

	return true;
}


////////////////////////////////////////////////////////////////
// Czy powinniśmy eksportować dany plik?
////////////////////////////////////////////////////////////////
bool CFileOperator::czyPlikIstnieje(const wchar_t* pelna_sciezka)
{
	return (INVALID_FILE_ATTRIBUTES != GetFileAttributes(pelna_sciezka));
}


////////////////////////////////////////////////////////////////
// Czy należy skopiować ważny plik silnika?
// (np. poziom "Lost Village", albo poziomy MultiPlayer)
////////////////////////////////////////////////////////////////
bool CFileOperator::czyNalezySkopiowacWaznyPlik(const StTestowyPlik &plik)
{
	int l;
	int w;
	int checkbox_offset = 0;

	if ((plik.world_id >= 0) && (plik.world_id < NUM_WORLDS))
	{
		for (w = 0; w < plik.world_id; w++)
		{
			checkbox_offset += KaoChallengersLevels[w].num;
		}

		for (l = 0; l < KaoChallengersLevels[plik.world_id].num; l++)
		{
			if (BST_CHECKED == SendMessage(CWindow::CheckBoxPoziomy[checkbox_offset + l], BM_GETCHECK, 0, 0))
			{
				if ((plik.level_id < 0) || (plik.level_id == l))
				{
					return true;
				}
			}
		}
	}
	else
	{
		return true;
	}

	return false;
}


////////////////////////////////////////////////////////////////
// Kopiuj ważne pliki silnika
////////////////////////////////////////////////////////////////
void CFileOperator::kopiujWaznePliki(int* pliki_skopiowane, eUnicodeString* katalog)
{
	int i;
	int p;
	eUnicodeString sciezka[2];
	wchar_t* tekst[2];

	/* Ustaw dane statyczne */

	const eUnicodeString engine_katalog(L"./kao_pliki/new_engine/");

	const wchar_t* nazwy_loading[2] = 
	{
		L"Copying original files...",
		L"Copying new files..."
	};

	const StTestowyPlik pliki_bezposrednie[] =
	{
		{(-1), (-1), L"elephant2.ini"},
		{(-1), (-1), L"levels.ini"},
		{(-1), (-1), L"items.ini"}
	};

	const StTestowyPlik pliki_posrednie[] =
	{
		{(-1), (-1), L"user.ini"},
		{(-1), (-1), L"misc/localize/lang_psp.ini"},
		{(-1), (-1), L"menu/ui/artefakty/winieta.tga"},
		{(-1), (-1), L"music/empty.ogg"},
		{5, 0, L"levels/26_ruins/scripts/katapulta_1sza/EnemyA.def"},
		{5, 0, L"levels/26_ruins/scripts/katapulta_1sza/EnemyB.def"},
		{5, 0, L"levels/26_ruins/scripts/katapulta_1sza/EnemyC.def"},
		{5, 0, L"levels/26_ruins/scripts/katapulta_1sza/EnemyD.def"},
		{8, (-1), L"characters/mp_kao/textures/skrzydlo.tga"},
		{8, (-1), L"characters/mp_hunter/textures/skrzydlo.tga"},
		{8, (-1), L"characters/mp_parrot/textures/skrzydlo.tga"},
		{8, (-1), L"characters/mp_pirat/textures/skrzydlo.tga"},
		{8, 0, L"build/pc/mp_start.ar"}
	};

	const int liczby_plikow[2] =
	{
		sizeof(pliki_bezposrednie) / sizeof(StTestowyPlik),
		sizeof(pliki_posrednie) / sizeof(StTestowyPlik)
	};

	const StTestowyPlik* nazwy_plikow[2] = 
	{
		pliki_bezposrednie,
		pliki_posrednie
	};

	/* Rozpocznij kopiowanie */

	for (p = 0; p < 2; p++)
	{
		CWindow::zmienTekstOknaLadowania(nazwy_loading[p]);

		pliki_skopiowane[p] = 0;
		pliki_skopiowane[2 + p] = 0;

		for (i = 0; i < liczby_plikow[p]; i++)
		{
			if (czyNalezySkopiowacWaznyPlik(nazwy_plikow[p][i]))
			{
				pliki_skopiowane[2 + p] += 1;

				sciezka[0] = (eUnicodeString)((0 == p) ? katalog[0] : engine_katalog)
					+ nazwy_plikow[p][i].nazwa;
				tekst[0] = sciezka[0].getText();

				sciezka[1] = katalog[1] + nazwy_plikow[p][i].nazwa;
				tekst[1] = sciezka[1].getText();

				if (czyPlikIstnieje(tekst[0]))
				{
					if (tworzPodkatalogi(tekst[1]))
					{
						if (0 != CopyFile(tekst[0], tekst[1], FALSE))
						{
							pliki_skopiowane[p] += 1;
						}
					}
				}
			}
		}
	}

	/* Zwróć wyniki */

	CWindow::zmienTekstOknaLadowania(L"");
}
