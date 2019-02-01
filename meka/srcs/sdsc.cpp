//-----------------------------------------------------------------------------
// MEKA - sdsc.c
// SDSC ROM Tag and Debug Console (designed by S8-Dev) - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "sdsc.h"
#include "debugger.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define SDSC_MAGIC      "SDSC"

bool have_sdsc_tag = false;

//-----------------------------------------------------------------------------
// SDSC command buffer state
//-----------------------------------------------------------------------------
int num_pending_bytes = 0;
char byte_buffer[3]; // We fill this in for multi-byte commands
char* p_next_byte = byte_buffer; // Points to the next one to write

//-----------------------------------------------------------------------------
// State for formatting
//-----------------------------------------------------------------------------
// This is the size of the buffer we accumulate into. We flush when we get a newline.
// Overflow is not handled particularly well.
const int sdsc_console_buffer_size = 16 * 1024;
// This is the buffer we accumulate into
char sdsc_console_buffer[sdsc_console_buffer_size];
// This is the location in the buffer for the next character to go to
char* p_next_char = sdsc_console_buffer;
// This is the offset in the buffer of the most recent format string,
// pointing at the leading '%'. We set it to NULL when not in a format
// string, and thus it also signals whether formatting is needed.
char* p_format_start = NULL;


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

    have_sdsc_tag = false;

    if (tsms.Size_ROM < 0x8000)
        return false;
    if (strncmp((const char *)Game_ROM + 0x7FE0, SDSC_MAGIC, 4) != 0)
        return false;

    Msg(MSGT_USER_LOG, "%s", Msg_Get(MSG_LoadROM_SDSC));

    // Name
    offset = *(u16 *)(Game_ROM + 0x7FEC);
    s = SDSC_String_Get (offset, TRUE);
    Msg(MSGT_USER_LOG, Msg_Get(MSG_LoadROM_SDSC_Name), s);
    free (s);

    // Version
    {
        int major = BCD_to_Dec (*(u8 *)(Game_ROM + 0x7FE4));
        int minor = BCD_to_Dec (*(u8 *)(Game_ROM + 0x7FE5));
        Msg(MSGT_USER_LOG, Msg_Get(MSG_LoadROM_SDSC_Version), major, minor);
    }

    // Date
    {
        int day   = BCD_to_Dec (*(u8 *) (Game_ROM + 0x7FE6));
        int month = BCD_to_Dec (*(u8 *) (Game_ROM + 0x7FE7));
        int year  = BCD_to_Dec (*(u16 *)(Game_ROM + 0x7FE8));
        if (year || month || day)
            Msg(MSGT_USER_LOG, Msg_Get(MSG_LoadROM_SDSC_Date), year, month, day);
    }

    // Author
    offset = *(u16 *)(Game_ROM + 0x7FEA);
    s = SDSC_String_Get (offset, FALSE);
    if (s)
    {
        Msg(MSGT_USER_LOG, Msg_Get(MSG_LoadROM_SDSC_Author), s);
        free (s);
    }

    // Release Note
    offset = *(u16 *)(Game_ROM + 0x7FEE);
    s = SDSC_String_Get (offset, FALSE);
    if (s)
    {
        // Msg(MSGT_USER_BOX, "len = %d", strlen(s));
        Msg(MSGT_USER_LOG, Msg_Get(MSG_LoadROM_SDSC_Release_Note), s);
        free (s);
    }

    have_sdsc_tag = true;

    return true;
}

void SDSC_Debug_Console_Control(char c)
{
    if (!have_sdsc_tag)
    {
        return;
    }

    *p_next_byte++ = c;
    if (--num_pending_bytes == -1)
    {
        // First byte of sequence
        switch (c)
        {
        case 1:
        case 2:
            num_pending_bytes = 0;
            break;
        case 3:
            num_pending_bytes = 1;
            break;
        case 4:
            num_pending_bytes = 2;
            break;
        default:
            Msg(MSGT_USER_LOG, "SDSC debug console control: unexpected command %02x", c);
            num_pending_bytes = 0;
        }
    }

    if (num_pending_bytes == 0)
    {
        // Sequence done
        switch (byte_buffer[0])
        {
        case 1:
            g_machine_pause_requests = 1;
            Debugger_Printf("SDSC debug console control: suspend emulation");
            break;
        case 2:
            Msg(MSGT_USER_LOG, "SDSC debug console control: clear console (not implemented)");
            break;
        case 3:
            Msg(MSGT_USER_LOG, "SDSC debug console control: set attribute to %02x (not implemented)", byte_buffer[1]);
            break;
        case 4:
            Msg(MSGT_USER_LOG, "SDSC debug console control: set cursor position to %d, %d (not implemented)", byte_buffer[1], byte_buffer[2]);
            break;
        default:
            Msg(MSGT_USER_LOG, "SDSC debug console control: unexpected value %d (not implemented)", byte_buffer[0]);
            break;
        }

        // Reset pointer
        p_next_byte = byte_buffer;
    }
}

// Formats and emits the error message into the SDSC console,
// setting state variables such that the format string is consumed
// Always returns false so it can be combined with a return
bool SDSC_Format_Error(const char* format, ...)
{
    if (!have_sdsc_tag)
    {
        return false;
    }

    // Print the message over the format string
    va_list params;
    va_start(params, format);
    const int chars_added = vsprintf_s(p_format_start, sdsc_console_buffer_size - (p_format_start - sdsc_console_buffer), format, params);
    va_end(params);
    // Point to the following char
    p_next_char = p_format_start + chars_added;
    // Signal that the formatting has been consumed
    p_format_start = NULL;
    // Always return false
    return false;
}

// Tries to parse the format string into the parameters
// Returns true if the format string was correctly consumed
// If there was an error, we return false and emit an error via SDSC_Format_Error()
bool SDSC_Try_Parse_Format(int& width, char& format, int& data_type, int& parameter)
{
    // We iterate through and use this to determine which part we are working on
    int state = 0;
    int parameter_bytes_remaining = 0;
    // We stick a bit in here so we can shift more in and detect when we're done
    // when this reaches the third byte
    data_type = 1;
    // We accumulate the ASCII width into here.
    // An explicit zero width ought to be an error, but we don't handle that,
    // and instead use zero as a signal for auto width.
    width = 0;
    // We accumulate the "parameter" into here.
    parameter = 0;

    // We walk from p_format_start to p_next_char-1 and see if it's valid
    // If it is, we do the work and replace the format string with the result, and point pNextChar after it
    // Else we just return
    for (char* p_char = p_format_start + 1; p_char < p_next_char; ++p_char)
    {
        const char c = *p_char;
        switch (state)
        {
        case 0:
            // Optional width specifier
            if (c >= '0' && c <= '9')
            {
                width *= 10;
                width += c - '0';
                break;
            }
            // Else fall through
        case 1:
            // Format specifier
            if (width > 256)
            {
                return SDSC_Format_Error("[Excessive width %d]", width);
            }
            switch (c)
            {
            case 'd':
            case 'u':
            case 'x':
            case 'X':
            case 'b':
            case 'a':
            case 's':
                format = c;
                state = 2;
                break;
            case '%':
                // Special case: "%%" = "%"
                format = c;
                return true;
            default:
                return SDSC_Format_Error("[Invalid format type '%c']", c);
            }
            break;
        case 2:
            // Data type
            // We mash chars together into an int...
            data_type <<= 8;
            data_type |= c;
            if (data_type & 0x010000)
            {
                data_type &= 0xffff;
                // We have two bytes
                // The second one is enough to determine the width
                switch (c)
                {
                case 'w':
                case 'b':
                    // Memory or VRAM address
                    parameter_bytes_remaining = 2;
                    break;
                case 'r':
                    // Register index
                    parameter_bytes_remaining = 1;
                    break;
                default:
                    SDSC_Format_Error("[Invalid data type '%c%c']", (data_type >> 8) & 0xff, c);
                    return false;
                }
                ++state;
            }
            break;
        case 3:
            // First parameter byte
            parameter |= (u8)c;
            ++state;
            if (--parameter_bytes_remaining == 0)
            {
                // Skip state 4
                ++state;
            }
            break;
        case 4:
            // Second parameter byte
            parameter |= (u8)c << 8;
            ++state;
            break;
        default:
            SDSC_Format_Error("[Formatting parser reached an invalid state]");
            return false;
        }
    }

    // Success if we got to 5
    return state == 5;
}

// Macro for combining two-character codes into ints
// Endianness needs to match the way we parse it in
#define FORMAT_TYPE(s) ((s)[0] << 8 | (s)[1])

bool SDSC_Get_Data(u16& data, int data_type, int parameter)
{
    switch (data_type)
    {
    case FORMAT_TYPE("mw"):
        // Read 16-bit data from Z80 address
        data = RdZ80_NoHook(parameter & 0xffff) | RdZ80_NoHook((parameter + 1) & 0xffff) << 8;
        break;
    case FORMAT_TYPE("mb"):
        // Read 8-bit data from Z80 address
        data = RdZ80_NoHook(parameter & 0xffff);
        break;
    case FORMAT_TYPE("vw"):
        // Read 16-bit data from VRAM
        data = VRAM[parameter & 0x3fff] | VRAM[(parameter + 1) & 0x3fff] << 8;
        break;
    case FORMAT_TYPE("vb"):
        // Read 8-bit data from VRAM
        data = VRAM[parameter & 0x3fff];
        break;
    case FORMAT_TYPE("pr"):
        // Read processor register
        // ASCII values are a non-standard extension
        switch (parameter)
        {
        case 'b':
        case 0x00: data = sms.R.BC.B.h; break;
        case 'c':
        case 0x01: data = sms.R.BC.B.l; break;
        case 'e':
        case 0x02: data = sms.R.DE.B.h; break;
        case 'd':
        case 0x03: data = sms.R.DE.B.l; break;
        case  'h':
        case 0x04: data = sms.R.HL.B.h; break;
        case 'l':
        case 0x05: data = sms.R.HL.B.l; break;
        case 'a':
        case 0x06: data = sms.R.AF.B.h; break;
        case 'f':
        case 0x07: data = sms.R.AF.B.l; break;
        case 'p':
        case 0x08: data = sms.R.PC.W; break;
        case 's':
        case 0x09: data = sms.R.SP.W; break;
        case 'x':
        case 0x0a: data = sms.R.IX.W; break;
        case 'y':
        case 0x0b: data = sms.R.IY.W; break;
        case 'B':
        case 0x0c: data = sms.R.BC.W; break;
        case 'D':
        case 0x0d: data = sms.R.DE.W; break;
        case 'H':
        case 0x0e: data = sms.R.HL.W; break;
        case 'A':
        case 0x0f: data = sms.R.AF.W; break;
        case 'r':
        case 0x10: data = sms.R.R; break;
        case 'i':
        case 0x11: data = sms.R.I; break;
        case 0x12: data = sms.R.BC1.W; break;
        case 0x13: data = sms.R.DE1.W; break;
        case 0x14: data = sms.R.HL1.W; break;
        case 0x15: data = sms.R.AF1.W; break;
        default:
            return SDSC_Format_Error("[Invalid processor register index %02x]", parameter);
        }
        break;
    case FORMAT_TYPE("vr"):
        if (parameter < 16)
        {
            // VDP register
            data = sms.VDP[parameter];
        }
        else if (parameter < 48)
        {
            // Palette
            int index = parameter - 16;
            switch (g_machine.driver_id)
            {
            case DRV_SMS:
                data = PRAM[index];
                break;
            case DRV_GG:
                index *= 2;
                data = PRAM[index] | ((u8)PRAM[index + 1] << 8);
                break;
            default:
                return SDSC_Format_Error("[Can't read palette for this emulated system]");
            }
        }
        else
        {
            return SDSC_Format_Error("[Invalid VDP register index %02x]", parameter);
        }
        break;
    default:
        return SDSC_Format_Error("[Invalid data type '%c%c']", (char)((data_type >> 8) & 0xff), (char)(data_type & 0xff));
    }
    // If we get here, it worked
    return true;
}

// Prints data as binary to buffer, skipping leading zeroes
int SDSC_Print_Binary(char* buffer, u16 data)
{
    char* p = buffer;
    for (int i = 0; i < 16; ++i)
    {
        const int bit = data >> (15 - i) & 1;
        if (bit == 0 && p == buffer && i < 15)
        {
            // Skip leading zeroes
            continue;
        }
        *p++ = '0' + bit;
    }
    return p - buffer;
}

// Prints data as ASCII to buffer
// Non-printable is emitted as '.'
// isChar = 1 will make us print exactly one char, even if the data is 0
// isChar = 0 will make us print up to <width> characters, or until a 0 is encountered
// (null-terminated string)
// offset is an address in the Z80 memory, or VRAM
int SDSC_Print_ASCII(char* buffer, int buffer_size, bool is_vram, bool is_char, int offset, int width)
{
    // Determine how many characters to emit
    const int max_chars = is_char ? 1 : width > 0 ? width : buffer_size;
    // We support wrapping the offset as we go
    char* p = buffer;
    for (int i = 0; i < max_chars; ++i)
    {
        // Get char
        const char c = is_vram ? VRAM[offset & 0x3fff] : RdZ80_NoHook(offset & 0xffff);
        // Stop on null for strings
        if (c == 0 && !is_char)
        {
            break;
        }
        // Put into buffer
        *p++ = isprint(c) ? c : '.';
        // Move on
        ++offset;
    }
    return p - buffer;
}

// Tries to format the text and emit it to the console
// Leaves global state untouched when there is insufficient data to produce a result
// (either successful formatting or an error message due to an invalid format string)
void SDSC_Try_Format(void)
{
    // First parse the format
    int width;
    char format;
    int data_type;
    int parameter;
    if (!SDSC_Try_Parse_Format(width, format, data_type, parameter))
    {
        return;
    }

    // Handle "%%" -> "%"
    if (format == '%')
    {
        p_next_char = p_format_start + 1;
        p_format_start = NULL;
        return;
    }

    // Now get the data
    u16 data;
    if (!SDSC_Get_Data(data, data_type, parameter))
    {
        return;
    }

    // Then print it into a local temporary buffer
    const int buffer_size = 256 + 1; // Spec says max width is 256
    char buffer[buffer_size];
    int formatted_length;
    switch (format)
    {
    case 'd':
        formatted_length = sprintf_s(buffer, buffer_size, "%d", data);
        break;
    case 'u':
        formatted_length = sprintf_s(buffer, buffer_size, "%u", (unsigned int)data);
        break;
    case 'x':
        formatted_length = sprintf_s(buffer, buffer_size, "%x", data);
        break;
    case 'X':
        formatted_length = sprintf_s(buffer, buffer_size, "%X", data);
        break;
    case 'b':
        formatted_length = SDSC_Print_Binary(buffer, data);
        break;
    case 'a':
    case 's':
        if (data_type != FORMAT_TYPE("mb") && data_type != FORMAT_TYPE("vb"))
        {
            SDSC_Format_Error("[Invalid format string: format type '%c' with data type '%c%c'", format, data_type >> 8, data_type & 0xff);
            return;
        }
        formatted_length = SDSC_Print_ASCII(buffer, buffer_size, data_type == FORMAT_TYPE("vb"), format == 'a', parameter, width);
        break;
    default:
        // Should already have checked this...
        SDSC_Format_Error("Invalid format string: format type '%c'", format);
        return;
    }

    // Check width
    if (width == 0)
    {
        // If no width is specified, use the "natural" width
        width = formatted_length;
    }
    else if (width > formatted_length)
    {
        // If it is wider, pad appropriately
        char padding;
        switch (format)
        {
        case 'x':
        case 'X':
        case 'b':
            padding = '0';
            break;
        default:
            padding = ' ';
            break;
        }
        int padding_count = width - formatted_length;
        const int space_in_buffer = sdsc_console_buffer_size - (p_format_start - sdsc_console_buffer);
        if (padding_count > space_in_buffer)
        {
            padding_count = space_in_buffer;
        }
        for (int i = 0; i < padding_count; ++i)
        {
            *p_format_start++ = padding;
        }
    }
    // Now copy from buffer into the global buffer
    char* p = buffer;
    if (width < formatted_length)
    {
        // Skip leading chars to fit
        p += formatted_length - width;
    }

    // Replace the format string with the result
    strncpy_s(p_format_start, sdsc_console_buffer_size - (p_format_start - sdsc_console_buffer), p, width);
    p_next_char = p_format_start + formatted_length;
    // And signal that we are done
    p_format_start = NULL;
}

void SDSC_Debug_Console_Data(char c)
{
    if (!have_sdsc_tag)
    {
        return;
    }

    // Append to buffer
    *p_next_char++ = c;
    if (p_format_start != NULL)
    {
        // Try if we can process it, or reject it
        SDSC_Try_Format();
    }
    else if (c == '%')
    {
        // Point at the leading '%'
        p_format_start = p_next_char - 1;
    }
    else if (c == 10)
    {
        // Emit buffer up to this point
        *p_next_char = NULL;
        Msg(MSGT_USER_LOG, "SDSC> %s", sdsc_console_buffer);
        p_next_char = sdsc_console_buffer;
    }
    else if (c == 13)
    {
        // Not supported
        --p_next_char;
    }
    else if (c < ' ' || c > 127)
    {
        Msg(MSGT_USER_LOG, "SDSC debug console: invalid character $%02x", c);
    }
    if (p_next_char - sdsc_console_buffer >= sdsc_console_buffer_size)
    {
        // Buffer overflow
        --p_next_char;
        Msg(MSGT_USER_LOG, "SDSC debug console: buffer overflow, dumping contents");
        *p_next_char = NULL;
        Msg(MSGT_USER_LOG, "SDSC> %s", sdsc_console_buffer);
        p_next_char = sdsc_console_buffer;
        p_format_start = NULL;
        // Push the last char back in
        *p_next_char++ = c;
    }
}

//-----------------------------------------------------------------------------

