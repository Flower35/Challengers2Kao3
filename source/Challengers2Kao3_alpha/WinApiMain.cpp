#include <Challengers2Kao3_alpha/WinApi.h>


////////////////////////////////////////////////////////////////
// WinApi: Konstrukcja
////////////////////////////////////////////////////////////////

HINSTANCE CWindow::Instancja = 0;
HBITMAP CWindow::Bitmapa = NULL;
HFONT CWindow::Czcionka[2] = 
{
	CreateFont(24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Verdana"),
	CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI")
};
OPENFILENAME CWindow::Ofn = {0};
wchar_t CWindow::NazwaPlikuOtwieranego[256] = {0};
HWND CWindow::Okna[4] = {0};
HWND CWindow::LoadingTekst = NULL;
HWND CWindow::EditBoxKatalogi[3] = {0};
HWND CWindow::CheckBoxPoziomy[NUM_LEVELS] = {0};
HWND CWindow::PrzyciskiSwiaty[NUM_WORLDS] = {0};
HWND CWindow::RadioButtonLatki[NUM_LATKI] = {0};
HWND CWindow::CheckBoxZaawansowane[NUM_OPCJE_ZAAWANSOWANE] = {0};

CWindow::CWindow() {}


////////////////////////////////////////////////////////////////
// WinApi: Wyczyść dane przy zamykaniu
////////////////////////////////////////////////////////////////
CWindow::~CWindow()
{
	DeleteObject(Czcionka[0]);
	DeleteObject(Czcionka[1]);
	DeleteObject(Bitmapa);
};


////////////////////////////////////////////////////////////////
// WinApi: Tworzenie okien i pokazywanie programu
////////////////////////////////////////////////////////////////
bool CWindow::utworzOkienka()
{
	HWND okienko;
	RECT wymiary;

	////////////////////////////////////////////////////////////////
	// * Rejestracja klas

	WNDCLASSEX WinClass; ZeroMemory(&WinClass, sizeof(WNDCLASSEX));
	
	WinClass.cbSize = sizeof(WNDCLASSEX);
	WinClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	WinClass.hIconSm = WinClass.hIcon = LoadIcon(Instancja, MAKEINTRESOURCE(ID_IKONA1));
	WinClass.hCursor = LoadCursor(Instancja, IDC_ARROW);
	WinClass.hInstance = Instancja;
	WinClass.lpfnWndProc = procedura;
	WinClass.lpszClassName = KLASA_OKNA_1;
	WinClass.style = (CS_OWNDC | CS_VREDRAW | CS_HREDRAW);
	if (!RegisterClassEx(&WinClass)) { return false; };

	WinClass.lpfnWndProc = procedura_druga;
	WinClass.lpszClassName = KLASA_OKNA_2;
	if (!RegisterClassEx(&WinClass)) { return false; };

	WinClass.lpfnWndProc = procedura_trzecia;
	WinClass.lpszClassName = KLASA_OKNA_3;
	if (!RegisterClassEx(&WinClass)) { return false; };

	WinClass.lpfnWndProc = procedura_czwarta;
	WinClass.lpszClassName = KLASA_OKNA_4;
	if (!RegisterClassEx(&WinClass)) { return false; };

	////////////////////////////////////////////////////////////////
	// * Okno główne

	wymiary = {0, 0, OKNO_SZEROKOSC, OKNO_WYSOKOSC};
	AdjustWindowRect(&wymiary, WS_OVERLAPPEDWINDOW, FALSE);

	int szerokosc = (wymiary.right - wymiary.left);
	int wysokosc = (wymiary.bottom - wymiary.top);
	const int pos_x = (GetSystemMetrics(SM_CXSCREEN) - szerokosc) /2;
	const int pos_y = (GetSystemMetrics(SM_CYSCREEN) - wysokosc) /2;
	const int pos_y_2 = (GetSystemMetrics(SM_CYSCREEN) - 128);

	if (NULL == (Okna[0] = CreateWindow(
		KLASA_OKNA_1,
		KOMUNIKAT_TYTUL,
		(WS_SYSMENU),
		pos_x, pos_y, szerokosc, wysokosc,
		NULL, NULL, Instancja, NULL)))
	{
		return false;
	};

	if (NULL == (okienko = CreateWindow(
		WC_STATIC,
		L"port <KAO CHALLENGERS> levels into\r\n<KAO: MYSTERY OF THE VOLCANO> engine",
		(WS_VISIBLE | WS_CHILD),
		16, 4, 592, 64,
		Okna[0], NULL, Instancja, NULL)))
	{
		return false;
	};
	SendMessage(okienko, WM_SETFONT, (WPARAM)Czcionka[0], 0);

	////////////////////////////////////////////////////////////////
	// * Okno ładowania

	wymiary = {0, 0, 640, 48};
	AdjustWindowRect(&wymiary, WS_OVERLAPPEDWINDOW, FALSE);
	szerokosc = (wymiary.right - wymiary.left);
	wysokosc = (wymiary.bottom - wymiary.top);

	if (NULL == (Okna[1] = CreateWindowEx(
		(WS_EX_TOPMOST | WS_EX_TOOLWINDOW),
		KLASA_OKNA_2,
		L"Please wait...",
		(WS_SYSMENU),
		pos_x, pos_y_2, szerokosc, wysokosc,
		Okna[0], NULL, Instancja, NULL)))
	{
		return false;
	};

	if (NULL == (LoadingTekst= CreateWindow(
		WC_STATIC,
		L"",
		(WS_VISIBLE | WS_CHILD),
		16, 4, 592, 64,
		Okna[1], NULL, Instancja, NULL)))
	{
		return false;
	};
	SendMessage(LoadingTekst, WM_SETFONT, (WPARAM)Czcionka[1], 0);


	////////////////////////////////////////////////////////////////
	// * Okno z wyborem poziomów

	wymiary = {0, 0, 640, 480};
	AdjustWindowRect(&wymiary, WS_OVERLAPPEDWINDOW, FALSE);
	szerokosc = (wymiary.right - wymiary.left);
	wysokosc = (wymiary.bottom - wymiary.top);

	if (NULL == (Okna[2] = CreateWindowEx(
		WS_EX_TOOLWINDOW,
		KLASA_OKNA_3,
		L"Select levels",
		(WS_SYSMENU),
		pos_x, pos_y, szerokosc, wysokosc,
		Okna[0], NULL, Instancja, NULL)))
	{
		return false;
	};

	int offx = 16;
	int offy = 8;
	int offl = 0;

	for (int w = 0; w < NUM_WORLDS; w++)
	{
		// Czy należy przejść do kolejnej kolumny
		if ((4 == w) || (7 == w))
		{
			offx += 208;
			offy = 8;
		}

		// Dodaj dwa przyciski
		if (NULL == (PrzyciskiSwiaty[w] = CreateWindow(
			WC_BUTTON,
			L"[x]",
			(WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON),
			offx, offy, 28, 24,
			Okna[2], (HMENU)(ID_PRZYCISKI_SWIATY + w), Instancja, NULL)))
		{
			return false;
		};
		SendMessage(PrzyciskiSwiaty[w], WM_SETFONT, (WPARAM)Czcionka[1], 0);

		// Dodaj napis o świecie
		if (NULL == (okienko = CreateWindow(
			WC_STATIC,
			KaoChallengersLevels[w].name,
			(WS_VISIBLE | WS_CHILD),
			(offx + 32), offy, 160, 32,
			Okna[2], NULL, Instancja, NULL)))
		{
			return false;
		};
		SendMessage(okienko, WM_SETFONT, (WPARAM)Czcionka[0], 0);

		// Przesuń się niżej i dodawaj kolejne poziomy
		offy += 28;
		for (int l = 0; l < KaoChallengersLevels[w].num; l++)
		{
			if (NULL == (CheckBoxPoziomy[offl] = CreateWindow(
				WC_BUTTON,
				KaoChallengersLevels[w].levels[l].name,
				(WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX),
				offx, offy, 192, 16,
				Okna[2], NULL, Instancja, NULL)))
			{
				return false;
			};
			SendMessage(CheckBoxPoziomy[offl], WM_SETFONT, (WPARAM)Czcionka[1], 0);
			SendMessage(CheckBoxPoziomy[offl], BM_SETCHECK, (WPARAM)BST_CHECKED, 0);

			// Będziemy dodawać kolejny CheckBox kilka pikseli niżej
			offl++;
			offy += 16;
		}
		offy += 16;
	}

	
	////////////////////////////////////////////////////////////////
	// * Okno z łatkami do gry

	wymiary = {0, 0, 640, 480};
	AdjustWindowRect(&wymiary, WS_OVERLAPPEDWINDOW, FALSE);
	szerokosc = (wymiary.right - wymiary.left);
	wysokosc = (wymiary.bottom - wymiary.top);

	if (NULL == (Okna[3] = CreateWindowEx(
		WS_EX_TOOLWINDOW,
		KLASA_OKNA_4,
		L"Select patches and change settings",
		(WS_SYSMENU),
		pos_x, pos_y, szerokosc, wysokosc,
		Okna[0], NULL, Instancja, NULL)))
	{
		return false;
	};

	const wchar_t* latki_nazwy[NUM_LATKI] =
	{
		L"Kao Challengers - SinglePlayer (storyline)",
		L"Kao Challengers - MultiPlayer (battle mode)"
	};

	const wchar_t* latki_opisy =
	/*
		L"Both options include:\r\n" \
		L" * Directly loading RAW (extracted) files\r\n" \
		L" * 16:9 Widescreen resolution\r\n" \
		L" * Disabled intro movies\r\n" \
		L" * Experimental changes in menus and gameplay\r\n" \
		L"";
	*/
		L"Use these directory names:\r\n" \
		L" * \"\\challengers_sp\\\" replaces \"\\Media\\\"\r\n  for Kao Challengers SP levels\r\n" \
		L" * \"\\challengers_mp\\\" replaces \"\\Media\\\"\r\n  for Kao Challengers MP levels\r\n" \
		L"";

	const wchar_t* zaawansowane_opcje_nazwy[NUM_OPCJE_ZAAWANSOWANE] =
	{
		L"[!] Decompress sound files (\"*.vag\" and \"*.ac3\") and re-compress music (\"*.at3\" into \"*.ogg\").",
		L"[!] Decompile textures (\"*.eb\" and \"*.ar\" into \"*.bmp\" or into \"*.tga\").",
		L"[!] Automatically replace scripts and textures\r\n (if directories \"kao_pliki/updated_scripts\" and \"kao_pliki/hd_textures\" exist).",
		L"Do not extract items that are disregarded during game loadings (some unnecessary or deprecated files).",
		L"Do not overwrite already existing files (faster extraction; earlier items have priority over later PAKs)."
	};

	/* Mini napis */
	if (NULL == (okienko = CreateWindow(
		WC_STATIC,
		L"Patches and tweaks",
		(WS_VISIBLE | WS_CHILD),
		16, 16, ((szerokosc / 2) - (2 * 16)), 32,
		Okna[3], NULL, Instancja, NULL)))
	{
		return false;
	};
	SendMessage(okienko, WM_SETFONT, (WPARAM)Czcionka[0], 0);

	/* Radio buttony z wyborem łatek */
	for (int i = 0; i < NUM_LATKI; i++)
	{
		if (NULL == (RadioButtonLatki[i] = CreateWindow(
			WC_BUTTON,
			latki_nazwy[i],
			(WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | ((0 == i) ? WS_GROUP : 0)),
			16, (48 + (i * 24)), ((szerokosc / 2) - (2 * 16)), 24,
			Okna[3], NULL, Instancja, NULL)))
		{
			return false;
		};
		SendMessage(RadioButtonLatki[i], WM_SETFONT, (WPARAM)Czcionka[1], 0);
	}
	SendMessage(RadioButtonLatki[0], BM_SETCHECK, (WPARAM)BST_CHECKED, 0);

	/* Krótkie opisy łatek */
	if (NULL == (okienko = CreateWindow(
		WC_STATIC,
		latki_opisy,
		(WS_VISIBLE | WS_CHILD),
		(szerokosc / 2), 32, ((szerokosc / 2) - (2 * 16)), 96,
		Okna[3], NULL, Instancja, NULL)))
	{
		return false;
	};
	SendMessage(okienko, WM_SETFONT, (WPARAM)Czcionka[1], 0);

	/* Przycisk do aplikowania modów */
	if (NULL == (okienko = CreateWindow(
		WC_BUTTON,
		L"Apply important game tweaks!",
		(WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON),
		(szerokosc - 384 - 48), 128, 384, 36,
		Okna[3], (HMENU)(ID_PRZYCISK + 3), Instancja, NULL)))
	{
		return false;
	};
	SendMessage(okienko, WM_SETFONT, (WPARAM)Czcionka[0], 0);

	/* Przycisk do kopiowania plików silnika */
	if (NULL == (okienko = CreateWindow(
		WC_BUTTON,
		L"Copy important game files!",
		(WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON),
		(szerokosc - 384 - 48), 176, 384, 36,
		Okna[3], (HMENU)(ID_PRZYCISK + 4), Instancja, NULL)))
	{
		return false;
	};
	SendMessage(okienko, WM_SETFONT, (WPARAM)Czcionka[0], 0);

	/* Mini napis */
	if (NULL == (okienko = CreateWindow(
		WC_STATIC,
		L"Advanced settings",
		(WS_VISIBLE | WS_CHILD),
		16, 256, (szerokosc - (2 * 32)), 32,
		Okna[3], NULL, Instancja, NULL)))
	{
		return false;
	};
	SendMessage(okienko, WM_SETFONT, (WPARAM)Czcionka[0], 0);

	/* Checkboxy z opcjami podczas eksportowania plików z PAK */
	for (int i = 0; i < NUM_OPCJE_ZAAWANSOWANE; i++)
	{
		if (NULL == (CheckBoxZaawansowane[i] = CreateWindow(
			WC_BUTTON,
			zaawansowane_opcje_nazwy[i],
			(WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | BS_MULTILINE),
			16, (288 + (i * 32)), 576, 32,
			Okna[3], NULL, Instancja, NULL)))
		{
			return false;
		};
		SendMessage(CheckBoxZaawansowane[i], WM_SETFONT, (WPARAM)Czcionka[1], 0);
		SendMessage(CheckBoxZaawansowane[i], BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
	}


	////////////////////////////////////////////////////////////////
	// * Pola z wyborem katalogów

	const wchar_t* editboxy_nazwy[2] =
	{
		L"Path to the [USRDIR] directory from [Kao Challengers]:",
		L"Path to a new [media] directory:"
	};
	for (int i = 0; i < 2; i++)
	{
		if (NULL == (okienko = CreateWindow(
			WC_BUTTON,
			editboxy_nazwy[i],
			(WS_VISIBLE | WS_CHILD | BS_GROUPBOX | WS_GROUP),
			8, (68 + i * 56), (OKNO_SZEROKOSC - 16), 48,
			Okna[0], NULL, Instancja, NULL)))
		{
			return false;
		};
		SendMessage(okienko, WM_SETFONT, (WPARAM)Czcionka[1], 0);
	
		if (NULL == (EditBoxKatalogi[i] = CreateWindowEx(
			WS_EX_CLIENTEDGE,
			WC_EDIT,
		#ifdef _DEBUG
			L"D:/KAO_Praca/KAO MODY 4/Kao Challengers EUR/",
		#else
			L"",
		#endif
			(WS_VISIBLE | WS_CHILD),
			16, (88 + i * 56), (OKNO_SZEROKOSC - 32), 24,
			Okna[0], NULL, Instancja, NULL)))
		{
			return false;
		};
		SendMessage(EditBoxKatalogi[i], WM_SETFONT, (WPARAM)Czcionka[1], 0);
	}

	////////////////////////////////////////////////////////////////
	// * Przyciski w oknie głównym

	const wchar_t* przyciski_nazwy[3] =
	{
		L"* Select levels *",
		L"* Export game files *",
		L"* Options and tweaks *"
	};
	for (int i = 0; i < 3; i++)
	{
		if (NULL == (okienko = CreateWindow(
			WC_BUTTON,
			przyciski_nazwy[i],
			(WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON),
			16, (184 + i * 48), 240, 36,
			Okna[0], (HMENU)(ID_PRZYCISK + i), Instancja, NULL)))
		{
			return false;
		};
		SendMessage(okienko, WM_SETFONT, (WPARAM)Czcionka[0], 0);
	}

	////////////////////////////////////////////////////////////////
	// * Napis o wersji

	if (NULL == (okienko = CreateWindow(
		WC_STATIC,
		TEKST_LAST_UPDATE,
		(WS_VISIBLE | WS_CHILD),
		16, (OKNO_WYSOKOSC - 24), (OKNO_SZEROKOSC - 32), 24,
		Okna[0], NULL, Instancja, NULL)))
	{
		return false;
	};
	SendMessage(okienko, WM_SETFONT, (WPARAM)Czcionka[1], 0);


	return true;
};


////////////////////////////////////////////////////////////////
// WinApi: Główna funkcja obiektu okna
////////////////////////////////////////////////////////////////
int CWindow::petlaProgramu()
{
	MSG Komunikat;

	if (!utworzOkienka())
	{
		MessageBox(
			NULL,
			L"Critical error: Could not register windows class!",
			KOMUNIKAT_TYTUL,
			(MB_OK | MB_ICONERROR));
		return (-1);
	};

	Bitmapa = LoadBitmap(Instancja, MAKEINTRESOURCE(ID_BITMAPA1));

	ZeroMemory(&Ofn, sizeof(OPENFILENAME));
	Ofn.lStructSize = sizeof(OPENFILENAME);
	swprintf_s(NazwaPlikuOtwieranego, 256, L"kao_tw.exe");
	Ofn.lpstrFile = NazwaPlikuOtwieranego;
	Ofn.nMaxFile = 256;
	Ofn.lpstrFilter = L"Executable files (*.exe)\0*.exe\0";
	Ofn.lpstrTitle = L"Select Kao the Kangaroo game executable";
	Ofn.hwndOwner = Okna[0];
	Ofn.hInstance = Instancja;
	/* WAŻNE! Flaga "OFN_NOCHANGEDIR" musi być ustawiona, inaczej nie działa kopiowanie plików z "./kao_pliki/" */
	Ofn.Flags = (OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOREADONLYRETURN);

	odtworzDzwiek(GLOS_KAO_ZROBIEWSZYSTKO);
	
	ShowWindow(Okna[0], SW_SHOW);

	while (GetMessage(&Komunikat, NULL, 0, 0))
	{
		TranslateMessage(&Komunikat);
		DispatchMessage(&Komunikat);
	}

	return 0;
};


////////////////////////////////////////////////////////////////
// WinApi:: Funkcja zmieniająca tekst w małym okienku
////////////////////////////////////////////////////////////////
void CWindow::zmienTekstOknaLadowania(const wchar_t* abc)
{
	MSG Komunikat;

	SetWindowText(LoadingTekst, abc);
	UpdateWindow(Okna[1]);

	/* Odblokuj możliwość przesuwania okien i nie pokazuj komunikatu "Brak odpowiedzi..." */
	while (PeekMessage(&Komunikat, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&Komunikat);
		DispatchMessage(&Komunikat);
	}
}


////////////////////////////////////////////////////////////////
// WinApi:: Czy jest pewna opcja podczas eksportowania zaznaczona?
////////////////////////////////////////////////////////////////
bool CWindow::czyJestOpcjaZaawansowana(int id)
{
	if ((id >= 0) && (id < NUM_OPCJE_ZAAWANSOWANE))
	{
		return (BST_CHECKED == SendMessage(CheckBoxZaawansowane[id], BM_GETCHECK, 0, 0));
	}

	return false;
}


////////////////////////////////////////////////////////////////
// WinApi:: Odtwórz plik dźwiękowy
////////////////////////////////////////////////////////////////
void CWindow::odtworzDzwiek(int id)
{
	switch (id)
	{
		case GLOS_KAO_SUPER:
		{
			id = ID_WAVE1; // "Super!"
			break;
		}

		case GLOS_KAO_ONIE:
		{
			id = ID_WAVE2; // "O nie..."
			break;
		}

		case GLOS_KAO_PROBLEMY:
		{
			id = ID_WAVE3; // "Zaraz problemy!"
			break;
		}

		case GLOS_KAO_ZROBIEWSZYSTKO:
		{
			id = ID_WAVE4; // "Zrobię wszystko, co w mojej mocy!"
			break;
		}

		case GLOS_KAO_PROSZE:
		{
			id = ID_WAVE5; // "Proszę!"
			break;
		}

		default:
		{
			return;
		}
	}

	PlaySound(MAKEINTRESOURCE(id), Instancja, (SND_RESOURCE | SND_ASYNC));
}


////////////////////////////////////////////////////////////////
// WinApi: Główna Funkcja (MAIN)
////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevious, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	MessageBox(
		NULL,
		L"[ DEBUG MODE ]\r\n\"D:\\KAO_Praca\\KAO MODY 4\\\"",
		KOMUNIKAT_TYTUL,
		(MB_OK | MB_ICONINFORMATION));
	if (0 == SetCurrentDirectory(L"D:\\KAO_Praca\\KAO MODY 4\\"))
	{
		MessageBox(
			NULL,
			L"Nie udało się ustawić ścieżki docelowej debugowej!",
			KOMUNIKAT_TYTUL,
			(MB_OK | MB_ICONERROR));

		return (-1);
	}
#endif

	/* Czasami program nie widzi folderu z danymi... :/ */
	{
		DWORD ftyp = GetFileAttributes(L"./kao_pliki/");

		if ((INVALID_FILE_ATTRIBUTES == ftyp) || (0 == (FILE_ATTRIBUTE_DIRECTORY & ftyp)))
		{
			MessageBox(
				NULL,
				L"WARNING:\r\nData directory (\"kao_pliki\") not found...",
				KOMUNIKAT_TYTUL,
				(MB_OK | MB_ICONWARNING));
		}
	}

	/* Uruchom aplikację */
	CWindow::Instancja = hInstance;
	return CWindow::petlaProgramu();
};
