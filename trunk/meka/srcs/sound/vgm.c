//-----------------------------------------------------------------------------
// MEKA - VGM.C
// VGM File Creation
//-----------------------------------------------------------------------------

#include "shared.h"
#include "db.h"
#include "fmunit.h"
#include "psg.h"
#include "tvtype.h"

//-----------------------------------------------------------------------------

void            VGM_Header_Init(t_vgm_header *h)
{
    assert(sizeof(t_vgm_header) == 64);
    memcpy(h->magic, VGM_MAGIC, sizeof (h->magic));
    h->eof_offset           = 0;                            // Unknown as of yet
    h->version_number       = VGM_VERSION;                  // VGM Version
    // FIXME: PSG?
    h->sn76489_clock        = g_machine.TV->CPU_clock;    // CPU Clock
    h->ym2413_clock         = 0;                            // Will be set back if VGM_FM_Used==TRUE
    h->gd3_offset           = 0;                            // Unknown as of yet
    h->total_samples        = 0;
    h->loop_offset          = 0;
    h->loop_samples         = 0;
    h->rate                 = g_machine.TV->screen_frequency;
    if (g_driver->snd == SND_SN76489)
    {
        h->sn76489_feedback     = 0x0009;
        h->sn76489_shift_width  = 16;
    }
    else if (g_driver->snd == SND_SN76489AN)
    {
        h->sn76489_feedback     = 0x0003;   // 2005/11/12: VGM specs are incorrect, says 0x0006
        h->sn76489_shift_width  = 15;
    }
    else
    {
        assert(0);
        h->sn76489_feedback     = 0;
        h->sn76489_shift_width  = 0;
    }
    h->_reserved            = 0;
    h->ym2612_clock         = 0;
    h->ym2151_clock         = 0;
    for (int i = 0; i != VGM_PADDING_SIZE; i++)
        h->_padding[i] = 0;
}

int             VGM_Start(t_vgm *VGM, const char *FileName, int Logging_Accuracy)
{
    int         i;

    VGM->Logging = Logging_Accuracy;
    VGM->File = fopen(FileName, "wb");
    if (VGM->File  == NULL)
        return (MEKA_ERR_FILE_WRITE);
    VGM_Header_Init(&VGM->vgm_header);
    GD3_Header_Init(&VGM->gd3_header);
    fwrite (&VGM->vgm_header, sizeof (VGM->vgm_header), 1, VGM->File);

    VGM->DataSize = 0;
    VGM->Cycles_Counter = 0;
    VGM_Update_Timing (VGM);

    // Initialize PSG State
    // Channel 0
    VGM_Data_Add_PSG (VGM, 0x90 | (PSG.Registers[1] & 0x0F));
    VGM_Data_Add_PSG (VGM, 0x80 | (PSG.Registers[0] & 0x0F));
    VGM_Data_Add_PSG (VGM, (PSG.Registers[0] >> 4) & 0x3F);
    // Channel 1
    VGM_Data_Add_PSG (VGM, 0xB0 | (PSG.Registers[3] & 0x0F));
    VGM_Data_Add_PSG (VGM, 0xA0 | (PSG.Registers[2] & 0x0F));
    VGM_Data_Add_PSG (VGM, (PSG.Registers[2] >> 4) & 0x3F);
    // Channel 2
    VGM_Data_Add_PSG (VGM, 0xD0 | (PSG.Registers[5] & 0x0F));
    VGM_Data_Add_PSG (VGM, 0xC0 | (PSG.Registers[4] & 0x0F));
    VGM_Data_Add_PSG (VGM, (PSG.Registers[4] >> 4) & 0x3F);
    // Channel 3 (Noise)
    VGM_Data_Add_PSG (VGM, 0xF0 | (PSG.Registers[7] & 0x0F));
    VGM_Data_Add_PSG (VGM, 0xE0 | (PSG.Registers[6] & 0x0F));

    // Initialize FM State
    VGM->FM_Used = FALSE;
    for (i = 0; i < YM2413_REGISTERS; i++)
        if (FM_Regs[i] != 0x00) // FIXME
        {
            VGM->FM_Used = TRUE;
            break;
        }
    if (VGM->FM_Used)
        for (i = 0; i < YM2413_REGISTERS; i++)
            if (FM_Regs_SavingFlags[i] != 0)
                VGM_Data_Add_FM (VGM, ((FM_Regs[i] & FM_Regs_SavingFlags[i]) << 8) | i);

    return (MEKA_ERR_OK);
}

void            VGM_Close(t_vgm *VGM)
{
    // Add end of file marker
    VGM_Data_Add_Byte (VGM, VGM_CMD_EOF);

    // Calculate final EOF_Relative
    VGM->vgm_header.eof_offset = sizeof (VGM->vgm_header) - 4 + VGM->DataSize;

    // Write GD3, increment EOF_Relative
    VGM->vgm_header.eof_offset += GD3_Header_Write(&VGM->gd3_header, VGM->File);
    GD3_Header_Close(&VGM->gd3_header);

    // Calculate final GD3_Relative
    VGM->vgm_header.gd3_offset = sizeof (VGM->vgm_header) - 0x14 + VGM->DataSize;   // 0x14 must be offsetof(gd3_offset)

    // Enable YM-2413 in header is enabled
    if (VGM->FM_Used)
        VGM->vgm_header.ym2413_clock = VGM->vgm_header.sn76489_clock;

    // Rewrite header with updated N_Samples & EOF_Relative
    fseek (VGM->File, 0, SEEK_SET);
    fwrite (&VGM->vgm_header, sizeof (VGM->vgm_header), 1, VGM->File);
    fclose (VGM->File);

    // Clean out
    VGM->Logging = VGM_LOGGING_NO;
    VGM->File = NULL;
}

//-----------------------------------------------------------------------------

void            VGM_Update_Timing (t_vgm *VGM)
{
    VGM->Samples_per_Frame = (g_machine.TV->id == TVTYPE_NTSC) ? 735 : 882;
    VGM->Cycles_per_Frame  = /*VGM->Header.PSG_Speed / 60*/ g_machine.TV_lines * opt.Cur_IPeriod;
    VGM->Samples_per_Cycle = (double)VGM->Samples_per_Frame / VGM->Cycles_per_Frame;
    VGM->Cycles_per_Sample = (double)VGM->Cycles_per_Frame / VGM->Samples_per_Frame;
}

void            VGM_NewFrame(t_vgm *VGM)
{
    if (VGM->Logging == VGM_LOGGING_ACCURACY_FRAME)
    {
        byte b;

        if (g_machine.TV->id == TVTYPE_NTSC)
        {
            b = VGM_CMD_WAIT_735;
            VGM->vgm_header.total_samples += 735;
        }
        else
        {
            b = VGM_CMD_WAIT_882;
            VGM->vgm_header.total_samples += 882;
        }
        VGM_Data_Add_Byte(VGM, b);
    }
    else
    {
        VGM->Cycles_Counter += VGM->Cycles_per_Frame;
    }
}

INLINE void     VGM_Wait_Samples(t_vgm *VGM)
{
    int CurCycle = ((tsms.VDP_Line + 1) * CPU_GetIPeriod()) - CPU_GetICount();
    double Waitd = ((double)(CurCycle + VGM->Cycles_Counter)) * VGM->Samples_per_Cycle;
    int Wait = Waitd;
    // Msg(0, "[%i] %i + %i * %.12f = Waiting %i samples..", tsms.VDP_Line, CurCycle, VGM->Cycles_Counter, VGM->Samples_per_Cycle, Wait);
    VGM_Data_Add_Wait(VGM, Wait);
    // VGM->Cycles_Counter = -CurCycle;
    VGM->Cycles_Counter -= ((double)Wait * VGM->Cycles_per_Sample);
}

//-----------------------------------------------------------------------------

void            VGM_Data_Add_GG_Stereo(t_vgm *VGM, byte Data)
{
    static char    buf[2] = { VGM_CMD_GG_STEREO, 0x00 };

    if (VGM->Logging == VGM_LOGGING_ACCURACY_SAMPLE)
        VGM_Wait_Samples (VGM);

    buf[1] = Data;
    fwrite (buf, 2, sizeof (char), VGM->File);
    VGM->DataSize += 2;
}

void            VGM_Data_Add_Byte(t_vgm *VGM, byte Data)
{
    // if (VGM->Logging == VGM_LOGGING_ACCURACY_SAMPLE)
    //    VGM_Wait_Samples (VGM);

    fwrite (&Data, 1, sizeof (byte), VGM->File);
    VGM->DataSize += 1;
}

void            VGM_Data_Add_PSG(t_vgm *VGM, byte Data)
{
    static char    buf[2] = { VGM_CMD_PSG, 0x00 };

    if (VGM->Logging == VGM_LOGGING_ACCURACY_SAMPLE)
        VGM_Wait_Samples (VGM);

    buf[1] = Data;
    fwrite (buf, 2, sizeof (byte), VGM->File);
    VGM->DataSize += 2;
}

void            VGM_Data_Add_FM(t_vgm *VGM, int RegData)
{
    static char    buf[3] = { VGM_CMD_YM2413, 0x00, 0x00 };

    if (VGM->Logging == VGM_LOGGING_ACCURACY_SAMPLE)
        VGM_Wait_Samples (VGM);

    *(word *)&buf[1] = RegData;
    fwrite (buf, 3, sizeof (byte), VGM->File);
    VGM->DataSize += 3;
    VGM->FM_Used = TRUE;
}

void            VGM_Data_Add_Wait(t_vgm *VGM, int Samples)
{
    static char    buf[3] = { VGM_CMD_WAIT, 0x00, 0x00 };

    if (Samples <= 0)
        return;
    VGM->vgm_header.total_samples += Samples;
    if (Samples > 0xFFFF)
    {
        *(word *)&buf[1] = 0xFFFF;
        do
        {
            fwrite (buf, 3, sizeof (byte), VGM->File);
            VGM->DataSize += 3;
            Samples -= 0xFFFF;
        }
        while (Samples > 0xFFFF);
    }
    *(word *)&buf[1] = Samples;
    fwrite (buf, 3, sizeof (byte), VGM->File);
    VGM->DataSize += 3;
}

//-----------------------------------------------------------------------------
// GD3
//-----------------------------------------------------------------------------

void            GD3_Header_Init(t_gd3_header *h)
{
    const char *name;

    memcpy(h->magic, GD3_MAGIC, 4);
    h->version = GD3_VERSION;
    h->data_length = 0;            // Unknown as of yet
    h->strings[GD3_S_NAME_TRACK_ENG]  = StrDupToU16 ("");
    h->strings[GD3_S_NAME_TRACK_JAP]  = StrDupToU16 ("");

    // English name
    name = DB.current_entry ? DB_Entry_GetCurrentName (DB.current_entry) : "";
    h->strings[GD3_S_NAME_GAME_ENG]   = StrDupToU16 (name);

    // Japanese name
    if (DB.current_entry)
    {
        const t_db_name *dbname = DB_Entry_GetNameByCountry(DB.current_entry, DB_COUNTRY_JP);
        name = dbname ? dbname->name : "";
    }
    else
	{
        name = "";
	}
    h->strings[GD3_S_NAME_GAME_JAP]   = StrDupToU16 (name);

    // System, Author, Date, File author (filled if MEKA is registered), Notes
    h->strings[GD3_S_NAME_SYSTEM_ENG] = StrDupToU16 (g_driver->full_name);
    h->strings[GD3_S_NAME_SYSTEM_JAP] = StrDupToU16 ("");
    h->strings[GD3_S_NAME_AUTHOR_ENG] = StrDupToU16 ("");
    h->strings[GD3_S_NAME_AUTHOR_JAP] = StrDupToU16 ("");
    h->strings[GD3_S_DATE]            = StrDupToU16 ("");
    h->strings[GD3_S_FILE_AUTHOR]     = StrDupToU16 (""); //registered.is ? registered.user_name_only : "");
    h->strings[GD3_S_NOTES]           = StrDupToU16 ("");
}

void            GD3_Header_Close(t_gd3_header *h)
{
    int         i;

    for (i = 0; i != GD3_S_MAX; i++)
        free (h->strings[i]);
}

// Write GD3 header to given Stdio file
int             GD3_Header_Write(t_gd3_header *h, FILE *f)
{
    int         i;

    // Calculate Data Length
    int Len = 0;
    for (i = 0; i != GD3_S_MAX; i++)
        Len += (StrLenU16 (h->strings[i]) + 1) * 2;
    h->data_length = Len;

    // Create buffer with all strings
    char* Buf = (char*)Memory_Alloc (Len);
    int Pos = 0;
    for (i = 0; i < GD3_S_MAX; i++)
    {
        StrCpyU16 ((word *)(Buf + Pos), h->strings[i]);
        Pos += StrLenU16 (h->strings[i]) * 2;
        *(word *)(Buf + Pos) = 0x0000;
        Pos += 2;
    }

    // Asserting that Pos == Len
    if (Pos != Len)
        Quit_Msg("Fatal Error in sound/vgm.c::GD3_Header_Write(), Pos != Len");

    // Write data
    fwrite (h, sizeof (h->magic) + sizeof (h->version) + sizeof (h->data_length), 1, f);
    fwrite (Buf, Len, 1, f);
    free (Buf);

    return (sizeof (h->magic) + sizeof (h->version) + sizeof (h->data_length) + Len);
}

//-----------------------------------------------------------------------------

