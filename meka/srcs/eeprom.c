//-----------------------------------------------------------------------------
// MEKA - eeprom.c
// EEPROM Emulation (model 93c46) - Code
//-----------------------------------------------------------------------------

#include "shared.h"
// #define DEBUG_EEPROM
#include "eeprom.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_93c46  EEPROM_93c46;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    BMemory_93c46_Load (FILE *f)
{
  if (fread (EEPROM_93c46.Data, EEPROM_93C46_DATA_SIZE, 1, f) == 1)
     Msg(MSGT_USER, Msg_Get(MSG_93c46_Loaded), EEPROM_93C46_DATA_SIZE);
  else
     Msg(MSGT_USER, "%s", Msg_Get(MSG_93c46_Load_Unable));
}

void    BMemory_93c46_Save (FILE *f)
{
  if (f && fwrite (EEPROM_93c46.Data, EEPROM_93C46_DATA_SIZE, 1, f) == 1)
     Msg(MSGT_USER, Msg_Get(MSG_93c46_Wrote), EEPROM_93C46_DATA_SIZE);
  else
     Msg(MSGT_USER, Msg_Get(MSG_93c46_Write_Unable), EEPROM_93C46_DATA_SIZE);
}

void    BMemory_93c46_Load_State (FILE *f)
{
  fread (&EEPROM_93c46, sizeof (EEPROM_93c46), 1, f);
}

void    BMemory_93c46_Save_State (FILE *f)
{
  fwrite (&EEPROM_93c46, sizeof (EEPROM_93c46), 1, f);
}

void    BMemory_93c46_Get_Infos (void **data, int *len)
{
  (*data) = EEPROM_93c46.Data;
  (*len)  = EEPROM_93C46_DATA_SIZE;
}

//-----------------------------------------------------------------------------

/* Write to 0xFFFC */
void    EEPROM_93c46_Control (byte v)
{
  if (v & 0x80)
     {
     #ifdef DEBUG_EEPROM
        Msg(MSGT_DEBUG, Msg_Get(MSG_93c46_Reset));
     #endif
     EEPROM_93c46_Init (EEPROM_93C46_INIT_ALL);
     return;
     }
  EEPROM_93c46.Enabled = (v & 0x08) ? TRUE : FALSE;
  #ifdef DEBUG_EEPROM
     Msg(MSGT_DEBUG, "At PC=%04X: 93c46: Enabled=%02X", CPU_GetPC, v);
  #endif
}

/* Clear 93c46 EEPROM data */
void    EEPROM_93c46_Clear (void)
{
  memset (EEPROM_93c46.Data, 0xFF, EEPROM_93C46_DATA_SIZE);
}

/* Initialiaze 93c46 EEPROM - do NOT clear its data! */
void    EEPROM_93c46_Init (int Init)
{
  if (Init == EEPROM_93C46_INIT_ALL)
     {
     EEPROM_93c46.Enabled = FALSE;
     EEPROM_93c46.Lines = 0x00 | EEPROM_93C46_LINE_DATA_OUT;
     EEPROM_93c46.Read_Only = TRUE;
     }
  EEPROM_93c46.Status = EEPROM_93C46_STATUS_START;
  EEPROM_93c46.Opcode = 0x0000;
  EEPROM_93c46.Position = 0;
}

/* Write to 0x8000 when 93c46 EEPROM is enabled */
void    EEPROM_93c46_Set_Lines (byte lines)
{
  #ifdef DEBUG_EEPROM
    {
    char * lines_desc = "0";
    switch (lines & 7)
      {
      case 1: lines_desc = "DATA";            break;
      case 2: lines_desc = "CLK";             break;
      case 3: lines_desc = "DATA | CLK";      break;
      case 4: lines_desc = "CS";              break;
      case 5: lines_desc = "DATA | CS";       break;
      case 6: lines_desc = "CLK | CS";        break;
      case 7: lines_desc = "DATA | CLK | CS"; break;
      }
    Msg(MSGT_DEBUG, "At PC=%04X: 93c46: Set Lines %s", CPU_GetPC, lines_desc);
    }
  #endif

  /* CS Line (Reset), 1->0 edge */
  if (!(lines & EEPROM_93C46_LINE_CS))
     {
     if (EEPROM_93c46.Lines & EEPROM_93C46_LINE_CS)
        {
        EEPROM_93c46_Init (EEPROM_93C46_INIT_NORMAL);
        }
     EEPROM_93c46.Lines = (EEPROM_93c46.Lines & ~0x07) | (lines & 0x07)
                        | EEPROM_93C46_LINE_DATA_OUT;
     return;
     }

  /* Clock Line, 0->1 edge */
  /* (This is the big part) */
  if ((lines & EEPROM_93C46_LINE_CLOCK) && !(EEPROM_93c46.Lines & EEPROM_93C46_LINE_CLOCK))
     {
     byte   data = lines & EEPROM_93C46_LINE_DATA_IN;
     EEPROM_93c46.Lines = (EEPROM_93c46.Lines & ~0x07) | (lines & 0x07);
     switch (EEPROM_93c46.Status)
       {
       // Start Bit ------------------------------------------------------------
       case EEPROM_93C46_STATUS_START:
            if (data)
               {
               #ifdef DEBUG_EEPROM
                  Msg(MSGT_DEBUG, "At PC=%04X: 93c46: START", CPU_GetPC);
               #endif
               EEPROM_93c46.Status = EEPROM_93C46_STATUS_OPCODE;
               EEPROM_93c46.Opcode = 0x0000;
               EEPROM_93c46.Position = 0;
               }
            return;
       // Opcode (2 Bits) + Address (6 Bits) Read ------------------------------
       case EEPROM_93C46_STATUS_OPCODE:
            EEPROM_93c46.Opcode = (EEPROM_93c46.Opcode << 1) | data;
            if (++EEPROM_93c46.Position == 8)
               {
               #ifdef DEBUG_EEPROM
                  char bitfield[9];
                  StrWriteBitfield (EEPROM_93c46.Opcode, 8, bitfield);
                  Msg(MSGT_DEBUG, "At PC=%04X: 93c46: OPCODE = %s", CPU_GetPC, bitfield);
               #endif
               switch (EEPROM_93c46.Opcode & 0xC0)
                 {
                 case 0x00: // 00: EXTENDED ------------------------------------
                      switch (EEPROM_93c46.Opcode & 0x30)
                        {
                        case 0x00: // 00: ERASE/WRITE DISABLE ------------------
                             #ifdef DEBUG_EEPROM
                                Msg(MSGT_DEBUG, "At PC=%04X: 93c46: E/W DISABLE", CPU_GetPC);
                             #endif
                             EEPROM_93c46.Read_Only = TRUE;
                             EEPROM_93c46.Status = EEPROM_93C46_STATUS_START;
                             return;
                        case 0x10: // 00: WRITE ALL ----------------------------
                             #ifdef DEBUG_EEPROM
                                Msg(MSGT_DEBUG, "At PC=%04X: 93c46: WRITE ALL", CPU_GetPC);
                             #endif
                             EEPROM_93c46.Position = 0;
                             EEPROM_93c46.Latch = 0x0000;
                             EEPROM_93c46.Status = EEPROM_93C46_STATUS_WRITING;
                             return;
                        case 0x20: // 00: ERASE ALL ----------------------------
                             if (EEPROM_93c46.Read_Only == FALSE)
                                {
                                #ifdef DEBUG_EEPROM
                                   Msg(MSGT_DEBUG, "At PC=%04X: 93c46: ERASE ALL", CPU_GetPC());
                                #endif
                                memset (EEPROM_93c46.Data, 0xFF, EEPROM_93C46_DATA_SIZE);
                                }
                             else
                                {
                                Msg(MSGT_DEBUG, "At PC=%04X: 93c46: ERASE ALL attempt, but E/W is Disabled!", CPU_GetPC());
                                }
                             EEPROM_93c46.Lines |= EEPROM_93C46_LINE_DATA_OUT; // Ready
                             EEPROM_93c46.Status = EEPROM_93C46_STATUS_START;
                             return;
                        case 0x30: // 00: ERASE/WRITE ENABLE -------------------
                             #ifdef DEBUG_EEPROM
                                Msg(MSGT_DEBUG, "At PC=%04X: 93c46: E/W ENABLE", CPU_GetPC);
                             #endif
                             EEPROM_93c46.Read_Only = FALSE;
                             EEPROM_93c46.Status = EEPROM_93C46_STATUS_START;
                             return;
                        }
                      return;
                 case 0x40: // 01: WRITE ---------------------------------------
                      #ifdef DEBUG_EEPROM
                         Msg(MSGT_DEBUG, "At PC=%04X: 93c46: WRITE %02X", CPU_GetPC, EEPROM_93c46.Opcode & 0x3F);
                      #endif
                      EEPROM_93c46.Position = 0;
                      EEPROM_93c46.Latch = 0x0000;
                      EEPROM_93c46.Status = EEPROM_93C46_STATUS_WRITING;
                      return;
                 case 0x80: // 10: READ ----------------------------------------
                      #ifdef DEBUG_EEPROM
                         Msg(MSGT_DEBUG, "At PC=%04X: 93c46: READ %02X", CPU_GetPC, EEPROM_93c46.Opcode & 0x3F);
                      #endif
                      EEPROM_93c46.Position = 0;
                      //EEPROM_93c46.Latch = (EEPROM_93c46.Opcode & 0x3F); // Address
                      EEPROM_93c46.Status = EEPROM_93C46_STATUS_READING;
                      EEPROM_93c46.Lines &= ~EEPROM_93C46_LINE_DATA_OUT; // Dummy Zero
                      return;
                 case 0xC0: // 11: ERASE ---------------------------------------
                      if (EEPROM_93c46.Read_Only == FALSE)
                         {
                         #ifdef DEBUG_EEPROM
                            Msg(MSGT_DEBUG, "At PC=%04X: 93c46: ERASE %02X", CPU_GetPC, EEPROM_93c46.Opcode & 0x3F);
                         #endif
                         EEPROM_93c46.Data[EEPROM_93c46.Opcode & 0x3F] = 0xFFFF;
                         EEPROM_93c46.Lines |= EEPROM_93C46_LINE_DATA_OUT; // Ready
                         EEPROM_93c46.Status = EEPROM_93C46_STATUS_START;
                         }
                      else
                         {
                         Msg(MSGT_DEBUG, "At PC=%04X: 93c46: ERASE attempt, but E/W is Disabled!", CPU_GetPC());
                         }
                      return;
                 }
               }
            return;
       // Reading --------------------------------------------------------------
       case EEPROM_93C46_STATUS_READING:
            if (EEPROM_93c46.Data [EEPROM_93c46.Opcode & 0x3F] & (0x8000 >> EEPROM_93c46.Position))
                 EEPROM_93c46.Lines |=  EEPROM_93C46_LINE_DATA_OUT; // Bit 1
            else EEPROM_93c46.Lines &= ~EEPROM_93C46_LINE_DATA_OUT; // Bit 0
            #ifdef DEBUG_EEPROM
               Msg(MSGT_DEBUG, "At PC=%04X: 93c46: READ BIT %d = %02X", CPU_GetPC, EEPROM_93c46.Position, EEPROM_93c46.Lines & EEPROM_93C46_LINE_DATA_OUT);
            #endif
            if (++EEPROM_93c46.Position == 16)
               {
               EEPROM_93c46.Position = 0;
               EEPROM_93c46.Opcode = 0x80 | ((EEPROM_93c46.Opcode + 1) & 0x3F);
               }
            return;
       // Writing --------------------------------------------------------------
       case EEPROM_93C46_STATUS_WRITING:
            EEPROM_93c46.Latch = (EEPROM_93c46.Latch << 1) | data;
            if (++EEPROM_93c46.Position == 16)
               {
               if (EEPROM_93c46.Read_Only == FALSE)
                  {
                  if ((EEPROM_93c46.Opcode & 0x40) == 0x40)
                     { // 01: WRITE -----------------------------------------------
                     #ifdef DEBUG_EEPROM
                        Msg(MSGT_DEBUG, "At PC=%04X: 93c46: WRITE %02X = %04X", CPU_GetPC, EEPROM_93c46.Opcode & 0x3F, EEPROM_93c46.Latch);
                     #endif
                     EEPROM_93c46.Data [EEPROM_93c46.Opcode & 0x3F] = EEPROM_93c46.Latch;
                     }
                  else
                     { // 00.01: WRITE ALL ----------------------------------------
                     int i;
                     #ifdef DEBUG_EEPROM
                        Msg(MSGT_DEBUG, "At PC=%04X: 93c46: WRITE ALL = %04X", CPU_GetPC, EEPROM_93c46.Latch);
                     #endif
                     for (i = 0; i < 64; i++)
                         {
                         EEPROM_93c46.Data [i] = EEPROM_93c46.Latch;
                         }
                     }
                  }
               else
                  {
                  Msg(MSGT_DEBUG, "At PC=%04X: 93c46: WRITE [ALL] attempt = %04X, but E/W is Disabled!", CPU_GetPC(), EEPROM_93c46.Latch);
                  }
               EEPROM_93c46.Lines |= EEPROM_93C46_LINE_DATA_OUT; // Ready
               EEPROM_93c46.Status = EEPROM_93C46_STATUS_START;
               }
            return;
       }
     Msg(MSGT_DEBUG, "Error #8190, EEPROM 93c46, CLK raising edge: undefined status.");
     return;
     }

  /* Data Line */
  EEPROM_93c46.Lines = (EEPROM_93c46.Lines & ~0x07) | (lines & 0x07);
}

/* Read from 0x8000 when 93c46 is enabled */
byte    EEPROM_93c46_Read (void)
{
  int   ret;

  ret = (EEPROM_93c46.Lines & EEPROM_93C46_LINE_CS)
      | ((EEPROM_93c46.Lines & EEPROM_93C46_LINE_DATA_OUT) >> EEPROM_93C46_LINE_DATA_OUT_POS)
      | EEPROM_93C46_LINE_CLOCK;
  #ifdef DEBUG_EEPROM
     Msg(MSGT_DEBUG, "At PC=%04X: 93c46: Read %d (Lines %d)", CPU_GetPC, ret, EEPROM_93c46.Lines);
  #endif
  return (ret);
}

// For Nomo's World Series Baseball
void    EEPROM_93c46_Direct_Write       (int Addr, byte Data)
{
  EEPROM_93c46.Data[Addr] = Data;
}

// For Nomo's World Series Baseball
byte    EEPROM_93c46_Direct_Read        (int Addr)
{
  return (EEPROM_93c46.Data[Addr]);
}

//-----------------------------------------------------------------------------

