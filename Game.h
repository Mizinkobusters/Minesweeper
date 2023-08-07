#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include <string>
#include <vector>
#include <unordered_map>

const int PANEL_SIZE = 32;

const int WINDOW_POS_X = 100;
const int WINDOW_POS_Y = 100;
const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;

const int MINE_NUM = 99;
const int FLAG_NUM = MINE_NUM;
const int BOARD_WIDTH = 30;
const int BOARD_HEIGHT = 16;

class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void Shutdown();
	void LoadData();
	void UnloadData();

	SDL_Texture* GetTexture(const std::string& fileName);

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	void InitGame();
	void GenerateBoard(int exceptX, int exceptY);
	void OpenPanel(int x, int y);
	bool CheckBoard();
	bool CheckGameEnd() { return mIsGameOver || mIsGameClear; }

	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;

	SDL_Point mMousePos;
	Uint32 mTicksCount;
	bool mIsRunning;

	int mPutFlagsNum;
	bool mIsFirstClick;
	bool mIsGameClear;
	bool mIsGameOver;

	bool mOpenBoard[BOARD_WIDTH + 2][BOARD_HEIGHT + 2];		// マス目が空いているか判別するフラグ盤面
	bool mMineBoard[BOARD_WIDTH + 2][BOARD_HEIGHT + 2];		// 爆弾があるか判別するフラグ盤面
	bool mFlagBoard[BOARD_WIDTH + 2][BOARD_HEIGHT + 2];		// フラグが置いてあるか判別するフラグ盤面
	int mMineNumBoard[BOARD_WIDTH + 2][BOARD_HEIGHT + 2];	// 周りの爆弾の数を格納する盤面

	std::unordered_map<std::string, SDL_Texture*> mTextures;
	SDL_Rect mPanel;
};