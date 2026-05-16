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

int main(void) {

    // ball position and speed initialization
    u32 ball_x = SCREEN_W / 2;
    u32 ball_y = SCREEN_H / 2;
    u32 ball_speed_x = 1;
    u32 ball_speed_y = 1;

    // Hide all sprites
    for (int i = 0; i < 128; i++) {
        oam_buffer[i].attr0 = ATTR0_HIDE;
        oam_buffer[i].attr1 = 0;
        oam_buffer[i].attr2 = 0;
        oam_buffer[i].fill  = 0;
    }

    // Load sprite tiles in VRAM
    memcpy(MEM_VRAM_OBJ, ballTiles, ballTilesLen);
    // Load sprite palette
    memcpy(MEM_PAL_OBJ, ballPal, ballPalLen);

    // Set OAM
    // Ball at the centre of the screen.
    oam_buffer[0].attr0 = ATTR0_Y(ball_y) | ATTR0_SQUARE | ATTR0_4BPP;
    oam_buffer[0].attr1 = ATTR1_X(ball_x) | ATTR1_SIZE_8x8;
    oam_buffer[0].attr2 = ATTR2_ID(0) | ATTR2_PALBANK(0);

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

        // update ball position in OAM
        oam_buffer[0].attr0 = ATTR0_Y(ball_y) | ATTR0_SQUARE | ATTR0_4BPP;
        oam_buffer[0].attr1 = ATTR1_X(ball_x) | ATTR1_SIZE_8x8;

        memcpy(MEM_OAM, oam_buffer, sizeof(oam_buffer));
    }
    return 0;
    
}