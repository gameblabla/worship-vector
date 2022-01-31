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
#include <stdint.h>
#include <string.h>
#include <eris/v810.h>
#include <eris/king.h>
#include <eris/tetsu.h>
#include <eris/romfont.h>
#include <eris/cd.h>
#include <eris/pad.h>
#include <eris/low/pad.h>
#include <eris/low/scsi.h>
#include <eris/low/soundbox.h>
#include "vars.h"
#include "ssystem.h"
#include "zmath.h"

static int button_state[8], button_time[8];

void Terminate(void) {
	//FreeSound();
	//SDL_Quit();
}

extern int color[16];
u32 padtype, paddata;

void InitGameCore(void) {
	int i;
	u16 microprog[16];
	
	InitMath();
	
	eris_king_init();
	eris_tetsu_init();
	
	eris_tetsu_set_priorities(0, 0, 1, 0, 0, 0, 0);
	eris_tetsu_set_king_palette(0, 0, 0, 0);
	eris_tetsu_set_rainbow_palette(0);

	eris_king_set_bg_prio(KING_BGPRIO_0, KING_BGPRIO_HIDE, KING_BGPRIO_HIDE, KING_BGPRIO_HIDE, 0);
	eris_king_set_bg_mode(KING_BGMODE_256_PAL, KING_BGPRIO_HIDE, KING_BGPRIO_HIDE, KING_BGPRIO_HIDE);
	eris_king_set_kram_pages(0, 0, 0, 0);

	for(i = 0; i < 16; i++) {
		microprog[i] = KING_CODE_NOP;
	}

	microprog[0] = KING_CODE_BG0_CG_0;
	microprog[1] = KING_CODE_BG0_CG_1;
	microprog[2] = KING_CODE_BG0_CG_2;
	microprog[3] = KING_CODE_BG0_CG_3;
	
	eris_king_disable_microprogram();
	eris_king_write_microprogram(microprog, 0, 16);
	eris_king_enable_microprogram();
	
	InitPalette();
	
	eris_tetsu_set_rainbow_palette(0);
	for(i = 0; i < 16; i++) {
		eris_tetsu_set_palette(i, color[i]);
	}

	eris_tetsu_set_video_mode(TETSU_LINES_262, 0, TETSU_DOTCLOCK_5MHz, TETSU_COLORS_16, TETSU_COLORS_16, 0, 0, 1, 0, 0, 0, 0);
	eris_king_set_bat_cg_addr(KING_BG0, 0, 0);
	eris_king_set_bat_cg_addr(KING_BG0SUB, 0, 0);
	eris_king_set_scroll(KING_BG0, 0, 0);
	eris_king_set_bg_size(KING_BG0, KING_BGSIZE_256, KING_BGSIZE_256, KING_BGSIZE_256, KING_BGSIZE_256);
	eris_pad_init(0);
	
	eris_low_scsi_reset();
	
	eris_king_set_kram_read(0, 1);
	eris_king_set_kram_write(0, 1);
	// Clear BG0's RAM
	for(i = 0x0; i < (256*256); i++) {
		eris_king_kram_write(0x88);
	}

	LoadSound();
	SetVolume(volume);
	eris_low_cdda_set_volume(63,63);

	return 1;
}

static Uint8 i_keyb[23];
Sint8 olddpad = -1;


static void CoreProcInput(void) {
	Uint8 i;
	int pad;
	
	padtype = eris_pad_type(0);
	paddata = eris_pad_read(0);
	paddata = eris_pad_read(0);
	for (i=0;i<6;i++)
	{
		switch (i)
		{
			case 0:
			default:
				pad = 3;
			break;
			case 1:
				pad = 4;
			break;
			case 2:
				pad = 5;
			break;
			case 3:
				pad = 7;
			break;
			case 4:
				pad = 0;
			break;
			case 5:
				pad = 6;
			break;
		}
		
		switch (button_state[i])
		{
			case 0:
				if(paddata & (1 << pad))
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
				if(!(paddata & (1 << pad)))
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
	
	if(paddata & (1 << 8)) i_keyb[0] = 1; // UP
	else i_keyb[0] = 0;
	
	if(paddata & (1 << 9)) i_keyb[2] = 1; // RIGHT
	else i_keyb[2] = 0;
	
	if(paddata & (1 << 10)) i_keyb[4] = 1; //DOWN
	else i_keyb[4] = 0;
	
	if(paddata & (1 << 11)) i_keyb[6] = 1; // LEFT
	else i_keyb[6] = 0;
	
	if(paddata & (1 << 1)) vbutton[6] = 1; // CONFIRM
	else vbutton[6] = 0;

	if (button_state[4] == 1 || button_state[3] == 1) vbutton[0] = 1;
	else vbutton[0] = 0;

	if (button_state[5] == 1) vbutton[1] = 1;
	else vbutton[1] = 0;
	
	/*if (keystate[SDLK_LSHIFT]) vbutton[2] = 1;
	else vbutton[2] = 0;*/
	
	if (button_state[2] == 1) vbutton[3] = 1;
	else vbutton[3] = 0;
	
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
	/*dpadi = 0;
	if (olddpad != dpad) {
		if (dpad >= 0)
			dpadi = 1;
	} else
		dpadi = 2;*/

	olddpad = dpad;

}

void GameCoreTick(void) {
	int i;
	CoreProcInput();
	eris_king_set_kram_read(0, 1);
	eris_king_set_kram_write(0, 1);
	for(i = 0x0; i <  (256*240); i+=2) {
		eris_king_kram_write(screen_buffering[i] << 8 | screen_buffering[i+1]);
	}
	voiceon = 0;
}

void ShutDownCore(void) {
}


void LoadSound(void) {
	PlayMusic_game(0);
}

void FreeSound(void) {

}


void PlayMusic_game(int itrack) {
	cd_playtrk(itrack + 2, 0);
	cd_endtrk(itrack + 3, 0x4);
	MusicPlaying = itrack;
}

void PlaySound_game(int ibank, int pan) {
}
void PlaySoundEx_game(int ibank, int pan) {
}

void SetVolume(int ivolume) {
	volume = ivolume;
}
