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
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#ifdef _TINSPIRE
#include <libndls.h>
#endif
#include "vars.h"
#include "ssystem.h"
#include "palette.h"
#include "zmath.h"

SDL_Surface *screen = NULL, *rl_screen;
static unsigned char button_state[8], button_time[8];

void Terminate(void) {
	FreeSound();
	if (screen)
	{
		SDL_FreeSurface(screen);
		screen = NULL;
	}
	
	#ifdef SCALING
	if (rl_screen)
	{
		SDL_FreeSurface(rl_screen);
		rl_screen = NULL;
	}
	#endif

	SDL_Quit();
}


static int audio_channels = 1;
static int audio_rate = 11025;

static Uint16 audio_format;
static int audio_buffers;

#ifdef SCALING
static int simple2x = 0;
#endif

Uint8 InitGameCore(void) {
	Uint16 i;
	InitMath();
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		return 0;
	}

	#ifdef SDLMIX_SOUND
	if (Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels)) {
		printf("Mix_QuerySpec: %s\n", Mix_GetError());
	}

	if (Mix_OpenAudio(audio_rate, audio_format, audio_channels,
			audio_buffers)) {
		printf("Unable to open audio!\n");
		//return 0;
	}
	#endif

	SDL_ShowCursor(SDL_DISABLE);
	
	rl_screen = SDL_SetVideoMode (0, 0, 8, SDL_HWSURFACE | SDL_HWPALETTE | SDL_FULLSCREEN);
	screen = SDL_CreateRGBSurface(SDL_HWSURFACE, 320, 240, 8, 0,0,0,0);

	InitPalette();
	
	for (i=0;i<256;i++)
	{
		screen->format->palette->colors[i].r=palette[i][0];
		screen->format->palette->colors[i].g=palette[i][1];
		screen->format->palette->colors[i].b=palette[i][2];
	}
	SDL_SetColors(screen,screen->format->palette->colors,0,256);
	#if defined(SCALING) || defined(FORCE2X)
	SDL_SetColors(rl_screen,screen->format->palette->colors,0,256);
	#endif
	
	screen_buffering = screen->pixels;

	LoadSound();
	SetVolume(volume);

	return 1;
}

static Uint8 i_keyb[23];


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

static void CoreProcInput(void) {
	Uint8 *keystate = SDL_GetKeyState(NULL);
	SDL_Event event;
	Uint8 i;
	int pad;
	
	for (i=0;i<8;i++)
	{
		switch (i)
		{
			case 0:
			default:
				pad = SDLK_TAB;
			break;
			case 1:
				pad = SDLK_BACKSPACE;
			break;
			case 2:
				pad = SDLK_SPACE;
			break;
			case 3:
				pad = SDLK_RETURN;
			break;
			case 4:
				pad = SDLK_LCTRL;
			break;
			case 5:
				pad = SDLK_LALT;
			break;
			case 6:
				pad = SDLK_l;
			break;
			case 7:
				pad = SDLK_o;
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
	

	dpad = -1;

	for(i=0;i<9;i++)
	{
		vbutton[i] = 0;
	}
	
	if (keystate[SDLK_UP]) i_keyb[0] = 1;
	else i_keyb[0] = 0;
	
	if (keystate[SDLK_RIGHT]) i_keyb[2] = 1;
	else i_keyb[2] = 0;
	
	if (keystate[SDLK_DOWN]) i_keyb[4] = 1;
	else i_keyb[4] = 0;
	
	if (keystate[SDLK_LEFT]) i_keyb[6] = 1;
	else i_keyb[6] = 0;
	
	if (keystate[SDLK_RETURN]) vbutton[6] = 1;
	else vbutton[6] = 0;

	if (button_state[4] == 1 || button_state[3] == 1) vbutton[0] = 1;
	else vbutton[0] = 0;

	if (button_state[5] == 1 || button_state[6] == 1) vbutton[1] = 1;
	else vbutton[1] = 0;
	
	/*if (keystate[SDLK_LSHIFT]) vbutton[2] = 1;
	else vbutton[2] = 0;*/
	
	if (button_state[2] == 1 || button_state[7] == 1) vbutton[3] = 1;
	else vbutton[3] = 0;
	
	if (keystate[SDLK_ESCAPE]) 
	{
		GameLoopEnabled=0;
	}
	
	if (button_state[0] == 1)
	{
		vbutton[4] = 1;
	}
	else vbutton[4] = 0;
	
	if (button_state[1] == 1)
	{
		vbutton[5] = 1;
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

	//Keyboard

	while (SDL_PollEvent (&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
			GameLoopEnabled=0;
			break;
		}
	}
// here
//printf("state %i dir:%i\n",dpadi,dpad);
//printf("v:%d, s:%d, m:%d\n", volume, soundon, musicon);

}

void GameCoreTick(void) {
	CoreProcInput();
	
	SDL_SoftStretch(screen, NULL, rl_screen, NULL);
	SDL_Flip(rl_screen);
	
#ifndef NODELAY
	if (gamespeed > 0)
#ifdef _TINSPIRE
		msleep(gamespeed * 10);
#else
		SDL_Delay(gamespeed * 10);
#endif
#endif

	voiceon = 0;
}

void ShutDownCore(void) {
}

// SOUND SYSTEM
// SOUND SYSTEM
#ifdef SDLMIX_SOUND
static Mix_Music *music = NULL;
static  Mix_Chunk *d_sound[4];
//Mix_Chunk *d_sound[8];
static  Mix_Chunk *e_sound;
#endif

void LoadSound(void) {
	#ifdef SDLMIX_SOUND
	d_sound[0] = Mix_LoadWAV("click.wav");
	d_sound[1] = Mix_LoadWAV("beep.wav");
	d_sound[2] = Mix_LoadWAV("order.wav");
	d_sound[3] = Mix_LoadWAV("victory.wav");
	/*
	 * d_sound[4]=Mix_LoadWAV("data/s4.wav");
	 * d_sound[5]=Mix_LoadWAV("data/s5.wav");
	 * d_sound[6]=Mix_LoadWAV("data/s6.wav");
	 * d_sound[7]=Mix_LoadWAV("data/s7.wav");
	 */
	PlayMusic(0);
	#endif
}

void FreeSound(void) {
	#ifdef SDLMIX_SOUND
	Uint8 i;

	for (i = 0; i < 4; i++) {
		Mix_FreeChunk(d_sound[i]);
		d_sound[i] = NULL;
	}

	if (music != NULL) {
		Mix_FreeMusic(music);
		music = NULL;
	}

	e_sound = NULL;
	
	Mix_CloseAudio();
	#endif
}

void PlayMusic(unsigned char itrack) {
	#ifdef SDLMIX_SOUND
	if (itrack == 128)
		Mix_PauseMusic();
	else {
		if (musicon)
		{
			if (music)
			{
				Mix_FreeMusic(music);
			}
			
			switch (itrack)
			{
				case 0:
				default:
					music = Mix_LoadMUS("maincontrol.ogg");
				break;
				case 1:
					music = Mix_LoadMUS("datasmasher.ogg");
				break;
				case 2:
					music = Mix_LoadMUS("defense.ogg");
				break;
			}
			
			Mix_PlayMusic(music, -1);
		}
	}
	MusicPlaying = itrack;
	#endif
}
void PlaySound(unsigned char ibank, signed char pan) {
	#ifdef SDLMIX_SOUND
	if (soundon) {
		//	Mix_SetPanning(0,127,127); // according to SDL doc, this works only for 2 channels, not 1 channel like we use
		//	/*
		Mix_PlayChannel(-1, d_sound[ibank], 0);
		//	*/
		//printf("Played %d\n", ibank);
	}
	#endif
}
void PlaySoundEx(unsigned char ibank, signed char pan) {
	//sprintf(spath,"%i.wav",ibank);
	/*sprintf(spath, "data/e%i.wav", ibank);
	Mix_SetPanning(0, 127, 127);
	e_sound = Mix_LoadWAV(spath);
	Mix_PlayChannel(-1, e_sound, 0);*/
}

void SetVolume(unsigned char ivolume) {
	#ifdef SDLMIX_SOUND
	volume = ivolume;
	#endif
	//printf("sfx %d\n", Mix_Volume(0, -1));
	//Mix_Volume(0,volume);
	//Mix_VolumeMusic(volume);
}
