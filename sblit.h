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

extern void DrawLine2(Uint16 x0, Uint16 y0, Uint16 x1, Uint16 y1, Uint8 color);
extern void DrawLine(Uint16 x0, Uint16 y0, Uint16 x1, Uint16 y1, Uint8 color);
extern void DrawRect(Uint16 x0, Uint16 y0, Uint16 x1, Uint16 y1, Uint8 color);
extern void DarkRect(Uint16 x0, Uint16 y0, Uint16 x1, Uint16 y1);

extern void s_drawtext(Uint16 x, Uint16 y, Uint8 c);
