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

#include "vars.h"
#include "zmath.h"

#define HOST_WIDTH_RESOLUTION SCREEN_WIDTH_GAME
#define HOST_HEIGHT_RESOLUTION 240
#define setPixel(buffer, x,y,c) *((uint8_t* restrict)buffer + ((x) + (y) * HOST_WIDTH_RESOLUTION)) = c;

static uint8_t n2DLib_font[] =
{
	0x70, 0x88, 0x98, 0xA8, 0xC8, 0x88, 0x70, 0x00,	/* Char 048 (0) */
	0x40, 0xC0, 0x40, 0x40, 0x40, 0x40, 0xE0, 0x00,	/* Char 049 (1) */
	0x70, 0x88, 0x08, 0x10, 0x20, 0x40, 0xF8, 0x00,	/* Char 050 (2) */
	0x70, 0x88, 0x08, 0x10, 0x08, 0x88, 0x70, 0x00,	/* Char 051 (3) */
	0x08, 0x18, 0x28, 0x48, 0xFC, 0x08, 0x08, 0x00,	/* Char 052 (4) */
	0xF8, 0x80, 0x80, 0xF0, 0x08, 0x88, 0x70, 0x00,	/* Char 053 (5) */
	0x20, 0x40, 0x80, 0xF0, 0x88, 0x88, 0x70, 0x00,	/* Char 054 (6) */
	0xF8, 0x08, 0x10, 0x20, 0x40, 0x40, 0x40, 0x00,	/* Char 055 (7) */
	0x70, 0x88, 0x88, 0x70, 0x88, 0x88, 0x70, 0x00,	/* Char 056 (8) */
	0x70, 0x88, 0x88, 0x78, 0x08, 0x08, 0x70, 0x00,	/* Char 057 (9) */
	0x00, 0x00, 0x60, 0x60, 0x00, 0x60, 0x60, 0x00,	/* Char 058 (:) */
	0x00, 0x00, 0x60, 0x60, 0x00, 0x60, 0x60, 0x20,	/* Char 059 (;) */
	0x10, 0x20, 0x40, 0x80, 0x40, 0x20, 0x10, 0x00,	/* Char 060 (<) */
	0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00,	/* Char 061 (=) */
	0x80, 0x40, 0x20, 0x10, 0x20, 0x40, 0x80, 0x00,	/* Char 062 (>) */
	0x78, 0x84, 0x04, 0x08, 0x10, 0x00, 0x10, 0x00,	/* Char 063 (?) */
	0x70, 0x88, 0x88, 0xA8, 0xB8, 0x80, 0x78, 0x00,	/* Char 064 (@) */
	0x20, 0x50, 0x88, 0x88, 0xF8, 0x88, 0x88, 0x00,	/* Char 065 (A) */
	0xF0, 0x88, 0x88, 0xF0, 0x88, 0x88, 0xF0, 0x00,	/* Char 066 (B) */
	0x70, 0x88, 0x80, 0x80, 0x80, 0x88, 0x70, 0x00,	/* Char 067 (C) */
	0xF0, 0x88, 0x88, 0x88, 0x88, 0x88, 0xF0, 0x00,	/* Char 068 (D) */
	0xF8, 0x80, 0x80, 0xE0, 0x80, 0x80, 0xF8, 0x00,	/* Char 069 (E) */
	0xF8, 0x80, 0x80, 0xE0, 0x80, 0x80, 0x80, 0x00,	/* Char 070 (F) */
	0x70, 0x88, 0x80, 0x80, 0x98, 0x88, 0x78, 0x00,	/* Char 071 (G) */
	0x88, 0x88, 0x88, 0xF8, 0x88, 0x88, 0x88, 0x00,	/* Char 072 (H) */
	0xE0, 0x40, 0x40, 0x40, 0x40, 0x40, 0xE0, 0x00,	/* Char 073 (I) */
	0x38, 0x10, 0x10, 0x10, 0x10, 0x90, 0x60, 0x00,	/* Char 074 (J) */
	0x88, 0x90, 0xA0, 0xC0, 0xA0, 0x90, 0x88, 0x00,	/* Char 075 (K) */
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xF8, 0x00,	/* Char 076 (L) */
	0x82, 0xC6, 0xAA, 0x92, 0x82, 0x82, 0x82, 0x00,	/* Char 077 (M) */
	0x84, 0xC4, 0xA4, 0x94, 0x8C, 0x84, 0x84, 0x00,	/* Char 078 (N) */
	0x70, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00,	/* Char 079 (O) */
	0xF0, 0x88, 0x88, 0xF0, 0x80, 0x80, 0x80, 0x00,	/* Char 080 (P) */
	0x70, 0x88, 0x88, 0x88, 0xA8, 0x90, 0x68, 0x00,	/* Char 081 (Q) */
	0xF0, 0x88, 0x88, 0xF0, 0xA0, 0x90, 0x88, 0x00,	/* Char 082 (R) */
	0x70, 0x88, 0x80, 0x70, 0x08, 0x88, 0x70, 0x00,	/* Char 083 (S) */
	0xF8, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00,	/* Char 084 (T) */
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00,	/* Char 085 (U) */
	0x88, 0x88, 0x88, 0x50, 0x50, 0x20, 0x20, 0x00,	/* Char 086 (V) */
	0x82, 0x82, 0x82, 0x82, 0x92, 0x92, 0x6C, 0x00,	/* Char 087 (W) */
	0x88, 0x88, 0x50, 0x20, 0x50, 0x88, 0x88, 0x00,	/* Char 088 (X) */
	0x88, 0x88, 0x88, 0x50, 0x20, 0x20, 0x20, 0x00,	/* Char 089 (Y) */
	0xF8, 0x08, 0x10, 0x20, 0x40, 0x80, 0xF8, 0x00,	/* Char 090 (Z) */
};


void DrawLine(Uint16 x0, Uint16 y0, Uint16 x1, Uint16 y1, Uint8 color) {
	Sint32 x, y;
	Sint32 i, ii, s, nsx, nsy;
	if (x0 > SCREEN_WIDTH_GAME-1)
		if (x1 > SCREEN_WIDTH_GAME-1)
			return;

	if (y0 > 239)
		if (y1 > 239)
			return;
	
	if (x0 == x1)
	{
		if (y0 == y1)
			return;
	}
		
	if (zabs(y1 - y0) > zabs(x1 - x0))
		s = zabs(y1 - y0);
	else
		s = zabs(x1 - x0);
	nsx = (x1 - x0) * 65536 / s;
	nsy = (y1 - y0) * 65536 / s;
	x = x0 << 16;
	y = y0 << 16;

	for (i = 0; i < s; i++)
	{
		x = x + nsx;
		y = y + nsy;
		ii = (x >> 16) + (y >> 16) * SCREEN_WIDTH_GAME;
		if ((ii >= 0) & (ii < 76800))
			if (x >= 0)
				if (x < 20971520)
					screen_buffering[ii] = color;
	}
}

typedef
 union
  {
   int32_t i;
   struct
    {
     int16_t lo; // endian-specific!
     int16_t hi;
    };
  } fixed_point;

void DrawLine2(Uint16 x, Uint16 y, Uint16 x2, Uint16 y2, Uint8 color) {
	
	if (x > SCREEN_WIDTH_GAME-1 || x2 > SCREEN_WIDTH_GAME-1) return;
	if (y > 239 || y2 > 239) return;

	int yLonger=0;
	int incrementVal, endVal;
	int shortLen=y2-y;
	int longLen=x2-x;
	
	if (abs(shortLen)>abs(longLen)) {
		int swap=shortLen;
		shortLen=longLen;
		longLen=swap;
		yLonger=1;
	}
	endVal=longLen;
	if (longLen<0) {
		incrementVal=-1;
		longLen=-longLen;
	} else incrementVal=1;
	int decInc;
	if (longLen==0) decInc=0;
	else decInc = (shortLen << 16) / longLen;
	int j=0;
	if (yLonger) {	
		for (int i=0;i!=endVal;i+=incrementVal) {
			setPixel(screen_buffering,x+(j >> 16),y+i, color);	
			j+=decInc;
		}
	} else {
		for (int i=0;i!=endVal;i+=incrementVal) {
			setPixel(screen_buffering,x+i,y+(j >> 16), color);
			j+=decInc;
		}
	}
	
	/*fixed_point f;
	int x; 
	int32_t m;
	
	if (x0 > SCREEN_WIDTH_GAME-1 || x1 > SCREEN_WIDTH_GAME-1) return;
	if (y0 > 239 || y1 > 239) return;
	
	m=((int32_t)(y1-y0)<<16)/(x1-x0);
	f.i=y0<<16;
	for (x=x0;x<=x1;x++,f.i+=m)
	{
		fixed_point g=f;
		g.i+=32767;
		setPixel(screen_buffering,x,g.hi, color);
	}*/
	
	/*int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1; 
	int err = dx + dy, e2; 
 
	if (x0 > SCREEN_WIDTH_GAME-1 || x1 > SCREEN_WIDTH_GAME-1) return;
	if (y0 > 239 || y1 > 239) return;
			
	for (;;)
	{
		setPixel(screen_buffering,x0,y0, color);
		if (x0 == x1 && y0 == y1) break;
		e2 = 2 * err;
		if (e2 >= dy) { err += dy; x0 += sx; }
		if (e2 <= dx) { err += dx; y0 += sy; } 
	}*/
	
	/*Sint32 x, y;
	Sint32 i, ii, s, nsx, nsy;

	if (x0 > SCREEN_WIDTH_GAME-1)
		if (x1 > SCREEN_WIDTH_GAME-1)
			return;

	if (y0 > 239)
		if (y1 > 239)
			return;
	if (x0 == x1)
		if (y0 == y1)
			return;

	if (zabs(y1 - y0) > zabs(x1 - x0))
			s = zabs(y1 - y0);
		else
			s = zabs(x1 - x0);
		nsx = (x1 - x0) * 65536 / s;
		nsy = (y1 - y0) * 65536 / s;
		x = x0 << 16;
		y = y0 << 16;

		for (i = 0; i < s; i++)
		{
			x = x + nsx;
			y = y + nsy;
			ii = (x >> 16) + (y >> 16) * SCREEN_WIDTH_GAME;

			if ((ii >= SCREEN_WIDTH_GAME) & (ii < SCREEN_WIDTH_GAME*SCREEN_HEIGHT_GAME))
				if (x >= 65536)
					//if (x < 20905984/1.7) {
					if (x < 13905984 ) {
						screen_buffering[ii - 1] = color;
						screen_buffering[ii - SCREEN_WIDTH_GAME] = color;
						screen_buffering[ii + 1] = color;
						screen_buffering[ii + SCREEN_WIDTH_GAME] = color;
					}
		}*/
}

void DarkRect(Uint16 x0, Uint16 y0, Uint16 x1, Uint16 y1) {

}

void DrawRect(Uint16 xx0, Uint16 yy0, Uint16 xx1, Uint16 yy1, Uint8 color) {
	/*Uint16 x0, x1, y0, y1;
	Sint32 i, ii;
	x0 = xx0;
	x1 = xx1;
	y0 = yy0;
	y1 = yy1;
	
	if (x0 > SCREEN_WIDTH_GAME) return;

	ii = x0 + y0 * SCREEN_WIDTH_GAME;
	for (i = x0; i < x1; i++) {
		ii++;
		if ((ii >= 0) & (ii < 76800))
			screen_buffering[ii] = color;
	}
	ii = x0 + y1 * SCREEN_WIDTH_GAME;
	for (i = x0; i < x1; i++) {
		ii++;
		if ((ii >= 0) & (ii < 76800))
			screen_buffering[ii] = color;
	}
	ii = x0 + y0 * SCREEN_WIDTH_GAME;
	for (i = y0; i < y1; i++) {
		ii += SCREEN_WIDTH_GAME;
		if ((ii >= 0) & (ii < 76800))
			screen_buffering[ii] = color;
	}
	ii = x1 + y0 * SCREEN_WIDTH_GAME;
	for (i = y0; i < y1; i++) {
		ii += SCREEN_WIDTH_GAME;
		if ((ii >= 0) & (ii < 76800))
			screen_buffering[ii] = color;
	}*/

}

void s_drawtext( Uint16 x, Uint16 y, Uint8 c)
{
	int8_t a, j;
	uint8_t *charSprite;
	uint8_t i, max = l_textstring + 1;
	x = x + 4;
	y = y + 4;
	
	if (x > SCREEN_WIDTH_GAME) return;
	
	for(i = 0; i < max; i++)
	{
		if (!(s_textstring[i] == 0 || s_textstring[i] == 32))
		{
			charSprite = s_textstring[i] * 8 + n2DLib_font - (48 * 8);
			
			// Draw charSprite as monochrome 8*8 image using given color
			for(a = 0; a < 8; a++)
			{
				for(j = 7; j >= 0; j--)
				{
					if((charSprite[a] >> j) & 1)
					{
						setPixel(screen_buffering, x + (7 - j), y + a, c)
					}
				}
			}
		}
		x += 8;	
	}
}
