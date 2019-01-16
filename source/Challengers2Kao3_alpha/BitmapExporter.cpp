#include <Challengers2Kao3_alpha/BitmapExporter.h>


////////////////////////////////////////////////////////////////
// Konstruktor klasy skanera bitmap
////////////////////////////////////////////////////////////////
CBitmapExporter::CBitmapExporter(HANDLE archiwum, eUnicodeString &folder, eUnicodeString &simple_name)
: CFileOperator(archiwum, false, INVALID_HANDLE_VALUE, true)
{
	Folder = folder;
	ItemName = simple_name;
	Wersja = 0;
	Piksele1 = nullptr;
	Piksele2 = nullptr;
	Rodzaj = 0;
	SizeX = 0;
	SizeY = 0;
}


////////////////////////////////////////////////////////////////
// Dekonstruktor klasy skanera bitmap
////////////////////////////////////////////////////////////////
CBitmapExporter::~CBitmapExporter()
{
	if (nullptr != Piksele1)
	{
		delete[](Piksele1);
	}

	if (nullptr != Piksele2)
	{
		delete[](Piksele2);
	}
}


////////////////////////////////////////////////////////////////
// Przygotuj siê do eksportowania bitmapy
////////////////////////////////////////////////////////////////
bool CBitmapExporter::wykonajZadanie(int jakie)
{
	int x;
	int y;

	try
	{
		if (0 == jakie)
		{
			/* SprawdŸ nag³ówek archiwum */
			czytajPlik(&x, 4);
			if ((*(int32_t*)"tate") != x)
			{
				throw EKomunikat(L"Invalid archive magic. Expected \"tate\".");
				return false;
			}

			/* SprawdŸ wersjê archiwum */
			czytajPlik(&Wersja, 4);
			if ((Wersja < 0x8B) || (Wersja > 0x90))
			{
				wchar_t test1[128];
				swprintf_s(test1, 128, L"Invalid archive version.\nMin.: %d, Max.: %d, File: %d.", 0x6A, 0x90, Wersja);
				throw EKomunikat(test1);
				return false;
			}

			// * Pomiñ liczbê obiektów
			przesunPlik(0x04);

			// * SprawdŸ, czy na pocz¹tku jest gad¿et "eBitmap"
			czytajPlik(&x, 4);
			czytajPlik(&y, 4);
			if ((0 != x) || (0x1001 != y))
			{
				throw EKomunikat(L"Invalid object type in archive. Expected \"eBitmap\".");
				return false;
			}
		
			if (przygotujTeksture())
			{
				if (!przygotujNazwe())
				{
					return false;
				}
			}
			else return false;
		}
		else
		{
			PelnaSciezka = Folder + PelnaSciezka;
			eksportujTeksture();
		}
	}
	catch (EKomunikat msg)
	{
		msg.pokazKomunikat(eUnicodeString(L"[Module: Bitmap Exporter]\r\n") + ItemName + L"\r\n\r\n");
	}

	return true;
}



////////////////////////////////////////////////////////////////
// Rozmiar jednego piksela w bitmapie.
// "kao2.00470AA0"
////////////////////////////////////////////////////////////////
float CBitmapExporter::getBytesPerPixel()
{
	switch (Rodzaj)
	{
		case RGBA8:
		case RGBX8:
		case RGBA8_SWIZZLED:
		case RGBX8_SWIZZLED:
		{
			return 0x04;
		}
		case RGB8:
		{
			return 0x03;
		}
		case PAL8_RGBA8:
		case PAL8_RGBX8:
		case PAL8_RGBA8_IDTEX8:
		case PAL8_RGBX8_IDTEX8:
		{
			return 0x01;
		}
		case DXT1:
		{
			return 0.5;
		}
		default:
		{
			return 0;
		}
	};
};


////////////////////////////////////////////////////////////////
// Czy bitmapa wymaga alokowania pamiêci na Paletê?
// "kao2.00470F40"
////////////////////////////////////////////////////////////////
bool CBitmapExporter::isUsingPalette()
{
	switch (Rodzaj)
	{
		case PAL8_RGBA8:
		case PAL8_RGBX8:
		case PAL8_RGBA8_IDTEX8:
		case PAL8_RGBX8_IDTEX8:
		{
			return true;
		}
		default:
		{
			return false;
		}
	};
};


////////////////////////////////////////////////////////////////
// Zwróæ pe³n¹ nazwê rodzaju eBitmapy
////////////////////////////////////////////////////////////////
const wchar_t* CBitmapExporter::getRodzajAsName()
{
	switch (Rodzaj)
	{
		case RGBA8:
		{
			return L"\"RGBA8\"";
		}
		case RGB8:
		{
			return L"\"RGB8\"";
		}
		case PAL8_RGBA8:
		{
			return L"\"PAL8_RGBA8\"";
		}
		case PAL8_RGBX8:
		{
			return L"\"PAL8_RGBX8\"";
		}
		case RGBX8:
		{
			return L"\"RGBX8\"";
		}
		case PAL8_RGBA8_IDTEX8:
		{
			return L"\"PAL8_RGBA8_IDTEX8\"";
		}
		case PAL8_RGBX8_IDTEX8:
		{
			return L"\"PAL8_RGBX8_IDTEX8\"";
		}
		case RGBA8_SWIZZLED:
		{
			return L"\"RGBA8_SWIZZLED\"";
		}
		case RGBX8_SWIZZLED:
		{
			return L"\"RGBX8_SWIZZLED\"";
		}
		case DXT1:
		{
			return L"\"DXT1\"";
		}
		default:
		{
			return L"unkown";
		}
	}
}


////////////////////////////////////////////////////////////////
// Rozmiar jednego piksela w nowym pliku
////////////////////////////////////////////////////////////////
int CBitmapExporter::getBytesPerPixelNew()
{
	switch (Rodzaj)
	{
		case RGBA8:
		{
			return BmpExt ? 3 : 4; 
		}
		case PAL8_RGBA8:
		{
			return BmpExt ? 1 : 4;
		}
		case RGB8:
		case RGBX8:
		{
			return 3;
		}
		case DXT1:
		{
			return 4;
		}
		default:
		{
			return 1;
		}
	}
}


////////////////////////////////////////////////////////////////
// Odczytywanie danych o teksturze
////////////////////////////////////////////////////////////////
bool CBitmapExporter::przygotujTeksture()
{
	int32_t x;
	int32_t y;
	bool zgodnosc = true;

	/* Odczytaj Szerokoœæ i Wysokoœæ Bitmapy */
	czytajPlik(&x, 0x04);
	czytajPlik(&y, 0x04);

	if (Wersja < 0x6B)
	{
		SizeX = x;
		SizeY = y;
	}
	else
	{
		czytajPlik(&SizeX, 0x04);
		czytajPlik(&SizeY, 0x04);

		if ((x != SizeX) || (y != SizeY))
		{
			zgodnosc = false;
		}
	}

	if ((!zgodnosc) || (0 != (SizeX % 4)) || (0 != (SizeY % 4)))
	{
		wchar_t test1[128];
		swprintf_s(test1, 128, L"Bitmap dimensions are not valid.\nX1=%d, X2=%d, Y1=%d, Y2=%d.", x, SizeX, y, SizeY);
		throw EKomunikat(test1);
		return false;
	}

	/* Odczytaj rodzaj Bitmapy */
	czytajPlik(&Rodzaj, 0x04);

	/* SprawdŸ, czy rodzaj jest obs³ugiwany */
	switch (Rodzaj)
	{
		case DXT1:
		case RGBA8:
		case PAL8_RGBA8:
		{
			break;
		}
		default:
		{
			wchar_t test[64];
			swprintf_s(test, 64, L"Unsupported \"eBitmap\" type 0x%04X (%s)", Rodzaj, getRodzajAsName());
			throw EKomunikat(test);
			return false;
		}
	}

	/* Oblicz wymiar do alokacji */
	x = int(getBytesPerPixel() * SizeX * SizeY);

	try
	{
		Piksele1 = new byte[x];
	}
	catch (std::bad_alloc)
	{
		throw EKomunikat(L"Could not allocate memory for input pixels.");
		return false;
	}

	/* Odczytaj bajty dla obrazka */
	czytajPlik(Piksele1, x);

	/* Odwróæ kolory palety, jeœli istnieje. Odwróæ RGBA->BGRA */
	if (isUsingPalette())
	{
		czytajPlik(Paleta, 0x0400);
	
		for (int i=0; i<256; i++)
		{
			Paleta[i] = (Paleta[i] & 0xFF00FF00) | ((Paleta[i] >>16) &0xFF) | ((Paleta[i] &0xFF) <<16);
		}
	}

	/* Serializuj nazwê pliku z Bitmap¹ */
	czytajPlik(&x, 0x04);
	if (0 == x)
	{
		czytajPlik(&x, 0x04);
		if ((x > 0) && (x < 128))
		{
			char test[128];
			czytajPlik(test, sizeof(char) * x);
			test[x] = 0x00;
			BitmapName = test;
		}
		else
		{
			throw EKomunikat(L"Incorrect bitmap name length!");
			return false;
		}
	}

	return true;
}


////////////////////////////////////////////////////////////////
// Przygotuj koñcow¹ nazwê pliku
////////////////////////////////////////////////////////////////
bool CBitmapExporter::przygotujNazwe()
{
	int x;

	/* Ustal format pliku wyjœciowego */
	switch (Rodzaj)
	{
		case RGBA8:
		case PAL8_RGBA8:
		case DXT1:
		{
			BmpExt = false;
			break;
		}
		default:
		{
			BmpExt = true;
		}
	}

	/* SprawdŸ, czy rzeczywiste formaty s¹ zgodne */
	if (BitmapName.getLength() > 4)
	{
		int prawdziwy_format = 0;

		if (BitmapName.hasExtension("bmp"))
		{
			prawdziwy_format = 1;
		}
		else if (BitmapName.hasExtension("tga"))
		{
			prawdziwy_format = 2;
		}

		if (0 == prawdziwy_format)
		{
			wchar_t test1[256];
			_convertString(PelnaSciezka, BitmapName);
			swprintf_s(test1, 256, L"(%s)\r\nIncorrect \"eBitmap\" file name extension.\r\nOnly BMP and TGA are valid!",
				PelnaSciezka.getText());
			PelnaSciezka = L"";
			throw EKomunikat(test1);
			return false;
		}
		else if (prawdziwy_format != (BmpExt ? 1 : 2))
		{
			switch (Rodzaj)
			{
				case RGBA8:
				case PAL8_RGBA8:
				{
					BmpExt = (1 == prawdziwy_format);
					break;
				}

				default:
				{
					wchar_t test1[256];
					swprintf_s(test1, 256, L"Unsupported \"eBitmap\" configuration...\r\nType: 0x%04X (%s)",
						Rodzaj,
						getRodzajAsName());
					PelnaSciezka = L"";
					throw EKomunikat(test1);
					return false;
				}
			}
		}
	}

	/* Ustal nazwê pliku wyjœciowego */
	if (BitmapName.getLength() <= 0)
	{
		return false;
	}
	else
	{
		x = (BitmapName.getLength() - 4);

		/* Usuñ rozszerzenie, jeœli istnieje */
		if (x >= 1)
		{
			if ('.' == BitmapName.getText()[x])
			{
				BitmapName = BitmapName.getSubstring(0, x);
			}
		}

		/* Pamiêtaj o ".." na pocz¹tku plików w Challengers! */
		if (BitmapName.getLength() > 3)
		{
			if (BitmapName.compare("../", 0, 3))
			{
				BitmapName = BitmapName.getSubstring(3, 0);
			}
		}

		/* Specjalne sprawdzanie, czy s¹ to pliki z "menu/interfejs3" */
		const char* specjalne_nazwy[][2] =
		{
			{"menu/interfejs3/medal", "menu/ui/medal"},
			{"menu/interfejs3/bumerang", "menu/ui/bumerang"},
			{"menu/interfejs3/krysztaly", "menu/ui/artefact_bkg"},
			{"menu/interfejs3/skarb", "menu/ui/skarb"},
			{"menu/interfejs3/gwiazdka", "menu/ui/gwiazdka"},
			{"menu/interfejs3/zegar", "menu/ui/zegar"}
		};
		const int specjalne_nazwy_num = sizeof(specjalne_nazwy) / (2 * sizeof(char*));
		for (int i = 0; i < specjalne_nazwy_num; i++)
		{
			if (BitmapName.compare(specjalne_nazwy[i][0]))
			{
				BitmapName = specjalne_nazwy[i][1];
				break;
			}
		}

		/* Utwórz koñcow¹ œcie¿kê */
		_convertString(PelnaSciezka, BitmapName);
		PelnaSciezka = PelnaSciezka
			+ (BmpExt ? L".bmp" : L".tga");
	}

	return true;
}

////////////////////////////////////////////////////////////////
// Eksport tekstury
////////////////////////////////////////////////////////////////
void CBitmapExporter::eksportujTeksture()
{
	char header[0x36];
	int32_t x;
	int32_t y;
	
	wchar_t* pelna_sciezka_text = PelnaSciezka.getText();

	/* Pomiñ plik, je¿eli ju¿ istnieje :) */
	if (czyPlikIstnieje(pelna_sciezka_text))
	{
		return;
	}

	/* Tworzenie podkatalogów */
	if (!tworzPodkatalogi(pelna_sciezka_text))
	{
		return;
	}

	/* Spróbuj utworzyæ plik docelowy */
	if (!otworzPlikDoZapisu(pelna_sciezka_text))
	{
		return;
	}

	/*** Przygotuj odpowiedni nag³ówek pliku ***/
	std::memset(header, 0x00, 0x36);
	if (BmpExt)
	{
		x = int(getBytesPerPixel() * SizeX * SizeY);
		y = isUsingPalette();
		header[0] = 'B';
		header[1] = 'M';
		*(int*)&(header[2]) = (y ? 0x0400 : 0) + 0x36 + x;
		header[0x0A] = 0x36;
		if (y) header[0x0B] = 0x04;
		header[0x0E] = 0x28;
		*(int*)&(header[0x12]) = SizeX;
		*(int*)&(header[0x16]) = SizeY;
		header[0x1A] = 0x01;
		header[0x1C] = y ? 0x08 : 0x18;
		*(int*)&(header[0x22]) = x;
	}
	else
	{
		header[0x02] = 0x02; 
		*(short*)&(header[0x0C]) = SizeX;
		*(short*)&(header[0x0E]) = SizeY;
		header[0x10] = 0x08 * getBytesPerPixelNew();
		header[0x11] = 0x08; 
	}

	/*** Przygotuj odpowiedni blok wyjœciowy (zapamiêtaj iloœæ bajtów) ***/
	y = SizeX * SizeY * getBytesPerPixelNew();

	try
	{
		Piksele2 = new byte[y];
	}
	catch (std::bad_alloc)
	{
		throw EKomunikat(L"Could not allocate memory for output pixels.");
		return;
	}

	/*** Specjalny format z Kao Challengers ***/
	if (0x4E == Rodzaj)
	{
		for (int Y=0; Y<(SizeY/2); Y+=2)
		{
			for (int X=0; X<SizeX; X+=4)
			{
				// ODCZYT po 8 bajtów (16+16+32 bitów)
				// WYJŒCIE na 32 bajty (32+32+32+32 bitów)
				// X roœnie co 4, bo kolejny output jest co 4 piksele po 32 bity
				// Y roœnie co 4, bo bloki output s¹ 4x4 piksele.
				// stride *4 bo to s¹ Bajty scanline 
				
				DecompressBlockBC1(X, Y*2, SizeX*4, (Piksele1 + Y*SizeX + X*2), Piksele2);
			}
		}

		/* Odwracamy obraz pionowo */
		if (nullptr != Piksele1)
		{
			delete[](Piksele1);
		}
		try
		{
			Piksele1 = new byte[y];
		}
		catch (std::bad_alloc)
		{
			throw EKomunikat(L"Could not allocate memory for output pixels.");
			return;
		}

		for (int Y=0; Y<SizeY; Y++)
		{
			std::memcpy(Piksele1 + 4*SizeX*Y, Piksele2 + 4*SizeX*SizeY - 4*SizeX*(1+Y), 4*SizeX);
		}

		if (nullptr != Piksele2)
		{
			delete[](Piksele2);
		}
		Piksele2 = Piksele1;
		Piksele1 = nullptr;
	}
	/*** [TGA, 8] 8-bitowa paleta RGBA na pe³n¹ TARGA 32-bit ***/
	else if ((!BmpExt) && (PAL8_RGBA8 == Rodzaj))
	/* (TGA 0x02) */
	{
		for (int Y=0; Y<SizeY; Y++)
		{
			for (int X=0; X<SizeX; X++)
			{
				// Tylko odbicie pionowe.
				*(int*)&(Piksele2[4*SizeX*SizeY - 4*(Y+1)*SizeX + 4*X]) = Paleta[Piksele1[Y*SizeX + X]];
			}
		}
	}
	/*** [BMP, 8] 8-bitowa paleta na 256-kolorow¹ Bitmapê ***/
	else if (BmpExt && (PAL8_RGBA8 == Rodzaj))
	/* (BMP 0x02) */
	{
		for (int Y=0; Y<SizeY; Y++)
		{
			// Tylko odbicie pionowe.
			std::memcpy(Piksele2 + SizeX*Y, Piksele1 + SizeX*SizeY - SizeX*(1+Y), SizeX);
		}
	}
	/*** [TGA, 32] Truecolor TARGA 32-bit ***/
	else if ((!BmpExt) && (RGBA8 == Rodzaj))
	/* (TGA 0x00) */
	{
		for (int Y=0; Y<SizeY; Y++)
		{
			for (int X=0; X<SizeX; X++)
			{
				x = *(int*)&(Piksele1[4*Y*SizeX + 4*X]);
				// Odbicie pionowe. Kolory BGR na RGB.
				*(int*)&(Piksele2[4*SizeX*SizeY - 4*(Y+1)*SizeX + 4*X]) = (x & 0xFF00FF00) | ((x >>16) &0xFF) | ((x &0xFF) <<16);
			}
		}
	}
	/*** [BMP, 32] Bitmapa 24-bitowa (UTRATA INFORMACJI O ALPHA) ***/
	else if (BmpExt && (RGBA8 == Rodzaj))
	/* (BMP 0x00) */
	{
		for (int Y=0; Y<SizeY; Y++)
		{
			for (int X=0; X<SizeX; X++)
			{
				// Odbicie pionowe. Kolory BGR na RGB.
				Piksele2[3*SizeX*SizeY - 3*(Y+1)*SizeX + 3*X + 0] = Piksele1[4*Y*SizeX + 4*X + 2];
				Piksele2[3*SizeX*SizeY - 3*(Y+1)*SizeX + 3*X + 1] = Piksele1[4*Y*SizeX + 4*X + 1];
				Piksele2[3*SizeX*SizeY - 3*(Y+1)*SizeX + 3*X + 2] = Piksele1[4*Y*SizeX + 4*X + 0];
			}
		}
	}
	/*** Nierozpoznana konfiguracja ***/
	else
	{
		wchar_t test1[256];
		swprintf_s(test1, 256, L"Unsupported \"eBitmap\" configuration...\r\nDestination extension: \"%s\"\r\nType: 0x%04X (%s)",
			(BmpExt ? L"BMP" : L"TGA"),
			Rodzaj,
			getRodzajAsName());
		throw EKomunikat(test1);
		return;
	}

	/*** Mo¿esz teraz wyczyœciæ "piksele1" ***/
	if (nullptr != Piksele1)
	{
		delete[](Piksele1);
		Piksele1 = nullptr;
	}

	/*** Zapisz nag³ówek obrazka ***/
	zapiszPlik(header, BmpExt ? 0x36 : 0x12);

	/*** Zapisz paletê, je¿eli jest to bitmapa 256-kolorowa ***/
	if (BmpExt && isUsingPalette())
	{
		zapiszPlik(Paleta, 0x0400);
	}

	/*** Zapisz odpowiedni¹ ilosæ bajtów z kolorami ***/
	zapiszPlik(Piksele2, y);

	/*** Opcjonalnie: Zamknij plik i wyczyœæ ostatnie piksele z pamiêci ***/
	CloseHandle(Plik2);
	Plik2 = INVALID_HANDLE_VALUE;
	if (nullptr != Piksele2)
	{
		delete[](Piksele2);
		Piksele2 = nullptr;
	}
}
