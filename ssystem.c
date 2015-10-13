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
#include <SDL.h>

#include "vars.h"
#include "ssystem.h"
#include "palette.h"
#include "zmath.h"

	unsigned char button_state[8], button_time[8];

#ifdef GP2X
#include "zlext.h"
#include "sys/mman.h"
#include "fcntl.h"

volatile unsigned short *MEM_REG;
unsigned long gp2x_dev=0;

void RamHack(void) //I't me! RamHack!
{
}
#endif
void Terminate(void) {
	FreeSound();
	SDL_Quit();
#ifdef GP2X
	chdir("/usr/gp2x");
	execl("/usr/gp2x/gp2xmenu", "/usr/gp2x/gp2xmenu", NULL);
#endif
}

SDL_Surface *screen = NULL;

Uint8 d_sound_count = 0;
int audio_channels = 1;
int audio_rate = 22050;

Uint16 audio_format;
int audio_buffers;

void InitGameCore(void) {
	InitMath();
	atexit(Terminate);

#ifdef GP2X
	RamHack();
#endif

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_ShowCursor(SDL_DISABLE);
	screen = SDL_SetVideoMode (320, 240, 8, SDL_SWSURFACE);

	InitPalette();
	
	Uint16 i;
	for (i=0;i<256;i++)
	{
		screen->format->palette->colors[i].r=palette[i][0];
		screen->format->palette->colors[i].g=palette[i][1];
		screen->format->palette->colors[i].b=palette[i][2];
	}
	SDL_SetColors(screen,screen->format->palette->colors,0,256);

	LoadSound();
	SetVolume(volume);
#ifdef GP2X
	zlExtInit();
#endif
}

#define AXIS_DEADZONE 7000
static const Sint8 angle_detection[9] = { 7, 0, 1, 6, -1, 2, 5, 4, 3 };
int i_keyb[23];
static const SDLKey sd_keyb[23] = { SDLK_KP8, SDLK_KP9, SDLK_KP6, SDLK_KP3,
		SDLK_KP2, SDLK_KP1, SDLK_KP4, SDLK_KP7, SDLK_LCTRL, 
		//SDLK_LALT,
		SDLK_BACKSPACE,
		SDLK_LSHIFT, SDLK_SPACE, SDLK_TAB, SDLK_BACKSPACE, SDLK_MINUS,
		SDLK_EQUALS, SDLK_ESCAPE, SDLK_1, SDLK_t, SDLK_UP, SDLK_RIGHT,
		SDLK_DOWN, SDLK_LEFT };
/*static const SDLKey sd_keyb[23]={SDLK_KP8,SDLK_KP9,SDLK_KP6,SDLK_KP3,SDLK_KP2,SDLK_KP1,SDLK_KP4,SDLK_KP7,
 * SDLK_SPACE,SDLK_q,SDLK_c,SDLK_a,SDLK_z,SDLK_x,SDLK_MINUS,SDLK_EQUALS,SDLK_ESCAPE,SDLK_TAB,SDLK_t,SDLK_UP,SDLK_RIGHT,SDLK_DOWN,SDLK_LEFT
 * };
 */

static const int sd_key_ref[11] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

#ifdef GP2X// GP2X button mapping
enum MAP_KEY
{
	VK_UP , // 0
	VK_UP_LEFT ,// 1
	VK_LEFT ,// 2
	VK_DOWN_LEFT ,// 3
	VK_DOWN ,// 4
	VK_DOWN_RIGHT ,// 5
	VK_RIGHT ,// 6
	VK_UP_RIGHT ,// 7
	VK_START ,// 8
	VK_SELECT ,// 9
	VK_FL ,// 10
	VK_FR ,// 11
	VK_FA ,// 12
	VK_FB ,// 13
	VK_FX ,// 14
	VK_FY ,// 15
	VK_VOL_UP ,// 16
	VK_VOL_DOWN ,// 17
	VK_FZ// 18 :)
};
#endif
#if defined(WIN32) || defined(GCW) || defined(LINUX)// WIN button mapping
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
#endif

Sint8 olddpad = -1;

void CoreProcInput(void) {
	Uint8 *keystate = SDL_GetKeyState(NULL);
	int i;
	int pad;

	dpad = -1;
	
	for (i=0;i<5;i++)
	{
		switch (i)
		{
			case 0:
				pad = SDLK_TAB;
			break;
			case 1:
				pad = SDLK_MENU;
			break;
			case 2:
				pad = SDLK_MINUS;
			break;
			case 3:
				pad = SDLK_PLUS;
			break;
			case 4:
				pad = SDLK_LCTRL;
			break;
		}
		
		switch (button_state[i])
		{
			case 0:
				if (keystate[pad])
				{
					button_state[i] = 1;
					button_time[i] = 0;
				}
			break;
				
			case 1:
				button_time[i] = button_time[i] + 1;
					
				if (button_time[i] > 0)
				{
					button_state[i] = 2;
					button_time[i] = 0;
				}
			break;
				
			case 2:
				if (!(keystate[pad]))
				{
					button_state[i] = 3;
					button_time[i] = 0;
				}
			break;
				
			case 3:
				button_time[i] = button_time[i] + 1;
					
				if (button_time[i] > 1)
				{
					button_state[i] = 0;
					button_time[i] = 0;
				}
			break;
		}	
	}
	
	for(i=0;i<9;i++)
	{
		vbutton[i] = 0;
	}
	
	if (button_state[4] == 1)
	{
		vbutton[0] = 1;
	}
	
	
	if (keystate[SDLK_BACKSPACE])
	{
		vbutton[2] = 1;
	}
	
	if (button_state[3] == 1)
	{
		vbutton[3] = 1;
	}
	else if (button_state[2] == 1)
	{
		vbutton[1] = 1;
	}
	
	if (button_state[0] == 1)
	{
		vbutton[4] = 1;
	}
	
	if (button_state[1] == 1)
	{
		vbutton[5] = 1;
	}
	
	if (keystate[SDLK_RETURN])
	{
		vbutton[6] = 1;
	}
	
	//AXBYLR-+SS
	/*button[0] = SDL_JoystickGetButton(joy, 0);
	button[1] = SDL_JoystickGetButton(joy, 1);
	button[2] = SDL_JoystickGetButton(joy, 2);
	button[3] = SDL_JoystickGetButton(joy, 3);
	button[4] = SDL_JoystickGetButton(joy, 4);
	button[5] = SDL_JoystickGetButton(joy, 5);
	button[8] = SDL_JoystickGetButton(joy, 6);*/

	//Keyboard
	SDL_Event event;

	while (SDL_PollEvent (&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
			GameLoopEnabled=0;
			break;
			case SDL_KEYDOWN:
			for (i=0;i<23;i++)
			if (event.key.keysym.sym==sd_keyb[i])
			i_keyb[i]=1;
			break;
			case SDL_KEYUP:
			for (i=0;i<23;i++)
			if (event.key.keysym.sym==sd_keyb[i])
			i_keyb[i]=0;
			break;

		}
	}
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
	
	
	//volume control
	if (volumedelay)
		volumedelay--;
	if (button[7]) {
		if (volume + volumepower < 127)
			SetVolume(volume + volumepower);
		else
			SetVolume(127);
		volumedelay = 20;
	} else if (button[6]) {
		if (volume - volumepower > 0)
			SetVolume(volume - volumepower);
		else
			SetVolume(0);
		volumedelay = 20;
	}

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
#ifdef GP2X
	zlExtFrame();
#endif

	SDL_LockSurface(screen);
	memcpy(screen->pixels,screen_buffering,76800);

	SDL_UnlockSurface(screen);

	SDL_Flip(screen);

#ifndef _TINSPIRE
	if (gamespeed > 0)
		SDL_Delay(gamespeed * 10);
#endif

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
