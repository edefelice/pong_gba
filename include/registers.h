#ifndef REGISTERS_H
#define REGISTERS_H

// Types
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef signed char    s8;
typedef signed short   s16;
typedef signed int     s32;

// Memory regions
#define MEM_IO       0x04000000
#define MEM_PAL_BG   ((u16*)0x05000000)
#define MEM_PAL_OBJ  ((u16*)0x05000200)
#define MEM_VRAM     ((u16*)0x06000000)
#define MEM_VRAM_OBJ ((u16*)0x06010000)
#define MEM_OAM      ((u16*)0x07000000)
#define MEM_VRAM_CHARBLOCK(n)   ((u16*)(0x06000000 + (n) * 0x4000))
#define MEM_VRAM_SCREENBLOCK(n) ((u16*)(0x06000000 + (n) * 0x0800))

// Display registers
#define REG_DISPCNT  *((volatile u32*)(MEM_IO + 0x0000))
#define REG_DISPSTAT *((volatile u16*)(MEM_IO + 0x0004))
#define REG_VCOUNT   *((volatile u16*)(MEM_IO + 0x0006))
#define REG_BG0CNT   *((volatile u16*)(MEM_IO + 0x0008))

// Display control bits
#define DCNT_MODE0   0x0000
#define DCNT_OBJ     0x1000   // enable sprites (bit 12)
#define DCNT_OBJ_1D  0x0040   // 1D sprite mapping (bit 6)
#define DCNT_BG0     0x0100   // BG0 

// --- OAM attribute 0 ---
#define ATTR0_HIDE          0x0200
#define ATTR0_4BPP          0x0000
#define ATTR0_SQUARE        0x0000
#define ATTR0_TALL          0x8000
#define ATTR0_Y(n)          ((n) & 0xFF)

// --- OAM attribute 1 ---
#define ATTR1_SIZE_8x8      0x0000
#define ATTR1_SIZE_16x32    0x8000
#define ATTR1_X(n)          ((n) & 0x1FF)

// --- OAM attribute 2 ---
#define ATTR2_ID(n)         ((n) & 0x3FF)
#define ATTR2_PALBANK(n)    (((n) & 0xF) << 12)

// BG control bits
#define BG_CBB(n)    (((n) & 0x3) << 2)
#define BG_SBB(n)    (((n) & 0x1F) << 8)
#define BG_4BPP      0x0000
#define BG_REG_32x32 0x0000
#define BG_PRIO(n)   ((n) & 0x3)

// Input
#define REG_KEYINPUT *((volatile u16*)(MEM_IO + 0x0130))

#define KEY_A        0x0001
#define KEY_B        0x0002
#define KEY_SELECT   0x0004
#define KEY_START    0x0008
#define KEY_RIGHT    0x0010
#define KEY_LEFT     0x0020
#define KEY_UP       0x0040
#define KEY_DOWN     0x0080
#define KEY_R        0x0100
#define KEY_L        0x0200
#define KEY_MASK     0x03FF

// Screen dimensions
#define SCREEN_W     240
#define SCREEN_H     160

#endif