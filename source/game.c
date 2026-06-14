#include <string.h>
#include "registers.h"
#include "game.h"
#include "graphics.h"
#include "ball.h"
#include "paddle.h"
#include "court.h"
#include "font.h"

// --- Game state (definitions) ---
int player_score;
int ai_score;
int game_over;

int ball_x;
int ball_y;
int ball_speed_x;
int ball_speed_y;

int lpaddle_x;
int lpaddle_y;
int lpaddle_speed;

int rpaddle_x;
int rpaddle_y;
int rpaddle_speed;

int hit_counter;

OBJ_ATTR oam_buffer[128];

// --- Private helpers ---
static int rect_overlap(int ax, int ay, int aw, int ah,
                 int bx, int by, int bw, int bh) {
    return (ax < bx + bw) && (ax + aw > bx) &&
           (ay < by + bh) && (ay + ah > by);
}

static void draw_score (int player, int ai) {
    int font_base = courtTilesLen / 32 + 1;
    int blank_tile = 0;

    // player score on top left
    MEM_VRAM_SCREENBLOCK(28)[32 + 2] = (player >= 10) ? font_base + (player / 10) : blank_tile;
    MEM_VRAM_SCREENBLOCK(28)[32 + 3] = font_base + (player % 10);

    // ai score on top right
    MEM_VRAM_SCREENBLOCK(28)[32 + 27] = (ai >= 10) ? font_base + (ai / 10) : blank_tile;
    MEM_VRAM_SCREENBLOCK(28)[32 + 28] = font_base + (ai % 10);

}

// Direction: -1 = left paddle (ball was going left, now goes right)
//             1 = right paddle (ball was going right, now goes left)
void handle_paddle_hit(s32 paddle_y, int direction) {
    ball_speed_x = -ball_speed_x;
    
    // Acceleration (limit at MAX_BALL_SPEED) every 2 paddle hits (any side)
    hit_counter++;
    if (hit_counter >= 8) {
        hit_counter = 0;
        if (direction < 0 && ball_speed_x < MAX_BALL_SPEED) {
            ball_speed_x++;
        }
        if (direction > 0 && ball_speed_x > -MAX_BALL_SPEED) {
            ball_speed_x--;
        }
    }
    
    // Bounce angle depending on where paddle is hit
    int hit_offset = (ball_y + BALL_SIZE / 2) - (paddle_y + PADDLE_H / 2);
    if (hit_offset < -10) {
        ball_speed_y = -2;
    }
    else if (hit_offset < -4) {
        ball_speed_y = -1;
    }
    else if (hit_offset > 10) {
        ball_speed_y = 2;
    }
    else if (hit_offset > 4) {
        ball_speed_y = 1;
    }
    else {
        ball_speed_y = 0;
    }
}

// --- Public functions ---
void reset_game(void) {
    player_score = 0;
    ai_score = 0;
    game_over = 0;
    hit_counter = 0;
    u16 r = REG_VCOUNT;
    
    ball_x = SCREEN_W / 2 - BALL_SIZE / 2;
    ball_y = SCREEN_H / 2 - BALL_SIZE / 2;
    ball_speed_x = (r & 1) ? BASE_SPEED : -BASE_SPEED;
    ball_speed_y = (r & 2) ? BASE_SPEED : -BASE_SPEED;
    
    lpaddle_x = PADDLE_MARGIN;
    lpaddle_y = SCREEN_H / 2 - PADDLE_H / 2;
    lpaddle_speed = 2;
    
    rpaddle_x = SCREEN_W - PADDLE_MARGIN - PADDLE_W;
    rpaddle_y = SCREEN_H / 2 - PADDLE_H / 2;
    rpaddle_speed = 2;
    
    MEM_PAL_BG[0] = 0x0000;
    draw_score(0, 0);
}

void update_ball(void) {
    // Update position
    ball_x += ball_speed_x;
    ball_y += ball_speed_y;
    
    // Left paddle collision
    if (rect_overlap(ball_x, ball_y, BALL_SIZE, BALL_SIZE,
                     lpaddle_x, lpaddle_y, PADDLE_W, PADDLE_H)) {
        ball_x = lpaddle_x + PADDLE_W;
        handle_paddle_hit(lpaddle_y, -1);
    }
    
    // Right paddle collision
    if (rect_overlap(ball_x, ball_y, BALL_SIZE, BALL_SIZE,
                     rpaddle_x, rpaddle_y, PADDLE_W, PADDLE_H)) {
        ball_x = rpaddle_x - BALL_SIZE;
        handle_paddle_hit(rpaddle_y, 1);
    }
    
    // Bounce on top/bottom walls
    if (ball_y < 0 || ball_y > SCREEN_H - BALL_SIZE) {
        ball_speed_y = -ball_speed_y;
    }
    
    // AI scores (ball exits left)
    if (ball_x <= 0) {
        ai_score++;
        ball_x = SCREEN_W / 2 - BALL_SIZE / 2;
        ball_y = SCREEN_H / 2 - BALL_SIZE / 2;
        ball_speed_x = -BASE_SPEED;
        ball_speed_y = (REG_VCOUNT & 1) ? BASE_SPEED : -BASE_SPEED;
        draw_score(player_score, ai_score);
        if (ai_score >= WIN_SCORE) {
            game_over = 2;
        }
        hit_counter = 0;
    }
    // Player scores (ball exits right)
    if (ball_x >= SCREEN_W - BALL_SIZE) {
        player_score++;
        ball_x = SCREEN_W / 2 - BALL_SIZE / 2;
        ball_y = SCREEN_H / 2 - BALL_SIZE / 2;
        ball_speed_x = BASE_SPEED;
        ball_speed_y = (REG_VCOUNT & 1) ? BASE_SPEED : -BASE_SPEED;
        draw_score(player_score, ai_score);
        if (player_score >= WIN_SCORE) {
            game_over = 1;
        }
        hit_counter = 0;
    }
}

void update_ai_paddle(void) {

    if (ball_speed_x > 0 && ball_x > SCREEN_W / 3) {
        // The ball is coming, predict y position
        int dx = rpaddle_x - ball_x;
        int frames_to_reach = dx / ball_speed_x;
        int predicted_y = ball_y + ball_speed_y * frames_to_reach;

        // Bounce on walls case
        if (predicted_y < 0) {
            predicted_y = -predicted_y;
        }
        if (predicted_y > SCREEN_H - BALL_SIZE) {
            predicted_y = 2 * (SCREEN_H - BALL_SIZE) - predicted_y;
        }

        int target = predicted_y - PADDLE_H / 2; // center paddle on predicted ball y
        int diff = target - rpaddle_y;
        // Move toward terget with tolerance to prevent jitter
        if (diff > rpaddle_speed) {
            rpaddle_y += rpaddle_speed;
        }
        else if (diff < -rpaddle_speed) {
            rpaddle_y -= rpaddle_speed;
        }
    }
    else {
        // The ball is going away, come back to the center
        int center = SCREEN_H / 2 - PADDLE_H / 2;
        if (rpaddle_y < center) {
            rpaddle_y += rpaddle_speed;
        }
        else if (rpaddle_y > center) {
            rpaddle_y -= rpaddle_speed;
        }
    }
    // Clipping
    if (rpaddle_y < DEAD_ZONE) {
        rpaddle_y = DEAD_ZONE;
    }
    if (rpaddle_y > SCREEN_H - PADDLE_H - DEAD_ZONE) {
        rpaddle_y = SCREEN_H - PADDLE_H - DEAD_ZONE;
    }
}

void update_player_paddle(void) {
    u16 keys = ~REG_KEYINPUT & KEY_MASK;
        if (keys & KEY_UP) {
            lpaddle_y -= lpaddle_speed;
            if (lpaddle_y < DEAD_ZONE) {
                lpaddle_y = DEAD_ZONE;
            }
        }

        if (keys & KEY_DOWN) {
            lpaddle_y += lpaddle_speed;
            if (lpaddle_y > SCREEN_H - PADDLE_H - DEAD_ZONE) {
                lpaddle_y = SCREEN_H - PADDLE_H - DEAD_ZONE;
            }
        }
}

void handle_game_over(void) {
    static int win_blink = 0;
    win_blink++;
    int color = (win_blink / 30) & 1;
    if (game_over == 1) {
        MEM_PAL_BG[0] = color ? 0x03E0 : 0x0000;  // green / black
    } else {
        MEM_PAL_BG[0] = color ? 0x001F : 0x0000;  // red / black
    }
    
    render_sprites();
    
    u16 keys = ~REG_KEYINPUT & KEY_MASK;
    if (keys & KEY_A) {
        reset_game();
    }
}