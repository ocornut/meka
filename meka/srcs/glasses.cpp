//-----------------------------------------------------------------------------
// MEKA - glasses.c
// 3D Glasses Support and Emulation - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "blit.h"
#include "blitintf.h"
#include "glasses.h"
#include "inputs_c.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_glasses   Glasses;

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

int     Glasses_ComPort_Initialize      (void);
void    Glasses_ComPort_Close           (void);
void    Glasses_ComPort_Write           (int left_enable, int right_enable);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Glasses_Init_Values (void)
{
    Glasses.Enabled = FALSE;
    Glasses_Set_Mode (GLASSES_MODE_SHOW_ONLY_RIGHT);
    // Note: RIGHT being the default is better because when ran in Japanese
    // mode, Space Harrier and Maze Walker keep only this side enabled for
    // one or two seconds.

#ifdef ARCH_WIN32
    Glasses.ComHandle = INVALID_HANDLE_VALUE;
    Glasses_Set_ComPort (1);
#else
    // MS-DOS users are likely to have a mouse and use COM 2 by default
    Glasses_Set_ComPort (2);
#endif
}

void    Glasses_Close (void)
{
    if (Glasses.Enabled && Glasses.Mode == GLASSES_MODE_COM_PORT)
    {
        Glasses_ComPort_Write (FALSE, FALSE);
        Glasses_ComPort_Close();
    }
}

int     Glasses_Must_Skip_Frame(void)
{
    static int security_cnt = 0;
    // Msg(MSGT_DEBUG, "%02X-%02X-%02X-%02X", RAM[0x1FF8], RAM[0x1FF9], RAM[0x1FFA], RAM[0x1FFB]);

    const int side = (sms.Glasses_Register & 1);
    const bool ret = (Glasses.Mode == GLASSES_MODE_SHOW_ONLY_LEFT && !side) || (Glasses.Mode == GLASSES_MODE_SHOW_ONLY_RIGHT && side);
    if (ret == FALSE)
    {
        security_cnt = 0;
    }
    else
    {
        if (++security_cnt >= 180) // Arbitrary value (180 updates, should be 3 seconds)
        {
            security_cnt = 0;
            Msg(MSGT_USER, "%s", Msg_Get(MSG_Glasses_Unsupported));
            // Msg(MSGT_USER_BOX, "%s", Msg_Get(MSG_Glasses_Unsupported2));
            Glasses_Switch_Enable();
        }
    }
    return (ret);
}

void    Glasses_Set_Mode (int mode)
{
    if (Glasses.Mode == GLASSES_MODE_COM_PORT && mode != GLASSES_MODE_COM_PORT)
        Glasses_ComPort_Close();
    Glasses.Mode = mode;
    if (Glasses.Mode == GLASSES_MODE_COM_PORT)
        if (!Glasses_ComPort_Initialize ())
        {
            Msg(MSGT_USER, Msg_Get(MSG_Glasses_Com_Port_Open_Error), Glasses.ComPort);
        }
}

void    Glasses_Set_ComPort (int port)
{
    Glasses.ComPort = port;
    if (Glasses.Mode == GLASSES_MODE_COM_PORT)
        if (!Glasses_ComPort_Initialize())
        {
            Msg(MSGT_USER, Msg_Get(MSG_Glasses_Com_Port_Open_Error), Glasses.ComPort);
        }
}

int     Glasses_ComPort_Initialize (void)
{
#ifdef ARCH_DOS

    int base = (Glasses.ComPort == 1) ? 0x3F0 : 0x2F0;

    // IER : Disable all interrupts
    asm volatile ("cli");
    outportb (base + 0x09, 0x00);
    asm volatile ("sti");
    // LCR : Set to 8 bits, 1 stop bit, no parity
    asm volatile ("cli");
    outportb (base + 0x0B, 0x83);
    asm volatile ("sti");
    // MCR : Set to zero, will be set again later
    asm volatile ("cli");
    outportb (base + 0x0C, 0x00);
    asm volatile ("sti");

    return true;

#elif ARCH_WIN32

    HANDLE  handle;
    DCB     dcb;
    char *  com_device_name;

    // Close previously opened COM device
    if (Glasses.ComHandle != INVALID_HANDLE_VALUE)
        Glasses_ComPort_Close();

    // Find COM device name
    if (Glasses.ComPort == 1)
        com_device_name = "COM1";
    else if (Glasses.ComPort == 2)
        com_device_name = "COM2";
    else
        return false;

    // Open COM device
    handle = CreateFile(com_device_name, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (handle == INVALID_HANDLE_VALUE)
        return false;

    // Get current state
    if (!GetCommState(handle, &dcb))
    {
        CloseHandle (handle);
        return false;
    }

    // Fill in DCB: 57,600 bps, 8 data bits, no parity, and 1 stop bit.
    dcb.BaudRate    = CBR_57600;    // set the baud rate
    dcb.ByteSize    = 8;            // data size, xmit, and rcv
    dcb.Parity      = NOPARITY;     // no parity bit
    dcb.StopBits    = ONESTOPBIT;   // one stop bit

    // Set new state
    if (!SetCommState(handle, &dcb))
    {
        CloseHandle (handle);
        return false;
    }

    Glasses.ComHandle = handle;
    return true;

#else

    return true;

#endif
}

void    Glasses_ComPort_Close (void)
{
#ifdef ARCH_WIN32
    if (Glasses.ComHandle == INVALID_HANDLE_VALUE)
        return;
    CloseHandle (Glasses.ComHandle);
    Glasses.ComHandle = INVALID_HANDLE_VALUE;
#endif
}

void    Glasses_ComPort_Write (int left_enable, int right_enable)
{
#ifdef ARCH_DOS

    int address = (Glasses.ComPort == 1) ? 0x3FC : 0x2FC;
    int data;
    if (left_enable)
        data = 0x03; // MCR = DTR | RTS
    else if (right_enable)
        data = 0x01; // MCR = DTR
    else
        data = 0x00;

    asm volatile ("cli");
    outportb (address, data);
    asm volatile ("sti");

#elif ARCH_WIN32

    DCB dcb;
    if (Glasses.ComHandle == INVALID_HANDLE_VALUE)
        return;
    if (!GetCommState(Glasses.ComHandle, &dcb))
        return;
    if (left_enable)
    {
        dcb.fDtrControl = DTR_CONTROL_ENABLE;
        dcb.fRtsControl = RTS_CONTROL_ENABLE;
    }
    else if (right_enable)
    {
        dcb.fDtrControl = DTR_CONTROL_ENABLE;
        dcb.fRtsControl = RTS_CONTROL_DISABLE;
    }
    else
    {
        dcb.fDtrControl = DTR_CONTROL_DISABLE;
        dcb.fRtsControl = RTS_CONTROL_DISABLE;
    }
    if (!SetCommState(Glasses.ComHandle, &dcb))
        return;

#else
    // Other systems (UNIX) yet unsupported
#endif
}

void    Glasses_Update (void)
{
    if (Glasses.Mode == GLASSES_MODE_COM_PORT)
    {
        if (sms.Glasses_Register & 1)
            Glasses_ComPort_Write (TRUE, FALSE);
        else
            Glasses_ComPort_Write (FALSE, TRUE);
    }
}

void    Glasses_Switch_Enable (void)
{
    Glasses_Close();
    Glasses.Enabled ^= 1;
    if (Glasses.Enabled)
    {
        gui_menu_check (menus_ID.glasses, 0);
        Msg(MSGT_USER, "%s", Msg_Get(MSG_Glasses_Enabled));
    }
    else
    {
        gui_menu_uncheck_range(menus_ID.glasses, 0, 0);
        Msg(MSGT_USER, "%s", Msg_Get(MSG_Glasses_Disabled));
    }
    gui_menu_active_range (Glasses.Enabled, menus_ID.glasses, 1, 4);
    Inputs_CFG_Peripherals_Draw();
}

void    Glasses_Switch_Mode_Show_Both (void)
{
    Glasses_Close();
    Glasses.Mode = GLASSES_MODE_SHOW_BOTH;
    gui_menu_uncheck_range (menus_ID.glasses, 1, 4);
    gui_menu_check (menus_ID.glasses, 1);
    Msg(MSGT_USER, "%s", Msg_Get(MSG_Glasses_Show_Both));
}

void    Glasses_Switch_Mode_Show_Left (void)
{
    Glasses_Close();
    Glasses.Mode = GLASSES_MODE_SHOW_ONLY_LEFT;
    gui_menu_uncheck_range (menus_ID.glasses, 1, 4);
    gui_menu_check (menus_ID.glasses, 2);
    Msg(MSGT_USER, "%s", Msg_Get(MSG_Glasses_Show_Left));
}

void    Glasses_Switch_Mode_Show_Right (void)
{
    Glasses_Close();
    Glasses.Mode = GLASSES_MODE_SHOW_ONLY_RIGHT;
    gui_menu_uncheck_range (menus_ID.glasses, 1, 4);
    gui_menu_check (menus_ID.glasses, 3);
    Msg(MSGT_USER, "%s", Msg_Get(MSG_Glasses_Show_Right));
}

void    Glasses_Switch_Mode_Com_Port (void)
{
    Glasses.Mode = GLASSES_MODE_COM_PORT;
    gui_menu_uncheck_range (menus_ID.glasses, 1, 4);
    gui_menu_check (menus_ID.glasses, 4);
    Msg(MSGT_USER, Msg_Get(MSG_Glasses_Com_Port), Glasses.ComPort);
    Msg(MSGT_USER_BOX, "%s", Msg_Get(MSG_Glasses_Com_Port2));
}

//-----------------------------------------------------------------------------

