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

#define Uint8 uint8_t
#define Uint16 uint16_t
#define Uint32 uint32_t
#define Uint64 uint64_t
#define Sint8 int8_t
#define Sint16 int16_t
#define Sint32 int
#define Sint64 int64_t

extern void InitGameCore(void);
extern void GameCoreTick(void);
extern void ShutDownCore(void);

extern void PlayMusic_game(int itrack);
extern void PlaySound_game(int ibank, int pan);
extern void PlaySoundEx_game(int ibank, int pan);
extern void SetVolume(int volume);
extern void LoadSound_game(void);
void FreeSound(void);
extern void Terminate(void);

extern Sint8 olddpad;
