#include <Challengers2Kao3_alpha/WinApi.h>

#include <Challengers2Kao3_alpha/PakExporter.h>
#include <Challengers2Kao3_alpha/PatchApplier.h>


////////////////////////////////////////////////////////////////
// Sprawdź poprawność ścieżek
////////////////////////////////////////////////////////////////
bool CWindow::sprawdzPoprawnoscSciezek(wchar_t* nazwa_wejsciowa, wchar_t* nazwa_koncowa, eUnicodeString* katalog)
{
	/* Odczytaj ścieżki do dwóch katalogów */
	GetWindowText(EditBoxKatalogi[0], nazwa_wejsciowa, 256);
	GetWindowText(EditBoxKatalogi[1], nazwa_koncowa, 256);

	katalog[0] = nazwa_wejsciowa;
	katalog[1] = nazwa_koncowa;

	/* Czy scieżki nie są puste? */
	if ((katalog[0].getLength() <= 1) || (katalog[1].getLength() <= 1))
	{
		return false;
	}

	for (int j = 0; j < 2; j++)
	{
		/* Usuń cudzysłów na początku */
		while (katalog[j].compare(L"\"", 0, 1))
		{
			katalog[j] = katalog[j].getSubstring(1, 0);
		}

		/* Usuń cudzysłów na końcu */
		while (katalog[j].compare(L"\"", (katalog[j].getLength() - 1), 1))
		{
			katalog[j] = katalog[j].getSubstring(0, (katalog[j].getLength() - 1));
		}

		/* Ścieżka powinna zawierać slash */
		if (katalog[j].getLength() <= 1)
		{
			return false;
		}
		else
		{
			wchar_t ostatni_znak = katalog[j].getText()[katalog[j].getLength() - 1];
			if ((L'/' != ostatni_znak) && (L'\\' != ostatni_znak))
			{
				katalog[j] += L"/";
			}
		}

		/* Zwróć do ExitBox ścieżkę po modyfikacjach */
		SetWindowText(EditBoxKatalogi[j], katalog[j].getText());
	}

	return true;
}


////////////////////////////////////////////////////////////////
// WinApi: Procedura zwrotna
////////////////////////////////////////////////////////////////
LRESULT CALLBACK CWindow::procedura(HWND Okno, UINT Komunikat, WPARAM wParam, LPARAM lParam)
{
	switch (Komunikat)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}

		case WM_CLOSE:
		{
			DestroyWindow(Okno);
			break;
		}

		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdcOkno = BeginPaint(Okno, &ps);
			HDC hdcMem = CreateCompatibleDC(hdcOkno);
			HGDIOBJ oldBitmap = SelectObject(hdcMem, Bitmapa);
			BITMAP bitmap;

			GetObject(Bitmapa, sizeof(bitmap), &bitmap);
			StretchBlt(hdcOkno,
				(OKNO_SZEROKOSC - BITMAPA1_SZEROKOSC - 32),
				(OKNO_WYSOKOSC - BITMAPA1_WYSOKOSC - 32),
				BITMAPA1_SZEROKOSC, BITMAPA1_WYSOKOSC,
				hdcMem,
				0, 0, bitmap.bmWidth, bitmap.bmHeight,
				SRCCOPY);

			SelectObject(hdcMem, oldBitmap);
			DeleteDC(hdcMem);

			EndPaint(Okno, &ps);
			break;
		} 

		case WM_COMMAND:
		{
			int x = LOWORD(wParam);

			if ((ID_PRZYCISK + 0) == x)
			{
				EnableWindow(Okna[0], FALSE);
				ShowWindow(Okna[2], SW_SHOW);
			}
			else if ((ID_PRZYCISK + 1) == x)
			{
				int current_level = 0;
				wchar_t nazwa_wejsciowa[256];
				wchar_t nazwa_koncowa[256];
				eUnicodeString katalog[2];
				eUnicodeString final_msg;
				int final_count[2] = {0};

				if (!sprawdzPoprawnoscSciezek(nazwa_wejsciowa, nazwa_koncowa, katalog))
				{
					return 1;
				}

				/* Sprawdzaj nasze pliki PAK */
				zmienTekstOknaLadowania(L"");
				EnableWindow(Okna[0], FALSE);
				ShowWindow(Okna[1], SW_SHOW);

				x = 0;

				for (int w = 0; w < NUM_WORLDS; w++)
				{
					for (int l = 0; l < KaoChallengersLevels[w].num; l++)
					{
						if (BST_CHECKED == SendMessage(CheckBoxPoziomy[current_level], BM_GETCHECK, 0, 0))
						{
							zmienTekstOknaLadowania(KaoChallengersLevels[w].levels[l].name);

							final_count[1] += 1;

							CPakExporter pak(katalog[0], KaoChallengersLevels[w].levels[l].pak, katalog[1]);
							if (0 == pak.beginExtracting())
							{
								zmienTekstOknaLadowania(L"");

								final_count[0] += 1;

								final_msg += L"\"";
								final_msg += KaoChallengersLevels[w].levels[l].pak;
								final_msg += L".pak\"\r\n";
							}
							else
							{
								swprintf_s(
									nazwa_wejsciowa,
									256,
									L"Finished extracting \"%s\" level,\r\nbut there were some errors...",
									KaoChallengersLevels[w].levels[l].name);

								odtworzDzwiek(GLOS_KAO_PROBLEMY);

								MessageBox(Okna[0], nazwa_wejsciowa, L"", MB_OK);
							}

							x++;
						}
						current_level++;
					}
				}

				ShowWindow(Okna[1], SW_HIDE);
				EnableWindow(Okna[0], TRUE);

				if (x > 0)
				{
					swprintf_s(
						nazwa_wejsciowa,
						256,
						L"Succesfully extracted levels:\r\n(%d / %d)\r\n\r\n",
						final_count[0], final_count[1]);

					odtworzDzwiek(GLOS_KAO_PROSZE);

					MessageBox(Okna[0],
						(eUnicodeString(nazwa_wejsciowa) + final_msg).getText(),
						KOMUNIKAT_TYTUL,
						MB_ICONINFORMATION);
				}
			}
			else if ((ID_PRZYCISK + 2) == x)
			{
				EnableWindow(Okna[0], FALSE);
				ShowWindow(Okna[3], SW_SHOW);
			};

			break; // WM_COMMAND
		}
	}; // switch (Komunikat)

	return DefWindowProc(Okno, Komunikat, wParam, lParam);
};


////////////////////////////////////////////////////////////////
// WinApi: Procedura okna ładowania
////////////////////////////////////////////////////////////////
LRESULT CALLBACK CWindow::procedura_druga(HWND Okno, UINT Komunikat, WPARAM wParam, LPARAM lParam)
{
	switch (Komunikat)
	{
		case WM_DESTROY:
		case WM_CLOSE:
		{
			return 1;
		}
	};

	return DefWindowProc(Okno, Komunikat, wParam, lParam);
};


////////////////////////////////////////////////////////////////
// WinApi: Procedura okna z wyborem poziomów
////////////////////////////////////////////////////////////////
LRESULT CALLBACK CWindow::procedura_trzecia(HWND Okno, UINT Komunikat, WPARAM wParam, LPARAM lParam)
{
	switch (Komunikat)
	{
		case WM_CLOSE:
		{
			ShowWindow(Okna[2], SW_HIDE);
			EnableWindow(Okna[0], TRUE);
			SetActiveWindow(Okna[0]);
			return 1;
		}

		case WM_COMMAND:
		{
			int x = LOWORD(wParam);

			if ((x >= ID_PRZYCISKI_SWIATY) && (x < ID_PRZYCISKI_SWIATY + NUM_WORLDS))
			{
				x -= ID_PRZYCISKI_SWIATY;

				bool zaznaczone = true;
				int id_start = 0;
				int id_koniec = 0;

				// Odszukaj początkowy i końcowy identyfikator
				for (int w = 0; w < x; w++)
				{
					id_start += KaoChallengersLevels[w].num;
				}
				id_koniec = id_start + KaoChallengersLevels[x].num;

				// Sprawdź stan wszystkich checkboxów w danym świecie
				for (int i = id_start; i < id_koniec; i++)
				{
					zaznaczone &= (BST_CHECKED == SendMessage(CheckBoxPoziomy[i], BM_GETCHECK, 0, 0));
				}

				// Odwróć zaznaczenia
				x = zaznaczone ? BST_UNCHECKED : BST_CHECKED;
				for (int i = id_start; i < id_koniec; i++)
				{
					SendMessage(CheckBoxPoziomy[i], BM_SETCHECK, (WPARAM)x, 0);
				}
			}

			break;
		} 
	}; //switch(Komunikat)

	return DefWindowProc(Okno, Komunikat, wParam, lParam);
};


////////////////////////////////////////////////////////////////
// WinApi: Procedura okna z wyborem patchów
////////////////////////////////////////////////////////////////
LRESULT CALLBACK CWindow::procedura_czwarta(HWND Okno, UINT Komunikat, WPARAM wParam, LPARAM lParam)
{
	switch (Komunikat)
	{
		case WM_CLOSE:
		{
			ShowWindow(Okna[3], SW_HIDE);
			EnableWindow(Okna[0], TRUE);
			SetActiveWindow(Okna[0]);
			return 1;
		}

		case WM_COMMAND:
		{
			int x = LOWORD(wParam);

			if ((ID_PRZYCISK + 3) == x)
			{
				if (GetOpenFileName(&Ofn))
				{
					// Przygotuj zebrane dane
					CPatchApplier abc(NazwaPlikuOtwieranego);

					// Pokaż okno z komunikatami
					zmienTekstOknaLadowania(L"");
					EnableWindow(Okna[3], FALSE);
					ShowWindow(Okna[1], SW_SHOW);

					for (x = 0; x < NUM_LATKI; x++)
					{
						if (BST_CHECKED == SendMessage(RadioButtonLatki[x], BM_GETCHECK, 0, 0))
						{
							break;
						}
					}

					x = abc.aplikujLatke(x);

					ShowWindow(Okna[1], SW_HIDE);
					EnableWindow(Okna[3], TRUE);

					if (0 == x)
					{
						odtworzDzwiek(GLOS_KAO_PROSZE);

						MessageBox(Okna[3],
							L"Executable patcher has finished all tasks.",
							KOMUNIKAT_TYTUL,
							MB_ICONINFORMATION);
					}
				}
			}
			else if ((ID_PRZYCISK + 4) == x)
			{
				int pliki_skopiowane[4] = {0};
				wchar_t nazwa_wejsciowa[256];
				wchar_t nazwa_koncowa[256];
				eUnicodeString katalog[2];

				if (!sprawdzPoprawnoscSciezek(nazwa_wejsciowa, nazwa_koncowa, katalog))
				{
					return 1;
				}

				/* Kopiuj pliki bezpośrednie i pliki pośrednie */

				zmienTekstOknaLadowania(L"");
				EnableWindow(Okna[3], FALSE);
				ShowWindow(Okna[1], SW_SHOW);

				CFileOperator abc(INVALID_HANDLE_VALUE, false, INVALID_HANDLE_VALUE, false);
				abc.kopiujWaznePliki(pliki_skopiowane, katalog);

				/* Wyświetl komunikat */

				ShowWindow(Okna[1], SW_HIDE);
				EnableWindow(Okna[3], TRUE);

				swprintf_s(
					nazwa_wejsciowa,
					256,
					L"Copied:\r\n\r\n%d of %d original files\r\n\r\n%d of %d new engine files",
					pliki_skopiowane[0], pliki_skopiowane[2],
					pliki_skopiowane[1], pliki_skopiowane[3]);

				odtworzDzwiek(GLOS_KAO_PROSZE);

				MessageBox(Okna[3],
					nazwa_wejsciowa,
					KOMUNIKAT_TYTUL,
					MB_ICONINFORMATION);
			}

			break;
		}
	}; //switch(Komunikat)

	return DefWindowProc(Okno, Komunikat, wParam, lParam);
};
