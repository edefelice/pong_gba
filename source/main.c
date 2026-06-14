#include <string.h>
#include "registers.h"
#include "game.h"
#include "graphics.h"
#include "ball.h"
#include "paddle.h"
#include "court.h"
#include "font.h"


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