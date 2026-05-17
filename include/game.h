#ifndef GAME_H
#define GAME_H

#include "registers.h"

// --- Game constants ---
#define DEAD_ZONE      4
#define PADDLE_W       8
#define PADDLE_H      32
#define BALL_SIZE      8
#define WIN_SCORE     11
#define BASE_SPEED     2
#define MAX_BALL_SPEED 6
#define PADDLE_MARGIN 16

// --- OAM structure ---
typedef struct tagOBJ_ATTR
{
    u16 attr0;
    u16 attr1;
    u16 attr2;
    s16 fill;
} __attribute__((aligned(4))) OBJ_ATTR;

// --- Game state (extern, defined in game.c) ---
extern int player_score;
extern int ai_score;
extern int game_over;

extern int ball_x;
extern int ball_y;
extern int ball_speed_x;
extern int ball_speed_y;

extern int lpaddle_x;
extern int lpaddle_y;
extern int lpaddle_speed;

extern int rpaddle_x;
extern int rpaddle_y;
extern int rpaddle_speed;

extern int hit_counter;

extern OBJ_ATTR oam_buffer[128];

// --- Public game functions ---
void reset_game(void);
void update_ball(void);
void update_ai_paddle(void);
void update_player_paddle(void);
void handle_game_over(void);

#endif