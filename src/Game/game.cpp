#include "game.hpp"

HWND Game::hwnd;
RECT Game::rect;
Processing::HDC_t Game::gdc;

Game::Side Game::side;
Game::Role Game::role;

void Game::Init() {
	printf("Looking for game window...\n");

	// get the window handle and rectangle info for the game window
	hwnd = NULL;
	while (hwnd == NULL) {
		hwnd = Utils::FindHwnd("League of Legends (TM) Client");
		if (hwnd)
			GetWindowRect(hwnd, &rect);

		Sleep(500);
	}
	printf("Game window found!\n");

	Sleep(1000);

	// get the devide context for the window
	gdc = Processing::HDC_t(GetDC(hwnd), 1280, 720);

	// just in case the window is in the background
	SetForegroundWindow(hwnd);
	SetActiveWindow(hwnd);
	SetFocus(hwnd);

	Play();
}

void Game::Play() {
	printf("Waiting for game...\n");
	Sleep(5000);

	while (!IsReady())
		Sleep(500);

	printf("Game is ready!\n");

	while (Processing::CompareRGB(GetPixel(gdc.hdc, 0, 0), 0, 0, 0))
		Sleep(100);
	
	Sleep(500);

	side = GetSide();
	role = Game::Role::TOP;

	// assume playing riven for testing
	uint8_t lvlup_q[2]{VK_CONTROL, 0x51};
	Input::Keys(2, lvlup_q);

	Sleep(20000);

	// initial move
	if (side == Game::Side::BLUE) {
		if (role == Game::Role::TOP)
			MinimapClick(15, 50, 1);
	}

	Sleep(20000);

	while (true) {
		int x, y;
		Processing::ClosestEnemyMinion(Processing::Screenshot(gdc), x, y);
		Sleep(1000);
	}
}

bool Game::IsReady() {
	COLORREF color = GetPixel(gdc.hdc, 630, 715);
	return !Processing::CompareRGB(color, 0, 11, 19);
}

Game::Side Game::GetSide() {
	cv::Mat img = Processing::Screenshot(gdc);
	double p = Processing::MatchTemplate(img, "bside.png");
	printf("%.2f\n", p);
	if (p > 0.5f)
		return Game::Side::BLUE;
	else
		return Game::Side::RED;
}

void Game::Click(const int rel_x, const int rel_y, const bool right_click) {
	Input::Click(rect.left + rel_x, rect.top + 25 + rel_y, right_click);
}

void Game::MinimapClick(const int rel_x, const int rel_y, const bool right_click) {
	Click(1099 + rel_x, 539 + rel_y, right_click);
}
