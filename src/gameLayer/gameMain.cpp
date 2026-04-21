#include "gameMain.hpp"
#include <raylib.h>
#include <fstream>
#include <iostream>
#include <random>

#define CELL_SIZE    10
#define ROWS         WIN_WIDTH / CELL_SIZE
#define COLS         WIN_HEIGHT / CELL_SIZE

// This random number generator will show a pattern, which is bad
struct BadRng {
	uint32_t state;
	BadRng(uint32_t seed = 42) : state(seed) {}
	uint32_t operator()() { return state = state * 1103515245 + 12345; } // glibc's rand()
	static constexpr uint32_t min() { return 0; }
	static constexpr uint32_t max() { return UINT32_MAX; }
};

struct GameData
{
	// Pseudo random number generator seeded with a true random number
	std::ranlux24_base rng{ std::random_device{}() };
	std::uniform_int_distribution<int> dist{ 0, 1 }; // 0 or 1
	BadRng badRng = {};
	int matrix[ROWS][COLS];
} gameData;


float getRandomFloat(std::ranlux24_base& rng, float min, float max)
{
	std::uniform_real_distribution<float> dist(min, max);
	return dist(rng);
}

bool getRandomChance(std::ranlux24_base& rng, float chance)
{
	float dice = getRandomFloat(rng, 0.0f, 1.0f);
	return dice <= chance;
}

bool initGame()
{
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			gameData.matrix[i][j] = gameData.dist(gameData.rng);

			//gameData.matrix[i][j] = getRandomChance(gameData.rng, 0.1f);

			//gameData.matrix[i][j] = (gameData.badRng() >> 5) & 1;
		}
	}
	return true;
}

bool updateGame()
{
	float deltaTime = GetFrameTime();
	if (deltaTime > 0.05f) deltaTime = 0.05f; // clamp to 20fps minimum

	std::ofstream f(RESOURCES_PATH "f.txt");
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			if (gameData.matrix[i][j] == 1)
				DrawRectangle(i * CELL_SIZE, j * CELL_SIZE, CELL_SIZE, CELL_SIZE, { 255, 255, 255, 255 });
			else
				DrawRectangle(i * CELL_SIZE, j * CELL_SIZE, CELL_SIZE, CELL_SIZE, { 0, 0, 0, 255 });

			f << i * CELL_SIZE << ", " << j * CELL_SIZE << "\n";
		}
	}
	f.close();

	return true;
}

void closeGame()
{
	std::ofstream f(RESOURCES_PATH "f.txt", std::ios::app);
	f << "CLOSED\n";
	f.close();
}