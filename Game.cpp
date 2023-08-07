#include "Game.h"
#include <vector>
#include <random>
#include <string>
#include <iostream>
#include <time.h>

Game::Game()
	:mWindow(nullptr)
	, mRenderer(nullptr)
	, mTicksCount(0)
	, mIsRunning(true)
	, mPutFlagsNum(0)
	, mIsFirstClick(true)
	, mIsGameClear(false)
	, mIsGameOver(false)
{

}

bool Game::Initialize()
{
	// SDL ������
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0)
	{
		SDL_Log("Unenable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	mWindow = SDL_CreateWindow(
		"Minesweeper v.1.0",
		WINDOW_POS_X,
		WINDOW_POS_Y,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		0
	);

	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	mRenderer = SDL_CreateRenderer(
		mWindow,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}

	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		SDL_Log("Unable to initialize SDL_Image: %s", SDL_GetError());
		return false;
	}

	LoadData();
	InitGame();
	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::Shutdown()
{
	UnloadData();

	IMG_Quit();

	SDL_DestroyWindow(mWindow);
	SDL_DestroyRenderer(mRenderer);
	SDL_Quit();
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mIsRunning = false;
			break;
		case SDL_MOUSEBUTTONDOWN:
			SDL_GetMouseState(&mMousePos.x, &mMousePos.y);
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				if (CheckGameEnd())
				{
					break;
				}
				int x = mMousePos.x / (PANEL_SIZE + 1);
				int y = mMousePos.y / (PANEL_SIZE + 1);
				if (x > BOARD_WIDTH || y > BOARD_HEIGHT)
				{
					break;
				}

				if (mIsFirstClick)
				{
					mIsFirstClick = false;
					GenerateBoard(x + 1, y + 1);
				}
				OpenPanel(x + 1, y + 1);
				if (mMineBoard[x + 1][y + 1])
				{
					mIsGameOver = true;
				}
			}
			if (event.button.button == SDL_BUTTON_RIGHT)
			{
				if (CheckGameEnd())
				{
					break;
				}
				int x = mMousePos.x / (PANEL_SIZE + 1);
				int y = mMousePos.y / (PANEL_SIZE + 1);
				if (x > BOARD_WIDTH || y > BOARD_HEIGHT)
				{
					break;
				}
				if (mFlagBoard[x + 1][y + 1])
				{
					mFlagBoard[x + 1][y + 1] = false;
					mPutFlagsNum -= 1;
				} 
				else
				{
					if (mPutFlagsNum >= FLAG_NUM) 
					{
						break;
					}
					mFlagBoard[x + 1][y + 1] = true;
					mPutFlagsNum += 1;
				}
			}
			break;
		default:
			break;
		}
	}

	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	if (keyState[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}
	if (keyState[SDL_SCANCODE_SPACE])
	{
		InitGame();
	}
}

void Game::UpdateGame()
{
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}
	mTicksCount = SDL_GetTicks();

	// �Q�[���N���A������
	bool isClear = false;
	isClear = CheckBoard();
	if (isClear)
	{
		mIsGameClear = true;
	}
}

void Game::GenerateOutput()
{
	SDL_SetRenderDrawColor(mRenderer, 127, 127, 127, 255);
	SDL_RenderClear(mRenderer);
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

	for (int y = 1; y <= BOARD_HEIGHT; y++)
	{
		for (int x = 1; x <= BOARD_WIDTH; x++)
		{
			mPanel.x = (x - 1) * (PANEL_SIZE + 1);
			mPanel.y = (y - 1) * (PANEL_SIZE + 1);
			mPanel.w = PANEL_SIZE;
			mPanel.h = PANEL_SIZE;

			SDL_RenderFillRect(mRenderer, &mPanel);

			if (mOpenBoard[x][y])
			{
				switch (mMineNumBoard[x][y])
				{
				case 1:
					SDL_RenderCopy(mRenderer, mTextures["Assets/Number_1.png"], nullptr, &mPanel);
					break;
				case 2:
					SDL_RenderCopy(mRenderer, mTextures["Assets/Number_2.png"], nullptr, &mPanel);
					break;
				case 3:
					SDL_RenderCopy(mRenderer, mTextures["Assets/Number_3.png"], nullptr, &mPanel);
					break;
				case 4:
					SDL_RenderCopy(mRenderer, mTextures["Assets/Number_4.png"], nullptr, &mPanel);
					break;
				case 5:
					SDL_RenderCopy(mRenderer, mTextures["Assets/Number_5.png"], nullptr, &mPanel);
					break;
				case 6:
					SDL_RenderCopy(mRenderer, mTextures["Assets/Number_6.png"], nullptr, &mPanel);
					break;
				case 7:
					SDL_RenderCopy(mRenderer, mTextures["Assets/Number_7.png"], nullptr, &mPanel);
					break;
				case 8:
					SDL_RenderCopy(mRenderer, mTextures["Assets/Number_8.png"], nullptr, &mPanel);
					break;
				case 9:
					SDL_RenderCopy(mRenderer, mTextures["Assets/Number_9.png"], nullptr, &mPanel);
					break;
				case 10:
					SDL_RenderCopy(mRenderer, mTextures["Assets/Number_10.png"], nullptr, &mPanel);
					break;
				default:
					SDL_RenderCopy(mRenderer, mTextures["Assets/Number_0.png"], nullptr, &mPanel);
					break;
				}
			}
			if (mFlagBoard[x][y])
			{
				SDL_RenderCopy(mRenderer, mTextures["Assets/Flag.png"], nullptr, &mPanel);
			}
			if (mMineBoard[x][y] && mOpenBoard[x][y])
			{
				SDL_RenderCopy(mRenderer, mTextures["Assets/Bomb.png"], nullptr, &mPanel);
			}
		}
	}
	SDL_Rect r = { WINDOW_WIDTH / 2 - (WINDOW_WIDTH / 2), WINDOW_HEIGHT / 2 - (240 / 2), WINDOW_WIDTH, 240};
	if (mIsGameClear)
	{
		SDL_RenderCopy(mRenderer, mTextures["Assets/GameClear.png"], nullptr, &r);
	}
	if (mIsGameOver)
	{
		SDL_RenderCopy(mRenderer, mTextures["Assets/GameOver.png"], nullptr, &r);
	}
	SDL_RenderPresent(mRenderer);
}

void Game::LoadData()
{
	GetTexture("Assets/Number_0.png");
	GetTexture("Assets/Number_1.png");
	GetTexture("Assets/Number_2.png");
	GetTexture("Assets/Number_3.png");
	GetTexture("Assets/Number_4.png");
	GetTexture("Assets/Number_5.png");
	GetTexture("Assets/Number_6.png");
	GetTexture("Assets/Number_7.png");
	GetTexture("Assets/Number_8.png");
	GetTexture("Assets/Number_9.png");
	GetTexture("Assets/Number_10.png");
	GetTexture("Assets/Flag.png");
	GetTexture("Assets/Bomb.png");
	GetTexture("Assets/GameOver.png");	
	GetTexture("Assets/GameClear.png");
}

void Game::UnloadData()
{
	for (auto tex : mTextures)
	{
		SDL_DestroyTexture(tex.second);
	}
	mTextures.clear();
}

void Game::InitGame()
{	
	// ����N���b�N�t���O�����Z�b�g
	mIsFirstClick = true;
	// �Q�[���G���h�t���O�����Z�b�g
	mIsGameOver = false;
	mIsGameClear = false;
	// �Ֆʏ�����
	for (int x = 0; x < BOARD_WIDTH + 2; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT + 2; y++)
		{
			mOpenBoard[x][y] = false;
			mMineBoard[x][y] = false;
			mFlagBoard[x][y] = false;
			mMineNumBoard[x][y] = 0;
		}
	}
}

SDL_Texture* Game::GetTexture(const std::string& fileName)
{
	SDL_Texture* tex = nullptr;
	auto iter = mTextures.find(fileName);
	if (iter != mTextures.end())
	{
		tex = iter->second;
	}
	else
	{
		SDL_Surface* surf = IMG_Load(fileName.c_str());
		if (!surf)
		{
			SDL_Log("Failed to load texture file %s", fileName.c_str());
			return nullptr;
		}
		tex = SDL_CreateTextureFromSurface(mRenderer, surf);
		SDL_FreeSurface(surf);
		if (!tex)
		{
			SDL_Log("Failed to convert surface to texture for %s", fileName.c_str());
			return nullptr;
		}
		mTextures.emplace(fileName.c_str(), tex);
	}
	return tex;
}

void Game::GenerateBoard(int exceptX, int exceptY)
{
	// ������������
	srand((unsigned int)time(NULL));

	// ���e��z�u
	for (int i = 0; i < MINE_NUM;)
	{
		int x, y;
		x = rand() % BOARD_WIDTH + 1;
		y = rand() % BOARD_HEIGHT + 1;

		// �����ʒu�������疳��
		if (x == exceptX && y == exceptY)
		{
			continue;
		}
		// ���łɔ��e���ݒu����Ă���Ζ���
		if (mMineBoard[x][y])
		{
			continue;
		}
		// ���e��ݒu
		mMineBoard[x][y] = true;
		// ���e�̎���̐������C���N�������g
		mMineNumBoard[x - 1][y - 1] += 1;
		mMineNumBoard[x][y - 1] += 1;
		mMineNumBoard[x + 1][y - 1] += 1;
		mMineNumBoard[x - 1][y] += 1;
		mMineNumBoard[x + 1][y] += 1;
		mMineNumBoard[x - 1][y + 1] += 1;
		mMineNumBoard[x][y + 1] += 1;
		mMineNumBoard[x + 1][y + 1] += 1;

		i++;
	}
}

void Game::OpenPanel(int x, int y)
{
	if (x < 1 || x > BOARD_WIDTH || y < 1 || y > BOARD_HEIGHT)
	{
		return;
	}
	if (mOpenBoard[x][y])
	{
		return;
	}
	mOpenBoard[x][y] = true;
	if (!mMineBoard[x][y] && !mMineNumBoard[x][y])
	{
		OpenPanel(x - 1, y - 1);
		OpenPanel(x    , y - 1);
		OpenPanel(x + 1, y - 1);
		OpenPanel(x - 1, y    );
		OpenPanel(x + 1, y    );
		OpenPanel(x - 1, y + 1);
		OpenPanel(x    , y + 1);
		OpenPanel(x + 1, y + 1);
	}
}

bool Game::CheckBoard()
{
	for (int x = 1; x <= BOARD_WIDTH; x++)
	{
		for (int y = 1; y <= BOARD_HEIGHT; y++)
		{
			if (!mMineBoard[x][y] && !mOpenBoard[x][y])
			{
				return false;
			}
		}
	}
	return true;
}