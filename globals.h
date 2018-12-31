#pragma once
//pixel size of the window
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 576
//tile size of the map
#define MAP_WIDTH 64
#define MAP_HEIGHT 36
//local map dimensions - determine how much memory map space used by the brain at one time
#define LMAP_W 64
#define LMAP_H 36
#define FRAMERATE 60
//all distances are squared
#define BASE_HEAR_DIST 4096
#define BASE_NE_DIST 4
#define BASE_FOLLOW_DIST 1024
//
#define BASE_LOS_CONE 3.14159 / 4
//sight range in tiles
#define BASE_SIGHT_RANGE 17

#define TILE_SIZE 16
#define SPRITE_SIZE 16
#define SPEED 1
//how many npcs are generated
#define NPC_NUM 9
//rmap legend (any numbers < RESERVED_RMAP_NUMBERS are not dereferenced)
#define RESERVED_RMAP_NUMBERS 100

#define MAX_ANIMS 64 // 2 ^ NUM_FLAGS
//how large the updates made in the memory map
#define LOOK_AHEAD_DIST 3
//in anytimedstar
//how many unsuccessful loops of findPath or improvePath before resetting or cancelling
#define FUBAR 64
#define HARD_FUBAR 8
#define INFLATION_FACTOR 10
//the percent change in updated maps before certain threshholds
#define MAP_CHANGE_TOLERANCE_LARGE 0.33
#define MAP_CHANGE_TOLERANCE_SMALL 0
#define EPSILON_STEP 1
//
//the additional tiles around actual position considered taken up by sprites
#define PERSONAL_SPACE 1


