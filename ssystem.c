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
#include <windows.h>
#include <mmsystem.h>
#include <stdint.h>
#include <stdbool.h>
#include "vars.h"
#include "ssystem.h"
#include "zmath.h"

#define DEFAULT_WINDOW_WIDTH 640
#define DEFAULT_WINDOW_HEIGHT 480

#define INTERNAL_WINDOW_WIDTH 320
#define INTERNAL_WINDOW_HEIGHT 240

/* Minimum amount of milliseconds for each update cycle:*/
#define STEP_RATE 13
BITMAPINFO* bi;
uint16_t i;
MSG msg;


HINSTANCE g_hInstance;
HWND g_hWnd;
HDC g_hDC;
HDC mdc;

HBITMAP bitmap;
HGDIOBJ oldbmp;

static unsigned char button_state[8], button_time[8];


void Terminate(void)
{
}

void InitGameCore(void)
{
	U32 color[16];
	InitMath();
	
	/* It's set to 640x480 but actually, we will detect the screen resolution later (unless WINDOWED is set) */
	g_hDC = GetDC(CreateWindow("STATIC", 0, 
	WS_POPUP | WS_VISIBLE | WS_MAXIMIZE,
	0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT + 25, 0, 0, 0, 0));

	bi = (BITMAPINFO*)HeapAlloc(GetProcessHeap(), 0, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256);
	bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi->bmiHeader.biWidth = INTERNAL_WINDOW_WIDTH;
	bi->bmiHeader.biHeight = -INTERNAL_WINDOW_HEIGHT;  //negative so (0,0) is at top left
	bi->bmiHeader.biPlanes = 1;
	bi->bmiHeader.biBitCount = 8;
	bi->bmiHeader.biClrUsed	= 256;

	color[0] = 0x000000;
	color[1] = 0x0000AA;
	color[2] = 0x00AA00;
	color[3] = 0x00AAAA;
	color[4] = 0xAA0000;
	color[5] = 0xAA00AA;
	color[6] = 0xAA5500;
	color[7] = 0xAAAAAA;
	color[8] = 0x555555;
	color[9] = 0x5555FF;
	color[10] = 0x55FF55;
	color[11] = 0x55FFFF;
	color[12] = 0xFF5555;
	color[13] = 0xFF55FF;
	color[14] = 0xFFFF55;
	color[15] = 0xFFFFFF;

	for (i = 0; i < 16; i++) {
		bi->bmiColors[i].rgbRed = (color[i] / 65536);
		bi->bmiColors[i].rgbGreen = (color[i] / 256) % 256;
		bi->bmiColors[i].rgbBlue = (color[i]) % 256;
	}

	for (i = 16; i < 256; i++) {
		bi->bmiColors[i].rgbRed  = bi->bmiColors[i - 16].rgbRed * 8 / 10;
		bi->bmiColors[i].rgbGreen = bi->bmiColors[i - 16].rgbGreen * 8 / 10;
		bi->bmiColors[i].rgbBlue = bi->bmiColors[i - 16].rgbBlue * 8 / 10;
	}
}

static Uint8 i_keyb[8];
Sint8 olddpad = -1;

static void CoreProcInput(void)
{
	Uint8 i;
	int pad;
	
	for (i=0;i<8;i++)
	{
		switch (i)
		{
			case 0:
			default:
				pad = VK_TAB;
			break;
			case 1:
				pad = VK_BACK;
			break;
			case 2:
				pad = VK_PRIOR;
			break;
			case 3:
				pad = VK_RETURN;
			break;
			case 4:
				pad = VK_CONTROL;
			break;
			case 5:
				pad = VK_NEXT;
			break;
			case 6:
				pad = VK_MENU;
			break;
			case 7:
				pad = VK_LSHIFT;
			break;
		}
		
		switch (button_state[i])
		{
			case 0:
				if (GetAsyncKeyState(pad))
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
				if (!(GetAsyncKeyState(pad)))
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
	
	if (GetAsyncKeyState(VK_UP)) i_keyb[0] = 1;
	else i_keyb[0] = 0;
	
	if (GetAsyncKeyState(VK_RIGHT)) i_keyb[2] = 1;
	else i_keyb[2] = 0;
	
	if (GetAsyncKeyState(VK_DOWN)) i_keyb[4] = 1;
	else i_keyb[4] = 0;
	
	if (GetAsyncKeyState(VK_LEFT)) i_keyb[6] = 1;
	else i_keyb[6] = 0;
	
	if (GetAsyncKeyState(VK_RETURN)) vbutton[6] = 1;
	else vbutton[6] = 0;

	if (button_state[4] == 1 || button_state[3] == 1) vbutton[0] = 1;
	else vbutton[0] = 0;

	if (button_state[5] == 1 || button_state[6] == 1) vbutton[1] = 1;
	else vbutton[1] = 0;
	
	if (GetAsyncKeyState(VK_RSHIFT)) vbutton[2] = 1;
	else vbutton[2] = 0;
	
	if (button_state[2] == 1 || button_state[7] == 1) vbutton[3] = 1;
	else vbutton[3] = 0;
	
	if (GetAsyncKeyState(VK_ESCAPE))
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

	for (i=0;i<7;i++)
	if (i_keyb[i]) dpad=i;

	if ((i_keyb[0]) & (i_keyb[2])) dpad=1;
	if ((i_keyb[2]) & (i_keyb[4])) dpad=3;
	if ((i_keyb[4]) & (i_keyb[6])) dpad=5;
	if ((i_keyb[6]) & (i_keyb[0])) dpad=7;
	
	//no more volume control
	dpadi = 0;
	if (olddpad != dpad) {
		if (dpad >= 0)
			dpadi = 1;
	} else
		dpadi = 2;

	olddpad = dpad;
}


void GameCoreTick(void)
{
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0) DispatchMessage(&msg);
	
	CoreProcInput();
	
	StretchDIBits(g_hDC,0,0,
	GetDeviceCaps(g_hDC,HORZRES),GetDeviceCaps(g_hDC,VERTRES),
	0,0,INTERNAL_WINDOW_WIDTH,INTERNAL_WINDOW_HEIGHT, screen_buffering,
	bi,
	DIB_RGB_COLORS, SRCCOPY);
	
	if (gamespeed > 0)
	{
		Sleep(gamespeed * 10);
	}

	voiceon = 0;
}

void ShutDownCore(void) {
}

void LoadSound_game(void) {
}

void FreeSound(void) {
}

void PlayMusic_game(unsigned char itrack) {

}
void PlaySound_game(unsigned char ibank, signed char pan) {
}
void PlaySoundEx_game(unsigned char ibank, signed char pan) {
}

void SetVolume(unsigned char ivolume) {
}
