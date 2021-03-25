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
#ifdef LINUX
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#else
#include <windows.h>
#endif
#include "ssystem.h"
#include "gameloop.h"
#include "initgame.h"
#include "mapdata.h"

#ifdef LINUX
void _start()
#else
void WinMainCRTStartup(VOID)
#endif
//int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	InitGameCore();
	ResetMap();
	StartGameLoop();
	#ifdef LINUX
	_exit(0);
	#endif
}
