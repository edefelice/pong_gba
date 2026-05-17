#include <string.h>

#include "../include/registers.h"
#include "../include/all_gfx.h"

// --- Game constants ---
#define DEAD_ZONE      4
#define PADDLE_W       8
#define PADDLE_H      32
#define BALL_SIZE      8
#define WIN_SCORE      11
#define BASE_SPEED     2
#define MAX_BALL_SPEED 6

// --- Game state ---
u32 player_score;
u32 ai_score;
int game_over;
int hit_counter;

s32 ball_x;
s32 ball_y;
s32 ball_speed_x;
s32 ball_speed_y;

s32 lpaddle_x;
s32 lpaddle_y;
s32 lpaddle_speed;

s32 rpaddle_x;
s32 rpaddle_y;
s32 rpaddle_speed;

typedef struct tagOBJ_ATTR
{
    u16 attr0;
    u16 attr1;
    u16 attr2;
    s16 fill;
} __attribute__((aligned(4))) OBJ_ATTR;

OBJ_ATTR oam_buffer[128];

int rect_overlap(int ax, int ay, int aw, int ah,
                 int bx, int by, int bw, int bh) {
    return (ax < bx + bw) && (ax + aw > bx) &&
           (ay < by + bh) && (ay + ah > by);
}

void draw_score (int player, int ai) {
    int font_base = courtTilesLen / 32 + 1;
    int blank_tile = 0;

    // player score on top left
    MEM_VRAM_SCREENBLOCK(28)[32 + 2] = (player >= 10) ? font_base + (player / 10) : blank_tile;
    MEM_VRAM_SCREENBLOCK(28)[32 + 3] = font_base + (player % 10);

    // ai score on top right
    MEM_VRAM_SCREENBLOCK(28)[32 + 27] = (ai >= 10) ? font_base + (ai / 10) : blank_tile;
    MEM_VRAM_SCREENBLOCK(28)[32 + 28] = font_base + (ai % 10);

}

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
    
    lpaddle_x = 16;
    lpaddle_y = SCREEN_H / 2 - PADDLE_H / 2;
    lpaddle_speed = 2;
    
    rpaddle_x = SCREEN_W - 2 * 16;
    rpaddle_y = SCREEN_H / 2 - PADDLE_H / 2;
    rpaddle_speed = 1;
    
    MEM_PAL_BG[0] = 0x0000;
    draw_score(0, 0);
}

void update_ai_paddle(void) {

    if (ball_speed_x > 0) {
        // The ball is coming, predict y position
        int dx = rpaddle_x - ball_x;
        int frames_to_reach = dx / ball_speed_x;
        int error = (frames_to_reach % 51);
        int predicted_y = ball_y + ball_speed_y * error;

        // Bounce on walls case
        if (predicted_y < 0) {
            predicted_y = -predicted_y;
        }
        if (predicted_y > SCREEN_H - BALL_SIZE) {
            predicted_y = 2 * (SCREEN_H - BALL_SIZE) - predicted_y;
        }

        int target = predicted_y - PADDLE_H / 2;
        int diff = target - rpaddle_y;
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

// Direction: -1 = left paddle (ball was going left, now goes right)
//             1 = right paddle (ball was going right, now goes left)
void handle_paddle_hit(s32 paddle_y, int direction) {
    ball_speed_x = -ball_speed_x;
    
    // Acceleration (limit at MAX_BALL_SPEED) every 2 ai hits
    // Acceleration every 2 paddle hits (any side)
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
    } else if (hit_offset < -4) {
        ball_speed_y = -1;
    } else if (hit_offset > 10) {
        ball_speed_y = 2;
    } else if (hit_offset > 4) {
        ball_speed_y = 1;
    } else {
        ball_speed_y = 0;
    }
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

void render_sprites(void) {
    oam_buffer[0].attr0 = ATTR0_Y(ball_y) | ATTR0_SQUARE | ATTR0_4BPP;
    oam_buffer[0].attr1 = ATTR1_X(ball_x) | ATTR1_SIZE_8x8;
    
    oam_buffer[1].attr0 = ATTR0_Y(lpaddle_y) | ATTR0_TALL | ATTR0_4BPP;
    oam_buffer[1].attr1 = ATTR1_X(lpaddle_x) | ATTR1_SIZE_8x32;
    
    oam_buffer[2].attr0 = ATTR0_Y(rpaddle_y) | ATTR0_TALL | ATTR0_4BPP;
    oam_buffer[2].attr1 = ATTR1_X(rpaddle_x) | ATTR1_SIZE_8x32;
    
    memcpy(MEM_OAM, oam_buffer, sizeof(oam_buffer));
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

int main(void) {

    // Hide all sprites
    for (int i = 0; i < 128; i++) {
        oam_buffer[i].attr0 = ATTR0_HIDE;
        oam_buffer[i].attr1 = 0;
        oam_buffer[i].attr2 = 0;
        oam_buffer[i].fill  = 0;
    }

    // Load sprite tiles in VRAM
    memcpy(MEM_VRAM_OBJ, ballTiles, ballTilesLen); // ball
    memcpy(&MEM_VRAM_OBJ[ballTilesLen/2], paddleTiles, paddleTilesLen); // left paddle
    // Load sprite palette
    memcpy(MEM_PAL_OBJ, ballPal, ballPalLen);

    // Load background tiles in CBB0
    memcpy(MEM_VRAM_CHARBLOCK(0), courtTiles, courtTilesLen);
    // Load background tilemap in SBB 28
    memcpy(MEM_VRAM_SCREENBLOCK(28), courtMap, courtMapLen);
    // Load background palette
    memcpy(MEM_PAL_BG, courtPal, courtPalLen);

    // Load font tiles in VRAM
    int font_base_tile = courtTilesLen / 32;
    memcpy(&MEM_VRAM_CHARBLOCK(0)[(font_base_tile * 16)], fontTiles, fontTilesLen);

    // Set OAM
    // Ball at the centre of the screen.
    oam_buffer[0].attr0 = ATTR0_Y(ball_y) | ATTR0_SQUARE | ATTR0_4BPP;
    oam_buffer[0].attr1 = ATTR1_X(ball_x) | ATTR1_SIZE_8x8;
    oam_buffer[0].attr2 = ATTR2_ID(0) | ATTR2_PALBANK(0);

    // left paddle
    oam_buffer[1].attr0 = ATTR0_Y(lpaddle_y) | ATTR0_TALL | ATTR0_4BPP;
    oam_buffer[1].attr1 = ATTR1_X(lpaddle_x) | ATTR1_SIZE_8x32;
    oam_buffer[1].attr2 = ATTR2_ID(1) | ATTR2_PALBANK(0);

    // right paddle
    oam_buffer[2].attr0 = ATTR0_Y(rpaddle_y) | ATTR0_TALL | ATTR0_4BPP;
    oam_buffer[2].attr1 = ATTR1_X(rpaddle_x) | ATTR1_SIZE_8x32;
    oam_buffer[2].attr2 = ATTR2_ID(1) | ATTR2_PALBANK(0);

    memcpy(MEM_OAM, oam_buffer, sizeof(oam_buffer));

    // configure BG0
    REG_BG0CNT = BG_CBB(0) | BG_SBB(28) | BG_4BPP | BG_REG_32x32 | BG_PRIO(1);
    // Set display mode 0, sprite on, mapping 1D, BG0
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;

    reset_game();

    while(1) {
        // VBlank wait
        while (REG_VCOUNT >= SCREEN_H);
        while (REG_VCOUNT < SCREEN_H);

        // if game over happens, background flashes
        if (game_over) {
            handle_game_over();
            continue;
        }

        static int frame_counter = 0;
        frame_counter++;
        if (frame_counter < 2) {
            render_sprites();
            continue;
        }
        frame_counter = 0;

        update_ball();
        update_ai_paddle();
        update_player_paddle();

        render_sprites();
    }
    return 0;
    
}