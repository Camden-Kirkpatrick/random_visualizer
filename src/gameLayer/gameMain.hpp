#pragma once
#include <cstdint>
#include <raylib.h>

#define WIN_WIDTH 1920
#define WIN_HEIGHT 1080
#define FPS 60

extern bool animate;
extern int frameCount;
extern uint16_t frameInterval;
extern int generatorSelection;
extern uint32_t cellSize;
extern int rows;
extern int cols;
extern uint32_t extractBit;
extern float colorPercentage;

bool initGame();
bool updateGame();
void closeGame();
void setState(uint32_t seed);
void setColors(Color c0, Color c1);
void resize();