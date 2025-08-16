//-----------------------------------------------------------------------------
// MEKA - rapidfir.c
// Simple rapid fire emulation/simulation - Code
//-----------------------------------------------------------------------------
// Note: a program could be written to analyze the speed of an official
// Sega Master System Rapid Fire, and simulate that as well.
//-----------------------------------------------------------------------------

#include "shared.h"
#include "rapidfir.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

int RapidFire;
int RapidFire_Count;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    RapidFire_Init()
{
    RapidFire = 0;
}

void    RapidFire_Update()
{
    RapidFire_Count = (RapidFire_Count + 1) & 7;
    if (RapidFire_Count & 4)
    {
        if (RapidFire & RAPIDFIRE_J1B1) tsms.Control[7] |= 0x0010;
        if (RapidFire & RAPIDFIRE_J1B2) tsms.Control[7] |= 0x0020;
        if (RapidFire & RAPIDFIRE_J2B1) tsms.Control[7] |= 0x0400;
        if (RapidFire & RAPIDFIRE_J2B2) tsms.Control[7] |= 0x0800;
    }
}

void    RapidFire_Switch_J1B1()
{
    RapidFire ^= RAPIDFIRE_J1B1;
    if (RapidFire & RAPIDFIRE_J1B1)
        Msg(MSGT_USER, Msg_Get(MSG_RapidFire_JxBx_On), 1, 1);
    else
        Msg(MSGT_USER, Msg_Get(MSG_RapidFire_JxBx_Off), 1, 1);
}

void    RapidFire_Switch_J1B2()
{
    RapidFire ^= RAPIDFIRE_J1B2;
    if (RapidFire & RAPIDFIRE_J1B2)
        Msg(MSGT_USER, Msg_Get(MSG_RapidFire_JxBx_On), 1, 2);
    else
        Msg(MSGT_USER, Msg_Get(MSG_RapidFire_JxBx_Off), 1, 2);
}

void    RapidFire_Switch_J2B1()
{
    RapidFire ^= RAPIDFIRE_J2B1;
    if (RapidFire & RAPIDFIRE_J2B1)
        Msg(MSGT_USER, Msg_Get(MSG_RapidFire_JxBx_On), 2, 1);
    else
        Msg(MSGT_USER, Msg_Get(MSG_RapidFire_JxBx_Off), 2, 1);
}

void    RapidFire_Switch_J2B2()
{
    RapidFire ^= RAPIDFIRE_J2B2;
    if (RapidFire & RAPIDFIRE_J2B2)
        Msg(MSGT_USER, Msg_Get(MSG_RapidFire_JxBx_On), 2, 2);
    else
        Msg(MSGT_USER, Msg_Get(MSG_RapidFire_JxBx_Off), 2, 2);
}

//-----------------------------------------------------------------------------

