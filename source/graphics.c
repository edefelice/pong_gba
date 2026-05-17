#include <string.h>
#include "../include/registers.h"
#include "../include/game.h"
#include "../include/graphics.h"

void render_sprites(void) {
    oam_buffer[0].attr0 = ATTR0_Y(ball_y) | ATTR0_SQUARE | ATTR0_4BPP;
    oam_buffer[0].attr1 = ATTR1_X(ball_x) | ATTR1_SIZE_8x8;
    
    oam_buffer[1].attr0 = ATTR0_Y(lpaddle_y) | ATTR0_TALL | ATTR0_4BPP;
    oam_buffer[1].attr1 = ATTR1_X(lpaddle_x) | ATTR1_SIZE_8x32;
    
    oam_buffer[2].attr0 = ATTR0_Y(rpaddle_y) | ATTR0_TALL | ATTR0_4BPP;
    oam_buffer[2].attr1 = ATTR1_X(rpaddle_x) | ATTR1_SIZE_8x32;
    
    memcpy(MEM_OAM, oam_buffer, sizeof(oam_buffer));
}