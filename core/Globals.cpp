#include "Globals.h"

unsigned int WINDOW_WIDTH = 0;
unsigned int WINDOW_HEIGHT = 0;

bool FULLSCREEN = false;
float RELATIVE_WINDOW_SIZE = FULLSCREEN ? 1 : 0.75;

float SCALE = 1.5; 
int WIDTH = 384 * SCALE; // 360 // 352
int HEIGHT = 224 * SCALE; // 200
// * 2 is good here if we can improve chunk gen time a lot

bool VSYNC_ENABLED = false;

bool BENCHMARK_TERRAIN_AND_BIOME_GEN = false;

float PLAYER_SCORE = 1.f;

bool USING_MOUSE = true;

#ifdef DBGBLD
bool DEBUG_MODE = true;
#endif

#ifndef DBGBLD
bool DEBUG_MODE = false;
#endif