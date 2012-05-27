//-----------------------------------------------------------------------------
// MEKA - hq2x.c
// HQ2X filter initialization and interface.
//-----------------------------------------------------------------------------
// Based on hq2x.cpp from HQ2X distribution.
//-----------------------------------------------------------------------------

//----------------------------------------------------------
//Copyright (C) 2003 MaxSt ( maxst@hiend3d.com )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later
//version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "shared.h"
#include "hq2x.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

// Note: 512 KB goes here
extern "C"
{
unsigned int   HQ2X_LUT16to32[65536];
unsigned int   HQ2X_RGBtoYUV[65536];
}

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    HQ2X_Init(void)
{
    int i, j, k, r, g, b, Y, u, v;

    for (i=0; i<65536; i++)
        HQ2X_LUT16to32[i] = ((i & 0xF800) << 8) + ((i & 0x07E0) << 5) + ((i & 0x001F) << 3);

    for (i=0; i<32; i++)
        for (j=0; j<64; j++)
            for (k=0; k<32; k++)
            {
                r = i << 3;
                g = j << 2;
                b = k << 3;
                Y = (r + g + b) >> 2;
                u = 128 + ((r - b) >> 2);
                v = 128 + ((-r + 2*g -b)>>3);
                HQ2X_RGBtoYUV[ (i << 11) + (j << 5) + k ] = (Y<<16) + (u<<8) + v;
            }

    /*
    int nMMXsupport = 0;

    __asm
    {
        mov  eax, 1
            cpuid
            and  edx, 0x00800000
            mov  nMMXsupport, edx
    }

    return nMMXsupport;
    */
}

//-----------------------------------------------------------------------------
