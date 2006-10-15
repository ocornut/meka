//-----------------------------------------------------------------------------
// MEKA - VGM.C
// VGM File Creation
//-----------------------------------------------------------------------------

#include "shared.h"
#include <stdarg.h>
#include "db.h"

//-----------------------------------------------------------------------------

void            VGM_Header_Init(t_vgm_header *h)
{
    int         i;

    memcpy(h->Magic, VGM_MAGIC, sizeof (h->Magic));
    h->EOF_Relative = 0;                        // Unknown as of yet
    h->Version = VGM_VERSION;                   // VGM Version
    // FIXME: PSG?
    h->PSG_Speed = cur_machine.TV->CPU_clock;   // CPU Clock
    h->FM_Speed = 0;                            // Will be set back if VGM_FM_Used==YES
    h->GD3_Relative = 0;                        // Unknown as of yet
    h->N_Samples = 0;
    h->Loop_Point_Relative = 0;
    h->Loop_N_Samples = 0;
    h->Rate = cur_machine.TV->screen_frequency;
    for (i = 0; i < VGM_PADDING_SIZE; i++)
        h->Padding[i] = 0;
}

int             VGM_Start(t_vgm *VGM, char *FileName, int Logging_Accuracy)
{
    int         i;

    VGM->Logging = Logging_Accuracy;
    VGM->File = fopen(FileName, "wb");
    if (VGM->File  == NULL)
        return (MEKA_ERR_FILE_WRITE);
    VGM_Header_Init (&VGM->Header);
    GD3_Header_Init (&VGM->GD3);
    fwrite (&VGM->Header, sizeof (VGM->Header), 1, VGM->File);

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
    VGM->FM_Used = NO;
    for (i = 0; i < YM2413_REGISTERS; i++)
        if (FM_Regs[i] != 0x00) // FIXME
        {
            VGM->FM_Used = YES;
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
    VGM->Header.EOF_Relative = sizeof (VGM->Header) - 4 + VGM->DataSize;

    // Write GD3, increment EOF_Relative
    VGM->Header.EOF_Relative += GD3_Header_Write(&VGM->GD3, VGM->File);
    GD3_Header_Close (&VGM->GD3);

    // Calculate final GD3_Relative
    VGM->Header.GD3_Relative = sizeof (VGM->Header) - 0x14 + VGM->DataSize;

    // Enable YM-2413 in header is enabled
    if (VGM->FM_Used)
        VGM->Header.FM_Speed = VGM->Header.PSG_Speed;

    // Rewrite header with updated N_Samples & EOF_Relative
    fseek (VGM->File, 0, SEEK_SET);
    fwrite (&VGM->Header, sizeof (VGM->Header), 1, VGM->File);
    fclose (VGM->File);

    // Clean out
    VGM->Logging = VGM_LOGGING_NO;
    VGM->File = NULL;
}

//-----------------------------------------------------------------------------

void            VGM_Update_Timing (t_vgm *VGM)
{
    VGM->Samples_per_Frame = (cur_machine.TV->id == TVTYPE_NTSC) ? 735 : 882;
    VGM->Cycles_per_Frame  = /*VGM->Header.PSG_Speed / 60*/ cur_machine.TV_lines * opt.Cur_IPeriod;
    VGM->Samples_per_Cycle = (double)VGM->Samples_per_Frame / VGM->Cycles_per_Frame;
    VGM->Cycles_per_Sample = (double)VGM->Cycles_per_Frame / VGM->Samples_per_Frame;
}

void            VGM_NewFrame(t_vgm *VGM)
{
    if (VGM->Logging == VGM_LOGGING_ACCURACY_FRAME)
    {
        byte b;

        if (cur_machine.TV->id == TVTYPE_NTSC)
        {
            b = VGM_CMD_WAIT_735;
            VGM->Header.N_Samples += 735;
        }
        else
        {
            b = VGM_CMD_WAIT_882;
            VGM->Header.N_Samples += 882;
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
    int CurCycle = ((tsms.VDP_Line + 1) * Get_IPeriod) - Get_ICount;
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
    VGM->FM_Used = YES;
}

void            VGM_Data_Add_Wait(t_vgm *VGM, int Samples)
{
    static char    buf[3] = { VGM_CMD_WAIT, 0x00, 0x00 };

    if (Samples <= 0)
        return;
    VGM->Header.N_Samples += Samples;
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

void            GD3_Header_Init(t_gd3 *h)
{
    char *      name;

    memcpy(h->Magic, GD3_MAGIC, 4);
    h->Version = GD3_VERSION;
    h->Data_Length = 0;            // Unknown as of yet
    h->Strings[GD3_S_NAME_TRACK_ENG]  = StrDupToUnicode ("");
    h->Strings[GD3_S_NAME_TRACK_JAP]  = StrDupToUnicode ("");

    // English name
    name = DB_CurrentEntry ? DB_Entry_GetCurrentName (DB_CurrentEntry) : "";
    h->Strings[GD3_S_NAME_GAME_ENG]   = StrDupToUnicode (name);

    // Japanese name
    if (DB_CurrentEntry)
    {
        t_db_name *dbname = DB_Entry_GetNameByCountry (DB_CurrentEntry, DB_COUNTRY_JP);
        name = dbname ? dbname->name : "";
    }
    else
        name = "";
    h->Strings[GD3_S_NAME_GAME_JAP]   = StrDupToUnicode (name);

    // System, Author, Date, File author (filled if MEKA is registered), Notes
    h->Strings[GD3_S_NAME_SYSTEM_ENG] = StrDupToUnicode (cur_drv->full_name);
    h->Strings[GD3_S_NAME_SYSTEM_JAP] = StrDupToUnicode ("");
    h->Strings[GD3_S_NAME_AUTHOR_ENG] = StrDupToUnicode ("");
    h->Strings[GD3_S_NAME_AUTHOR_JAP] = StrDupToUnicode ("");
    h->Strings[GD3_S_DATE]            = StrDupToUnicode ("");
    h->Strings[GD3_S_FILE_AUTHOR]     = StrDupToUnicode (""); //registered.is ? registered.user_name_only : "");
    h->Strings[GD3_S_NOTES]           = StrDupToUnicode ("");
}

void            GD3_Header_Close(t_gd3 *h)
{
    int         i;

    for (i = 0; i < GD3_S_MAX; i++)
        free (h->Strings[i]);
}

// Write GD3 header to given Stdio file
int             GD3_Header_Write(t_gd3 *h, FILE *f)
{
    int         i;
    int         Len;
    int         Pos;
    char *      Buf;

    // Calculate Data Length
    Len = 0;
    for (i = 0; i < GD3_S_MAX; i++)
        Len += (StrLenUnicode (h->Strings[i]) + 1) * 2;
    h->Data_Length = Len;

    // Create buffer with all strings
    Buf = Memory_Alloc (Len);
    Pos = 0;
    for (i = 0; i < GD3_S_MAX; i++)
    {
        StrCpyUnicode ((word *)(Buf + Pos), h->Strings[i]);
        Pos += StrLenUnicode (h->Strings[i]) * 2;
        *(word *)(Buf + Pos) = 0x0000;
        Pos += 2;
    }

    // Asserting that Pos == Len
    if (Pos != Len)
        Quit_Msg ("Fatal Error in sound/vgm.c::GD3_Header_Write(), Pos != Len");

    // Write data
    fwrite (h, sizeof (h->Magic) + sizeof (h->Version) + sizeof (h->Data_Length), 1, f);
    fwrite (Buf, Len, 1, f);
    free (Buf);

    return (sizeof (h->Magic) + sizeof (h->Version) + sizeof (h->Data_Length) + Len);
}

//-----------------------------------------------------------------------------

