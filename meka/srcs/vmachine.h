//-----------------------------------------------------------------------------
// MEKA - vmachine.h
// Virtual Machine - Headers
//-----------------------------------------------------------------------------

// Machine Positions ----------------------------------------------------------
// - machine are relative to (Xmax, 0)
// - machine subparts are relative to the machine itself
#define VMACHINE_SMS_POS_X           (-338)
#define VMACHINE_SMS_POS_Y           (+28)
#define VMACHINE_SMS_CART_POS_X      (+160)
#define VMACHINE_SMS_CART_POS_Y      (+0)
#define VMACHINE_SMS_LIGHT_POS_X     (+151)
#define VMACHINE_SMS_LIGHT_POS_Y     (+49)
#define VMACHINE_COLECO_POS_X        (-320)
#define VMACHINE_COLECO_POS_Y        (+42)

// Status Simulation ----------------------------------------------------------
void    Free_ROM();
void    Machine_ON();
void    Machine_OFF();
void    Machine_Init();
void    Machine_Insert_Cartridge();
void    Machine_Remove_Cartridge();

// Graphics -------------------------------------------------------------------
void    VMachine_Draw();
void    VMachine_Init_Colors();

extern int g_machine_flags;

#define MACHINE_POWER_ON        (1 << 0)
#define MACHINE_CART_INSERTED   (1 << 1)
#define MACHINE_ROM_LOADED      (1 << 2)
#define MACHINE_NOT_IN_BIOS     (1 << 3)
#define MACHINE_PAUSED          (1 << 4)
#define MACHINE_DEBUGGING       (1 << 5)

#define MACHINE_RUN             (MACHINE_POWER_ON | MACHINE_CART_INSERTED | MACHINE_ROM_LOADED)

//-----------------------------------------------------------------------------

