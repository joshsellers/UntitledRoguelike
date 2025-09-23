#ifndef _GLOBALS_H
#define _GLOBALS_H

#define DBGBLD

extern unsigned int WINDOW_WIDTH, WINDOW_HEIGHT;
extern float RELATIVE_WINDOW_SIZE;

extern bool FULLSCREEN;

extern float SCALE;
extern int WIDTH;
extern int HEIGHT;

extern bool VSYNC_ENABLED;

extern float SFX_VOLUME;
extern float MUSIC_VOLUME;

extern bool DAMAGE_PARTICLES_ENABLED;

extern bool AUTOSAVE_ENABLED;
extern long long AUTOSAVE_INTERVAL_SECONDS;

extern bool BACKUP_ENABLED;

extern bool DISABLE_ACHIEVEMENTS;
extern bool DISABLE_STATS;
extern bool DISABLE_UNLOCKS;

extern bool HARD_MODE_ENABLED;

extern bool MID_GAME_PERF_BOOST;

extern bool BENCHMARK_TERRAIN_AND_BIOME_GEN;

extern bool STEAMAPI_INITIATED;

extern float PLAYER_SCORE;

extern bool USING_MOUSE;

extern bool DIAGONAL_MOVEMENT_ENABLED;

extern bool MOVEMENT_RESETS_AIM;

extern bool AUTO_RELOAD_ENABLED;

extern bool UPCOMING_FEATURES_ENABLED;

extern constexpr int SAVE_FILE_NOT_SELECTED = -1;
extern int SELECTED_SAVE_FILE;

extern bool DEBUG_MODE;

#endif
