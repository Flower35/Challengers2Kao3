#include <Challengers2Kao3_alpha/PakExporter.h>
#include <Challengers2Kao3_alpha/WinApi.h>

#include <Challengers2Kao3_alpha/BitmapExporter.h>
#include <Challengers2Kao3_alpha/SoundExporter.h>


////////////////////////////////////////////////////////////////
// Czy dany ITEM mo¿na pomin¹æ podczas wypakowywania?
////////////////////////////////////////////////////////////////
bool CPakExporter::isItemSkipable(eUnicodeString &nazwa)
{
	int x;

	const wchar_t* item_names[] =
	{
		L"/enemyMap.dat",
		L"/saveicon.png",
		L"menu/ui/selectBar.tga.eb",
		L"menu/ui/background/bg_sp.tga.eb",
		L"menu/ui/background/bg_mp.tga.eb",
		L"menu/menuMP/mp_01.tga.eb",
		L"menu/menuMP/mp_02.tga.eb",
		L"menu/menuMP/mp_03.tga.eb",
		L"menu/menuMP/mp_04.tga.eb",
		L"menu/menuMP/mp_05.tga.eb",
		L"menu/menuMP/mp_06.tga.eb",
		L"menu/menuMP/mp_07.tga.eb",
		L"menu/menuMP/mp_08.tga.eb",
		L"menu/menuMP/mp_09.tga.eb",
		L"menu/menuMP/mp_snowboard.tga.eb",
		L"menu/menuMP/mp_bobslej.tga.eb",
		L"menu/menuMP/mp_RaceBoat.tga.eb",
		L"menu/menuMP/mp_bobslej02.tga.eb",
		L"menu/menuMP/mp_snowboard02.tga.eb",
		L"misc/demo/kao_small_logo.tga.eb",
		L"misc/powerBars/loading_bar.tga.eb",
		L"scripts/persistent_floats.def",
		L"levels/mp_select/scripts/heroes.def"
	};
	const int item_names_size = sizeof(item_names) / sizeof(wchar_t*);

	int dlugosc = nazwa.getLength();
	
	for (int i = 0; i < item_names_size; i++)
	{
		x = _getCharArrayLength(item_names[i]);
		if (nazwa.compare(item_names[i], (dlugosc - x), 0, false))
		{
			return true;
		}
	}

	return false;
}


////////////////////////////////////////////////////////////////
// Czy za dany ITEM mo¿na podstawiæ inny plik?
////////////////////////////////////////////////////////////////
bool CPakExporter::isItemUpgradable(eUnicodeString &nazwa)
{
	eUnicodeString sciezka1;
	wchar_t* tekst1;
	eUnicodeString sciezka2;
	wchar_t* tekst2;

	if (nazwa.hasExtension(L"def") || nazwa.hasExtension(L"ini"))
	{
		sciezka1 = eUnicodeString(L"./kao_pliki/updated_scripts/") + nazwa;
	}
	else if (nazwa.hasExtension(L"bmp") || nazwa.hasExtension(L"tga"))
	{
		sciezka1 = eUnicodeString(L"./kao_pliki/hd_textures/") + nazwa;
	}
	else
	{
		return false;
	}

	tekst1 = sciezka1.getText();

	if (czyPlikIstnieje(tekst1))
	{
		sciezka2 = FolderDocelowy + nazwa;
		tekst2 = sciezka2.getText();

		if (czyPlikIstnieje(tekst2))
		{
			return true;
		}

		if (!tworzPodkatalogi(tekst2))
		{
			return false;
		}

		if (0 != CopyFile(tekst1, tekst2, FALSE))
		{
			return true;
		}
	}

	return false;
}


////////////////////////////////////////////////////////////////
// SprawdŸ, czy ITEM powinien mieæ zmienion¹ nazwê
////////////////////////////////////////////////////////////////
void CPakExporter::opracujKoncowaNazweItema(int32_t &typ, int32_t &podtyp, eUnicodeString &nazwa)
{
	/*** Czy jest to plik graficzny? ***/

	if (CWindow::czyJestOpcjaZaawansowana(OPCJA_ZAAWANSOWANA_DECOMPILE_GFX))
	{
		if (nazwa.hasExtension(L"eb"))
		{
			podtyp = 0;
			typ = ITEM_RODZAJ_GRAFICZNY;
			return;
		}
		else if (czyItemPasujeDoWzorca(nazwa, L"ar", L"/misc/", L"misc/"))
		{
			podtyp = 1;
			typ = ITEM_RODZAJ_GRAFICZNY;
			return;
		}
	}

	/*** Czy jest to plik dŸwiêkowy? ***/

	if (CWindow::czyJestOpcjaZaawansowana(OPCJA_ZAAWANSOWANA_DECOMPRESS_SFX))
	{
		if (nazwa.hasExtension(L"vag"))
		{
			podtyp = SOUND_RODZAJ_VAG;
			typ = ITEM_RODZAJ_DZWIEKOWY;
		}
		else if (nazwa.hasExtension(L"ac3"))
		{
			podtyp = SOUND_RODZAJ_AT3_MONO;
			typ = ITEM_RODZAJ_DZWIEKOWY;
		}
		else if (czyItemPasujeDoWzorca(nazwa, L"at3", L"music\\psp\\", L"music/"))
		{
			podtyp = SOUND_RODZAJ_AT3_STEREO;
			typ = ITEM_RODZAJ_DZWIEKOWY;
		}

		if (ITEM_RODZAJ_DZWIEKOWY == typ)
		{
			wchar_t* new_ext = nullptr;
			switch (podtyp)
			{
				case SOUND_RODZAJ_VAG:
				case SOUND_RODZAJ_AT3_MONO:
				{
					new_ext = L"wav";
					break;
				}
				case SOUND_RODZAJ_AT3_STEREO:
				{
					new_ext = L"ogg";
					break;
				}
			}
			if (nullptr != new_ext)
			{
				/* Docelowy wskaŸnik jest  automatycznie liczony jako "wchar_t*" (nie trzeba mno¿yæ przez 2) */
				std::memcpy(
					(nazwa.getText() + nazwa.getLength() - 3),
					new_ext, (3 * sizeof(wchar_t)));
			}

			return;
		}
	}

	/*** Czy jest to inny plik? ***/

	czyItemPasujeDoWzorca(nazwa, L"ar", L"build/pc/psp/", L"build/pc/");

	typ = ITEM_RODZAJ_ZWYKLY;
}


////////////////////////////////////////////////////////////////
// Zapisywanie nowego pliku z paczki
////////////////////////////////////////////////////////////////
int CPakExporter::eksportujItemSubfunkcja(int rozmiar, int offset_koncowy, eUnicodeString &nazwa)
{
	int32_t typ_pliku;
	int32_t podtyp_pliku;

	eUnicodeString pelna_sciezka;
	wchar_t* pelna_sciezka_text = nullptr;

	/* Czy plik mo¿na pomin¹æ, ze wzglêdu na silnik "Tajemnicy Wulkanu"? */
	if (CWindow::czyJestOpcjaZaawansowana(OPCJA_ZAAWANSOWANA_SKIPABLES))
	{
		if (isItemSkipable(nazwa))
		{
			return 0;
		}
	}

	/* Czy dany plik mo¿na podmieniæ poprzez kopiowanie? */
	if (CWindow::czyJestOpcjaZaawansowana(OPCJA_ZAAWANSOWANA_UPGRADABLES))
	{
		if (isItemUpgradable(nazwa))
		{
			return 0;
		}
	}

	/* Czy plik musi mieæ zmienion¹ nazwê? */
	opracujKoncowaNazweItema(typ_pliku, podtyp_pliku, nazwa);

	/* Czy dany rodzaj itemu bêdzie wymaga³ utworzenia pe³nej œcie¿ki? */
	switch (typ_pliku)
	{
		case ITEM_RODZAJ_GRAFICZNY:
		{
			break;
		}

		default:
		{
			pelna_sciezka = FolderDocelowy + nazwa;

			/* Zapamiêtaj œcie¿kê wynikow¹ wypakowywanego pliku */
			pelna_sciezka_text = pelna_sciezka.getText();

			/* WyjdŸ, jeœli docelowy plik ju¿ istnieje */
			if (CWindow::czyJestOpcjaZaawansowana(OPCJA_ZAAWANSOWANA_DONT_OVERWRITE))
			{
				if (czyPlikIstnieje(pelna_sciezka_text))
				{
					return 0;
				}
			}
		}
	}

	/* Wykonaj odpowiednie operacje */
	switch (typ_pliku)
	{
		case ITEM_RODZAJ_GRAFICZNY:
		{
			CBitmapExporter bmpEx(
				Plik1,
				FolderDocelowy,
				nazwa);

			if (bmpEx.wykonajZadanie(0))
			{
				/* WyjdŸ, jeœli docelowy plik ju¿ istnieje */
				if (CWindow::czyJestOpcjaZaawansowana(OPCJA_ZAAWANSOWANA_DONT_OVERWRITE))
				{
					if (czyPlikIstnieje(pelna_sciezka_text))
					{
						return 0;
					}
				}

				/* Nazwê Bitmapy mo¿emy sprawdziæ dopiero po jej odczytaniu */
				if (CWindow::czyJestOpcjaZaawansowana(OPCJA_ZAAWANSOWANA_UPGRADABLES))
				{
					if (isItemUpgradable(bmpEx.PelnaSciezka))
					{
						return 0;
					}
				}

				/* Wyeksportuj oryginalny plik */
				bmpEx.wykonajZadanie(1);
			}

			break;
		}

		case ITEM_RODZAJ_DZWIEKOWY:
		{
			CSoundExporter sndEx(
				podtyp_pliku,
				Plik1,
				pelna_sciezka,
				nazwa,
				offset_koncowy);

			sndEx.skanujPlik();

			break;
		}

		default:
		{
			/* Alokowanie pamiêci */
			try
			{
				Alokacja = new byte[rozmiar];
			}
			catch (std::bad_alloc)
			{
				wchar_t test1[256];
				swprintf_s(test1, 256, L"(%s)\r\nUnable to allocate 0x%08X bytes!", nazwa.getText(), rozmiar);
				throw EKomunikat(test1);
				return 1;
			}

			/* Odczyt danych z paczki */
			czytajPlik(Alokacja, rozmiar);

			/* Tworzenie podkatalogów */
			if (!tworzPodkatalogi(pelna_sciezka_text))
			{
				return 1;
			}

			/* Zapisywanie danych */
			if (!otworzPlikDoZapisu(pelna_sciezka_text))
			{
				return 1;
			}
			zapiszPlik(Alokacja, rozmiar);

			/* Wy³¹czenie uchwytu docelowego */
			CloseHandle(Plik2);
			Plik2 = INVALID_HANDLE_VALUE;
		}
	}

	return 0;
}
