#include <string.h>

#include "../include/registers.h"
#include "../include/all_gfx.h"

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

int main(void) {

    // ball position and speed initialization
    s32 ball_x = SCREEN_W / 2 - 4;
    s32 ball_y = SCREEN_H / 2 - 4;
    s32 ball_speed_x = 1;
    s32 ball_speed_y = 1;

    // paddles position and speed initialization
    s32 lpaddle_x = 16;
    s32 lpaddle_y = SCREEN_H / 2 - 16;
    s32 paddle_speed = 2;
     

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

    // Set OAM
    // Ball at the centre of the screen.
    oam_buffer[0].attr0 = ATTR0_Y(ball_y) | ATTR0_SQUARE | ATTR0_4BPP;
    oam_buffer[0].attr1 = ATTR1_X(ball_x) | ATTR1_SIZE_8x8;
    oam_buffer[0].attr2 = ATTR2_ID(0) | ATTR2_PALBANK(0);

    // left paddle
    oam_buffer[1].attr0 = ATTR0_Y(lpaddle_y) | ATTR0_TALL | ATTR0_4BPP;
    oam_buffer[1].attr1 = ATTR1_X(lpaddle_x) | ATTR1_SIZE_16x32;
    oam_buffer[1].attr2 = ATTR2_ID(1) | ATTR2_PALBANK(0);

    memcpy(MEM_OAM, oam_buffer, sizeof(oam_buffer));

    // Set display mode 0, sprite on, mapping 1D
    REG_DISPCNT = DCNT_MODE0 | DCNT_OBJ | DCNT_OBJ_1D;

    while(1) {
        // VBlank wait
        while (REG_VCOUNT >= SCREEN_H);
        while (REG_VCOUNT < SCREEN_H);

        // update ball position based on speed
        ball_x += ball_speed_x;
        ball_y += ball_speed_y;

        // left paddle collision
        if (rect_overlap(ball_x, ball_y, 8, 8,
                 lpaddle_x, lpaddle_y, 16, 32)) {
            ball_speed_x = -ball_speed_x;
            ball_x = lpaddle_x + 16;
        }

        // ball bounce on walls
        if(ball_y < 0 || ball_y > SCREEN_H - 8) {
            ball_speed_y = -ball_speed_y;
        }
        if (ball_x <= 0 || ball_x >= SCREEN_W - 8) {
            ball_speed_x = -ball_speed_x;
        }

        // read input
        u16 keys = ~REG_KEYINPUT & KEY_MASK;
        if (keys & KEY_UP) {
            lpaddle_y -= paddle_speed;
            if (lpaddle_y < 0) {
                lpaddle_y = 0;
            }
        }

        if (keys & KEY_DOWN) {
            lpaddle_y += paddle_speed;
            if (lpaddle_y > SCREEN_H - 32) {
                lpaddle_y = SCREEN_H - 32;
            }
        }

        // update ball position in OAM
        oam_buffer[0].attr0 = ATTR0_Y(ball_y) | ATTR0_SQUARE | ATTR0_4BPP;
        oam_buffer[0].attr1 = ATTR1_X(ball_x) | ATTR1_SIZE_8x8;

        // update left paddle position in OAM
        oam_buffer[1].attr0 = ATTR0_Y(lpaddle_y) | ATTR0_TALL | ATTR0_4BPP;
        oam_buffer[1].attr1 = ATTR1_X(lpaddle_x) | ATTR1_SIZE_16x32;

        memcpy(MEM_OAM, oam_buffer, sizeof(oam_buffer));
    }
    return 0;
    
}