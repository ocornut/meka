//-----------------------------------------------------------------------------
// MEKA - inputs_c.h
// Inputs Configuration Applet - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_app_inputs_config
{
    t_gui_box * box;
    bool        active;
    bool        dirty;

    int         Current_Source;
    int         Current_Map;           // if != -1 then mapping is being changed
    t_widget *  CheckBox_Enabled;
    t_widget *  CheckBox_Emulate_Digital;
    bool        CheckBox_Emulate_Digital_Value;
};

extern t_app_inputs_config     Inputs_CFG;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Inputs_CFG_Switch                       ();
void    Inputs_CFG_Init_Applet                  ();
void    Inputs_CFG_Update                       (t_app_inputs_config *app);

void    Inputs_CFG_Current_Source_Draw          (void);
byte    Inputs_CFG_Current_Source_Draw_Map      (int i, ALLEGRO_COLOR Color);
void    Inputs_CFG_Current_Source_Change        (t_widget *w);

void    Inputs_CFG_Peripherals_Draw             (void);

void    Inputs_CFG_Peripheral_Change            (int Player, t_input_peripheral Periph);
void    Inputs_CFG_Peripheral_Change_Handler    (t_widget *w);

void    Inputs_CFG_Map_Change_Handler           (t_widget *w);
void    Inputs_CFG_Map_Change_Update            (void);
void    Inputs_CFG_Map_Change_End               (void);

void    Inputs_CFG_Emulate_Digital_Handler      (t_widget *w);

//-----------------------------------------------------------------------------
