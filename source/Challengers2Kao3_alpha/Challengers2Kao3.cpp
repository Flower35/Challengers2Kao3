#include <Challengers2Kao3_alpha/Challengers2Kao3.h>
#include <Challengers2Kao3_alpha/WinApi.h>


////////////////////////////////////////////////////////////////
// Nazwy poziomów z Kao Challengers
////////////////////////////////////////////////////////////////

StIniLevel::StIniLevel(const wchar_t* a, const wchar_t* b)
{
	this->pak = a;
	this->name = b;
};

StIniWorld::StIniWorld(const wchar_t* a, int b, const StIniLevel* c)
{
	this->name = a;
	this->num = b;
	this->levels = c;
};

const StIniLevel KaoChallengersWorld1[WORLD1_NUM_LEVELS] = 
{
	StIniLevel(L"21_ladownia", L"Statek"),
	StIniLevel(L"00_hub", L"Mroczne Doki")
};

const StIniLevel KaoChallengersWorld2[WORLD2_NUM_LEVELS] =
{
	StIniLevel(L"02_forest_of_canada", L"Bobrowy Las"),
	StIniLevel(L"03_forest_of_canada_2", L"Ucieczka"),
	StIniLevel(L"05_wielkie_drzewa", L"Wielkie Drzewa"),
	StIniLevel(L"06_wodospady", L"Sp³yw"),
	StIniLevel(L"22_boss_szaman", L"Jaskinia Szamana")
};

const StIniLevel KaoChallengersWorld3[WORLD3_NUM_LEVELS] =
{
	StIniLevel(L"07_igloo_village", L"Zimowa Wioska"),
	StIniLevel(L"08_ice_cave", L"Lodowa Jaskinia"),
	StIniLevel(L"10_snowboard", L"Szalona Jazda"),
	StIniLevel(L"11_kopalnie_krysztalow", L"Kopalnie kryszta³ów"),
	StIniLevel(L"24_boss_kolejarz", L"Stacja")
};

const StIniLevel KaoChallengersWorld4[WORLD4_NUM_LEVELS] =
{
	StIniLevel(L"19_orka", L"Wyœcig"),
	StIniLevel(L"13_water", L"Niebezpieczna rafa"),
	StIniLevel(L"16_water_ucieczka_rekiny", L"G³êboki ocean"),
	StIniLevel(L"23_boss_octopus", L"Oœmiornica")
};

const StIniLevel KaoChallengersWorld5[WORLD5_NUM_LEVELS] =
{
	StIniLevel(L"14_kladki", L"Podró¿ na wyspê"),
	StIniLevel(L"15_wyspa", L"Wyspa skarbów"),
	StIniLevel(L"17_wulkan", L"Wulkan"),
	StIniLevel(L"20_boss_pirat", L"Piracka przystañ")
};

const StIniLevel KaoChallengersWorld6[WORLD6_NUM_LEVELS] =
{
	StIniLevel(L"26_ruins", L"Lost Village"),
	StIniLevel(L"27_the_chase", L"The Chase"),
	StIniLevel(L"28_swamps", L"Swamps"),
	StIniLevel(L"29_witchs_hut", L"Witch’s Hut")
};

const StIniLevel KaoChallengersWorld7[WORLD7_NUM_LEVELS] =
{
	StIniLevel(L"12_port", L"Opuszczone miasto"),
	StIniLevel(L"18_port2", L"Galeon Huntera"),
	StIniLevel(L"25_hunter", L"Decyduj¹ce starcie")
};

const StIniLevel KaoChallengersWorld8[WORLD8_NUM_LEVELS] =
{
	StIniLevel(L"bonus_skakanka", L"Bonus: Skakanka"),
	StIniLevel(L"bonus_drzewo", L"Bonus: Drzewa"),
	StIniLevel(L"bonus_strzelnica", L"Bonus: Strzelanie"),
	StIniLevel(L"bunus_bieg", L"Bonus: Wyœcig"),
	StIniLevel(L"bonus_baseball", L"Bonus: Mini Baseball"),
};

const StIniLevel KaoChallengersWorld9[WORLD9_NUM_LEVELS] = 
{
	StIniLevel(L"mp_start", L"Lobby (MP_START)"),
	StIniLevel(L"mp_01", L"The island"),
	StIniLevel(L"mp_02", L"The pyramid"),
	StIniLevel(L"mp_03", L"The dungeon"),
	StIniLevel(L"mp_04", L"Winter battle"),
	StIniLevel(L"mp_05", L"The castle"),
	StIniLevel(L"mp_06", L"The palace"),
	StIniLevel(L"mp_07", L"The galleon"),
	StIniLevel(L"mp_08", L"Lava world"),
	StIniLevel(L"mp_09", L"Dark necropolis"),
	StIniLevel(L"mp_snowboard", L"Down the hill"),
	StIniLevel(L"mp_bobslej", L"Crazy pipe 1"),
	StIniLevel(L"mp_RaceBoat", L"Water challenge"),
	StIniLevel(L"mp_bobslej02", L"Crazy pipe 2"),
	StIniLevel(L"mp_snowboard02", L"Down the hill 2")
};

const StIniWorld KaoChallengersLevels[NUM_WORLDS] =
{
	StIniWorld(
		L"£adownia",
		WORLD1_NUM_LEVELS,
		KaoChallengersWorld1),
	StIniWorld(
		L"Las Kanadyjski",
		WORLD2_NUM_LEVELS,
		KaoChallengersWorld2),
	StIniWorld(
		L"Zimowa Kraina",
		WORLD3_NUM_LEVELS,
		KaoChallengersWorld3),
	StIniWorld(
		L"Œwiat Wodny",
		WORLD4_NUM_LEVELS,
		KaoChallengersWorld4),
	StIniWorld(
		L"Tropiki",
		WORLD5_NUM_LEVELS,
		KaoChallengersWorld5),
	StIniWorld(
		L"WiedŸma",
		WORLD6_NUM_LEVELS,
		KaoChallengersWorld6),
	StIniWorld(
		L"Port",
		WORLD7_NUM_LEVELS,
		KaoChallengersWorld7),
	StIniWorld(
		L"Bonusy",
		WORLD8_NUM_LEVELS,
		KaoChallengersWorld8),
	StIniWorld(
		L"Multiplayer",
		WORLD9_NUM_LEVELS,
		KaoChallengersWorld9)
};


////////////////////////////////////////////////////////////////
// THROW_ERROR_MESSAGE (Zastrze¿enie)
////////////////////////////////////////////////////////////////

EKomunikat::EKomunikat(const wchar_t *msg, int type)
{
	Tekst = msg;
	switch (type)
	{
		case 1:
		{
			Rodzaj = MB_ICONWARNING;
			break;
		}
		case 2:
		{
			Rodzaj = MB_ICONINFORMATION;
			break;
		}
		default:
		{
			Rodzaj = MB_ICONERROR;
			type = 0;
			break;
		}
	}

	CWindow::odtworzDzwiek(type);
};

void EKomunikat::pokazKomunikat(eUnicodeString header)
{
	MessageBox(
		CWindow::Okna[0],
		(header + Tekst).getText(),
		KOMUNIKAT_TYTUL,
		Rodzaj);
};
