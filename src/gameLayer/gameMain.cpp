#include "gameMain.hpp"
#include <raylib.h>
#include <fstream>
#include <iostream>
#include <random>

// --- Runtime settings controlled by the ImGui menu ---
bool animate = false;             // Whether the grid re-randomizes itself over time
int frameCount = 0;               // Counts frames since last regeneration (for animation timing)
uint16_t frameInterval = 60;      // Regenerate every N frames when animating
int generatorSelection = 1;       // 1 = good RNG, 2 = bad RNG, 3 = probability-based
uint32_t cellSize = 10;           // Size of each cell in pixels
uint32_t extractBit = 8;          // Which bit to extract from the bad RNG's output
float colorPercentage = 0.5f;     // Percent chance of there being a certain color
int rows = WIN_WIDTH / cellSize;  // Number of grid columns (horizontal cells)
int cols = WIN_HEIGHT / cellSize; // Number of grid rows (vertical cells)

// A deliberately poor Linear Congruential Generator (LCG), used to
// visualize the patterns that emerge from low-quality random numbers.
// Each call updates state = state * A + C and returns the new state.
//
// Low bits of an LCG are very non-random:
//   Bit 0 flips every call (period 2), bit 1 has period 4, bit N has period 2^(N+1).
//   High bits look much more random. That's why (badRng() >> extractBit) & 1
//   varies in quality depending on which bit you pick.
//   Ex. (badRng() >> 4) & 1 is more predictable than (badRng() >> 12) & 1
// 
// Warning: if rows * cols is a multiple of 2^(extractBit+1), the animation will
// appear completely frozen. Each regeneration lands at the exact same point in
// the bit cycle, so every regeneration produces the identical image. Change
// cellSize or extractBit to break the alignment.
struct BadRng {
	uint32_t seed = 42;
	uint32_t operator()() { return seed = seed * 1103515245 + 12345; }
};

// All mutable game state lives in here so we can reset/resize it cleanly.
struct GameData
{
	uint32_t seed = 42;                          // Seed used to initialize the good RNG
	std::ranlux24_base rng{ seed };              // High-quality pseudo-random engine
	std::uniform_int_distribution<int> dist{ 0, 1 }; // Produces 0 or 1 uniformly
	BadRng badRng{};                             // Low-quality generator for comparison
	std::vector<std::vector<int>> matrix;        // The grid of 0s and 1s we draw
	Color colors[2] = { WHITE, BLACK };          // colors[0] -> 1, colors[1] -> 0
} gameData;

// Reseed the good RNG so that regenerating with the same seed gives the same image.
void setState(uint32_t seed)
{
	gameData.seed = seed;
	gameData.rng.seed(seed);
}

// Update the two cell colors used when drawing the grid.
void setColors(Color c0, Color c1)
{
	gameData.colors[0] = c0;
	gameData.colors[1] = c1;
}

// Recompute grid dimensions and resize the matrix to match the current cellSize.
// Must be called any time cellSize changes.
void resize()
{
	rows = WIN_WIDTH / cellSize;
	cols = WIN_HEIGHT / cellSize;
	gameData.matrix.assign(rows, std::vector<int>(cols, 0));
}

// Get a uniformly distributed float in [min, max) using the good RNG.
float getRandomFloat(std::ranlux24_base& rng, float min, float max)
{
	std::uniform_real_distribution<float> dist(min, max);
	return dist(rng);
}

// Return true with the given probability (e.g. 0.25 -> 25% chance).
bool getRandomChance(std::ranlux24_base& rng, float chance)
{
	float dice = getRandomFloat(rng, 0.0f, 1.0f);
	return dice <= chance;
}

// Fill the entire matrix with fresh random values using the selected generator.
bool initGame()
{
	resize(); // Make sure matrix is sized correctly before writing into it

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (generatorSelection == 1)
				// Good RNG: genuinely random 0 or 1 per cell
				gameData.matrix[i][j] = gameData.dist(gameData.rng);
			else if (generatorSelection == 2)
				// Bad RNG: extract one bit from the LCG output.
				// Low bits show obvious patterns; higher bits hide them more.
				gameData.matrix[i][j] = (gameData.badRng() >> extractBit) & 1;
			else
				// Probability mode: (colorPercentage)% of cells become 1
				gameData.matrix[i][j] = getRandomChance(gameData.rng, colorPercentage);
		}
	}
	return true;
}

// Runs every frame. Optionally re-randomizes the grid on an interval, then draws it.
bool updateGame()
{
	float deltaTime = GetFrameTime();
	if (deltaTime > 0.05f) deltaTime = 0.05f; // clamp to 20fps minimum

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			// Re-randomize each cell every frameInterval frames (only when animating)
			if (animate)
				if (frameCount == frameInterval)
					if (generatorSelection == 1)
						gameData.matrix[i][j] = gameData.dist(gameData.rng);
					else if (generatorSelection == 2)
						gameData.matrix[i][j] = (gameData.badRng() >> extractBit) & 1;
					else
						gameData.matrix[i][j] = getRandomChance(gameData.rng, colorPercentage);

			// Draw the cell using color[0] for 1, color[1] for 0
			if (gameData.matrix[i][j] == 1)
				DrawRectangle(i * cellSize, j * cellSize, cellSize, cellSize, gameData.colors[0]);
			else
				DrawRectangle(i * cellSize, j * cellSize, cellSize, cellSize, gameData.colors[1]);
		}
	}

	// Reset frame counter once the interval is reached so animation keeps firing.
	// Without this the counter would eventually overflow and animation would stop.
	if (frameCount % frameInterval == 0)
		frameCount = 0;
	frameCount++;

	DrawText(TextFormat("%i FPS", GetFPS()), 0, 0, 30, RED);

	return true;
}

// Called on shutdown — writes a marker to a file so we can confirm clean exit.
void closeGame()
{
	std::ofstream f(RESOURCES_PATH "f.txt");
	f << "CLOSED\n";
	f.close();
}
