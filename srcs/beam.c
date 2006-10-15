//-----------------------------------------------------------------------------
// MEKA - beam.c
// VDP Refresh Beam Position - Code
//-----------------------------------------------------------------------------

#include "shared.h"

//-----------------------------------------------------------------------------

// Horizontal Beam Counter (X)
// Two dots are equivalent to one count, and three counts are equivalent
// to four CPU clock pulses (1H = 342 dots = 171 counts = 228 CPU clock pulses)

// F4h ---- FFh | 00h --------- 20h --LCD-- 6FH -------- 93H | E9H ---- F3H
//  <-- 12 --->   <------------------ 148 ----------------->   <--- 11 --->

int         Beam_Calc_X (void)
{
    int     c;

    c = ((Get_IPeriod - Get_ICount) * 256) / Get_IPeriod;
    return (c);

  /*c = ((sms.R.IPeriod - sms.R.ICount) * 0x80) / sms.R.IPeriod;
  return (0x08 + c);*/

  /*c = (sms.R.IPeriod - sms.R.ICount) * 0.75;
  c %= 171;
  if (c < 12)
     return (c + 244);
  if (c < 12 + 148)
     return (c - 12);
  return (c + 72);*/
}

int         Beam_X (void)
{
    return (LightGun_X ()); // FIXME: ...
    // return (Beam_Calc_X () / 2);
}

// Vertical Beam Counter (Y)
// One line is equivalent to one count

// NTSC
// 1 frame = 262 lines
// D8H ---- FFH | 00H ----- 18H --LCD-- A7H ----- DAH | D5H -- D7H
// At the end of the effective screen, counter jump from DAH to D5H

// PAL/SECAM
// 1 frame = 313 lines
// 00h -- F2h | BAh -- FFh (unconfirmed)

INLINE int  Beam_Calc_Y (void)
{
    int c = tsms.VDP_Line;
    if (Get_ICount < 8)
        c = (c + 1) % cur_machine.TV_lines;

    // Msg (MSGT_USER, "At PC=%04X, Read Beam Y%s", sms.R.PC.W, (Get_ICount < 8) ? " (Affected)" : "");
    // Msg (MSGT_USER, "At PC=%04X, Read Beam Y, returning %d", sms.R.PC.W, (c < 256) ? c : 255);

    // return ((c < 255) ? c : 255);

    switch (cur_machine.TV->id)
    {
    case TVTYPE_NTSC:
        {
            if (c <= 0xDA)
                return (c);
            //if (c - 6 == 0xE0 || c - 6 == 0xDF)
            //{
            //    Msg(MSGT_USER, "PC = %04X", CPU_GetPC);
            //}
            return (c - 6);
        }
    case TVTYPE_PAL_SECAM:
        {
            if (c <= 0xF2)
                //Msg (MSGT_USER, "%d @ Read Beam Y, returning %X", tsms.VDP_Line, c);
                return (c);
            // Msg (MSGT_USER, "%d @ Read Beam Y, returning %X", tsms.VDP_Line, c - 57);
            return (c - 57);
        }
    default:
        {
            Msg (MSGT_USER, "BeamY Error: unknown TV Type");
            return ((c < 255) ? c : 255);
        }
    }
}

int         Beam_Y (void)
{
    return (Beam_Calc_Y ());
}

//-----------------------------------------------------------------------------

