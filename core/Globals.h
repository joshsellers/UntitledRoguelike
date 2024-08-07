#ifndef _GLOBALS_H
#define _GLOBALS_H

// DON'T FORGET replace MAJOR_RELEASE with permanent values upon release (items, biomes)
#define DBGBLD

extern unsigned int WINDOW_WIDTH, WINDOW_HEIGHT;
extern float RELATIVE_WINDOW_SIZE;

extern bool FULLSCREEN;

extern float SCALE;
extern int WIDTH;
extern int HEIGHT;

extern bool VSYNC_ENABLED;

extern bool AUTOSAVE_ENABLED;
extern long long AUTOSAVE_INTERVAL_SECONDS;

extern bool BENCHMARK_TERRAIN_AND_BIOME_GEN;

extern float PLAYER_SCORE;

extern bool USING_MOUSE;

extern bool DIAGONAL_MOVEMENT_ENABLED;

extern bool UPCOMING_FEATURES_ENABLED;

extern bool DEBUG_MODE;

#endif
