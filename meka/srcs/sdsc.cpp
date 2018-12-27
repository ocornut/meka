//-----------------------------------------------------------------------------
// MEKA - sdsc.c
// SDSC ROM Tag (designed by S8-Dev) - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "sdsc.h"

//-----------------------------------------------------------------------------
// SDSC ROM Tag - Version 1.01
//-----------------------------------------------------------------------------
// ROM Offset 0x7FE0 (4 Bytes): "SDSC" (ASCII: 0x53, 0x44, 0x53, 0x43)
// ROM Offset 0x7FE4 (1 Byte): Major Program version number (BCD)
// ROM Offset 0x7FE5 (1 Byte): Minor Program version number (BCD)
// ROM Offset 0x7FE6 (1 Byte): Day (BCD)
// ROM Offset 0x7FE7 (1 Byte): Month (BCD)
// ROM Offset 0x7FE8 (2 Bytes): Year (BCD) (Little-endian: 0x01 0x20 for 2001)
// ROM Offset 0x7FEA (2 Bytes): ROM Address of program author, zero-terminated string
// ROM Offset 0x7FEC (2 Bytes): ROM Address of program name, zero-terminated string.
// ROM Offset 0x7FEE (2 Bytes): ROM Address of program release notes, zero-terminated string.
// ROM Offset 0x7FF0 (16 Bytes): Normal SMS/GG header
//-----------------------------------------------------------------------------

// Convert a BCD number to decimal
// Note: no error handling is done, if using A-F values
static int     BCD_to_Dec(int bcd)
{
	int    ret;
	int    pow;

	ret = 0;
	pow = 1;
	while (bcd > 0)
	{
		ret += (bcd & 0xF) * pow;
		bcd >>= 4;
		pow *= 10;
	}

	return (ret);
}

//-----------------------------------------------------------------------------
// SDSC_String_Get (int offset, int verbose_error)
// Retrieve string at given ROM offset
//-----------------------------------------------------------------------------
char *  SDSC_String_Get (int offset, int verbose_error)
{
    if (offset == 0x0000 || offset == 0xFFFF)
        return (verbose_error ? strdup (Msg_Get(MSG_LoadROM_SDSC_Unknown)) : NULL);
    if (offset >= tsms.Size_ROM)
        return (verbose_error ? strdup (Msg_Get(MSG_LoadROM_SDSC_Error)) : NULL);

    int len = 0;
    char* src = (char *)(Game_ROM + offset);
    while (src[len] != EOSTR && offset + len < tsms.Size_ROM)
        len++;
    // Note: we are not relying on StrNDup there, since it relies on strlen
    char* result = (char*)malloc(sizeof (char) * (len + 1));
    strncpy (result, src, len);
    result[len] = EOSTR;
    return (result);
}

// Called after ROM loading
// Read SDSC Header and display it if found
// Return whether something has been displayed or not
// (return value currently not used)
int         SDSC_Read_and_Display (void)
{
    char *  s;
    int     offset;

    if (tsms.Size_ROM < 0x8000)
        return false;
    if (strncmp((const char *)Game_ROM + 0x7FE0, SDSC_MAGIC, 4) != 0)
        return false;

    Msg(MSGT_USER_BOX, "%s", Msg_Get(MSG_LoadROM_SDSC));

    // Name
    offset = *(u16 *)(Game_ROM + 0x7FEC);
    s = SDSC_String_Get (offset, TRUE);
    Msg(MSGT_USER_BOX, Msg_Get(MSG_LoadROM_SDSC_Name), s);
    free (s);

    // Version
    {
        int major = BCD_to_Dec (*(u8 *)(Game_ROM + 0x7FE4));
        int minor = BCD_to_Dec (*(u8 *)(Game_ROM + 0x7FE5));
        Msg(MSGT_USER_BOX, Msg_Get(MSG_LoadROM_SDSC_Version), major, minor);
    }

    // Date
    {
        int day   = BCD_to_Dec (*(u8 *) (Game_ROM + 0x7FE6));
        int month = BCD_to_Dec (*(u8 *) (Game_ROM + 0x7FE7));
        int year  = BCD_to_Dec (*(u16 *)(Game_ROM + 0x7FE8));
        if (year || month || day)
            Msg(MSGT_USER_BOX, Msg_Get(MSG_LoadROM_SDSC_Date), year, month, day);
    }

    // Author
    offset = *(u16 *)(Game_ROM + 0x7FEA);
    s = SDSC_String_Get (offset, FALSE);
    if (s)
    {
        Msg(MSGT_USER_BOX, Msg_Get(MSG_LoadROM_SDSC_Author), s);
        free (s);
    }

    // Release Note
    offset = *(u16 *)(Game_ROM + 0x7FEE);
    s = SDSC_String_Get (offset, FALSE);
    if (s)
    {
        // Msg(MSGT_USER_BOX, "len = %d", strlen(s));
        Msg(MSGT_USER_BOX, Msg_Get(MSG_LoadROM_SDSC_Release_Note), s);
        free (s);
    }

    return true;
}

//-----------------------------------------------------------------------------

