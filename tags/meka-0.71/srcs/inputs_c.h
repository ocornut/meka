//-----------------------------------------------------------------------------
// MEKA - inputs_c.h
// Inputs Configuration Applet - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct
{
  BITMAP *  Bmp;
  byte      Active, ID;
  int       Res_X, Res_Y;
  int       Current_Source;
  int       Current_Map;           // if != -1 then mapping is being changed
  t_widget *CheckBox_Enabled;
  t_widget *CheckBox_Emulate_Digital;
  byte      CheckBox_Emulate_Digital_Value;
} Inputs_CFG;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Inputs_CFG_Switch                       (void);
void    Inputs_CFG_Init_Applet                  (void);

void    Inputs_CFG_Current_Source_Draw          (void);
byte    Inputs_CFG_Current_Source_Draw_Map      (int i, int Color);
void    Inputs_CFG_Current_Source_Change        (t_widget *w);

void    Inputs_CFG_Peripherals_Draw             (void);

void    Inputs_CFG_Peripheral_Change            (int Player, int Periph);
void    Inputs_CFG_Peripheral_Change_Handler    (t_widget *w);

void    Inputs_CFG_Map_Change_Handler           (t_widget *w);
void    Inputs_CFG_Map_Change_Update            (void);
void    Inputs_CFG_Map_Change_End               (void);

void    Inputs_CFG_Emulate_Digital_Handler      (t_widget *w);

//-----------------------------------------------------------------------------