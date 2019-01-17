#include <Challengers2Kao3_alpha/PakExporter.h>
#include <Challengers2Kao3_alpha/WinApi.h>


////////////////////////////////////////////////////////////////
// Konstruktor klasy
////////////////////////////////////////////////////////////////
CPakExporter::CPakExporter(eUnicodeString &in_folder, const wchar_t* pak_name, eUnicodeString &out_folder)
: CFileOperator(INVALID_HANDLE_VALUE, true, INVALID_HANDLE_VALUE, true)
{
	Items = 0;
	Langs = 0;

	Alokacja = nullptr;

	SciezkaPlikuPak = in_folder + pak_name + L".pak";
	FolderDocelowy = out_folder;
	NazwaPlikuPak = pak_name;
}


////////////////////////////////////////////////////////////////
// Dekonstruktor klasy
////////////////////////////////////////////////////////////////
CPakExporter::~CPakExporter()
{
	if (nullptr != Alokacja)
	{
		delete[](Alokacja);
	}
}


////////////////////////////////////////////////////////////////
// Zmieñ wzorcow¹ nazwê pliku
////////////////////////////////////////////////////////////////
bool CPakExporter::czyItemPasujeDoWzorca(eUnicodeString &item_name, const wchar_t* ext, const wchar_t* old_prefix, const wchar_t* new_prefix)
{
	int x = item_name.getLength();
	int y = _getCharArrayLength(old_prefix);

	if (y >= x)
	{
		return false;
	}

	if (!item_name.hasExtension(ext))
	{
		return false;
	}

	if (!item_name.compare(old_prefix, 0, y, false))
	{
		return false;
	}

	item_name = eUnicodeString(new_prefix) + item_name.getFilename();
	return true;
}


////////////////////////////////////////////////////////////////
// Szybkie testowanie StreamSize podczas eksportu
////////////////////////////////////////////////////////////////
bool CPakExporter::sprawdzStreamSize(int x)
{
	int y = Rozmiary[0];
	for (int i = 1; i < (Langs+1); i++)
	{
		y += Rozmiary[i];
	}
	return ((x != Rozmiary[0]) && (x != y));
}


////////////////////////////////////////////////////////////////
// Generuje pocz¹tek komunikatu o b³êdzie
////////////////////////////////////////////////////////////////
eUnicodeString CPakExporter::getErrorMessageHeader()
{
	return eUnicodeString(L"[") + NazwaPlikuPak + L"]\r\n\r\n";
}


////////////////////////////////////////////////////////////////
// [OBIEKT_PAK] Sprawdzanie nag³ówków podczas exportu
////////////////////////////////////////////////////////////////
int CPakExporter::odczytajNaglowki()
{
	int32_t x;
	int32_t pak_size;
	char test[48];

	/*** Spróbuj otworzyæ plik wejœciowy ***/
	if (!otworzPlikDoOdczytu(SciezkaPlikuPak.getText()))
	{
		return 1;
	}

	pak_size = GetFileSize(Plik1, 0);

	/*** SprawdŸ nag³ówek "TATE" ***/
	czytajPlik(&x, 0x04);
	if ((*(int32_t*)"TATE") != x)
	{
		throw EKomunikat(L"Incorrect PAK header (expected: \"TATE\").");
		return 1;
	}

	/*** Odczytaj podstawowe informacje ***/
	czytajPlik(&(Rozmiary[0]), 0x04);
	czytajPlik(&Items, 0x04);
	czytajPlik(&Langs, 0x04);

	if (Langs > 8)
	{
		throw EKomunikat(L"Too many langs, max 8.");
		return 1;
	}

	/*** Oczytaj "stream name" ***/
	czytajPlik(test, 48);
	test[48 - 1] = 0;
	_convertString(StreamName, test);
	if (!StreamName.compare(NazwaPlikuPak))
	{
		/* Tylko warning, nie zamyka funkcji */
		EKomunikat(
			L" WARNING: \"StreamName\" does not match file name!\r\n\r\n",
			0x01).pokazKomunikat(getErrorMessageHeader());
	}

	/*** Odczytaj jêzyki ***/
	for (int i = 0; i < Langs; i++)
	{
		czytajPlik(LangNazwy[i], 0x04);
		czytajPlik(&(Rozmiary[1 + i]), 0x04);
	}
	for (int i = Langs; i < 8; i++)
	{
		LangNazwy[i][0] = 0x00;
		Rozmiary[1 + i] = 0;
	}

	/*** SprawdŸ stream size ***/
	if (sprawdzStreamSize(pak_size))
	{
		throw EKomunikat(L"Incorrect PAK file size.");
		return 1;
	}

	return 0;
}


////////////////////////////////////////////////////////////////
// Zapisywanie nowego pliku z paczki
////////////////////////////////////////////////////////////////
int CPakExporter::eksportujItem(int rozmiar, eUnicodeString &nazwa)
{
	int32_t current_offset;
	int32_t end_offset = (przesunPlik(0) + rozmiar);

	int32_t test = eksportujItemSubfunkcja(rozmiar, end_offset, nazwa);

	if (nullptr != Alokacja)
	{
		delete[](Alokacja);
		Alokacja = nullptr;
	}

	/* ZAWSZE: Odczytywanie mog³o skoñczyæ siê na wczeœniejszym offsecie */
	/* (plik ju¿ istnieje; ostatnie 4 bajty eBitmap; œmieciowe dane VAGp) */
	current_offset = przesunPlik(0);
	if (current_offset < end_offset)
	{
		przesunPlik(end_offset - current_offset);
	}
	else if (current_offset > end_offset)
	{
		throw EKomunikat(L"File pointer went beyond the scope of item size...");
		return 1;
	}

	return test;
}


////////////////////////////////////////////////////////////////
// Eksportowanie itemów z sektora
////////////////////////////////////////////////////////////////
int CPakExporter::skanujBlokItems(int rozmiar, int &id)
{
	int32_t x;
	int32_t y;
	char item_name[0x70];
	eUnicodeString item_unicode;
	int item_size;

	while ((x = przesunPlik(0)) < rozmiar)
	{
		if (0 != (y = (x % 128)))
		{
			przesunPlik(128 - y);
		}
		if (przesunPlik(0) >= rozmiar)
		{
			return 0;
		}

		/* Odczytaj item magic */
		czytajPlik(&x, 4); 
		if ((*(int32_t*)"item") != x)
		{
			if ((Langs <= 0) && (id != Items))
			{
				wchar_t test1[64];
				swprintf_s(test1, 64, L"Incorrect item magic at offset 0x%08X.", przesunPlik(0));
				throw EKomunikat(test1);
				return 1;
			}
			/* Je¿eli istniej¹ jêzyki, to sektor TATE koñczy siê na wczeœniejszym ID! */
			else return 0;
		}
		else
		{
			/* Odczytaj rozmiar pliku oraz ID */
			czytajPlik(&item_size, 4);
			czytajPlik(&x, 4);
			
			/* Odczytaj nazwê pliku */
			przesunPlik(4);
			czytajPlik(item_name, 0x70);
			item_name[0x70 -1] = 0x00;
			/* Usuñ spacje na koñcu nazwy (np. "spoon.def ") */
			for (y = (0x70 - 1); y >= 0; y--)
			{
				if (0x20 == item_name[y])
				{
					item_name[y] = 0x00;
				}
				else if (0x00 != item_name[y])
				{
					break;
				}
			}
			_convertString(item_unicode, item_name);

			CWindow::zmienTekstOknaLadowania((eUnicodeString(L"[") + NazwaPlikuPak + L"]\r\n" + item_unicode).getText());

			if (x != id)
			{
				wchar_t test1[128];
				swprintf_s(test1, 128, L"Item ID is incorrect! (stream: %d, expected: %d, offset: 0x%08X)", x, id, przesunPlik(0));
				throw EKomunikat(test1);
				return 1;
			}

			/* Wyeksportuj plik */
			if (0 != eksportujItem(item_size, item_unicode))
			{
				return 1;
			}

			/* Przesuñ licznik identyfikatorów */
			id++;
		}
	}
	return 0;
}


////////////////////////////////////////////////////////////////
// Wypakuj ca³e archiwum
////////////////////////////////////////////////////////////////
int CPakExporter::beginExtracting()
{
	int32_t max_block_size;
	int32_t last_item_id;
	int32_t id_lang;

	try
	{
		/*** Przygotuj i sprawdŸ archiwum przed eksportem ***/
		if (0 != odczytajNaglowki())
		{
			return 1;
		}

		/*** Rozpocznij wypakowywanie plików z g³ównego sektora ***/
		SetFilePointer(Plik1, 0x80, 0, FILE_BEGIN);
		max_block_size = Rozmiary[0];
		if (0 != skanujBlokItems(max_block_size, (last_item_id = 0)))
		{
			return 1;
		}

		/*** Kontynuuj wypakowywanie z sektorów jêzykowych ***/
		for (int i = 0; i < Langs; i++)
		{
			SetFilePointer(Plik1, max_block_size, 0, FILE_BEGIN);
			max_block_size += Rozmiary[i+1];

			if (0 != skanujBlokItems(max_block_size, (id_lang = last_item_id)))
			{
				return 1;
			}
		}

	}
	catch (EKomunikat msg)
	{
		msg.pokazKomunikat(getErrorMessageHeader());
		return 1;
	}

	return 0;
}
