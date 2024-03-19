#include "Globals.h"

unsigned int WINDOW_WIDTH = 0;
unsigned int WINDOW_HEIGHT = 0;

bool FULLSCREEN = false;
float RELATIVE_WINDOW_SIZE = FULLSCREEN ? 1 : 0.75;

int SCALE = 4; // this doesn't do anything
int WIDTH = 360 * 1.5; 
int HEIGHT = 202 * 1.5;

bool BENCHMARK_TERRAIN_AND_BIOME_GEN = false;

bool STEAMAPI_INITIATED = false;

bool IS_MULTIPLAYER_CONNECTED = false;

unsigned int MAX_PACKETS_OUT_PER_SECOND = 60;

float PLAYER_SCORE = 1.f;