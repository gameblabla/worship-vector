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

#include <string.h>
#ifdef _TINSPIRE
#include <libndls.h>
#endif
#include "vars.h"
#include "ssystem.h"
#include "palette.h"
#include "zmath.h"

unsigned char button_state[8], button_time[8];

void Terminate(void) {
	FreeSound();
	lcd_init(SCR_TYPE_INVALID);
}

Uint8 d_sound_count = 0;
int audio_channels = 1;
int audio_rate = 22050;

Uint16 audio_format;
int audio_buffers;

uint16_t* getPaletteRegisters()
{
	return (uint16_t*)0xC0000200;
}

static uint16_t mapRgbToNspireColor(uint8_t r1, uint8_t g1, uint8_t b1)
{
	uint16_t r = r1 / 8;
	uint16_t g = g1 / 8;
	uint16_t b = b1 / 8;

	return b | (g << 5) | (r << 10);
}


void InitGameCore(void) 
{
	InitMath();

	if(!lcd_init(SCR_320x240_8))
		return;

	InitPalette();
	memset(screen_buffering, 255, 320 * 240);
	lcd_blit(screen_buffering, SCR_320x240_8);
	
	/*Uint16 i;
	for (i=0;i<256;i++)
	{
		//screen->format->palette->colors[i].r=palette[i][0];
		//screen->format->palette->colors[i].g=palette[i][1];
		//screen->format->palette->colors[i].b=palette[i][2];
	}*/
	

    uint32_t* paletteColor = (uint32_t*)getPaletteRegisters();
    for(uint32_t i = 0; i < 256; i += 2)
    {
        uint32_t lo = mapRgbToNspireColor(palette[i][0], palette[i][1], palette[i][2]);
		uint32_t hi = mapRgbToNspireColor(palette[i + 1][0], palette[i + 1][1], palette[i + 1][2]);
        *paletteColor++ = (lo | (hi << 16));
    }
	
	//SDL_SetColors(screen,screen->format->palette->colors,0,256);

	LoadSound();
	SetVolume(volume);
#ifdef GP2X
	zlExtInit();
#endif
}

#define AXIS_DEADZONE 7000
static const Sint8 angle_detection[9] = { 7, 0, 1, 6, -1, 2, 5, 4, 3 };
int i_keyb[23];

static const int sd_key_ref[11] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

enum MAP_KEY
{
	VK_FA , // 0
	VK_FX ,// 1
	VK_FB ,// 2
	VK_FY ,// 3
	VK_VOL_DOWN ,// 4
	VK_VOL_UP ,// 5
	VK_FL ,// 6
	VK_FR ,// 7
	VK_SELECT ,// 8
	VK_START ,// 9
	VK_FZ// 10
};

Sint8 olddpad = -1;

void CoreProcInput(void) {
	int i;
	int pad;

	dpad = -1;

	for(i=0;i<9;i++)
	{
		vbutton[i] = 0;
	}
	
	if (isKeyPressed(KEY_NSPIRE_UP)) i_keyb[0] = 1;
	else i_keyb[0] = 0;
	
	if (isKeyPressed(KEY_NSPIRE_RIGHT)) i_keyb[2] = 1;
	else i_keyb[2] = 0;
	
	if (isKeyPressed(KEY_NSPIRE_DOWN)) i_keyb[4] = 1;
	else i_keyb[4] = 0;
	
	if (isKeyPressed(KEY_NSPIRE_LEFT)) i_keyb[6] = 1;
	else i_keyb[6] = 0;
	
	if (isKeyPressed(KEY_NSPIRE_ENTER)) vbutton[6] = 1;
	else vbutton[6] = 0;

	if (isKeyPressed(KEY_NSPIRE_CTRL)) vbutton[0] = 1;
	else vbutton[0] = 0;
	
	if (isKeyPressed(KEY_NSPIRE_SHIFT)) vbutton[1] = 1;
	else vbutton[1] = 0;
	
	if (isKeyPressed(KEY_NSPIRE_VAR)) vbutton[2] = 1;
	else vbutton[2] = 0;
	
	if (isKeyPressed(KEY_NSPIRE_7)) vbutton[3] = 1;
	else vbutton[3] = 0;
	
	if (isKeyPressed(KEY_NSPIRE_ESC)) 
	{
		GameLoopEnabled=0;
	}
	
	if (isKeyPressed(KEY_NSPIRE_TAB))
	{
		vbutton[4] = 1;
		msleep(30);
	}
	else vbutton[4] = 0;
	
	if (isKeyPressed(KEY_NSPIRE_MENU))
	{
		vbutton[5] = 1;
		msleep(30);
	}
	else vbutton[5] = 0;

	for (i=0;i<8;i++)
	if (i_keyb[i]) dpad=i;

	if ((i_keyb[0]) & (i_keyb[2])) dpad=1;
	if ((i_keyb[2]) & (i_keyb[4])) dpad=3;
	if ((i_keyb[4]) & (i_keyb[6])) dpad=5;
	if ((i_keyb[6]) & (i_keyb[0])) dpad=7;
	
	if (i_keyb[19] | i_keyb[20] | i_keyb[21] | i_keyb[22])
	{
		if ((i_keyb[19])&(i_keyb[21]==0)&(i_keyb[20]==0)&(i_keyb[22]==0)) dpad=0;
		else
		if ((i_keyb[20])&(i_keyb[21]==0)&(i_keyb[22]==0)&(i_keyb[19]==0)) dpad=2;
		else
		if ((i_keyb[21])&(i_keyb[22]==0)&(i_keyb[20]==0)&(i_keyb[19]==0)) dpad=4;
		else
		if ((i_keyb[22])&(i_keyb[21]==0)&(i_keyb[20]==0)&(i_keyb[19]==0)) dpad=6;
		else
		if (i_keyb[19] & i_keyb[20]) dpad=1; else
		if (i_keyb[20] & i_keyb[21]) dpad=3; else
		if (i_keyb[21] & i_keyb[22]) dpad=5; else
		if (i_keyb[22] & i_keyb[19]) dpad=7;

	}

	for (i=8;i<19;i++)
	if (i_keyb[i]) button[i-8]=1;
	

	//no more volume control
	dpadi = 0;
	if (olddpad != dpad) {
		if (dpad >= 0)
			dpadi = 1;
	} else
		dpadi = 2;

	olddpad = dpad;

// here
//printf("state %i dir:%i\n",dpadi,dpad);
//printf("v:%d, s:%d, m:%d\n", volume, soundon, musicon);

}

void GameCoreTick(void) {
	CoreProcInput();

	lcd_blit(screen_buffering, SCR_320x240_8);

	if (gamespeed > 0)
		msleep(gamespeed * 10);

	voiceon = 0;
}

void ShutDownCore(void) {
#ifdef GP2X
	zlExtShutDown();
#endif
}

// SOUND SYSTEM
char spath[128];

void LoadSound(void) 
{
}

void FreeSound(void) 
{
}

void PlayMusic(unsigned char itrack) 
{
}
void PlaySound(unsigned char ibank, signed char pan) 
{
}

void PlaySoundEx(unsigned char ibank, signed char pan) 
{
}

void SetVolume(unsigned char ivolume) 
{
	volume = ivolume;
}
