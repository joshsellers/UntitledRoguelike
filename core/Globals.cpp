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

float SFX_VOLUME = 1.f;
float MUSIC_VOLUME = 1.f;

bool DAMAGE_PARTICLES_ENABLED = false;

bool AUTOSAVE_ENABLED = true;
long long AUTOSAVE_INTERVAL_SECONDS = 60 * 5;

bool BACKUP_ENABLED = false;

bool DISABLE_ACHIEVEMENTS = false;
bool DISABLE_STATS = false;
bool DISABLE_UNLOCKS = false;

bool UNLOCK_ALL_ITEMS = false;

bool HARD_MODE_ENABLED = false;

bool MID_GAME_PERF_BOOST = false;

bool BENCHMARK_TERRAIN_AND_BIOME_GEN = false;

bool STEAMAPI_INITIATED = false;

float PLAYER_SCORE = 1.f;

bool USING_MOUSE = true;

bool DIAGONAL_MOVEMENT_ENABLED = true;

bool MOVEMENT_RESETS_AIM = true;

bool AUTO_RELOAD_ENABLED = true;

bool UPCOMING_FEATURES_ENABLED = true;

int SELECTED_SAVE_FILE = SAVE_FILE_NOT_SELECTED;

#ifdef DBGBLD
bool DEBUG_MODE = true;
#endif

#ifndef DBGBLD
bool DEBUG_MODE = false;
#endif