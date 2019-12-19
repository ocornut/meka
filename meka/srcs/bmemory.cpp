//-----------------------------------------------------------------------------
// MEKA - bmemory.c
// Battery Backed Memory Emulation - Code
//-----------------------------------------------------------------------------
// TODO: rename everything to OB*
//                           (OnBoard Memory)
//-----------------------------------------------------------------------------

#include "shared.h"
#include "mappers.h"
#include "eeprom.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    BMemory_Verify_Usage (void)
{
    int   i;
    const byte *p;

    while (sms.SRAM_Pages > 1)
    {
        p = &SRAM[(sms.SRAM_Pages - 1) * 0x2000];
        for (i = 0; i < 0x2000; i++)
            if (p[i] != 0x00)
                return;
        sms.SRAM_Pages--;
    }
}

void    BMemory_Get_Infos (void **data, int *len)
{
    switch (g_machine.mapper)
    {
    case MAPPER_Standard:
        BMemory_SRAM_Get_Infos (data, len);
        return;
    case MAPPER_93c46:
        BMemory_93c46_Get_Infos (data, len);
        return;
    default:
        (*data) = NULL;
        (*len) = 0;
        return;
    }
}

//-----------------------------------------------------------------------------

void        BMemory_Load (void)
{
    ALLEGRO_FILE *  f;

    // FIXME: Clear() handler
    // May want to totally move BMemory stuff to a driver based system
    memset (SRAM, 0, 0x8000);
    if (g_machine.mapper == MAPPER_93c46)
        EEPROM_93c46_Clear();

    f = al_fopen(g_env.Paths.BatteryBackedMemoryFile, "rb");
    if (f == NULL)
        return;
    switch (g_machine.mapper)
    {
    case MAPPER_Standard:       BMemory_SRAM_Load (f);  break;
    case MAPPER_93c46:          BMemory_93c46_Load (f); break;
    }
    al_fclose (f);
}

void        BMemory_Save (void)
{
    ALLEGRO_FILE *  f;

    BMemory_Verify_Usage();
    switch (g_machine.mapper)
    {
    case MAPPER_Standard:       if (sms.SRAM_Pages == 0) return; break;
    case MAPPER_93c46:          break;
    default:                    return;
    }
    if (!al_filename_exists(g_env.Paths.SavegameDirectory))
        al_make_directory(g_env.Paths.SavegameDirectory);
    f = al_fopen(g_env.Paths.BatteryBackedMemoryFile, "wb");
    switch (g_machine.mapper)
    {
    case MAPPER_Standard:       BMemory_SRAM_Save (f); break;
    case MAPPER_93c46:          BMemory_93c46_Save (f); break;
    }
    if (f)
        al_fclose (f);
}

void    BMemory_Load_State (ALLEGRO_FILE *f)
{
    switch (g_machine.mapper)
    {
    case MAPPER_Standard:       BMemory_SRAM_Load_State (f);  break;
    case MAPPER_93c46:          BMemory_93c46_Load_State (f); break;
    }
}

void    BMemory_Save_State (ALLEGRO_FILE *f)
{
    switch (g_machine.mapper)
    {
    case MAPPER_Standard:       BMemory_SRAM_Save_State (f);  break;
    case MAPPER_93c46:          BMemory_93c46_Save_State (f); break;
    }
}

//-----------------------------------------------------------------------------
// SRAM
//-----------------------------------------------------------------------------

void    BMemory_SRAM_Load (ALLEGRO_FILE *f)
{
    sms.SRAM_Pages = 0;
    do
    {
        if (al_fread ( f, &SRAM[sms.SRAM_Pages * 0x2000], 0x2000) == 1)
            sms.SRAM_Pages++;
        else
            break;
    }
    while (sms.SRAM_Pages < 4); // This is the max value

    if (sms.SRAM_Pages > 0)
        Msg(MSGT_USER, Msg_Get(MSG_SRAM_Loaded), sms.SRAM_Pages * 8);
    else
        Msg(MSGT_USER, "%s", Msg_Get(MSG_SRAM_Load_Unable));
}

void    BMemory_SRAM_Save (ALLEGRO_FILE *f)
{
    if (f && al_fwrite ( f, SRAM, sms.SRAM_Pages * 0x2000) == 1)
        Msg(MSGT_USER, Msg_Get(MSG_SRAM_Wrote), sms.SRAM_Pages * 8);
    else
        Msg(MSGT_USER, Msg_Get(MSG_SRAM_Write_Unable), sms.SRAM_Pages * 8);
}

void    BMemory_SRAM_Load_State (ALLEGRO_FILE *f)
{
    al_fread ( f, SRAM, sms.SRAM_Pages * 0x2000);
    if (sms.SRAM_Pages < 4)
        memset (SRAM + sms.SRAM_Pages * 0x2000, 0, (4 - sms.SRAM_Pages) * 0x2000);
}

void    BMemory_SRAM_Save_State (ALLEGRO_FILE *f)
{
    if (sms.SRAM_Pages > 0)
        al_fwrite ( f, SRAM, sms.SRAM_Pages * 0x2000);
}

void    BMemory_SRAM_Get_Infos (void **data, int *len)
{
    if (sms.SRAM_Pages > 0)
    {
        (*data) = SRAM;
        (*len)  = sms.SRAM_Pages * 0x2000;
    }
    else
    {
        (*data) = NULL;
        (*len) = 0;
    }
}

//-----------------------------------------------------------------------------

