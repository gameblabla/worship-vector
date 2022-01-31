/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include "ssystem.h"
#include "types.h"

#define SCREEN_WIDTH_GAME 256
#define SCREEN_HEIGHT_GAME 240

extern uint8_t screen_buffering[SCREEN_WIDTH_GAME*240];
//#define screen_buffering screen->pixels

//extern Uint8 palette[16][3];
extern Uint8 button[12], vbutton[12], buttonpressed;
extern Sint8 dpad, dpadi;
extern Uint32 count;

extern Sint32 f_sin[4096], f_cos[4096];

extern Uint8 GameLoopEnabled;

extern Uint8 volume, volumedelay, volumepower;

extern Uint8 MusicPlaying, voiceon;

extern Uint8 NextColor0[256], NextColor2[256];

extern Uint32 GameMode, NextGameMode, GameModeEndCount;

extern Sint32 zlvertex, vbufferx[8], vbuffery[8], vbufferz[8],
		vvis[8];

extern Sint32 camera[6], tcamera[6];

extern Sint32 cursorx, cursory, cursormodecount;
extern Uint8 cursormode;

extern Uint8 map[64][64], mapunlock[8][8];
extern Sint8 mapz[65][65];
extern Sint32 turret0[64][64], turret1[64][64];
extern Uint8 wave;
extern Sint32 funds;

extern char s_textstring[16];
extern Sint8 l_textstring;
extern Uint8 dpadpower;

extern Uint8 cursorvisual, menuitem, towermenu;
extern Sint32 funds, cost, gamespeed;

extern Sint32 aimap[32][32], dirmap[32][32], spawnx, spawny;
extern Sint32 bullet[128][16], vline[128][16], mob[128][16], newvline[16],
		newbullet[16];
#ifdef BLENDING
extern Uint8 soundon, musicon,screenblend;
#else
extern Uint8 soundon, musicon;
#endif

extern Uint16 wavedelay, mobdelay, wavemobleft;
extern Sint32 iwavedata[100][8];

extern Uint8 lives;
