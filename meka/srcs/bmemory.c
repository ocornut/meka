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
    FILE *  f;

    // FIXME: Clear() handler
    // May want to totally move BMemory stuff to a driver based system
    memset (SRAM, 0, 0x8000);
    if (g_machine.mapper == MAPPER_93c46)
        EEPROM_93c46_Clear();

    f = fopen(g_env.Paths.BatteryBackedMemoryFile, "rb");
    if (f == NULL)
        return;
    switch (g_machine.mapper)
    {
    case MAPPER_Standard:       BMemory_SRAM_Load (f);  break;
    case MAPPER_93c46:          BMemory_93c46_Load (f); break;
    }
    fclose (f);
}

void        BMemory_Save (void)
{
	FILE *  f;

	BMemory_Verify_Usage();
	switch (g_machine.mapper)
	{
	case MAPPER_Standard:       if (sms.SRAM_Pages == 0) return; break;
	case MAPPER_93c46:          break;
	default:                    return;
	}
	if (!al_filename_exists(g_env.Paths.SavegameDirectory))
		al_make_directory(g_env.Paths.SavegameDirectory);
	f = fopen(g_env.Paths.BatteryBackedMemoryFile, "wb");
	switch (g_machine.mapper)
	{
	case MAPPER_Standard:       BMemory_SRAM_Save (f); break;
	case MAPPER_93c46:          BMemory_93c46_Save (f); break;
	}
	if (f)
		fclose (f);
}

void    BMemory_Load_State (FILE *f)
{
	switch (g_machine.mapper)
	{
	case MAPPER_Standard:       BMemory_SRAM_Load_State (f);  break;
	case MAPPER_93c46:          BMemory_93c46_Load_State (f); break;
	}
}

void    BMemory_Save_State (FILE *f)
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

void    BMemory_SRAM_Load (FILE *f)
{
	sms.SRAM_Pages = 0;
	do
	{
		if (fread (&SRAM[sms.SRAM_Pages * 0x2000], 0x2000, 1, f) == 1)
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

void    BMemory_SRAM_Save (FILE *f)
{
	if (f && fwrite (SRAM, sms.SRAM_Pages * 0x2000, 1, f) == 1)
		Msg(MSGT_USER, Msg_Get(MSG_SRAM_Wrote), sms.SRAM_Pages * 8);
	else
		Msg(MSGT_USER, Msg_Get(MSG_SRAM_Write_Unable), sms.SRAM_Pages * 8);
}

void    BMemory_SRAM_Load_State (FILE *f)
{
	fread (SRAM, sms.SRAM_Pages * 0x2000, 1, f);
	if (sms.SRAM_Pages < 4)
		memset (SRAM + sms.SRAM_Pages * 0x2000, 0, (4 - sms.SRAM_Pages) * 0x2000);
}

void    BMemory_SRAM_Save_State (FILE *f)
{
	if (sms.SRAM_Pages > 0)
		fwrite (SRAM, sms.SRAM_Pages * 0x2000, 1, f);
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

