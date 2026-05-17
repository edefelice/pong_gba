//
// all_gfx.h
//
// Header che unisce tutti gli header grafici
// Data: 2026-05-18 00:10:55

#ifdef __cplusplus
extern "C"{
#endif

//{{BLOCK(ball)

//======================================================================
//
//	ball, 8x8@4, 
//	Transparent color : FF,00,FF
//	+ palette 256 entries, not compressed
//	+ 1 tiles not compressed
//	Total size: 512 + 32 = 544
//
//	Time-stamp: 2026-05-18, 00:10:55
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_BALL_H
#define GRIT_BALL_H

#define ballTilesLen 32
extern const unsigned int ballTiles[8];

#define ballPalLen 512
extern const unsigned short ballPal[256];

#endif // GRIT_BALL_H

//}}BLOCK(ball)

//{{BLOCK(court)

//======================================================================
//
//	court, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 3 tiles (t|f reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 96 + 2048 = 2656
//
//	Time-stamp: 2026-05-18, 00:10:55
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_COURT_H
#define GRIT_COURT_H

#define courtTilesLen 96
extern const unsigned int courtTiles[24];

#define courtMapLen 2048
extern const unsigned short courtMap[1024];

#define courtPalLen 512
extern const unsigned short courtPal[256];

#endif // GRIT_COURT_H

//}}BLOCK(court)

//{{BLOCK(font)

//======================================================================
//
//	font, 80x8@4, 
//	+ palette 256 entries, not compressed
//	+ 11 tiles not compressed
//	+ regular map (flat), not compressed, 10x1 
//	Total size: 512 + 352 + 20 = 884
//
//	Time-stamp: 2026-05-18, 00:10:55
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_FONT_H
#define GRIT_FONT_H

#define fontTilesLen 352
extern const unsigned int fontTiles[88];

#define fontMapLen 20
extern const unsigned short fontMap[10];

#define fontPalLen 512
extern const unsigned short fontPal[256];

#endif // GRIT_FONT_H

//}}BLOCK(font)

//{{BLOCK(paddle)

//======================================================================
//
//	paddle, 8x32@4, 
//	Transparent color : FF,00,FF
//	+ palette 256 entries, not compressed
//	+ 4 tiles not compressed
//	Total size: 512 + 128 = 640
//
//	Time-stamp: 2026-05-18, 00:10:55
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_PADDLE_H
#define GRIT_PADDLE_H

#define paddleTilesLen 128
extern const unsigned int paddleTiles[32];

#define paddlePalLen 512
extern const unsigned short paddlePal[256];

#endif // GRIT_PADDLE_H

//}}BLOCK(paddle)

#ifdef __cplusplus
};
#endif

